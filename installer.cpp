#include "installer.h"

bool Installer::installTheme(QString &urlPackage)
{
    QDir destino(QDir::homePath()+"/.themes");
    qDebug() << "INSTALLING GTK THEME " << urlPackage << " IN "  << destino.path();

    //Verifica primero si es que existe la carpetas donde se guardan los temas
    if(!destino.exists()){
        //Trata de crear la carpeta de los temas
        qDebug() << "WARNING: The folder for saving the icons does no exist, tying to create it";
        if(QProcess::execute("mkdir", QStringList(destino.path())) != 0){
            qDebug() << "ERROR: Folder " << destino.path() << " cannot be created";
            return false;
        }

        qDebug() << "Folder successfully created";

    }


    // ejecutaremos el siguiente comando: tar -xvf fichero -C destino

    QStringList argumentos;
    argumentos << "-xvf" << urlPackage << "-C" << destino.path();

    /**
      Si execute devuelve 0 significa que el comando se ejecuto satisfactoriamente
      */
    if(QProcess::execute("tar", argumentos) != 0){
        return false;
    }

    return true;
}


bool Installer::installIconTheme(QString &urlPackage)
{


    QDir destino(QDir::homePath()+"/.icons");

    //Verifica primero si es que existe la carpetas donde se guardan los temas
    if(!destino.exists()){
        //Trata de crear la carpeta de los temas
        qDebug() << "WARNING: The folder for saving the icons does no exist, tying to create it";
        if(QProcess::execute("mkdir", QStringList(destino.path())) != 0){
            qDebug() << "ERROR: In folder " << destino.path() << " cannot be created";
            return false;
        }

        qDebug() << "Folder successfully created";

    }

    qDebug() << "INSTALLING ICONS THEME"<< urlPackage << " IN : " << destino.path();

    // Argumentos para el comando tar
    QStringList argumentos;
    argumentos << "-xvf" << urlPackage << "-C" << destino.path();

    /**
      Si execute devuelve 0 significa que el comando se ejecuto satisfactoriamente
      */
    if(QProcess::execute("tar", argumentos) != 0){
        return false;
    }

    return true;

}
