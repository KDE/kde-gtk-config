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

#include <QMenu>
#include <QDir>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QAbstractButton>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

#include <KAboutData>
#include <KPluginFactory>
#include <DownloadDialog>
#include <KLocalizedString>
#include <KTar>
#include <KIO/DeleteJob>

#include "config.h"
#include "ui_gui.h"
#include "gtkconfigkcmodule.h"

K_PLUGIN_FACTORY_WITH_JSON(GTKConfigKCModuleFactory, "kde-gtk-config.json", registerPlugin<GTKConfigKCModule>();)

GTKConfigKCModule::GTKConfigKCModule(QWidget* parent, const QVariantList& args )
    : KCModule(parent)
    , currentGtk2Theme()
    , currentGtk3Theme()
    , gtk2ThemesModel()
    , gtk3ThemesModel()
    , gtkConfigInterface(
        QStringLiteral("org.kde.GtkConfig"),
        QStringLiteral("/GtkConfig"),
        QStringLiteral("org.kde.GtkConfig")
    )
    , ui(new Ui::GUI)
{
    Q_UNUSED(args)
    auto *about = new KAboutData("cgc", i18n("GTK Application Style"), PROJECT_VERSION,
                    QString(), KAboutLicense::LGPL_V3, i18n("Copyright 2011 José Antonio Sánchez Reynaga"));
    about->addAuthor(i18n("José Antonio Sánchez Reynaga (antonioJASR)"),i18n("Main Developer"), "joanzare@gmail.com");
    about->addAuthor(i18n("Aleix Pol i Gonzalez"), i18n("Feature development. Previews, code refactoring."), "aleixpol@blue-systems.com");
    about->addCredit(i18n("Manuel Tortosa (manutortosa)"), i18n("Ideas, tester, internationalization"));
    about->addCredit(i18n("Adrián Chaves Fernández (Gallaecio)"), i18n("Internationalization"));
    setAboutData(about);
    setButtons(KCModule::Default | KCModule::Apply);
    ui->setupUi(this);

    ui->cb_theme->setModel(&gtk2ThemesModel);
    ui->cb_theme_gtk3->setModel(&gtk3ThemesModel);

    connect(ui->cb_theme, SIGNAL(currentTextChanged(const QString &)), this, SLOT(themesSelectionsChanged()));
    connect(ui->cb_theme_gtk3, SIGNAL(currentTextChanged(const QString &)), this, SLOT(themesSelectionsChanged()));

    connect(ui->removeGtk2Theme, &QAbstractButton::clicked, this, &GTKConfigKCModule::removeGtk2Theme);
    connect(ui->removeGtk3Theme, &QAbstractButton::clicked, this, &GTKConfigKCModule::removeGtk3Theme);
    
    connect(ui->gtk2Preview, &QAbstractButton::clicked, this, &GTKConfigKCModule::showGtk2Preview);
    connect(ui->gtk3Preview, &QAbstractButton::clicked, this, &GTKConfigKCModule::showGtk3Preview);
    
    connect(ui->installFromFile, &QAbstractButton::clicked, this, &GTKConfigKCModule::installGtkThemeFromFile);

    auto *ghnsMenu = new QMenu(this);
    ghnsMenu->addAction(QIcon::fromTheme("get-hot-new-stuff"), i18n("Get New GNOME/GTK2 Application Styles..."), this, &GTKConfigKCModule::installGtk2ThemeFromGHNS);
    ghnsMenu->addAction(QIcon::fromTheme("get-hot-new-stuff"), i18n("Get New GNOME/GTK3 Application Styles..."), this, &GTKConfigKCModule::installGtk3ThemeFromGHNS);
    ui->newThemes->setMenu(ghnsMenu);
    ui->newThemes->setIcon(QIcon::fromTheme("get-hot-new-stuff"));
}

GTKConfigKCModule::~GTKConfigKCModule()
{
    delete ui;
}

void GTKConfigKCModule::installGtk2ThemeFromGHNS()
{
    KNS3::DownloadDialog downloadDialog("cgctheme.knsrc", this);
    if (downloadDialog.exec()) {
        updateThemesListsWithoutLoosingSelection();
    }
}

void GTKConfigKCModule::installGtk3ThemeFromGHNS()
{
    KNS3::DownloadDialog downloadDialog("cgcgtk3.knsrc", this);
    if (downloadDialog.exec()) {
        updateThemesListsWithoutLoosingSelection();
    }
}

void GTKConfigKCModule::installGtkThemeFromFile()
{
    QString themeArchivePath = QFileDialog::getOpenFileName(
        this,
        i18n("Select GTK Theme Archive"),
        QDir::home().path(),
        i18n("GTK Theme Archive (*.tar.xz *.tar.gz *.tar.bz2)")
    );

    if (themeArchivePath.isEmpty()) {
        return;
    }

    QString themesInstallDirectoryPath(QDir::homePath() + QStringLiteral("/.themes"));
    QDir::home().mkpath(themesInstallDirectoryPath);
    KTar themeArchive(themeArchivePath);
    themeArchive.open(QIODevice::ReadOnly);

    QStringList archiveSubitems = themeArchive.directory()->entries();
    if (!archiveSubitems.contains(QStringLiteral("gtk-2.0")) && archiveSubitems.indexOf(QRegExp("gtk-3.*")) == -1) {
        QMessageBox::warning(this, i18n("Invalid GTK Theme archive"), i18n("%1 is not a valid GTK Theme archive.", QDir(themeArchivePath).dirName()));
        return;
    }

    themeArchive.directory()->copyTo(themesInstallDirectoryPath);

    updateThemesListsWithoutLoosingSelection();
}

void GTKConfigKCModule::removeGtk2Theme()
{
    const QModelIndex index = gtk2ThemesModel.index(ui->cb_theme->currentIndex(), 1);
    const QString themePath = gtk2ThemesModel.data(index).value<QString>();
    KIO::DeleteJob* deleteJob = KIO::del(QUrl::fromLocalFile(themePath), KIO::HideProgressInfo);
    connect(deleteJob, &KJob::finished, this, &GTKConfigKCModule::updateThemesListsWithoutLoosingSelection);
}

void GTKConfigKCModule::removeGtk3Theme()
{
    const QModelIndex index = gtk3ThemesModel.index(ui->cb_theme_gtk3->currentIndex(), 1);
    const QString themePath = gtk3ThemesModel.data(index).value<QString>();
    KIO::del(QUrl::fromLocalFile(themePath), KIO::HideProgressInfo);
    KIO::DeleteJob* deleteJob = KIO::del(QUrl::fromLocalFile(themePath), KIO::HideProgressInfo);
    connect(deleteJob, &KJob::finished, this, &GTKConfigKCModule::updateThemesListsWithoutLoosingSelection);
}

void GTKConfigKCModule::showGtk2Preview()
{
    gtkConfigInterface.call(QStringLiteral("showGtk2ThemePreview"), ui->cb_theme->currentText());
}

void GTKConfigKCModule::showGtk3Preview()
{
    gtkConfigInterface.call(QStringLiteral("showGtk3ThemePreview"), ui->cb_theme_gtk3->currentText());
}

void GTKConfigKCModule::save()
{
    saveGtk2Theme();
    saveGtk3Theme();
}

void GTKConfigKCModule::defaults()
{
    int gtk2DefaultThemeIndex = ui->cb_theme->findText(QStringLiteral("Breeze"));
    ui->cb_theme->setCurrentIndex(gtk2DefaultThemeIndex);

    int gtk3DefaultThemeIndex = ui->cb_theme_gtk3->findText(QStringLiteral("Breeze"));
    ui->cb_theme_gtk3->setCurrentIndex(gtk3DefaultThemeIndex);
}

void GTKConfigKCModule::load()
{
    loadGtkThemes();

    selectCurrentGtk2ThemeInCheckbox();
    selectCurrentGtk3ThemeInCheckbox();
}

void GTKConfigKCModule::updateThemesListsWithoutLoosingSelection()
{
    QString currentGtk2ThemeChoice = ui->cb_theme->currentText();
    QString currentGtk3ThemeChoice = ui->cb_theme_gtk3->currentText();

    loadGtkThemes();

    // Restore correct gtk configuration, if the theme that was being used is deleted
    if (!gtk2ThemesModel.containsTheme(currentGtk2Theme)) {
        ui->cb_theme->setCurrentText(QStringLiteral("Breeze"));
        saveGtk2Theme();
    }

    if (!gtk3ThemesModel.containsTheme(currentGtk3Theme)) {
        ui->cb_theme_gtk3->setCurrentText(QStringLiteral("Breeze"));
        saveGtk3Theme();
    }

    // Restore choices, that was before possible themes deletion
    if (gtk2ThemesModel.containsTheme(currentGtk2ThemeChoice)) {
        ui->cb_theme->setCurrentText(currentGtk2ThemeChoice);
    } else {
        ui->cb_theme->setCurrentText(QStringLiteral("Breeze"));
    }

    if (gtk3ThemesModel.containsTheme(currentGtk3ThemeChoice)) {
        ui->cb_theme_gtk3->setCurrentText(currentGtk3ThemeChoice);
    } else {
        ui->cb_theme_gtk3->setCurrentText(QStringLiteral("Breeze"));
    }
}

void GTKConfigKCModule::themesSelectionsChanged()
{
    if (ui->cb_theme->currentText() == currentGtk2Theme &&
        ui->cb_theme_gtk3->currentText() == currentGtk3Theme) {
        emit changed(false);
    } else {
        markAsChanged();
    }
    updateDeletionPossibilityForSelectedGtk2Theme();
    updateDeletionPossibilityForSelectedGtk3Theme();
}

void GTKConfigKCModule::saveGtk2Theme()
{
    currentGtk2Theme = ui->cb_theme->currentText();
    gtkConfigInterface.call(QStringLiteral("setGtk2Theme"), currentGtk2Theme);
}

void GTKConfigKCModule::saveGtk3Theme()
{
    currentGtk3Theme = ui->cb_theme_gtk3->currentText();
    gtkConfigInterface.call(QStringLiteral("setGtk3Theme"), currentGtk3Theme);
}

void GTKConfigKCModule::loadGtkThemes()
{
    QStringList possibleThemesPaths = possiblePathsToGtkThemes();
    loadGtk2Themes(possibleThemesPaths);
    loadGtk3Themes(possibleThemesPaths);
}

void GTKConfigKCModule::loadGtk2Themes(const QStringList &possibleThemesPaths)
{
    QMap<QString, QString> gtk2ThemesNames;

    for (const QString &possibleThemePath : possibleThemesPaths) {
        // If the directory has a gtk-2.0 directory inside, it is the GTK2 theme for sure
        QDir possibleThemeDirectory(possibleThemePath);
        bool hasGtk2DirectoryInside = possibleThemeDirectory.exists(QStringLiteral("gtk-2.0"));
        if (hasGtk2DirectoryInside) {
            gtk2ThemesNames.insert(possibleThemeDirectory.dirName(), possibleThemeDirectory.path());
        }
    }

    gtk2ThemesModel.setThemesList(gtk2ThemesNames);
}

void GTKConfigKCModule::loadGtk3Themes(const QStringList &possibleThemesPaths)
{
    QMap<QString, QString> gtk3ThemesNames;

    static const QStringList gtk3SubdirPattern(QStringLiteral("gtk-3.*"));
    for (const QString &possibleThemePath : possibleThemesPaths) {
        // If the directory contains any of gtk-3.X folders, it is the GTK3 theme for sure
        QDir possibleThemeDirectory(possibleThemePath);
        if (!possibleThemeDirectory.entryList(gtk3SubdirPattern, QDir::Dirs).isEmpty()) {
            gtk3ThemesNames.insert(possibleThemeDirectory.dirName(), possibleThemeDirectory.path());
        }
    }

    gtk3ThemesModel.setThemesList(gtk3ThemesNames);
}

QStringList GTKConfigKCModule::possiblePathsToGtkThemes() {
    QStringList possibleThemesPaths;

    QStringList themesLocationsPaths = QStandardPaths::locateAll(
            QStandardPaths::GenericDataLocation,
            QStringLiteral("themes"),
            QStandardPaths::LocateDirectory);
    themesLocationsPaths << QDir::homePath() + QStringLiteral("/.themes");

    for (const QString& themesLocationPath : themesLocationsPaths) {
        QStringList possibleThemesDirectoriesNames = QDir(themesLocationPath).entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        for (const QString &possibleThemeDirectoryName : possibleThemesDirectoriesNames) {
            possibleThemesPaths += themesLocationPath + '/' + possibleThemeDirectoryName;
        }
    }

    return possibleThemesPaths;
}

void GTKConfigKCModule::selectCurrentGtk2ThemeInCheckbox()
{
    QDBusReply<QString> dbusReply = gtkConfigInterface.call(QStringLiteral("gtk2Theme"));
    currentGtk2Theme = dbusReply.value();
    ui->cb_theme->setCurrentText(currentGtk2Theme);
}

void GTKConfigKCModule::selectCurrentGtk3ThemeInCheckbox()
{
    QDBusReply<QString> dbusReply = gtkConfigInterface.call(QStringLiteral("gtk3Theme"));
    currentGtk3Theme = dbusReply.value();
    ui->cb_theme_gtk3->setCurrentText(currentGtk3Theme);
}

void GTKConfigKCModule::updateDeletionPossibilityForSelectedGtk2Theme()
{
    const QModelIndex index = gtk2ThemesModel.index(ui->cb_theme->currentIndex(), 1);
    const QString themePath = gtk2ThemesModel.data(index).value<QString>();
    if (themePath.contains(QDir::homePath())) {
        ui->removeGtk2Theme->setDisabled(false);
    } else {
        ui->removeGtk2Theme->setDisabled(true);
    }
}

void GTKConfigKCModule::updateDeletionPossibilityForSelectedGtk3Theme()
{
    const QModelIndex index = gtk3ThemesModel.index(ui->cb_theme_gtk3->currentIndex(), 1);
    const QString themePath = gtk3ThemesModel.data(index).value<QString>();
    if (themePath.contains(QDir::homePath())) {
        ui->removeGtk3Theme->setDisabled(false);
    } else {
        ui->removeGtk3Theme->setDisabled(true);
    }
}


GtkThemesListModel::GtkThemesListModel(QObject* parent)
: QAbstractTableModel(parent),
themesList()
{

}

QVariant GtkThemesListModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.row() < 0 || index.row() > themesList.count()) {
            return QVariant();
        }

        if (index.column() == 0) {
            return themesList.keys().at(index.row());
        } else {
            return themesList.values().at(index.row());
        }
    } else {
        return QVariant();
    }
}

int GtkThemesListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return themesList.count();
}

int GtkThemesListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 2;
}

void GtkThemesListModel::setThemesList(const QMap<QString, QString>& themes)
{
    beginResetModel();
    themesList = themes;
    endResetModel();
}

bool GtkThemesListModel::containsTheme(const QString &themeName)
{
    return themesList.contains(themeName);
}

#include "gtkconfigkcmodule.moc"
