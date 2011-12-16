#include "appearencegtk.h"
#include <kdebug.h>

AppearenceGTK::AppearenceGTK()
{
    loadFileConfig();
}

//SETTERS
void AppearenceGTK::setTheme(const QString& tema) { settings["theme"] = tema;}
void AppearenceGTK::setIcon(const QString& ic) { settings["icon"] = ic;}
void AppearenceGTK::setIconFallBack(const QString& fall) { settings["icon_fallback"] = fall; }
void AppearenceGTK::setFont(const QString& fo) { settings["font"] = fo;}
void AppearenceGTK::setThemeGtk3(const QString& theme) { settings["themegtk3"] = theme; }
void AppearenceGTK::setShowIconsInButtons(bool show) { settings["show_icons_buttons"] = show ? "1" : "0"; }
void AppearenceGTK::setShowIconsInMenus(bool show) { settings["show_icons_menus"] = show ? "1" : "0"; }
void AppearenceGTK::setToolbarStyle(const QString& toolbar_style) { settings["toolbar_style"] = toolbar_style; }

// GETTERS
QString AppearenceGTK::getTheme() const { return settings["theme"];}
QString AppearenceGTK::getIcon() const { return settings["icon"];}
QString AppearenceGTK::getIconFallBack() const { return settings["icon_fallback"]; }
QString AppearenceGTK::getFont() const { return settings["font"]; }
QString AppearenceGTK::getThemeGtk3() const { return settings["themegtk3"]; }
QString AppearenceGTK::getToolbarStyle() const { return settings["toolbar_style"]; }
bool AppearenceGTK::getShowIconsInButtons() const { return settings["show_icons_buttons"]=="1"; }
bool AppearenceGTK::getShowIconsInMenus() const { return settings["show_icons_menus"]=="1"; }

QStringList AppearenceGTK::getAvaliableIcons()
{
    QStringList paths = getAvaliableIconsPaths();
    QStringList ret;
    
    foreach(const QString& s, paths)
        ret << QDir(s).dirName();

    return ret;
}

QStringList AppearenceGTK::getAvaliableIconsPaths()
{
    QStringList availableIcons;

    //List existing folders in the root themes directory
    //TODO: Use KStandardDirs
    QDir root("/usr/share/icons");
    QDirIterator iter(root.path(), QDir::AllDirs|QDir::NoDotAndDotDot);
    while(iter.hasNext()) {
        QString urlActual = iter.next();

        //we filter out the default and kde4 default
        if(iter.fileName()!="default" && iter.fileName()!="default.kde4") {
            availableIcons << urlActual;
        }
    }

//  We verify if there are themes in the home folder
    QDir userIconsDir(QDir::homePath()+"/.icons");

    //someties there's no .icons folder
    if(userIconsDir.exists()) {
        QDirIterator it(userIconsDir.path(), QDir::NoDotAndDotDot|QDir::AllDirs);
        while(it.hasNext()) {
            QString currentPath = it.next();
            QDir dir(currentPath);

            if(dir.entryInfoList(QStringList("*cursor*")).isEmpty() && !dir.entryInfoList(QStringList("index.theme")).isEmpty()) {
                availableIcons << currentPath;
            }
        }
    }

    return availableIcons;
}

QStringList AppearenceGTK::getAvaliableThemesPaths() const
{
    //TODO: Port to KStandardDirs
    QDir root("/usr/share/themes");
    QFileInfoList availableThemes = root.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //Check if there are themes installed by the user
    QDir user(QDir::homePath()+"/.themes");
    availableThemes += user.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //we just want actual themes
    QStringList paths;
    for(QFileInfoList::iterator it=availableThemes.begin(); it!=availableThemes.end(); ++it) {
        bool hasGtkrc = QDir(it->filePath()).exists("gtk-2.0");

        //If it doesn't exist, we don't want it on the list
        if(hasGtkrc)
            paths += it->filePath();
    }

    return paths;
}

QStringList AppearenceGTK::getAvaliableGtk3Themes()
{
    //TODO: port to kstandarddirs
    QDir root("/usr/share/themes");
    QFileInfoList availableThemes = root.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //Also show the user-installed themes
    QDir user(QDir::homePath()+"/.themes");
    availableThemes += user.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //we just want actual themes
    QStringList themes;
    for(QFileInfoList::iterator it=availableThemes.begin(); it!=availableThemes.end(); ++it) {
        bool hasGtkrc = QDir(it->filePath()).exists("gtk-3.0");

        //If it doesn't exist, we don't want it on the list
        if(hasGtkrc)
            themes += it->fileName();
    }

    return themes;
}


QStringList AppearenceGTK::getAvaliableThemes() const
{
    QStringList temas, temasDisponibles=getAvaliableThemesPaths();

    foreach(const QString& i, temasDisponibles) {
        QDir temp(i);
        temas << temp.dirName();
    }

    return temas;

}

QRegExp valueRx("([a-zA-Z\\-]+) *= *\"?([^\"\\n]+)\"?\\n", Qt::CaseSensitive, QRegExp::RegExp2);
QMap<QString,QString> readSettingsTuples(const QString& allText)
{
    QMap<QString,QString> ret;
    for(int offset=0; offset>=0; offset=valueRx.indexIn(allText, offset+valueRx.cap(0).size())) {
        ret[valueRx.cap(1)] = valueRx.cap(2);
    }
    return ret;
}

QString readMapDefaultValue(const QMap<QString,QString>& map, const QString& key, const QString& defaultValue)
{
    return map.contains(key) ? map[key] : defaultValue;
}

////////////////////////////////////
// Methods responsible of file creation

bool AppearenceGTK::loadGTK2Config()
{
    
    QFile configFile(QDir::homePath()+"/.gtkrc-2.0");
    
    bool canRead = configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    
    if(canRead) {
        kDebug() << "The gtk2 config file exists...";
        
        //we read the file and put it to a string list
        //TODO: when we don't need the theme path this won't be needed anymore
        QTextStream flow(&configFile);
        QString allText = flow.readAll();
        QStringList text = allText.split('\n');
        
        QMap<QString, QString> foundSettings=readSettingsTuples(allText);

        settings["theme"] = foundSettings["gtk-theme-name"];
        settings["icon"] = foundSettings["gtk-icon-theme-name"];
        settings["icon_fallback"] = foundSettings["gtk-fallback-icon-theme"];
        settings["font"] = foundSettings["gtk-font-name"];
        settings["toolbar_style"] = readMapDefaultValue(foundSettings, "gtk-toolbar-style", "GTK_TOOLBAR_ICONS");
        settings["show_icons_buttons"] = readMapDefaultValue(foundSettings, "gtk-button-images", "0");
        settings["show_icons_menus"] = readMapDefaultValue(foundSettings, "gtk-menu-images", "0");
    }
    
    return canRead;
}

bool AppearenceGTK::loadGTK3Config()
{
    QFile fileGtk3(QDir::homePath()+"/.config/gtk-3.0/settings.ini");
    bool canRead=fileGtk3.open(QIODevice::ReadOnly | QIODevice::Text);
    
    if(canRead) {
        kDebug() << "The gtk3 config file exists...";

        QTextStream flow(&fileGtk3);

        QMap<QString, QString> foundSettings = readSettingsTuples(flow.readAll());
        
        settings["themegtk3"] = foundSettings["gtk-theme-name"];
        settings["icon"] = foundSettings["gtk-icon-theme-name"];
        settings["icon_fallback"] = foundSettings["gtk-fallback-icon-theme"];
        settings["font"] = foundSettings["gtk-font-name"];
        
        settings["toolbar_style"] = readMapDefaultValue(foundSettings, "gtk-toolbar-style", "GTK_TOOLBAR_ICONS");
        settings["show_icons_buttons"] = readMapDefaultValue(foundSettings, "gtk-button-images", "0");
        settings["show_icons_menus"] = readMapDefaultValue(foundSettings, "gtk-menu-images", "0");
    }
    
    return canRead;
}

bool AppearenceGTK::loadFileConfig()
{
    settings.clear();
    
    bool is_settings_read = loadGTK2Config();
    is_settings_read = loadGTK3Config() | is_settings_read;
    Q_ASSERT(is_settings_read);
    
    //Couldn't find any configuration, set some defaults
    //TODO: don't overwrite gtk2 values if we don't have gtk3 and vice-versa
    if(!is_settings_read) {
        kDebug() << "Couldn't find any config file for gtk2 or gtk3";

        //If the load wasn't successful, create configurations with default values
        settings["themegtk3"] = "Raleigh";
        settings["theme"] = "oxygen-gtk";
        settings["icon"] = "oxygen-refit-2-2.5.0";
        settings["icon_fallback"] = "oxygen-refit-2-2.5.0";
        settings["font"] = "Bitstream Vera Sans 10";
        settings["toolbar_style"] = "GTK_TOOLBAR_ICONS";
        settings["show_icons_buttons"] = "1";
        settings["show_icons_menus"] = "1";

        return saveFileConfig();
    } else
        return true;
}

QString AppearenceGTK::themesGtkrcFile(const QString& themeName) const
{
    QStringList themes=getAvaliableThemesPaths();
    themes=themes.filter(QRegExp("/"+themeName+"/?$"));
    if(themes.size()==1) {
        QDirIterator it(themes.first(), QDirIterator::Subdirectories);
        while(it.hasNext()) {
            it.next();
            if(it.fileName()=="gtkrc") {
                kDebug() << "\tgtkrc file found at : " << it.filePath();
                return it.filePath();
            }
        }
    }
    
    return QString();
}

bool AppearenceGTK::saveGTK2Config(const QString& gtkrcFile) const
{
    QFile gtkrc(gtkrcFile);

    if(!gtkrc.open(QIODevice::WriteOnly | QIODevice::Text)) {
        kDebug() << "There was unable to write the file .gtkrc-2.0";
        return false;
    }

    QTextStream flow(&gtkrc);

    flow << "# File created by Chakra Gtk Config" << "\n"
         << "# Configs for GTK2 programs \n\n";

    QString themeGtkrcFile=themesGtkrcFile(getTheme());
    
    //TODO: is this really needed?
    if(!themeGtkrcFile.isEmpty())
        flow << "include \"" << themeGtkrcFile << "\"\n"; //We include the theme's gtkrc file
    
    if(QFile::exists("/etc/gtk-2.0/gtkrc"))
        flow  << "include \"/etc/gtk-2.0/gtkrc\"\n"; //We include the /etc's config file

    int nameEnd = settings["font"].lastIndexOf(QRegExp(" ([0-9]+|bold|italic)"));
    QString fontFamily = settings["font"].left(nameEnd);

    //TODO: is this really needed?
    flow << "style \"user-font\" \n"
            << "{\n"
            << "\tfont_name=\""<< fontFamily << "\"\n"
            << "}\n";
    
    flow << "widget_class \"*\" style \"user-font\"\n";
    flow << "gtk-font-name=\"" << settings["font"] << "\"\n";
    flow << "gtk-theme-name=\"" << settings["theme"] << "\"\n";
    flow << "gtk-icon-theme-name=\""<< settings["icon"] << "\"\n";
    flow << "gtk-fallback-icon-theme=\"" << settings["icon_fallback"] << "\"\n";
    flow << "gtk-toolbar-style=" << settings["toolbar_style"] << "\n";
    flow << "gtk-menu-images=" << settings["show_icons_buttons"] << "\n";
    flow << "gtk-button-images=" << settings["show_icons_menus"] << "\n";
    
    //we're done with the  ~/.gtk-2.0 file
    gtkrc.close();
    
    //TODO: do we really need the linked file?
    if(QFile::remove(gtkrcFile+"-kde4"))
        kDebug() << "ready to create the symbolic link";
    
    if(!QFile::link(
       (gtkrcFile),
       (gtkrcFile+"-kde4")
    ))
        kDebug() << "Couldn't create the symboling link to .gtkrc-2.0-kde4 :(";
    else
        kDebug() << "Symbolic link created for .gtkrc-2.0-kde4 :D";
    
    return true;
}

bool AppearenceGTK::saveGTK3Config(const QString& rootDir) const
{
    /////////GTK 3 support
    //Opening GTK3 config file $ENV{XDG_CONFIG_HOME}/gtk-3.0/settings.ini
    //TODO: use XDG_CONFIG_HOME, instead
    QDir::home().mkpath(rootDir+"/.config/gtk-3.0/"); //we make sure the path exists
    QFile file_gtk3(rootDir+"/.config/gtk-3.0/settings.ini");
    
    if(!file_gtk3.open(QIODevice::WriteOnly | QIODevice::Text)) {
        kDebug() << "Couldn't open GTK3 config file for writing at:" << file_gtk3.fileName();
        return false;
    }
    
    QTextStream flow3(&file_gtk3);
    flow3 << "[Settings]" << "\n";
    flow3 << "gtk-font-name=" << settings["font"] << "\n";
    flow3 << "gtk-theme-name=" << settings["themegtk3"] << "\n";
    flow3 << "gtk-icon-theme-name= "<< settings["icon"] << "\n";
    flow3 << "gtk-fallback-icon-theme=" << settings["icon_fallback"] << "\n";
    flow3 << "gtk-toolbar-style=" << settings["toolbar_style"] << "\n";
    flow3 << "gtk-menu-images=" << settings["show_icons_buttons"] << "\n";
    flow3 << "gtk-button-images=" << settings["show_icons_menus"] << "\n";
    
    return true;
}

bool AppearenceGTK::saveFileConfig()
{
    return saveGTK2Config(QDir::homePath()+"/.gtkrc-2.0") && saveGTK3Config(QDir::homePath());
}
