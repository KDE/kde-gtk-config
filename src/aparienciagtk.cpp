#include "aparienciagtk.h"

AppearenceGTK::AppearenceGTK()
{
    //We load the gtk config file
    loadFileConfig();
    
//     getAvaliableGtk3Themes();
}

//SETTERS
void AppearenceGTK::setTheme(const QString& tema) { settings["theme"] = tema;}
void AppearenceGTK::setIcon(const QString& ic) { settings["icon"] = ic;}
void AppearenceGTK::setIconFallBack(const QString& fall) { settings["icon_fallback"] = fall; }
void AppearenceGTK::setFont(const QString& fo) { settings["font"] = fo;}
void AppearenceGTK::setThemePath(const QString& temaPath) { settings["theme_path"] = temaPath; }

void AppearenceGTK::setThemeGtk3(const QString& theme)
{
  settings["themegtk3"] = theme;
}

void AppearenceGTK::setThemeGtk3Path(const QString& themePath)
{
    settings["themegtk3_path"] = themePath;
}

// GETTERS
QString AppearenceGTK::getTheme() const { return settings["theme"];}
QString AppearenceGTK::getThemePath() const { return settings["theme_path"]; }
QString AppearenceGTK::getIcon() const { return settings["icon"];}
QString AppearenceGTK::getIconFallBack() const { return settings["icon_fallback"]; }
QString AppearenceGTK::getFont() const { return settings["font"]; }

QString AppearenceGTK::getThemeGtk3() const
{
    return settings["themegtk3"];
}

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
        if(!urlActual.contains( QRegExp("(/default)$") ) && !urlActual.contains( QRegExp("(/default\\.kde4)$"))) {
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

QStringList AppearenceGTK::getAvaliableThemesPaths()
{
    //TODO: Port to KStandardDirs
    QDir root("/usr/share/themes");
    QFileInfoList availableThemes = root.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //Check if there are themes installed by the user
    QDir user(QDir::homePath()+"/.themes");

    if(user.exists()) {
        availableThemes += user.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);
    }

    //we just want actual themes
    QStringList paths;
    for(QFileInfoList::iterator it=availableThemes.begin(); it!=availableThemes.end(); ) {
        bool hasGtkrc = QDir(it->filePath()).exists("gtk-2.0");

        //If it doesn't exist, we don't want it on the list
        if(!hasGtkrc) {
            qDebug() << "Folder : " << it->filePath() << " does not contain a gtk-2.0 folder, discard it";
            it = availableThemes.erase(it);
        } else {
            ++it;
            paths += it->filePath();
        }
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
    if(user.exists()) {
        availableThemes += user.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);
    }

    //we just want actual themes
    QStringList themes;
    for(QFileInfoList::iterator it=availableThemes.begin(); it!=availableThemes.end(); ) {
        bool hasGtkrc = QDir(it->filePath()).exists("gtk-3.0");

        //If it doesn't exist, we don't want it on the list
        if(!hasGtkrc) {
            qDebug() << "Folder : " << it->filePath() << " does not contain a gtk-3.0 folder, discard it";
            it = availableThemes.erase(it);
        } else {
            ++it;
            themes += it->fileName();
        }
    }
    
    qDebug() << "*** Temas GTK3 path:" << themes;

    return themes;
}


QStringList AppearenceGTK::getAvaliableThemes()
{
    QStringList temas, temasDisponibles=getAvaliableThemesPaths();

    foreach(const QString& i, temasDisponibles) {
        QDir temp(i);
        temas << temp.dirName();
    }

    return temas;

}

////////////////////////////////////
// METODOS PARA CREACION DE FICHEROS

bool AppearenceGTK::loadFileConfig() {

    //Verificar si el archivo de configuracion existe
    QFile configFile(QDir::homePath()+"/.gtkrc-2.0");
    
    bool is_settings_read = false;

    if(configFile.exists()) {
        qDebug() << "The config file already exist ... ";

        /**
          Variables a sacar desde el archivo
            include "/path/to/theme/gtkrc"
            font-name="Nombre de fuente"
            gtk-theme-name="nombre-tema"
            gtk-icon-theme-name="OxygenRefit2"
            gtk-fallback-icon-theme = "oxygen-refit-2-2.5.0"
            gtk-toolbar-style = GTK_TOOLBAR_ICONS
            gtk-menu-images = true
            gtk-button-images = true

          */

        //Abrir el archivo para la lectura
        if(configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

            //Leemos el archivo y lo guardamos como una una lista de cadenas
            QTextStream flow(&configFile);
            QStringList text = flow.readAll().split('\n');

            QString themePath, //Ruta donde se encuentra instalado el tema configurado actualmente
                icon, //Nombre del tema de iconos
                font, //Nombre del tipo de letra
                themeName, //Nombre del tema configurado
                iconFallback,
                
                showIconMenus, //Indica si los menus tiene iconosDisponibles
                showIconButtons, //Indica si los Botones tiene iconosDisponibles
                toolbarStyle //Indica el estilo de la barra de herramientas
                ;
            //Quitamos espacios en blanco, comentarios y las lineas que no necesito
            foreach(const QString& i, text) {

                //Quitamos comentarios y los espacios vacios
                if(i.startsWith('#') || i.isEmpty()) {
                    text.removeAll(i);
                    continue;
                }

                //La linea que contiene este include no la necesito para leer
                if(i.contains("/etc/gtk-2.0/gtkrc") || i.contains("widget_class")) {
                    text.removeAll(i);
                    continue;
                }

                //Obtengo el nombre del tema instalado
                if(i.contains("gtk-theme-name")) {
                    themeName = (i.mid(i.indexOf("=")+1)).remove("\"");
                    text.removeAll(i);
                    continue;
                }

            }
            
           // qDebug() << "PASDFSADFD"  << texto;

            // Obtenemos los atributos de los temas
            foreach(const QString& i, text) {
                //Encontramos la linea que contiene include
                if(i.contains("include")) {
                    //Extraer la url del tema
                    //qDebug() << ">> Encontrado linea de include: " << i;
                    themePath = QString(i).replace(QRegExp("(include|\")"), ""); // quitar la palabra include y los "
                    themePath = themePath.trimmed(); // Quitar espacios en blancos
                    //cortamos una cadena, desde el inicio hasta donde esta el nombre del tema
                    themePath = themePath.mid(
                            0,
                                    themePath.indexOf(themeName) + themeName.length()
                            );

                    continue;
                }

                // Obtenemos el nombre de la fuente
                if(i.contains("gtk-font-name")) {
                    font =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                    continue;
                }

                // Obtenemos el nombre del tema del icono
                if(i.contains("gtk-icon-theme-name")) {
                    icon =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }

                // Obtenemos el nombre del tema fallback
                if(i.contains("gtk-fallback-icon-theme")) {
                    iconFallback =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-toolbar-style gtk-button-images
                if(i.contains("gtk-toolbar-style")) {
                    toolbarStyle =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-button-images
                if(i.contains("gtk-button-images")) {
                    showIconButtons =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-button-images
                if(i.contains("gtk-menu-images")) {
                    showIconMenus =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }

            }
            
            //qDebug() << "Check Settings >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<";
            if(toolbarStyle.isNull()) {
                 toolbarStyle = "GTK_TOOLBAR_ICONS";
                // qDebug() << "PICKED OPTIONS toolbar style: " << toolbar_style;
            }
            
            if(showIconButtons.isNull()) {
                 showIconButtons = "0";
                 //qDebug() << "PICKED OPTIONS, show icons in buttons: " << show_icons_buttons;
            }
            
            if(showIconMenus.isNull()) {
                 showIconMenus = "0";
                // qDebug() << "PICKED OPTIONS, show icons in menus: " << show_icons_menus;
            }
            //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";;
            
            

            //Ingresamos los datos
            settings["theme_path"] = themePath;
            settings["theme"] = themeName;
            settings["icon"] = icon;
            settings["icon_fallback"] = iconFallback;
            settings["font"] = font;
            settings["toolbar_style"] = toolbarStyle;
            settings["show_icons_buttons"] = showIconButtons;
            settings["show_icons_menus"] = showIconMenus;

            //Cerramos el archivo
            configFile.close();
            
            is_settings_read = true;
        }

    }
    
    
    //Leemos los datos para temas GTK3
    QFile fileGtk3(QDir::homePath()+"/.config/gtk-3.0/settings.ini");
    
    if(fileGtk3.exists()) {
        qDebug() << "The config file already exist gtk3 :) ... ";

        /**
          Variables a sacar desde el archivo
            include "/path/to/theme/gtkrc"
            font-name="Nombre de fuente"
            gtk-theme-name="nombre-tema"
            gtk-icon-theme-name="OxygenRefit2"
            gtk-fallback-icon-theme = "oxygen-refit-2-2.5.0"
            gtk-toolbar-style = GTK_TOOLBAR_ICONS
            gtk-menu-images = true
            gtk-button-images = true

          */

        //Abrir el archivo para la lectura
        if(fileGtk3.open(QIODevice::ReadOnly | QIODevice::Text)) {

            //Leemos el archivo y lo guardamos como una una lista de cadenas
            QTextStream flow(&fileGtk3);
            QStringList text = flow.readAll().split('\n');

            QString 
                icon, //Nombre del tema de iconos
                font, //Nombre del tipo de letra
                themeName, //Nombre del tema configurado
                iconFallback,
                
                showIconsMenus, //Indica si los menus tiene iconosDisponibles
                showIconsButtons, //Indica si los Botones tiene iconosDisponibles
                toolbarStyle //Indica el estilo de la barra de herramientas
                ;
            //Quitamos espacios en blanco, comentarios y las lineas que no necesito
            foreach(const QString& i, text) {

                //Quitamos comentarios y los espacios vacios
                if(i.startsWith('#') || i.isEmpty()) {
                    text.removeAll(i);
                    continue;
                }

                //La linea que contiene este include no la necesito para leer
                if(i.contains("[Settings]")) {
                    text.removeAll(i);
                    continue;
                }

                

            }
            
        

            // Obtenemos los atributos de los temas
            foreach(const QString& i, text) {
          
                //Obtengo el nombre del tema instalado
                if(i.contains("gtk-theme-name")) {
                    themeName = (i.mid(i.indexOf("=")+1)).remove("\"");
                    text.removeAll(i);
                    continue;
                }
                

                // Obtenemos el nombre de la fuente
                if(i.contains("gtk-font-name")) {
                    font =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                    continue;
                }

                // Obtenemos el nombre del tema del icono
                if(i.contains("gtk-icon-theme-name")) {
                    icon =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }

                // Obtenemos el nombre del tema fallback
                if(i.contains("gtk-fallback-icon-theme")) {
                    iconFallback =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-toolbar-style gtk-button-images
                if(i.contains("gtk-toolbar-style")) {
                    toolbarStyle =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-button-images
                if(i.contains("gtk-button-images")) {
                    showIconsButtons =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-button-images
                if(i.contains("gtk-menu-images")) {
                    showIconsMenus =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }

            }
            
            //qDebug() << "Check Settings >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<";
            if(toolbarStyle.isNull()) {
                 toolbarStyle = "GTK_TOOLBAR_ICONS";
                // qDebug() << "PICKED OPTIONS toolbar style: " << toolbar_style;
            }
            
            if(showIconsButtons.isNull()) {
                 showIconsButtons = "0";
                 //qDebug() << "PICKED OPTIONS, show icons in buttons: " << show_icons_buttons;
            }
            
            if(showIconsMenus.isNull()) {
                 showIconsMenus = "0";
                // qDebug() << "PICKED OPTIONS, show icons in menus: " << show_icons_menus;
            }
            //qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";;
            
            

            //Ingresamos los datos
            settings["themegtk3"] = themeName;
            settings["icon"] = icon;
            settings["icon_fallback"] = iconFallback;
            settings["font"] = font;
            settings["toolbar_style"] = toolbarStyle;
            settings["show_icons_buttons"] = showIconsButtons;
            settings["show_icons_menus"] = showIconsMenus;

            qDebug()<< "***********" << settings;
            
            //Cerramos el archivo
            fileGtk3.close();
            
            is_settings_read = true;
        }
    }
    
    //Si no se leyo ninguna configuracion
    if(!is_settings_read) {
        // no existe el archivo
        qDebug() << "File's does not exist, setting default configs  ...";

        // Si no existe el archivo crearlo, y configurarlo con valores por defecto
        settings["theme_path"] = "/usr/share/themes/oxygen-gtk";
        settings["themegtk3"] = "Raleigh";
        settings["theme"] = "oxygen-gtk";
        settings["icon"] = "oxygen-refit-2-2.5.0";
        settings["icon_fallback"] = "oxygen-refit-2-2.5.0";
        settings["font"] = "Bitstream Vera Sans 10";
        settings["toolbar_style"] = "GTK_TOOLBAR_ICONS";
        settings["show_icons_buttons"] = "1";
        settings["show_icons_menus"] = "1";

        saveFileConfig();
    }
    
 

    return true;
}

bool AppearenceGTK::saveFileConfig()
{
    //Sobreescribimos el fichero
    QFile gtkrc(QDir::homePath()+"/.gtkrc-2.0");

    //Verifica si el archivo se pudo abrir
    if(!gtkrc.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "There was unable to write the file .gtkrc-2.0";
        return false;
    }

    QTextStream flow(&gtkrc);

    flow << "# File created by Chakra Gtk Config" << "\n"
            << "# Configs for GTK2 progrmans \n\n";

    //qDebug() << "\tSearch for gtkrc files in folder :  " << settings["theme_path"];

    //Buscar archivo gtkrc en el tema
    QDir theme(settings["theme_path"]);
    QDirIterator it(theme, QDirIterator::Subdirectories);
    QString temp;
    while(it.hasNext()) {
        // Buscar archivo gtkrc
        if(it.next().contains("gtkrc")) {
            qDebug() << "\tgtkrc file found at : " << it.filePath();
            temp = it.filePath();
            break;
        }
    }

    //Escribimos la primera linea
    flow << "include \"" << temp << "\"\n";

    //Escribimos la linea : include "/etc/gtk-2.0/gtkrc"
    flow  << "include \"/etc/gtk-2.0/gtkrc\"\n";

    //Escribimos ahora style: "user-font"
    //{
    //        font-name:"Nombre de fuente"
    //}


    QString nombre_fuente = settings["font"].mid(
            0,
            settings["font"].lastIndexOf(QRegExp("([0-9]|[0-9][0-9]|[0-9][0-9][0-9])"))-1
            );

    nombre_fuente = nombre_fuente.replace(QRegExp("(bold|italic)"), "");
    nombre_fuente = nombre_fuente.trimmed();

    flow << "style \"user-font\" \n"
            << "{\n"
            << "\tfont_name=\""<< nombre_fuente << "\"\n"
            << "}\n";

    // Escribir linea : widget_class "*" style "user-font"
    flow << "widget_class \"*\" style \"user-font\"\n";


    //Especificamos el nombre del tema
    flow << "gtk-font-name=\"" << settings["font"] << "\"\n";

    // Escribir linea : gtk-theme-name="nombre-tema"
    flow << "gtk-theme-name=\"" << settings["theme"] << "\"\n";

    // Escribir linea : gtk-icon-theme-name
    flow << "gtk-icon-theme-name=\""<< settings["icon"] << "\"\n";

    flow << "gtk-fallback-icon-theme=\"" << settings["icon_fallback"] << "\"\n";
    
    
    /**
     *  New features:
     *      gtk-toolbar-style = GTK_TOOLBAR_ICONS
            gtk-menu-images = true
            gtk-button-images = true
            
            
            settings["toolbar_style"] = toolbar_style;
            settings["show_icons_buttons"] = show_icons_buttons;
            settings["show_icons_menus"] = show_icons_menus;
            
            
     */
    
    flow << "gtk-toolbar-style=" << settings["toolbar_style"] << "\n";
    flow << "gtk-menu-images=" << settings["show_icons_buttons"] << "\n";
    flow << "gtk-button-images=" << settings["show_icons_menus"] << "\n";
    

    //CErramos el archivo para finalizar
    gtkrc.close();
    
    //tratamos de borrar el archivo .gtkrc-2.0-kde4 anterior si existe
    if(QFile::remove(QDir::homePath()+"/.gtkrc-2.0-kde4"))
        qDebug() << "listo para crear enlaze simbolico";
    
    //Creamos enlaze hacia el archivo .gtkrc-2.0-kde4
    if(!QFile::link(
       (QDir::homePath()+"/.gtkrc-2.0"),
       (QDir::homePath()+"/.gtkrc-2.0-kde4")         
    ))
        qDebug() << "no se pudo crear enlaze simbolico al archivo .gtkrc-2.0-kde4 :(";
    else
        qDebug() << "enlace simbolico creado ,  al archivo .gtkrc-2.0-kde4 :D";
    
    //GTK 3 support
    //Archivo de Configuracion  ${XDG_CONFIG_HOME}/gtk-3.0/settings.ini
    
    QFile file_gtk3(QDir::homePath()+"/.config/gtk-3.0/settings.ini");
    if(!file_gtk3.exists()) {
        qDebug() << "NO EXISTE " << QDir::homePath()+"/.config/gtk-3.0/settings.ini" << "Crearlo";
        
        QDir gtk3(QDir::homePath()+"/.config/gtk-3.0");
        
        if(!gtk3.exists()) {
            qDebug() << "- NO EXISTE " << QDir::homePath()+"/.config/gtk-3.0";
            
            QDir config(QDir::homePath()+"/.config");
            
            if(config.mkdir("gtk-3.0")) {
                qDebug() << "Carpeta gtk-3.0 creada satisfactoriamente";
                
            }
            else{
                qDebug() << "No se pudo crear la carpeta "  << QDir::homePath()+"/.config/gtk-3.0 " << " verifica permisos";
                return false;
            }
            
        }
        
    }
    
    //Carpeta gtk3-0 creada podemos continuar
    
    if(!file_gtk3.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo " << file_gtk3.fileName();
    }
    
    QTextStream flujo3(&file_gtk3);
    
    flujo3 << "[Settings]" << "\n";
    flujo3 << "gtk-font-name=" << settings["font"] << "\n";
    flujo3 << "gtk-theme-name=" << settings["themegtk3"] << "\n";
    flujo3 << "gtk-icon-theme-name= "<< settings["icon"] << "\n";
    flujo3 << "gtk-fallback-icon-theme=" << settings["icon_fallback"] << "\n";
    flujo3 << "gtk-toolbar-style=" << settings["toolbar_style"] << "\n";
    flujo3 << "gtk-menu-images=" << settings["show_icons_buttons"] << "\n";
    flujo3 << "gtk-button-images=" << settings["show_icons_menus"] << "\n";
    
    file_gtk3.close();
    
    return true;

}


/* Obtiene el estilo de la barra de tareas*/
QString AppearenceGTK::getToolbarStyle() const
{
     return settings["toolbar_style"];
}

/* */
void AppearenceGTK::setToolbarStyle(const QString& toolbar_style)
{    
     settings["toolbar_style"] = toolbar_style;
}


QString AppearenceGTK::getShowIconsInButtons() const
{
     return settings["show_icons_buttons"];
}

QString AppearenceGTK::getShowIconsInMenus() const
{
     return settings["show_icons_menus"];
}

void AppearenceGTK::setShowIconsInButtons(const QString& buttons)
{
     settings["show_icons_buttons"] = buttons;
}

void AppearenceGTK::setShowIconsInMenus(const QString& menus)
{
     settings["show_icons_menus"] = menus;
}
