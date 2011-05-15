#ifndef INSTALLER_H
#define INSTALLER_H
#include <QtCore>
class Installer
{
public:
    /**
      Este metodo se encarga de desempaquetar el archivo
      que contiene el tema gtk2, en la carpeta donde se almacenan
      los temas gtk2

      Para desempaquetar utiliza el comando tar:
      >> tar -xvf tema.tar.gz -C /home/usuario/.themes
      */
    static bool installTheme(QString &urlPackage);

    /**
      Desempaqueta el archivo que contiene los ficheros del tema de iconos,
      en la carpeta donde se almacenan los iconos

      Para desempaquetar utiliza el comando tar:
      >> tar -xvf tema.tar.gz -C /home/usuario/.icons
      */
    static bool installIconTheme(QString &urlPackage);

};

#endif // INSTALLER_H
