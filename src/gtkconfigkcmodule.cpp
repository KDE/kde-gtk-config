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
#include <KPluginFactory>
#include <KProcess>
#include <KMessageBox>
#include <KIconTheme>
#include <KLocalizedString>
#include <QStandardPaths>
#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QDir>
#include <config.h>
#include "ui_gui.h"
#include "abstractappearance.h"
#include "iconthemesmodel.h"
#include "fontshelpers.h"
#include <QSortFilterProxyModel>
#include <qstringlistmodel.h>
#include <QSvgRenderer>
#include <QPainter>

K_PLUGIN_FACTORY_WITH_JSON(GTKConfigKCModuleFactory, "kde-gtk-config.json", registerPlugin<GTKConfigKCModule>();)

QMap<QString, int> gtkToolbarInit()
{
    QMap<QString, int> gtkToolbar;
    gtkToolbar["GTK_TOOLBAR_ICONS"] = 0;
    gtkToolbar["GTK_TOOLBAR_TEXT"] = 1;
    gtkToolbar["GTK_TOOLBAR_BOTH_HORIZ"] = 2;
    gtkToolbar["GTK_TOOLBAR_BOTH"] = 3;
    return gtkToolbar;
}

static QMap<QString, int> gtkToolbar = gtkToolbarInit();

GTKConfigKCModule::GTKConfigKCModule(QWidget* parent, const QVariantList& args )
    : KCModule(parent)
    , ui(new Ui::GUI)
    , installer(0)
    , uninstaller(0)
    , m_saveEnabled(true)
{
    Q_UNUSED(args);
    KAboutData *acercade = new KAboutData("cgc", i18n("GTK Application Style"), PROJECT_VERSION,
                    QString(), KAboutLicense::LGPL_V3, i18n("Copyright 2011 José Antonio Sánchez Reynaga"));
    acercade->addAuthor(i18n("José Antonio Sánchez Reynaga (antonioJASR)"),i18n("Main Developer"), "joanzare@gmail.com");
    acercade->addAuthor(i18n("Aleix Pol i Gonzalez"), i18n("Feature development. Previews, code refactoring."), "aleixpol@blue-systems.com");
    acercade->addCredit(i18n("Manuel Tortosa (manutortosa)"), i18n("Ideas, tester, internationalization"));
    acercade->addCredit(i18n("Adrián Chaves Fernández (Gallaecio)"), i18n("Internationalization"));
    setAboutData(acercade);
    setButtons(KCModule::Default | KCModule::Apply);
    ui->setupUi(this);
    appareance = new AppearenceGTK;

    m_cursorsModel = new CursorThemesModel(this);
    QSortFilterProxyModel *cursorsProxyModel = new QSortFilterProxyModel(this);
    cursorsProxyModel->setSourceModel(m_cursorsModel);
    cursorsProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    cursorsProxyModel->setSortRole(Qt::DisplayRole);
    cursorsProxyModel->sort(0);
    ui->cb_cursor->setModel(cursorsProxyModel);

    m_iconsModel = new IconThemesModel(false, this);
    QSortFilterProxyModel *iconsProxyModel = new QSortFilterProxyModel(this);
    iconsProxyModel->setSourceModel(m_iconsModel);
    iconsProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    iconsProxyModel->setSortRole(Qt::DisplayRole);
    iconsProxyModel->sort(0);
    ui->cb_icon->setModel(iconsProxyModel);
    ui->cb_icon_fallback->setModel(iconsProxyModel);
    
    m_tempGtk2Preview = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+ "/gtkrc-2.0";
    m_tempGtk3Preview = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+ "/.config/gtk-3.0/settings.ini";
    
    const QIcon previewIcon = QIcon::fromTheme("document-preview");
    ui->gtk2Preview->setIcon(previewIcon);
    ui->gtk3Preview->setIcon(previewIcon);
    
    QString gtk2Preview = QStandardPaths::findExecutable("gtk_preview",  {CMAKE_INSTALL_FULL_LIBEXECDIR});
    QString gtk3Preview = QStandardPaths::findExecutable("gtk3_preview", {CMAKE_INSTALL_FULL_LIBEXECDIR});
    
    m_p2 = new KProcess(this);
    m_p2->setEnv("GTK2_RC_FILES", m_tempGtk2Preview, true);
    if(!gtk2Preview.isEmpty()) {
        *m_p2 << gtk2Preview;
        connect(m_p2, SIGNAL(finished(int)), this, SLOT(untogglePreview()));
    }
    
    m_p3 = new KProcess(this);
    m_p3->setEnv("XDG_CONFIG_HOME", QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/.config");
    if(!gtk3Preview.isEmpty()) {
        *m_p3 << gtk3Preview;
        connect(m_p3, SIGNAL(finished(int)), this, SLOT(untogglePreview()));
    }
    
    ui->gtk2Preview->setVisible(!gtk2Preview.isEmpty());
    ui->gtk3Preview->setVisible(!gtk3Preview.isEmpty());
    
    //UI changes
    connect(ui->cb_theme, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_theme_gtk3, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->checkBox_theme_gtk3_prefer_dark, &QAbstractButton::clicked, this, &GTKConfigKCModule::appChanged);
    connect(ui->cb_cursor, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon_fallback ,SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->font, &KFontRequester::fontSelected, this, &GTKConfigKCModule::appChanged);
    connect(ui->cb_toolbar_icons, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->checkBox_icon_gtk_menus, &QAbstractButton::clicked, this, &GTKConfigKCModule::appChanged);
    connect(ui->checkBox_icon_gtk_buttons, &QAbstractButton::clicked, this, &GTKConfigKCModule::appChanged);
    connect(ui->buttonGroup_primary_button_warps_slider, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(appChanged()));

    //preview updates
    connect(ui->cb_icon_fallback, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));
    connect(ui->cb_icon, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));
    connect(ui->gtk2Preview, &QAbstractButton::clicked, this, &GTKConfigKCModule::runGtk2IfNecessary);
    connect(ui->gtk3Preview, &QAbstractButton::clicked, this, &GTKConfigKCModule::runGtk3IfNecessary);
    
    QMenu* m = new QMenu(this);
    m->addAction(QIcon::fromTheme("get-hot-new-stuff"), i18n("Download GTK2 themes..."), this, &GTKConfigKCModule::showThemeGHNS);
    m->addAction(QIcon::fromTheme("get-hot-new-stuff"), i18n("Download GTK3 themes..."), this, &GTKConfigKCModule::installThemeGTK3GHNS);
    m->addAction(QIcon::fromTheme("archive-insert"), i18n("Install a local theme..."), this, &GTKConfigKCModule::showDialogForInstall);
    m->addAction(QIcon::fromTheme("archive-remove"), i18n("Uninstall a local theme..."), this, &GTKConfigKCModule::showDialogForUninstall);
    ui->newThemes->setMenu(m);
    ui->newThemes->setIcon(QIcon::fromTheme("get-hot-new-stuff"));
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

void GTKConfigKCModule::syncUI()
{
    appareance->setThemeGtk3(ui->cb_theme_gtk3->currentText());
    appareance->setTheme(ui->cb_theme->currentText());
    appareance->setApplicationPreferDarkTheme(ui->checkBox_theme_gtk3_prefer_dark->isChecked());
    appareance->setCursor(ui->cb_cursor->itemData(ui->cb_cursor->currentIndex(), CursorThemesModel::DirNameRole).toString());
    appareance->setIcon(ui->cb_icon->itemData(ui->cb_icon->currentIndex(), IconThemesModel::DirNameRole).toString());
    appareance->setIconFallback(ui->cb_icon_fallback->itemData(ui->cb_icon_fallback->currentIndex(), IconThemesModel::DirNameRole).toString());
    appareance->setFont(fontToString(ui->font->font()));

    appareance->setToolbarStyle(gtkToolbar.key(ui->cb_toolbar_icons->currentIndex()));
    appareance->setShowIconsInButtons(ui->checkBox_icon_gtk_buttons->isChecked());
    appareance->setShowIconsInMenus(ui->checkBox_icon_gtk_menus->isChecked());
    appareance->setPrimaryButtonWarpsSlider(ui->buttonGroup_primary_button_warps_slider->checkedButton() == ui->radioButton_warp);
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

void GTKConfigKCModule::refreshLists()
{
    refreshThemesUi(true);

    const auto newFont = stringToFont(appareance->getFont());
    if (newFont != ui->font->font())
        ui->font->setFont(newFont);
    
    ui->cb_toolbar_icons->setCurrentIndex(gtkToolbar[appareance->getToolbarStyle()]);
    
    ui->checkBox_icon_gtk_buttons->setChecked(appareance->getShowIconsInButtons());
    ui->checkBox_icon_gtk_menus->setChecked(appareance->getShowIconsInMenus());
    const bool warps = appareance->getPrimaryButtonWarpsSlider();
    ui->radioButton_warp->setChecked(warps);
    ui->radioButton_dont_warp->setChecked(!warps);

    makePreviewIconTheme();
}

void tryIcon(QLabel* label, const QString& fallback, const QString& theme, const QString& iconName)
{
    label->setToolTip(iconName);

    auto findIconAt = [label, theme, iconName](const QDir &where) -> bool {
        const QString path = IconThemesModel::findIconRecursivelyByName(iconName, where);

        if(!path.isEmpty()) {
            QPixmap p;
            QSize s(label->width(), label->height());
            if (path.endsWith(".svg") || path.endsWith(".svgz")) {
                QImage image(s, QImage::Format_ARGB32_Premultiplied);
                image.fill(Qt::transparent);
                QPainter painter(&image);
                QSvgRenderer r(path);
                r.render(&painter);
                painter.end();

                p = QPixmap::fromImage(image);
            } else {
                p = {path};
                Q_ASSERT(!p.isNull());
                p = p.scaled(s);
            }
            label->setPixmap(p);
            return true;
        }
        return false;
    };
    if (!theme.isEmpty() && findIconAt(QDir(theme)))
        return;
    if (findIconAt(fallback))
        return;
    
    QIcon notFoundIcon = QIcon::fromTheme("application-x-zerosize");
    QPixmap noIcon(notFoundIcon.pixmap(48,48));
    label->setPixmap(noIcon);
    
    qWarning() << "could not find icon" << iconName;
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
    if (m_loading)
        return;

    savePreviewConfig();
    emit changed(true);
}


void GTKConfigKCModule::savePreviewConfig()
{
    if(!m_saveEnabled || !(ui->gtk2Preview->isChecked() || ui->gtk3Preview->isChecked()))
        return;
//     qDebug() << "saving UI...";
    
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
    } else if(ui->gtk2Preview->isChecked()) {
        appareance->gtk2Appearance()->saveSettings(m_tempGtk2Preview);
    }
}

void GTKConfigKCModule::runGtk2IfNecessary(bool checked)
{
    KProcess* p = m_p2;
    KProcess* np = m_p3;
    
    if(checked) {
        np->kill();
        np->waitForFinished();
        savePreviewConfig();
        if(p->state() == QProcess::NotRunning)
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
        if(p->state() == QProcess::NotRunning)
            p->start();
    } else {
        p->kill();
        p->waitForFinished();
    }
}

void GTKConfigKCModule::save()
{
/*    qDebug() << "******************************************* INSTALLATION :\n"
            << "theme : " << appareance->getTheme() << "\n"
            << "themeGTK3 : " << appareance->getThemeGtk3() << "\n"
            << "icons : " << appareance->getIcon() << "\n"
            << "fallback icons : " << appareance->getIconFallback() << "\n"
            << "cursors : " << appareance->getCursor() << "\n"
            << "font family : " << appareance->getFont() << "\n"
            << "toolbar style : " << appareance->getToolbarStyle() << "\n"
            << "icons in buttons : " << appareance->getShowIconsInButtons() << "\n"
            << "icons in menus : " << appareance->getShowIconsInMenus() << "\n"
            << "********************************************************";
    */
    syncUI();
    if(!appareance->saveFileConfig())
        KMessageBox::error(this, i18n("Failed to save configuration."));
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
    refreshThemesUi(false);

//     qDebug() << "loading defaults...";
    m_saveEnabled = false;
    ui->font->setFont(font());
    bool showIcons = !QCoreApplication::testAttribute(Qt::AA_DontShowIconsInMenus);
    ui->checkBox_icon_gtk_buttons->setChecked(showIcons);
    ui->checkBox_icon_gtk_menus->setChecked(showIcons);

    // this makes it consistent with Qt apps and restores the old Gtk behavior
    ui->radioButton_dont_warp->setChecked(true);
    
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
    bool someCorrect = appareance->loadFileConfig();
    m_loading = true;
    if(someCorrect) {
        refreshLists();
    } else {
        defaults();
    }
    m_loading = false;
    
    m_saveEnabled = true;
}

class MyStringListModel : public QAbstractListModel
{
public:
    MyStringListModel(const QStringList &texts, QObject* parent) : QAbstractListModel(parent), m_texts(texts)
    {
    }

    QVariant data(const QModelIndex & index, int role) const override
    {
        if (role != Qt::DisplayRole || !index.isValid() || index.row()>=m_texts.count())
            return {};

        return m_texts[index.row()];
    }

    int rowCount(const QModelIndex & parent) const override { return parent.isValid() ? 0 : m_texts.count(); }

    void setStrings(const QSet<QString> &list) {
        const auto current = m_texts.toSet();

        const auto oldRows = QSet<QString>(current).subtract(list);
        const auto newRows = QSet<QString>(list).subtract(current);
        if (!newRows.isEmpty()) {
            beginInsertRows({}, m_texts.count(), m_texts.count() + newRows.count());
            m_texts += newRows.toList();
            endInsertRows();
        }

        int from = -1;
        for(const auto &row: oldRows) {
            for(; from<m_texts.count();) {
                const auto idx = m_texts.indexOf(row, from);
                if (idx<0)
                    break;
                beginRemoveRows({}, idx, idx);
                m_texts.removeAt(idx);
                endRemoveRows();
                from = idx + 1;
            }
        }
    }

private:
    QStringList m_texts;
};

void refreshComboSameCurrentValue(QComboBox* combo, const QString& temp, const QStringList& texts)
{
    const auto model = dynamic_cast<MyStringListModel*>(combo->model());
    if (!model) {
        combo->setModel(new MyStringListModel(texts, combo));
    } else {
        model->setStrings(texts.toSet());
    }

    const int idx = combo->findText(temp);
    combo->setCurrentIndex(qMax(0, idx));
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

    // dark theme for gtk3
    ui->checkBox_theme_gtk3_prefer_dark->setChecked(appareance->getApplicationPreferDarkTheme());

    //cursors
    QString currentCursor = useConfig ? appareance->getCursor() : ui->cb_cursor->currentText();
    int currentCursorIdx = ui->cb_cursor->findData(currentCursor, CursorThemesModel::DirNameRole);
    ui->cb_cursor->setCurrentIndex(qMax(currentCursorIdx, 0));
    
    //icons
    QString currentIcon = useConfig ? appareance->getIcon() : ui->cb_icon->currentText(),
            currentFallback = useConfig ? appareance->getIconFallback() : ui->cb_icon_fallback->currentText();
    int currentIconIdx = ui->cb_icon->findData(currentIcon, IconThemesModel::DirNameRole);
    int currentFallbackIdx = ui->cb_icon_fallback->findData(currentFallback, IconThemesModel::DirNameRole);
    ui->cb_icon->setCurrentIndex(qMax(currentIconIdx, 0));
    ui->cb_icon_fallback->setCurrentIndex(qMax(currentFallbackIdx, 0));
    
    m_saveEnabled = wasenabled;
    if(currentCursorIdx<0 || currentIconIdx<0 || currentFallbackIdx<0)
        emit changed(true);
}

void GTKConfigKCModule::showDialogForInstall()
{
    if(!installer) {
        installer =  new DialogInstaller(this);
        connect(installer, &DialogInstaller::themeInstalled, this, &GTKConfigKCModule::refreshLists);
    }
    
    installer->exec();
    refreshThemesUi();
}

void GTKConfigKCModule::showDialogForUninstall()
{
    if(!uninstaller) {
        uninstaller = new DialogUninstaller(this, appareance);
        connect(uninstaller, &DialogUninstaller::themeUninstalled, this, &GTKConfigKCModule::refreshLists);
    }
    
    uninstaller->refreshListsForUninstall();
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

#include "gtkconfigkcmodule.moc"
