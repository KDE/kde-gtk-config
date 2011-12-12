/*
    <KCMModule>
    Copyright (C) <2011>  <Jose Antonio Sanchez Reynaga>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "gtkconfigkcmodule.h"
#include <kaboutdata.h>
#include <KGenericFactory>
#include <KPluginFactory>
#include <QtGui>

K_PLUGIN_FACTORY(GTKConfigKCModuleFactory, registerPlugin<GTKConfigKCModule>();)
K_EXPORT_PLUGIN(GTKConfigKCModuleFactory("cgc","kcm_cgc"))

QMap<QString, int> gtkToolbarInit()
{
    QMap<QString, int> gtkToolbar;
    gtkToolbar["GTK_TOOLBAR_ICONS"] = 0;
    gtkToolbar["GTK_TOOLBAR_TEXT"] = 1;
    gtkToolbar["GTK_TOOLBAR_BOTH"] = 2;
    gtkToolbar["GTK_TOOLBAR_BOTH_HORIZ"] = 3;
    return gtkToolbar;
}

static QMap<QString, int> gtkToolbar = gtkToolbarInit();

GTKConfigKCModule::GTKConfigKCModule(QWidget* parent, const QVariantList& args )
    : KCModule(GTKConfigKCModuleFactory::componentData(), parent)
    , ui(new Ui::GUI)
{
    Q_UNUSED(args);
    KAboutData *acercade = new KAboutData("cgc","kcm_cgc",ki18n("Chakra Gtk Config"), "1.7",
                    ki18n("Config your Gtk Apps"),
                    KAboutData::License_GPL_V2,
                    ki18n("Copyright 2011 José Antonio Sánchez Reynaga"));
    acercade->addAuthor(ki18n("José Antonio Sánchez Reynaga (antonioJASR)"),ki18n("Main Developer"), "joanzare@gmail.com");
    acercade->addCredit(ki18n("Manuel Tortosa (manutortosa)"), ki18n("Ideas, tester, internationalization"));
    acercade->addCredit(ki18n("Adrián Chaves Fernández (Gallaecio)"), ki18n("Internationalization"));
    setAboutData(acercade);
    
    ui->setupUi(this);
    appareance = new AppearenceGTK;
    refreshLists();
    makePreviewIconTheme();
    makePreviewFont();
    
    //UI changes
    connect(ui->cb_theme, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_theme_gtk3, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon_fallback ,SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_font, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_font_style, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->spin_font_tam, SIGNAL(valueChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_toolbar_icons, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->checkBox_icon_gtk_menus, SIGNAL(clicked(bool)), this, SLOT(appChanged()));
    connect(ui->checkBox_icon_gtk_buttons, SIGNAL(clicked(bool)), this, SLOT(appChanged()));

    //preview updates
    connect(ui->cb_icon_fallback, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));
    connect(ui->cb_icon, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));

    //installers connections
    connect(ui->clb_add_theme, SIGNAL(clicked(bool)), this, SLOT(showDialogForInstall()));
    connect(ui->clb_remove_theme, SIGNAL(clicked(bool)), this, SLOT(showDialogForUninstall()));
    
    installer =  new DialogInstaller(this);
    uninstaller = new DialogUninstaller(this, appareance);
    
    connect(installer, SIGNAL(themeInstalled()), SLOT(refreshLists()));
    connect(uninstaller, SIGNAL(themeUninstalled()), SLOT(refreshLists()));
    
    //font preview
    connect(ui->cb_font, SIGNAL(activated(QString)), this, SLOT(makePreviewFont()));
    connect(ui->cb_font_style, SIGNAL(activated(QString)), this, SLOT(makePreviewFont()));
    connect(ui->spin_font_tam, SIGNAL(valueChanged(int)), this, SLOT(makePreviewFont()));
    
    //GHNS connections
    connect(ui->but_theme_ghns, SIGNAL(clicked(bool)), this, SLOT(showThemeGHNS()));
    connect(ui->but_theme_gtk3_ghns, SIGNAL(clicked(bool)), this, SLOT(installThemeGTK3GHNS()));
}

GTKConfigKCModule::~GTKConfigKCModule()
{
    delete ui;
    delete appareance;
}

void GTKConfigKCModule::showThemeGHNS()
{
     KNS3::DownloadDialog dialogo("cgctheme.knsrc", this);
     if(dialogo.exec()) {
          refreshLists();
     }
}

void GTKConfigKCModule::installThemeGTK3GHNS()
{
     KNS3::DownloadDialog dialogo("cgcgtk3.knsrc", this);
     if(dialogo.exec()) {
          refreshLists();
     }
}

void GTKConfigKCModule::refreshLists()
{
    refreshThemesUi(true);

    QString font = appareance->getFont();
    if(!font.isEmpty()) {
        QRegExp fontRx(QString(" (italic)? *(bold)? *([0-9]+)$"));
        int pos = fontRx.indexIn(font)-1;
        QString fontFamily = font.left(pos-1);

        bool bold = !fontRx.cap(1).isEmpty();
        bool italic = !fontRx.cap(2).isEmpty();
        int fontSize = fontRx.cap(3).toInt();

        int style=bold | (italic<<1);

        ui->cb_font->setCurrentFont(QFont(fontFamily));
        ui->cb_font_style->setCurrentIndex(style);
        ui->spin_font_tam->setValue(fontSize);
    }
    
    ui->cb_toolbar_icons->setCurrentIndex(gtkToolbar[appareance->getToolbarStyle()]);
    
    ui->checkBox_icon_gtk_buttons->setChecked(appareance->getShowIconsInButtons());
    ui->checkBox_icon_gtk_menus->setChecked(appareance->getShowIconsInMenus());
}

void tryIcon(QLabel* label, const QString& fallback, const QString& theme, const QStringList& relativePaths)
{
    label->setToolTip(relativePaths.first());
    
    foreach(const QString & relpath, relativePaths) {
        QPixmap p(theme+relpath);
        if(!p.isNull()) {
            label->setPixmap(p);
            return;
        }
        
        QPixmap pFallback(fallback+relpath);
        if(!pFallback.isNull()) {
            label->setPixmap(pFallback);
            return;
        }
    }
    
    
    KIcon notFoundIcon("application-x-zerosize");
    QPixmap noIcon(notFoundIcon.pixmap(48,48));
    label->setPixmap(noIcon);
    
    qDebug() << "could not find icon" << relativePaths;
}

void GTKConfigKCModule::makePreviewIconTheme()
{
    int icon_fallback = ui->cb_icon_fallback->currentIndex();
    QString path_fallback = appareance->getAvaliableIconsPaths()[icon_fallback];
    
    int icon = ui->cb_icon->currentIndex();
    QString path_icon = appareance->getAvaliableIconsPaths()[icon];

    tryIcon(ui->lb_prev_1, path_fallback, path_icon,
            QStringList() << "/48x48/places/folder_home.png" << "/places/48/folder_home.png" << "/places/48/folder_home.svg");
    tryIcon(ui->lb_prev_2, path_fallback, path_icon,
            QStringList() << "/48x48/places/user-trash.png" << "/places/48/user-trash.png" << "/places/48/user-trash.svg");
    tryIcon(ui->lb_prev_3, path_fallback, path_icon,
            QStringList() << "/48x48/actions/document-print.png"<< "/actions/48/document-print.png" << "/actions/48/document-print.svg");
    tryIcon(ui->lb_prev_4, path_fallback, path_icon,
            QStringList() << "/48x48/places/user-desktop.png" << "/places/48/user-desktop.png" << "/places/48/user-desktop.svg");
    tryIcon(ui->lb_prev_5, path_fallback, path_icon,
            QStringList() << "/48x48/places/user-bookmarks.png" << "/places/48/user-bookmarks.png" << "/places/48/user-bookmarks.svg");
    tryIcon(ui->lb_prev_6, path_fallback, path_icon,
            QStringList() << "/48x48/places/network-server.png" << "/places/48/network-server.png" << "/places/48/network-server.svg");
    tryIcon(ui->lb_prev_7, path_fallback, path_icon,
            QStringList() << "/48x48/actions/help.png" << "/actions/48/help.png" << "/actions/48/help.svg");
    tryIcon(ui->lb_prev_8, path_fallback, path_icon,
            QStringList() << "/48x48/places/start-here.png" << "/places/48/start-here.png" << "/places/48/start-here.svg");
    tryIcon(ui->lb_prev_9, path_fallback, path_icon,
            QStringList() << "/48x48/actions/up.png" << "/actions/48/up.png" << "/actions/48/up.svg");
}

void GTKConfigKCModule::appChanged()
{
   emit(changed(true));
}

void GTKConfigKCModule::save()
{
    appareance->setThemeGtk3(ui->cb_theme_gtk3->currentText());
    appareance->setTheme(ui->cb_theme->currentText());

    foreach(const QString& i, appareance->getAvaliableThemesPaths()) {
        if(i.contains(ui->cb_theme->currentText())) {
            appareance->setThemePath(i);
            break;
        }
    }


    appareance->setIcon(ui->cb_icon->currentText());
    appareance->setIconFallBack(ui->cb_icon_fallback->currentText());

    QString estilo;
    int pos = ui->cb_font_style->currentIndex();

    if(pos & 1)
        estilo += " bold";
    if(pos & 2)
        estilo += " italic";
    
    QString font = ui->cb_font->currentFont().family() + estilo + ' ' + QVariant(ui->spin_font_tam->value()).toString();
    appareance->setFont( font );

    appareance->setToolbarStyle(gtkToolbar.key(ui->cb_toolbar_icons->currentIndex()));
    appareance->setShowIconsInButtons(ui->checkBox_icon_gtk_buttons->isChecked());
    appareance->setShowIconsInMenus(ui->checkBox_icon_gtk_menus->isChecked());

    kDebug() << "******************************************* INSTALLATION :\n"
            << "theme : " << appareance->getTheme() << "\n"
            << "themeGTK3 : " << appareance->getThemeGtk3() << "\n"
            << "theme path : " << appareance->getThemePath() << "\n"
            << "icons : " << appareance->getIcon() << "\n"
            << "fallback icons : " << appareance->getIconFallBack() << "\n"
            << "font family : " << appareance->getFont() << "\n"
            << "toolbar style : " << appareance->getToolbarStyle() << "\n"
            << "icons in buttons : " << appareance->getShowIconsInButtons() << "\n"
            << "icons in menus : " << appareance->getShowIconsInMenus() << "\n"
            
            << "********************************************************";
    
    if(!appareance->saveFileConfig())
        QMessageBox::warning(this, "ERROR", i18n("It was not possible to save the config"));
}


void GTKConfigKCModule::defaults()
{
    QFont f = font();
    
    appareance->setFont(QString("%1 %2").arg(f.family()).arg(f.pointSize()));
    appareance->setTheme("oxygen-gtk"); //TODO: review, should use system's settings, for better integration
    appareance->setThemePath("/usr/share/themes/oxygen-gtk");
    appareance->setIcon("oxygen-refit-2-2.5.0");
    appareance->setIconFallBack("oxygen");
    
    refreshLists();
}

void GTKConfigKCModule::refreshThemesUi(bool useConfig)
{
    //themes
    QString temp;
    temp = useConfig ? appareance->getTheme() : ui->cb_theme->currentText(); //The currently selected theme
    ui->cb_theme->clear();
    ui->cb_theme->addItems(appareance->getAvaliableThemes());
    ui->cb_theme->setCurrentIndex(ui->cb_theme->findText(temp));
    
    temp = useConfig ? appareance->getThemeGtk3() : ui->cb_theme_gtk3->currentText();
    ui->cb_theme_gtk3->clear();
    ui->cb_theme_gtk3->addItems(appareance->getAvaliableGtk3Themes());
    ui->cb_theme_gtk3->setCurrentIndex(ui->cb_theme_gtk3->findText(temp));
    
    //icons
    QStringList icons = appareance->getAvaliableIcons();
    temp = useConfig ? appareance->getIcon() : ui->cb_icon->currentText();
    ui->cb_icon->clear();
    ui->cb_icon->addItems(icons);
    ui->cb_icon->setCurrentIndex(ui->cb_icon->findText(temp));
    
    //fallback icons
    temp = useConfig ? appareance->getIconFallBack() : ui->cb_icon_fallback->currentText();
    ui->cb_icon_fallback->clear();
    ui->cb_icon_fallback->addItems(icons);
    ui->cb_icon_fallback->setCurrentIndex(ui->cb_icon_fallback->findText(temp));
}

void GTKConfigKCModule::showDialogForInstall()
{
    installer->exec();
    
    refreshThemesUi();
}

void GTKConfigKCModule::showDialogForUninstall()
{
    uninstaller->refresthListsForUninstall();
    uninstaller->exec();
    
    refreshThemesUi();
}

void GTKConfigKCModule::makePreviewFont()
{
    QFont fuente(ui->cb_font->currentFont());
    
    int pos = ui->cb_font_style->currentIndex();
    fuente.setBold(pos & 1);
    fuente.setItalic(pos & 2);
    
    fuente.setPointSize(ui->spin_font_tam->value());
    
    ui->lb_font_preview->setFont(fuente);
}   
