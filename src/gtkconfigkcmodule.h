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

#ifndef GTKCONFIGKCMODULE_H
#define GTKCONFIGKCMODULE_H

#include <KCModule>

#include <QDBusInterface>
#include <QAbstractTableModel>
#include <QMap>

class GtkThemesListModel : public QAbstractTableModel {
    Q_OBJECT
public:
    GtkThemesListModel(QObject *parent = nullptr);

    void setThemesList(const QMap<QString, QString> &themes);
    bool containsTheme(const QString &themeName);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QMap<QString, QString> themesList;
};

class QStringList;
namespace Ui { class GUI; }

class GTKConfigKCModule : public KCModule
{
    Q_OBJECT
public:
    explicit GTKConfigKCModule(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    ~GTKConfigKCModule() override;
    
    void save() override;
    void defaults() override;
    void load() override;
    
private Q_SLOTS:
    void updateThemesListsWithoutLoosingSelection();

    void installGtk2ThemeFromGHNS();
    void installGtk3ThemeFromGHNS();
    void installGtkThemeFromFile();
    void removeGtk2Theme();
    void removeGtk3Theme();

    void showGtk2Preview();
    void showGtk3Preview();

    void themesSelectionsChanged();
    
private:
    void saveGtk2Theme();
    void saveGtk3Theme();

    void loadGtkThemes();
    void loadGtk2Themes(const QStringList &possibleThemesPaths);
    void loadGtk3Themes(const QStringList &possibleThemesPaths);

    static QStringList possiblePathsToGtkThemes();

    void selectCurrentGtk2ThemeInCheckbox();
    void selectCurrentGtk3ThemeInCheckbox();

    void updateDeletionPossibilityForSelectedGtk2Theme();
    void updateDeletionPossibilityForSelectedGtk3Theme();

    QString currentGtk2Theme;
    QString currentGtk3Theme;
    GtkThemesListModel gtk2ThemesModel;
    GtkThemesListModel gtk3ThemesModel;
    QDBusInterface gtkConfigInterface;
    Ui::GUI *ui;
};

#endif // GTKCONFIGKCMODULE_H
