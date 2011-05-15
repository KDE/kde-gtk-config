#ifndef APARIENCIAGTK_H
#define APARIENCIAGTK_H

#include <QtCore>
/**
  Esta clase es la encargada de administradar los temas gtk de Chakra Linux.
  Carga configuraciones desde el archivo .gtkrc-2.0
  */
class AparienciaGTK
{
public:
    AparienciaGTK();

    void setTheme(QString);
    void setThemePath(QString);
    void setIcon(QString);
    void setIconFallBack(QString);
    void setFont(QString);
    
    //New features
    void setToolbarStyle(QString);
    void setShowIconsInMenus(QString);
    void setShowIconsInButtons(QString);

    QString getTheme(); //Obtiene el nombre del tema actualmente utilizado
    QString getThemePath(); //Obtiene las rutas absolutas del tema utilizado actualmente
    QString getIcon(); //Obtiene el nombre del tema de iconos utilizado actualmente
    QString getFont(); //Obtiene el tipo de letra que se esta utilizando
    QString getIconFallBack(); // Obtiene el tema de iconos fallback
    QStringList getAvaliableIcons(); //Obtiene temas de icono disponibles en el sistema
    QStringList getAvaliableThemes(); //Obtiene todos los temas de icono disponibles
    QStringList getAvaliableThemesPaths(); //Obtiene las rutas absolutas de los temas disponibles en el sistema
    QStringList getAvaliableIconsPaths();//Obtiene las rutas absolutas de los temas de iconos disponibles en el sistema
    QString getToolbarStyle(); //Obtiene el estilo de la barra de herramientas
    QString getShowIconsInMenus();
    QString getShowIconsInButtons();

    bool loadFileConfig(); //Carga los datos del archivo .gtkrc-2.0
    bool saveFileConfig(); //Guarda Cambios en el archivo .gtkrc-2.0 y .gtkrc-2.0-kde4

private:
    QMap <QString, QString> settings; //Aqui se guardan las Configuraciones del sistema



};

#endif // APARIENCIAGTK_H
