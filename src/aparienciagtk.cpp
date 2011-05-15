#include "aparienciagtk.h"

AparienciaGTK::AparienciaGTK()
{

    qDebug() << "CREATING GTK APPEARANCE";

    //Cargamos información del archivo .gtkrc-2.0 al instanciar esta clase
    loadFileConfig();


}

//SETTERS
void AparienciaGTK::setTheme(QString tema){ settings["theme"] = tema;}
void AparienciaGTK::setIcon(QString ic){ settings["icon"] = ic;}
void AparienciaGTK::setIconFallBack(QString fall){ settings["icon_fallback"] = fall; }
void AparienciaGTK::setFont(QString fo){ settings["font"] = fo;}
void AparienciaGTK::setThemePath(QString temaPath){ settings["theme_path"] = temaPath; }

// GETTERS
QString AparienciaGTK::getTheme(){ return settings["theme"];}
QString AparienciaGTK::getThemePath(){ return settings["theme_path"]; }
QString AparienciaGTK::getIcon(){ return settings["icon"];}
QString AparienciaGTK::getIconFallBack(){ return settings["icon_fallback"]; }
QString AparienciaGTK::getFont(){ return settings["font"]; }

QStringList AparienciaGTK::getAvaliableIcons()
{
    QStringList iconosDisponibles;

    //Listo las carpetas que existen en el directorio de los temas en root
    QDir root("/usr/share/icons");
    QDirIterator iterador(root);
    while(iterador.hasNext()){
        QString urlActual = iterador.next();

        if(!(urlActual == "/usr/share/icons/." || urlActual == "/usr/share/icons/.."))
            iconosDisponibles << urlActual;
        
         //Quitamos la carpeta Default y Default Kde4 
        if(urlActual.contains( QRegExp("(/default)$") ) || urlActual.contains( QRegExp("(/default\\.kde4)$"))){
            iconosDisponibles.removeAll(urlActual);
        }
    }

    //Verificamos si existen temas desde la carpeta del usuario
    QString usuario_path = QDir::homePath()+"/.icons";
    QDir usuario(usuario_path);

    //Algunas veces no existe la carpeta .icons
    if(usuario.exists()){
        //Obtener los temas de iconos instalados en la carpeta de usuario
        QDirIterator it(usuario);
        while(it.hasNext()){
            QString urlActual = it.next();

            if(!(urlActual == (usuario_path+"/.") || urlActual == (usuario_path+"/..") )){
                iconosDisponibles << urlActual;
            }

        }
    }

    //Filtro, sólo se permiten carpetas, tambien quita la carpeta Default, Default.KDe4
    foreach(QString i, iconosDisponibles){
        QFileInfo archivo(i);
        //Si hay un archivo eliminado de la lista
        if(archivo.isFile()){
            iconosDisponibles.removeAll(i);
            continue;
        }
        
    }

    // Filtro, quitar los temas del mouse, ya que tambien ahi se instalan
    foreach(QString i, iconosDisponibles){

        QDirIterator iterador(i);
        bool icono = false;
        while(iterador.hasNext()){

            QString carpeta = iterador.next();

            //Buscar la carpeta cursor, si existe esa carpeta significa que es un tema del mouse
            if(carpeta.contains("cursor")){
                iconosDisponibles.removeAll(i);
                break; //Encontramos un tema de mouse que no siga buscando
            }

            //Buscar que tenga un archivo index.theme
            if(carpeta.contains("index.theme"))
                icono = true;
        }

        //Si no se encontró el archivo index.theme significa que no es un tema de iconos
        if(!icono)
            iconosDisponibles.removeAll(i);

    }

    //devolvemos los puros nombres de los iconos
    QStringList nombreIconos;

    foreach(QString i, iconosDisponibles){

        QDir temp(i);
        nombreIconos << temp.dirName();

    }


    return nombreIconos;

}

QStringList AparienciaGTK::getAvaliableIconsPaths()
{
    QStringList iconosDisponibles;

    //Listo las carpetas que existen en el directorio de los temas en root
    QDir root("/usr/share/icons");
    QDirIterator iterador(root);
    while(iterador.hasNext()){
        QString urlActual = iterador.next();

        if(!(urlActual == "/usr/share/icons/." || urlActual == "/usr/share/icons/..")){
            iconosDisponibles << urlActual;
        }
        
           //Quitamos la carpeta Default y Default Kde4 
        if(urlActual.contains( QRegExp("(/default)$") ) || urlActual.contains( QRegExp("(/default\\.kde4)$"))){
            iconosDisponibles.removeAll(urlActual);
        }
    }

    //Verificar si existen temas desde la carpeta del usuario
    QString usuario_path = QDir::homePath()+"/.icons";
    QDir usuario(usuario_path);

    //Algunas veces no existe la carpeta .icons
    if(usuario.exists()){
        QDirIterator it(usuario);
        while(it.hasNext()){
            QString urlActual = it.next();

            if(!(urlActual == (usuario_path+"/.") || urlActual == (usuario_path+"/..") )){
                iconosDisponibles << urlActual;
            }

        }
    }

    //Filtro, verifica si en la lista de carpetas existe un archivos
    // tambien quita las carpetas de icono no validos
    foreach(QString i, iconosDisponibles){
        QFileInfo archivo(i);
        //Si hay un archivo eliminado de la lista
        if(archivo.isFile()){
            iconosDisponibles.removeAll(i);
        }
    }

    // Filtro, quitar los temas del mouse, ya que tambien ahi se instalan
    foreach(QString i, iconosDisponibles){

        //qDebug() << ">>> " << i;
        //Esto es para iterar en los contenidos de la carpeta
        QDirIterator iterador(i);

        bool icono = false;
        while(iterador.hasNext()){

            QString carpeta = iterador.next();
            //qDebug() << "\t" << carpeta;

            //Buscar la carpeta cursor, si existe esa carpeta significa que es un tema del mouse
            if(carpeta.contains("cursor")){
                //qDebug() << carpeta << "es un tema de mouse ";
                iconosDisponibles.removeAll(i);
                break; //Encontramos un tema de mouse que no siga buscando
            }

            //Buscar que tenga un archivo index.theme
            if(carpeta.contains("index.theme")){
                icono = true;
                //qDebug() << carpeta << "puede ser tema de iconos";
            }

        }

        //Si no se encontro el archivo index.theme significa que no es un tema de iconos
        if(!icono){
            iconosDisponibles.removeAll(i);
        }
    }

    return iconosDisponibles;

}

QStringList AparienciaGTK::getAvaliableThemesPaths(){
    QStringList temasDisponibles;

    //Listo las carpetas que existen en el directorio de los temas en root
    QDir root("/usr/share/themes");
    QDirIterator iterador(root);
    while(iterador.hasNext()){
        QString urlActual = iterador.next();

        if(!(urlActual == "/usr/share/themes/." || urlActual == "/usr/share/themes/..")){
            temasDisponibles << urlActual;
        }
    }

    //Verificar si existen temas desde la carpeta del usuario
    QString usuario_path = QDir::homePath()+"/.themes";
    QDir usuario(usuario_path);

    //Algunas veces no existe la carpeta .themes
    if(usuario.exists()){
        QDirIterator it(usuario);
        while(it.hasNext()){
            QString urlActual = it.next();

            if(!(urlActual == (usuario_path+"/.") || urlActual == (usuario_path+"/..") )){
                temasDisponibles << urlActual;
            }
        }
    }

    //FILTRO, verificar si en cada directorio, existe el archivo gtkrc
    foreach(QString i, temasDisponibles){

        bool gtkrc = false; //bandera de ayuda, si es true significa que si existe el archivo gtkrc
        QDirIterator itera(i);
        while(itera.hasNext()){

            QString urlActual = itera.next();

            if(itera.filePath().contains(QRegExp("(gtk-2.0)$"))){
                gtkrc = true;
                break;
            }
        }

        //Si no existe el fichero gtkrc, no es un tema gtk valido, eliminar de la lista de temas
        if(!gtkrc){
            qDebug() << "Folder : " << i << " does not contain a gtk-2.0 folder, discard it";
            temasDisponibles.removeAll(i);
        }

    }

    return temasDisponibles;
}

QStringList AparienciaGTK::getAvaliableThemes()
{
    QStringList temasDisponibles;

    //Listo las carpetas que existen en el directorio de los temas en root
    QDir root("/usr/share/themes");
    QDirIterator iterador(root);
    while(iterador.hasNext()){
        QString urlActual = iterador.next();

        if(!(urlActual == "/usr/share/themes/." || urlActual == "/usr/share/themes/..")){
            temasDisponibles << urlActual;
        }
    }

    //Verificar si existen temas desde la carpeta del usuario
    QString usuario_path = QDir::homePath()+"/.themes";
    QDir usuario(usuario_path);

    //Algunas veces no existe la carpeta .themes
    if(usuario.exists()){
        QDirIterator it(usuario);
        while(it.hasNext()){
            QString urlActual = it.next();

            if(!(urlActual == (usuario_path+"/.") || urlActual == (usuario_path+"/..") )){
                temasDisponibles << urlActual;
            }
        }
    }

    //FILTRO, verificar si en cada directorio, existe el archivo gtkrc
    foreach(QString i, temasDisponibles){

        bool gtkrc = false; //bandera de ayuda, si es true significa que si existe el archivo gtkrc
        QDirIterator itera(i);
        while(itera.hasNext()){

            QString urlActual = itera.next();

            if(itera.filePath().contains(QRegExp("(gtk-2.0)$"))){
                gtkrc = true;
                break;
            }
        }

        //Si no existe el fichero gtkrc, no es un tema gtk valido, eliminar de la lista de temas
        if(!gtkrc){
            qDebug() << "Folder : " << i << " does not contain a gtk-2.0 folder, discard it";
            temasDisponibles.removeAll(i);
        }

    }

    QStringList temas;

    foreach(QString i, temasDisponibles){
        QDir temp(i);
        temas << temp.dirName();
    }

    return temas;

}

// METODOS PARA CREACION DE FICHEROS

bool AparienciaGTK::loadFileConfig(){

    //Verificar si el archivo de configuracion existe
    QFile archivo(QDir::homePath()+"/.gtkrc-2.0");

    if(archivo.exists()){
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
        if(archivo.open(QIODevice::ReadOnly | QIODevice::Text)){

            //Leemos el archivo y lo guardamos como una una lista de cadenas
            QTextStream flujo(&archivo);
            QStringList texto = flujo.readAll().split('\n');

            QString tema_path, //Ruta donde se encuentra instalado el tema configurado actualmente
                icono, //Nombre del tema de iconos
                fuente, //Nombre del tipo de letra
                nombre_tema, //Nombre del tema configurado
                icono_fallback,
                
                show_icons_menus, //Indica si los menus tiene iconosDisponibles
                show_icons_buttons, //Indica si los Botones tiene iconosDisponibles
                toolbar_style //Indica el estilo de la barra de herramientas
                ;
            //Quitamos espacios en blanco, comentarios y las lineas que no necesito
            foreach(QString i, texto){

                //Quitamos comentarios y los espacios vacios
                if(i.startsWith('#') || i.isEmpty()){
                    texto.removeAll(i);
                    continue;
                }

                //La linea que contiene este include no la necesito para leer
                if(i.contains("/etc/gtk-2.0/gtkrc") || i.contains("widget_class")){
                    texto.removeAll(i);
                    continue;
                }

                //Obtengo el nombre del tema instalado
                if(i.contains("gtk-theme-name")){
                    nombre_tema = (i.mid(i.indexOf("=")+1)).remove("\"");
                    texto.removeAll(i);
                    continue;
                }

            }
            
            qDebug() << "PASDFSADFD"  << texto;

            // Obtenemos los atributos de los temas
            foreach(QString i, texto){
                //Encontramos la linea que contiene include
                if(i.contains("include")){
                    //Extraer la url del tema
                    //qDebug() << ">> Encontrado linea de include: " << i;
                    tema_path = i.replace(QRegExp("(include|\")"), ""); // quitar la palabra include y los "
                    tema_path = tema_path.trimmed(); // Quitar espacios en blancos
                    //cortamos una cadena, desde el inicio hasta donde esta el nombre del tema
                    tema_path = tema_path.mid(
                            0,
                            tema_path.indexOf(nombre_tema) + nombre_tema.length()
                            );

                    continue;
                }

                // Obtenemos el nombre de la fuente
                if(i.contains("gtk-font-name")){
                    fuente =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                    continue;
                }

                // Obtenemos el nombre del tema del icono
                if(i.contains("gtk-icon-theme-name")){
                    icono =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }

                // Obtenemos el nombre del tema fallback
                if(i.contains("gtk-fallback-icon-theme")){
                    icono_fallback =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-toolbar-style gtk-button-images
                if(i.contains("gtk-toolbar-style")){
                    toolbar_style =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-button-images
                if(i.contains("gtk-button-images")){
                    show_icons_buttons =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }
                
                //Obtenemos el estilo del toolbar gtk-button-images
                if(i.contains("gtk-menu-images")){
                    show_icons_buttons =(
                            (i.mid( i.indexOf("=")+1 ))
                             .trimmed() //Quitamos espacios en blanco alrededor
                             ).replace("\"", ""); // Quitamos los parentesis "
                }

            }
            
            qDebug() << "Check Settings >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<";
            if(toolbar_style.isNull()){
                 toolbar_style = "GTK_TOOLBAR_ICONS";
                 qDebug() << "PICKED OPTIONS toolbar style: " << toolbar_style;
            }
            
            if(show_icons_buttons.isNull()){
                 show_icons_buttons = "0";
                 qDebug() << "PICKED OPTIONS, show icons in buttons: " << show_icons_buttons;
            }
            
            if(show_icons_menus.isNull()){
                 show_icons_menus = "0";
                 qDebug() << "PICKED OPTIONS, show icons in menus: " << show_icons_menus;
            }
            qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";;
            
            

            //Ingresamos los datos
            settings["theme_path"] = tema_path;
            settings["theme"] = nombre_tema;
            settings["icon"] = icono;
            settings["icon_fallback"] = icono_fallback;
            settings["font"] = fuente;
            settings["toolbar_style"] = toolbar_style;
            settings["show_icons_buttons"] = show_icons_buttons;
            settings["show_icons_menus"] = show_icons_menus;

            //Cerramos el archivo
            archivo.close();
        }

    }
    else{
        // no existe el archivo
        qDebug() << "File does not exist, setting configs ...";

        // Si no existe el archivo crearlo, y configurarlo con valores por defecto
        settings["theme_path"] = "/usr/share/themes/oxygen-gtk";
        settings["theme"] = "oxygen-gtk";
        settings["icon"] = "oxygen-refit-2-2.5.0";
        settings["icon_fallback"] = "oxygen-refit-2-2.5.0";
        settings["font"] = "Bitstream Vera Sans 10";
        settings["toolbar_style"] = "GTK_TOOLBAR_ICONS";
        settings["show_icons_buttons"] = "1";
        settings["show_icons_menus"] = "1";

        saveFileConfig();  

        return false;
    }

    return true;
}

bool AparienciaGTK::saveFileConfig()
{
    //Sobreescribimos el fichero
    QFile gtkrc(QDir::homePath()+"/.gtkrc-2.0");

    //Verifica si el archivo se pudo abrir
    if(!gtkrc.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "There was unable to write the file .gtkrc-2.0";
        return false;
    }

    QTextStream flujo(&gtkrc);

    flujo << "# File created by Chakra Gtk Config" << "\n"
            << "# Configs for GTK2 progrmans \n\n";

    qDebug() << "\tSearch for gtkrc files in folder :  " << settings["theme_path"];

    //Buscar archivo gtkrc en el tema
    QDir tema(settings["theme_path"]);
    QDirIterator it(tema, QDirIterator::Subdirectories);
    QString temp;
    while(it.hasNext()){
        // Buscar archivo gtkrc
        if(it.next().contains("gtkrc")){
            qDebug() << "\tgtkrc file found at : " << it.filePath();
            temp = it.filePath();
            break;
        }
    }

    //Escribimos la primera linea
    flujo << "include \"" << temp << "\"\n";

    //Escribimos la linea : include "/etc/gtk-2.0/gtkrc"
    flujo  << "include \"/etc/gtk-2.0/gtkrc\"\n";

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

    flujo << "style \"user-font\" \n"
            << "{\n"
            << "\tfont_name=\""<< nombre_fuente << "\"\n"
            << "}\n";

    // Escribir linea : widget_class "*" style "user-font"
    flujo << "widget_class \"*\" style \"user-font\"\n";


    //Especificamos el nombre del tema
    flujo << "gtk-font-name=\"" << settings["font"] << "\"\n";

    // Escribir linea : gtk-theme-name="nombre-tema"
    flujo << "gtk-theme-name=\"" << settings["theme"] << "\"\n";

    // Escribir linea : gtk-icon-theme-name
    flujo << "gtk-icon-theme-name=\""<< settings["icon"] << "\"\n";

    flujo << "gtk-fallback-icon-theme=\"" << settings["icon_fallback"] << "\"\n";
    
    
    /**
     *  New features:
     *      gtk-toolbar-style = GTK_TOOLBAR_ICONS
            gtk-menu-images = true
            gtk-button-images = true
            
            
            settings["toolbar_style"] = toolbar_style;
            settings["show_icons_buttons"] = show_icons_buttons;
            settings["show_icons_menus"] = show_icons_menus;
            
            
     */
    
    flujo << "gtk-toolbar-style=" << settings["toolbar_style"] << "\n";
    flujo << "gtk-menu-images=" << settings["show_icons_buttons"] << "\n";
    flujo << "gtk-button-images=" << settings["show_icons_menus"] << "\n";
    

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

    return true;

}


/* Obtiene el estilo de la barra de tareas*/
QString AparienciaGTK::getToolbarStyle()
{
     return settings["toolbar_style"];
}

/* */
void AparienciaGTK::setToolbarStyle(QString toolbar_style)
{    
     settings["toolbar_style"] = toolbar_style;
}


QString AparienciaGTK::getShowIconsInButtons()
{
     return settings["show_icons_buttons"];
}

QString AparienciaGTK::getShowIconsInMenus()
{
     return settings["show_icons_menus"];
}

void AparienciaGTK::setShowIconsInButtons(QString buttons)
{
     settings["show_icons_buttons"] = buttons;
}

void AparienciaGTK::setShowIconsInMenus(QString menus)
{
     settings["show_icons_menus"] = menus;
}



