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

#include <QStandardPaths>
#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QDir>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QSvgRenderer>
#include <QPainter>

#include <KAboutData>
#include <KPluginFactory>
#include <KProcess>
#include <KMessageBox>
#include <KIconTheme>
#include <KLocalizedString>

#include "config.h"
#include "ui_gui.h"
#include "abstractappearance.h"
#include "gtkconfigkcmodule.h"

K_PLUGIN_FACTORY_WITH_JSON(GTKConfigKCModuleFactory, "kde-gtk-config.json", registerPlugin<GTKConfigKCModule>();)

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

    m_tempGtk2Preview = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/gtkrc-2.0";
    m_tempGtk3Preview = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/.config/gtk-3.0/settings.ini";
    
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
    
    // UI changes
    connect(ui->cb_theme, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_theme_gtk3, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->checkBox_theme_gtk3_prefer_dark, &QAbstractButton::clicked, this, &GTKConfigKCModule::appChanged);

    // Preview updates
    connect(ui->gtk2Preview, &QAbstractButton::clicked, this, &GTKConfigKCModule::runGtk2IfNecessary);
    connect(ui->gtk3Preview, &QAbstractButton::clicked, this, &GTKConfigKCModule::runGtk3IfNecessary);
    
    QMenu *m = new QMenu(this);
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
}

void GTKConfigKCModule::appChanged()
{
    if (m_loading) {
        return;
    }

    savePreviewConfig();
    emit changed(true);
}


void GTKConfigKCModule::savePreviewConfig()
{
    if(!m_saveEnabled || !(ui->gtk2Preview->isChecked() || ui->gtk3Preview->isChecked())) {
        return;
    }
    
    syncUI();
    
    if(ui->gtk3Preview->isChecked()) {
        // We don't want to recursively loop between savePreviewConfig and runIfNecessary
        m_saveEnabled = false;
        m_p3->kill();
        appareance->gtk3Appearance()->saveSettings(m_tempGtk3Preview);
        
        // Need to make sure runIfNecessary() to know that it's not running
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
    
    if (checked) {
        np->kill();
        np->waitForFinished();
        savePreviewConfig();
        if(p->state() == QProcess::NotRunning) {
            p->start();
        }
    } else {
        p->kill();
        p->waitForFinished();
    }
}

void GTKConfigKCModule::runGtk3IfNecessary(bool checked)
{
    KProcess* p = m_p3;
    KProcess* np = m_p2;
    
    if (checked) {
        np->kill();
        np->waitForFinished();
        savePreviewConfig();
        if(p->state() == QProcess::NotRunning) {
            p->start();
        }
    } else {
        p->kill();
        p->waitForFinished();
    }
}

void GTKConfigKCModule::save()
{
    syncUI();
    if(!appareance->saveFileConfig()) {
        KMessageBox::error(this, i18n("Failed to save configuration."));
    }
}

void setComboItem(QComboBox* combo, const QStringList& texts)
{
    for (const QString &text : texts) {
        int pos = combo->findText(text);
        if(pos >= 0) {
            combo->setCurrentIndex(pos);
            return;
        }
    }
}

void GTKConfigKCModule::defaults()
{
    refreshThemesUi(false);

    m_saveEnabled = false;
    
    setComboItem(ui->cb_theme, QStringList("oxygen-gtk") << "Clearlooks");
    setComboItem(ui->cb_theme_gtk3, QStringList("oxygen-gtk") << "Adwaita");
    
    m_saveEnabled = true;
    
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
        if (role != Qt::DisplayRole || !index.isValid() || index.row()>=m_texts.count()) {
            return {};
        }

        return m_texts[index.row()];
    }

    int rowCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : m_texts.count();
    }

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
        for (const auto &row: oldRows) {
            for(; from < m_texts.count();) {
                const auto idx = m_texts.indexOf(row, from);
                if (idx < 0) {
                    break;
                }
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
    // Theme gtk2
    bool wasenabled = m_saveEnabled;
    m_saveEnabled = false;
    
    refreshComboSameCurrentValue(ui->cb_theme,
        useConfig ? appareance->getTheme() : ui->cb_theme->currentText(),
        appareance->gtk2Appearance()->installedThemesNames());
    
    // Theme gtk3
    refreshComboSameCurrentValue(ui->cb_theme_gtk3,
        useConfig ? appareance->getThemeGtk3() : ui->cb_theme_gtk3->currentText(),
        appareance->gtk3Appearance()->installedThemesNames());

    // Dark theme for gtk3
    ui->checkBox_theme_gtk3_prefer_dark->setChecked(appareance->getApplicationPreferDarkTheme());

    
    m_saveEnabled = wasenabled;
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
    if(sender() == m_p2) {
        ui->gtk2Preview->setChecked(false);
    } else {
        ui->gtk3Preview->setChecked(false);
    }
}

#include "gtkconfigkcmodule.moc"
