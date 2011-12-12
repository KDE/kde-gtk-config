#ifndef INSTALLER_H
#define INSTALLER_H
#include <QtCore>
class Installer
{
public:
    /**
      This function will unpack the @p urlPackage GTK theme packages wherever
      it has to be installed
      */
    static bool installTheme(const QString& urlPackage);

    /**
      This function will unpack the @p urlPackage icons theme packages wherever
      it has to be installed
      */
    static bool installIconTheme(const QString& urlPackage);

};

#endif // INSTALLER_H
