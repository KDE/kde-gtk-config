#include "installer.h"

bool Installer::installTheme(QString &urlPackage)
{
    QDir destino(QDir::homePath()+"/.themes");
    qDebug() << "INSTALANDO TEMA GTK " << urlPackage << " EN "  << destino.path();

    //Verifica primero si es que existe la carpetas donde se guardan los temas
    if(!destino.exists()){
        //Trata de crear la carpeta de los temas
        qDebug() << "PELIGRO: No existe carpeta donde se almacena los temas GTK, tratando de crearla";
        if(QProcess::execute("mkdir", QStringList(destino.path())) != 0){
            qDebug() << "ERROR: La carpeta " << destino.path() << " no se pudo crear";
            return false;
        }

        qDebug() << "Carpeta Creada satisfactoriamente";

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
        qDebug() << "PELIGRO: No existe carpeta donde se almacena los iconos, tratando de crearla";
        if(QProcess::execute("mkdir", QStringList(destino.path())) != 0){
            qDebug() << "ERROR: La carpeta " << destino.path() << " no se pudo crear";
            return false;
        }

        qDebug() << "Carpeta Creada satisfactoriamente";

    }

    qDebug() << "INSTALANDO TEMA DE ICONOS"<< urlPackage << " EN : " << destino.path();

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
