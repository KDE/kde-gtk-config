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

    QString getTheme(); //Obtiene el nombre del tema actualmente utilizado
    QString getThemePath();
    QString getIcon(); //Obtiene el nombre del tema de iconos utilizado actualmente
    QString getFont(); //Obtiene el tipo de letra que se esta utilizando
    QString getIconFallBack(); // Obtiene el tema de iconos fallback
    QStringList getAvaliableIcons(); //Obtiene temas de icono disponibles en el sistema
    QStringList getAvaliableThemes(); //Obtiene todos los temas de icono disponibles
    QStringList getAvaliableThemesPaths();
    QStringList getAvaliableIconsPaths();

    bool loadFileConfig(); //Carga el archivo de configuracion de aplicaciones gtk al QMap settings
    bool saveFileConfig(); //Guarda las configuraciones desde QMap Settings al archivo de configuraciones

private:
    QMap <QString, QString> settings; //Configuraciones del sistema



};

#endif // APARIENCIAGTK_H
