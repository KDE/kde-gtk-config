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
   
    QString getIcon() const; //Obtiene el nombre del tema de iconos utilizado actualmente
    QString getFont() const; //Obtiene el tipo de letra que se esta utilizando
    QString getIconFallBack() const; // Obtiene el tema de iconos fallback
    QString getToolbarStyle() const; //Obtiene el estilo de la barra de herramientas
    bool getShowIconsInMenus() const;
    bool getShowIconsInButtons() const;
    
    static QStringList getAvaliableIconsPaths();//Obtiene las rutas absolutas de los temas de iconos disponibles en el sistema
    static QStringList getAvaliableIcons(); //Obtiene temas de icono disponibles en el sistema
    QStringList getAvaliableThemes() const;
    QStringList getAvaliableGtk3Themes();
    QStringList getAvaliableThemesPaths() const;

    bool loadFileConfig();
    bool saveFileConfig();

private:
    QString themesGtkrcFile(const QString& themeName) const;
    bool saveGTK2Config() const;
    bool saveGTK3Config() const;
    bool loadGTK2Config();
    bool loadGTK3Config();
    
    QMap <QString, QString> settings;
};

#endif // APARIENCIAGTK_H
