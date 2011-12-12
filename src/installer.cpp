#include "installer.h"

bool Installer::installTheme(const QString &urlPackage)
{
    QString destino(QDir::homePath()+"/.themes");
    QDir::home().mkpath(destino);

    //TODO: Port to KArchive
    QStringList argumentos;
    argumentos << "-xvf" << urlPackage << "-C" << destino;
    return QProcess::execute("tar", argumentos) == 0;
}

bool Installer::installIconTheme(const QString &urlPackage)
{

    QString destino(QDir::homePath()+"/.icons");
    QDir::home().mkpath(destino);
    
    //TODO: Port to KArchive
    QStringList argumentos;
    argumentos << "-xvf" << urlPackage << "-C" << destino;
    return QProcess::execute("tar", argumentos) == 0;
}
