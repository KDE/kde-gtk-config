#ifndef APARIENCIAGTK_H
#define APARIENCIAGTK_H

#include <QtCore>
/**
 * This class is responsible of administrating the GTK themes. It loads the 
 * configurations from the .gtkrc-2.0 file.
  */
class AppearenceGTK
{
public:
    AppearenceGTK();

    void setTheme(const QString&);
    void setThemeGtk3(const QString &theme);
    void setThemePath(const QString&);
    void setIcon(const QString&);
    void setIconFallBack(const QString&);
    void setFont(const QString&);
    
    //New features
    void setToolbarStyle(const QString&);
    void setShowIconsInMenus(bool show);
    void setShowIconsInButtons(bool show);

    QString getTheme() const; //Obtiene el nombre del tema actualmente utilizado
    QString getThemeGtk3() const;
    QString getThemePath() const; //Obtiene las rutas absolutas del tema utilizado actualmente
   
    QString getIcon() const; //Obtiene el nombre del tema de iconos utilizado actualmente
    QString getFont() const; //Obtiene el tipo de letra que se esta utilizando
    QString getIconFallBack() const; // Obtiene el tema de iconos fallback
    QString getToolbarStyle() const; //Obtiene el estilo de la barra de herramientas
    bool getShowIconsInMenus() const;
    bool getShowIconsInButtons() const;
    
    static QStringList getAvaliableIconsPaths();//Obtiene las rutas absolutas de los temas de iconos disponibles en el sistema
    static QStringList getAvaliableIcons(); //Obtiene temas de icono disponibles en el sistema
    QStringList getAvaliableThemes(); //Obtiene todos los temas de icono disponibles
    QStringList getAvaliableGtk3Themes(); //Obtiene todos los temas de icono disponibles

    QStringList getAvaliableThemesPaths(); //Obtiene las rutas absolutas de los temas disponibles en el sistema

    bool loadGTK2Config();
    bool loadGTK3Config();
    
    bool loadFileConfig(); //Carga los datos del archivo .gtkrc-2.0
    bool saveFileConfig(); //Guarda Cambios en el archivo .gtkrc-2.0 y .gtkrc-2.0-kde4

private:
    QMap <QString, QString> settings; //Aqui se guardan las Configuraciones del sistema



};

#endif // APARIENCIAGTK_H
