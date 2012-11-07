/* KDE GTK Configuration Module
 * 
 * Copyright 2011 José Antonio Sanchez Reynaga <joanzare@gmail.com>
 * Copyright 2011 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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
#include "abstractappearance.h"
#include "iconthemesmodel.h"
#include <kicontheme.h>

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
    , installer(0)
    , uninstaller(0)
    , m_saveEnabled(true)
{
    Q_UNUSED(args);
    KAboutData *acercade = new KAboutData("cgc","kcm_cgc",ki18n("KDE GTK Config"), "2.0",
                    ki18n("Configure your GTK Applications"),
                    KAboutData::License_LGPL_V3,
                    ki18n("Copyright 2011 José Antonio Sánchez Reynaga"));
    acercade->addAuthor(ki18n("José Antonio Sánchez Reynaga (antonioJASR)"),ki18n("Main Developer"), "joanzare@gmail.com");
    acercade->addAuthor(ki18n("Aleix Pol i Gonzalez"), ki18n("Feature development. Previews, code refactoring."), "aleixpol@blue-systems.com");
    acercade->addCredit(ki18n("Manuel Tortosa (manutortosa)"), ki18n("Ideas, tester, internationalization"));
    acercade->addCredit(ki18n("Adrián Chaves Fernández (Gallaecio)"), ki18n("Internationalization"));
    setAboutData(acercade);
    
    ui->setupUi(this);
    appareance = new AppearenceGTK;
    m_iconsModel = new IconThemesModel(false, this);
    ui->cb_icon->setModel(m_iconsModel);
    ui->cb_icon_fallback->setModel(m_iconsModel);
    
    m_tempGtk2Preview = KGlobal::dirs()->saveLocation("tmp", "gtkrc-2.0", false);
    m_tempGtk3Preview = KGlobal::dirs()->saveLocation("tmp", ".config/gtk-3.0/settings.ini", false);
    
    const KIcon previewIcon("document-preview");
    ui->gtk2Preview->setIcon(previewIcon);
    ui->gtk3Preview->setIcon(previewIcon);
    
    m_p2 = new KProcess(this);
    m_p2->setEnv("GTK2_RC_FILES", m_tempGtk2Preview, true);
    *m_p2 << KStandardDirs::findExe("gtk_preview");
    
    m_p3 = new KProcess(this);
    m_p3->setEnv("XDG_CONFIG_HOME", KGlobal::dirs()->saveLocation("tmp", ".config"));
    *m_p3 << KStandardDirs::findExe("gtk3_preview");
    
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
    connect(ui->gtk2Preview, SIGNAL(clicked(bool)), this, SLOT(runGtk2IfNecessary(bool)));
    connect(ui->gtk3Preview, SIGNAL(clicked(bool)), this, SLOT(runGtk3IfNecessary(bool)));
    
    connect(m_p2, SIGNAL(finished(int)), this, SLOT(untogglePreview()));
    connect(m_p3, SIGNAL(finished(int)), this, SLOT(untogglePreview()));
    
    QMenu* m = new QMenu(this);
    m->addAction(KIcon("get-hot-new-stuff"), i18n("Download GTK2 themes..."), this, SLOT(showThemeGHNS()));
    m->addAction(KIcon("get-hot-new-stuff"), i18n("Download GTK3 themes..."), this, SLOT(installThemeGTK3GHNS()));
    m->addAction(KIcon("archive-insert"), i18n("Install a local theme..."), this, SLOT(showDialogForInstall()));
    m->addAction(KIcon("archive-remove"), i18n("Uninstall a local theme..."), this, SLOT(showDialogForUninstall()));
    ui->newThemes->setMenu(m);
    ui->newThemes->setIcon(KIcon("download"));
}

GTKConfigKCModule::~GTKConfigKCModule()
{
    m_p2->kill();
    m_p3->kill();
    
    QFile::remove(m_tempGtk2Preview);
    QFile::remove(m_tempGtk3Preview);
    delete appareance;
    
    m_p2->waitForFinished();
    m_p3->waitForFinished();
    delete ui;
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

void GTKConfigKCModule::syncUI()
{
    appareance->setThemeGtk3(ui->cb_theme_gtk3->currentText());
    appareance->setTheme(ui->cb_theme->currentText());
    appareance->setIcon(ui->cb_icon->itemData(ui->cb_icon->currentIndex(), IconThemesModel::DirNameRole).toString());
    appareance->setIconFallback(ui->cb_icon_fallback->itemData(ui->cb_icon_fallback->currentIndex(), IconThemesModel::DirNameRole).toString());
    appareance->setFont(fontToString(ui->font->font()));

    appareance->setToolbarStyle(gtkToolbar.key(ui->cb_toolbar_icons->currentIndex()));
    appareance->setShowIconsInButtons(ui->checkBox_icon_gtk_buttons->isChecked());
    appareance->setShowIconsInMenus(ui->checkBox_icon_gtk_menus->isChecked());
}

void GTKConfigKCModule::showThemeGHNS()
{
     KNS3::DownloadDialog d("cgctheme.knsrc", this);
     if(d.exec()) {
          refreshLists();
     }
}

void GTKConfigKCModule::installThemeGTK3GHNS()
{
     KNS3::DownloadDialog d("cgcgtk3.knsrc", this);
     if(d.exec()) {
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

void GTKConfigKCModule::refreshLists()
{
    refreshThemesUi(true);

    QString font = appareance->getFont();
//     Q_ASSERT(!font.isEmpty());
    ui->font->setFont(stringToFont(font));
    
    ui->cb_toolbar_icons->setCurrentIndex(gtkToolbar[appareance->getToolbarStyle()]);
    
    ui->checkBox_icon_gtk_buttons->setChecked(appareance->getShowIconsInButtons());
    ui->checkBox_icon_gtk_menus->setChecked(appareance->getShowIconsInMenus());
}

void tryIcon(QLabel* label, const QString& fallback, const QString& theme, const QString& iconName)
{
    label->setToolTip(iconName);
    
    QString ret;
    if(!theme.isEmpty())
        ret = IconThemesModel::findIconRecursivelyByName(iconName, theme);
    
    if(!ret.isEmpty()) {
        QPixmap p(ret);
        Q_ASSERT(!p.isNull());
        label->setPixmap(p);
        return;
    }
    
    if(!fallback.isEmpty())
        ret = IconThemesModel::findIconRecursivelyByName(iconName, fallback);
    
    if(!ret.isEmpty()) {
        QPixmap p(ret);
        Q_ASSERT(!p.isNull());
        label->setPixmap(p);
        return;
    }
    
    KIcon notFoundIcon("application-x-zerosize");
    QPixmap noIcon(notFoundIcon.pixmap(48,48));
    label->setPixmap(noIcon);
    
    kDebug() << "could not find icon" << iconName;
}

void GTKConfigKCModule::makePreviewIconTheme()
{
    int icon_fallback = ui->cb_icon_fallback->currentIndex();
    QString path_fallback = ui->cb_icon->itemData(icon_fallback, IconThemesModel::PathRole).toString();
    
    int icon = ui->cb_icon->currentIndex();
    QString path_icon = ui->cb_icon->itemData(icon, IconThemesModel::PathRole).toString();
    
    tryIcon(ui->lb_prev_1, path_fallback, path_icon, "user-home");
    tryIcon(ui->lb_prev_2, path_fallback, path_icon, "folder");
    tryIcon(ui->lb_prev_3, path_fallback, path_icon, "user-trash");
    tryIcon(ui->lb_prev_4, path_fallback, path_icon, "document-print");
    tryIcon(ui->lb_prev_5, path_fallback, path_icon, "user-desktop");
    tryIcon(ui->lb_prev_6, path_fallback, path_icon, "network-server");
    tryIcon(ui->lb_prev_7, path_fallback, path_icon, "system-help");
    tryIcon(ui->lb_prev_8, path_fallback, path_icon, "start-here");
    tryIcon(ui->lb_prev_9, path_fallback, path_icon, "go-up");
}

void GTKConfigKCModule::appChanged()
{
    savePreviewConfig();
    emit changed(true);
}


void GTKConfigKCModule::savePreviewConfig()
{
    if(!m_saveEnabled || !(ui->gtk2Preview->isChecked() || ui->gtk3Preview->isChecked()))
        return;
    kDebug() << "saving UI...";
    
    syncUI();
    
    if(ui->gtk3Preview->isChecked()) {
        //we don't want to recursively loop between savePreviewConfig and runIfNecessary
        m_saveEnabled = false;
        m_p3->kill();
        appareance->gtk3Appearance()->saveSettings(m_tempGtk3Preview);
        
        //need to make sure runIfNecessary() to know that it's not running
        m_p3->waitForFinished();
        
        m_p3->start();
        ui->gtk3Preview->setChecked(true);
        m_saveEnabled = true;
    } else if(ui->gtk2Preview->isChecked())
        appareance->gtk2Appearance()->saveSettings(m_tempGtk2Preview);
}

void GTKConfigKCModule::runGtk2IfNecessary(bool checked)
{
    KProcess* p = m_p2;
    KProcess* np = m_p3;
    
    if(checked) {
        np->kill();
        np->waitForFinished();
        savePreviewConfig();
        if(p->state()!=KProcess::Running)
            p->start();
    } else {
        p->kill();
        p->waitForFinished();
    }
}

void GTKConfigKCModule::runGtk3IfNecessary(bool checked)
{
    KProcess* p = m_p3;
    KProcess* np = m_p2;
    
    if(checked) {
        np->kill();
        np->waitForFinished();
        savePreviewConfig();
        if(p->state()!=KProcess::Running)
            p->start();
    } else {
        p->kill();
        p->waitForFinished();
    }
}

void GTKConfigKCModule::save()
{
    kDebug() << "******************************************* INSTALLATION :\n"
            << "theme : " << appareance->getTheme() << "\n"
            << "themeGTK3 : " << appareance->getThemeGtk3() << "\n"
            << "icons : " << appareance->getIcon() << "\n"
            << "fallback icons : " << appareance->getIconFallback() << "\n"
            << "font family : " << appareance->getFont() << "\n"
            << "toolbar style : " << appareance->getToolbarStyle() << "\n"
            << "icons in buttons : " << appareance->getShowIconsInButtons() << "\n"
            << "icons in menus : " << appareance->getShowIconsInMenus() << "\n"
            << "********************************************************";
    syncUI();
    if(!appareance->saveFileConfig())
        QMessageBox::warning(this, "ERROR", i18n("It was not possible to save the config"));
}

void setComboItem(QComboBox* combo, const QStringList& texts)
{
    foreach(const QString& text, texts) {
        int pos = combo->findText(text);
        if(pos>=0) {
            combo->setCurrentIndex(pos);
            return;
        }
    }
}

void GTKConfigKCModule::defaults()
{
    kDebug() << "loading defaults...";
    m_saveEnabled = false;
    ui->font->setFont(font());
    bool showIcons = !QCoreApplication::testAttribute(Qt::AA_DontShowIconsInMenus);
    ui->checkBox_icon_gtk_buttons->setChecked(showIcons);
    ui->checkBox_icon_gtk_menus->setChecked(showIcons);
    
    setComboItem(ui->cb_theme, QStringList("oxygen-gtk") << "Clearlooks");
    setComboItem(ui->cb_theme_gtk3, QStringList("oxygen-gtk") << "Adwaita");
    
    QStringList icons;
    icons << KIconLoader::global()->theme()->name() << "GNOME";
    setComboItem(ui->cb_icon, icons);
    
    int idx = ui->cb_icon->currentIndex();
    if(idx>=0) {
        setComboItem(ui->cb_icon_fallback, icons.mid(icons.indexOf(ui->cb_icon->currentText())+1));
    }
    m_saveEnabled = true;
    
    makePreviewIconTheme();
    appChanged();
}

void GTKConfigKCModule::load()
{
    m_saveEnabled = false;
    refreshThemesUi();
    
    bool someCorrect = appareance->loadFileConfig();
    if(someCorrect) {
        refreshLists();
        makePreviewIconTheme();
    } else
        defaults();
    
    m_saveEnabled = true;
}

void refreshComboSameCurrentValue(QComboBox* combo, const QString& temp, const QStringList& texts)
{
    combo->clear();
    combo->addItems(texts);
    int idx = combo->findText(temp);
    if(idx>=0)
        combo->setCurrentIndex(idx);
}

void GTKConfigKCModule::refreshThemesUi(bool useConfig)
{
    //theme gtk2
    bool wasenabled = m_saveEnabled;
    m_saveEnabled = false;
    
    refreshComboSameCurrentValue(ui->cb_theme,
        useConfig ? appareance->getTheme() : ui->cb_theme->currentText(),
        appareance->gtk2Appearance()->installedThemesNames());
    
    //theme gtk3
    refreshComboSameCurrentValue(ui->cb_theme_gtk3,
        useConfig ? appareance->getThemeGtk3() : ui->cb_theme_gtk3->currentText(),
        appareance->gtk3Appearance()->installedThemesNames());
    
    //icons
    QString currentIcon = useConfig ? appareance->getIcon() : ui->cb_icon->currentText(),
            currentFallback = useConfig ? appareance->getIconFallback() : ui->cb_icon_fallback->currentText();
    int currentIconIdx = ui->cb_icon->findData(currentIcon, IconThemesModel::DirNameRole);
    int currentFallbackIdx = ui->cb_icon_fallback->findData(currentFallback, IconThemesModel::DirNameRole);
    ui->cb_icon->setCurrentIndex(qMax(currentIconIdx, 0));
    ui->cb_icon_fallback->setCurrentIndex(qMax(currentFallbackIdx, 0));
    
    m_saveEnabled = wasenabled;
    if(currentIconIdx<0 || currentFallbackIdx<0)
        emit changed(true);
}

void GTKConfigKCModule::showDialogForInstall()
{
    if(!installer) {
        installer =  new DialogInstaller(this);
        connect(installer, SIGNAL(themeInstalled()), SLOT(refreshLists()));
    }
    
    installer->exec();
    refreshThemesUi();
}

void GTKConfigKCModule::showDialogForUninstall()
{
    if(!uninstaller) {
        uninstaller = new DialogUninstaller(this, appareance);
        connect(uninstaller, SIGNAL(themeUninstalled()), SLOT(refreshLists()));
    }
    
    uninstaller->refresthListsForUninstall();
    uninstaller->exec();
    
    refreshThemesUi();
}

void GTKConfigKCModule::untogglePreview()
{
    if(sender()==m_p2)
        ui->gtk2Preview->setChecked(false);
    else
        ui->gtk3Preview->setChecked(false);
}
