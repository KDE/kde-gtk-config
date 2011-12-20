/* KDE GTK Configuration Module
 * 
 * Copyright 2011 José Antonio Sanchez Reynaga <joanzare@gmail.com>
 * Copyright 2011 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtkconfigkcmodule.h"
#include <kaboutdata.h>
#include <KGenericFactory>
#include <KPluginFactory>
#include <KProcess>
#include <KStandardDirs>
#include <QtGui>
#include <QX11EmbedContainer>
#include "ui_gui.h"

K_PLUGIN_FACTORY(GTKConfigKCModuleFactory, registerPlugin<GTKConfigKCModule>();)
K_EXPORT_PLUGIN(GTKConfigKCModuleFactory("cgc","kde-gtk-config"))

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
    KAboutData *acercade = new KAboutData("cgc","kcm_cgc",ki18n("KDE Gtk Config"), "1.7",
                    ki18n("Config your Gtk Apps"),
                    KAboutData::License_LGPL_V3,
                    ki18n("Copyright 2011 José Antonio Sánchez Reynaga"));
    acercade->addAuthor(ki18n("José Antonio Sánchez Reynaga (antonioJASR)"),ki18n("Main Developer"), "joanzare@gmail.com");
    acercade->addAuthor(ki18n("Aleix Pol Gonzalez"), ki18n("Feature development. Previews, code refactoring."), "aleixpol@kde.org");
    acercade->addCredit(ki18n("Manuel Tortosa (manutortosa)"), ki18n("Ideas, tester, internationalization"));
    acercade->addCredit(ki18n("Adrián Chaves Fernández (Gallaecio)"), ki18n("Internationalization"));
    setAboutData(acercade);
    
    ui->setupUi(this);
    appareance = new AppearenceGTK;
    refreshLists();
    makePreviewIconTheme();
    
    //UI changes
    connect(ui->cb_theme, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_theme_gtk3, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon_fallback ,SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->font, SIGNAL(fontSelected(QFont)), this, SLOT(appChanged()));
    connect(ui->cb_toolbar_icons, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->checkBox_icon_gtk_menus, SIGNAL(clicked(bool)), this, SLOT(appChanged()));
    connect(ui->checkBox_icon_gtk_buttons, SIGNAL(clicked(bool)), this, SLOT(appChanged()));

    //preview updates
    connect(ui->cb_icon_fallback, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));
    connect(ui->cb_icon, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));
    connect(ui->previewVersion, SIGNAL(activated(int)), this, SLOT(changePreview(int)));

    //installers connections
    connect(ui->clb_add_theme, SIGNAL(clicked(bool)), this, SLOT(showDialogForInstall()));
    connect(ui->clb_remove_theme, SIGNAL(clicked(bool)), this, SLOT(showDialogForUninstall()));
    
    installer =  new DialogInstaller(this);
    uninstaller = new DialogUninstaller(this, appareance);
    
    connect(installer, SIGNAL(themeInstalled()), SLOT(refreshLists()));
    connect(uninstaller, SIGNAL(themeUninstalled()), SLOT(refreshLists()));
    
    //GHNS connections
    connect(ui->but_theme_ghns, SIGNAL(clicked(bool)), this, SLOT(showThemeGHNS()));
    connect(ui->but_theme_gtk3_ghns, SIGNAL(clicked(bool)), this, SLOT(installThemeGTK3GHNS()));
    
    connect(ui->preview, SIGNAL(clientIsEmbedded()), SLOT(previewOn()));
    connect(ui->preview, SIGNAL(clientClosed()), SLOT(previewOff()));
    
    m_tempGtk2Preview = KGlobal::dirs()->saveLocation("tmp", "gtkrc-2.0", false);
    m_tempGtk3Preview = KGlobal::dirs()->saveLocation("tmp", "/gtk-3.0/settings.ini", false);
    QFile::copy(QDir::homePath()+"/.gtkrc-2.0", m_tempGtk2Preview);
    
    m_p2 = new KProcess(this);
    m_p2->setEnv("GTK2_RC_FILES", m_tempGtk2Preview, true);
    *m_p2 << KStandardDirs::findExe("gtk_preview") << QString::number(ui->preview->winId());
    m_p2->start();
    
    m_p3 = new KProcess(this);
    m_p3->setEnv("GTK_DATA_PREFIX", KGlobal::dirs()->findDirs("tmp", QString()).first());
    *m_p3 << KStandardDirs::findExe("gtk3_preview") << QString::number(ui->preview->winId());
}

GTKConfigKCModule::~GTKConfigKCModule()
{
    m_p2->kill();
    m_p3->kill();
    
    QFile::remove(m_tempGtk2Preview);
    delete ui;
    delete appareance;
    
    m_p2->waitForFinished();
    m_p3->waitForFinished();
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

QFont stringToFont(const QString& font)
{
    QRegExp fontRx(QString(" (italic)? *(bold)? *([0-9]+)$"));
    int pos = fontRx.indexIn(font);
    QString fontFamily = font.left(pos);

    bool italic = !fontRx.cap(1).isEmpty();
    QFont::Weight bold = fontRx.cap(2).isEmpty() ? QFont::Normal : QFont::Bold;
    int fontSize = fontRx.cap(3).toInt();
    
    return QFont(fontFamily, fontSize, bold, italic);
}

QString fontToString(const QFont& f)
{
    QString style;

    if(f.bold())
        style += " bold";
    if(f.italic())
        style += " italic";
    
    return f.family() + style + ' ' + QString::number(f.pointSize());
}

void GTKConfigKCModule::refreshLists()
{
    refreshThemesUi(true);

    QString font = appareance->getFont();
    Q_ASSERT(!font.isEmpty());
    ui->font->setFont(stringToFont(font));
    
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
    
    kDebug() << "could not find icon" << relativePaths;
}

void GTKConfigKCModule::makePreviewIconTheme()
{
    int icon_fallback = ui->cb_icon_fallback->currentIndex();
    QString path_fallback;
    if(icon_fallback>=0)
        path_fallback = appareance->getAvaliableIconsPaths()[icon_fallback];
    
    int icon = ui->cb_icon->currentIndex();
    QString path_icon;
    if(icon>=0)
        path_icon = appareance->getAvaliableIconsPaths()[icon];

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

void GTKConfigKCModule::savePreviewConfig()
{
    appareance->setThemeGtk3(ui->cb_theme_gtk3->currentText());
    appareance->setTheme(ui->cb_theme->currentText());
    appareance->setIcon(ui->cb_icon->currentText());
    appareance->setIconFallBack(ui->cb_icon_fallback->currentText());
    appareance->setFont(fontToString(ui->font->font()));

    appareance->setToolbarStyle(gtkToolbar.key(ui->cb_toolbar_icons->currentIndex()));
    appareance->setShowIconsInButtons(ui->checkBox_icon_gtk_buttons->isChecked());
    appareance->setShowIconsInMenus(ui->checkBox_icon_gtk_menus->isChecked());
    
    appareance->saveGTK2Config(m_tempGtk2Preview);
    appareance->saveGTK3Config(m_tempGtk3Preview);
    
    QTimer::singleShot(500, this, SLOT(refreshPreview()));
}

void GTKConfigKCModule::appChanged()
{
    savePreviewConfig();
    emit changed(true);
}

void GTKConfigKCModule::save()
{
    kDebug() << "******************************************* INSTALLATION :\n"
            << "theme : " << appareance->getTheme() << "\n"
            << "themeGTK3 : " << appareance->getThemeGtk3() << "\n"
            << "icons : " << appareance->getIcon() << "\n"
            << "fallback icons : " << appareance->getIconFallBack() << "\n"
            << "font family : " << appareance->getFont() << "\n"
            << "toolbar style : " << appareance->getToolbarStyle() << "\n"
            << "icons in buttons : " << appareance->getShowIconsInButtons() << "\n"
            << "icons in menus : " << appareance->getShowIconsInMenus() << "\n"
            << "********************************************************";
    
    if(!appareance->saveFileConfig())
        QMessageBox::warning(this, "ERROR", i18n("It was not possible to save the config"));
    else
        KProcess::startDetached(KStandardDirs::findExe("reload_gtk_apps"), QStringList(QString::number(ui-> preview->winId())));
}

void GTKConfigKCModule::refreshPreview()
{
//TODO: sadly after refreshing the theme, the host window 
//      doesn't resize to the needed size
//      **now, the dirty hack dance**

    static bool switchsize=false;
    switchsize=!switchsize;
    ui->preview->resize(ui->preview->size()+(switchsize ? QSize(1,0) : QSize(-1,0)));
}

void GTKConfigKCModule::defaults()
{
    appareance->setFont(fontToString(font()));
    appareance->setTheme("oxygen-gtk"); //TODO: review, should use system's settings, for better integration
    appareance->setIcon("oxygen-refit-2-2.5.0");
    appareance->setIconFallBack("oxygen");
    
    refreshLists();
}

void GTKConfigKCModule::refreshThemesUi(bool useConfig)
{
    //theme gtk2
    QString temp;
    temp = useConfig ? appareance->getTheme() : ui->cb_theme->currentText(); //The currently selected theme
    ui->cb_theme->clear();
    ui->cb_theme->addItems(appareance->getAvaliableThemes());
    ui->cb_theme->setCurrentIndex(ui->cb_theme->findText(temp));
    
    //theme gtk3
    temp = useConfig ? appareance->getThemeGtk3() : ui->cb_theme_gtk3->currentText();
    QStringList themes=appareance->getAvaliableGtk3Themes();
    ui->cb_theme_gtk3->clear();
    ui->cb_theme_gtk3->addItems(themes);
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

void GTKConfigKCModule::previewOn()
{
    kDebug() << "client connected!!";
}

void GTKConfigKCModule::previewOff()
{
    kDebug() << "client disconnected!";
    //TODO: re-run
}

void GTKConfigKCModule::changePreview(int idx)
{
    if(idx==0) {
        m_p3->kill();
        m_p2->start();
    } else {
        m_p3->start();
        m_p2->kill();
    }
}