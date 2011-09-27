#include "thread.h"

Thread::Thread(QString accion): action(accion)
{

}

void Thread::run()
{

    if(urlPackage.isEmpty()){
        qDebug() << "*** ERROR: There's nothing to do";
        return;
    }

    if(action == "icon"){
        qDebug() << "Installing icons theme";
        // Instalamos el Tema de Icono
        success = Installer::installIconTheme(urlPackage);
    }

    if(action == "theme"){
        qDebug() << "Installing GTK theme";
        success = Installer::installTheme(urlPackage);
    }


}

void Thread::setUrlPackage(QString package)
{
    urlPackage = package;
}

bool Thread::isSuccess()
{
    return this->success;
}




// HILOS PARA EL ANALISIS, class implementation

void ThreadAnalisysTheme::run()
{
    qDebug()<< "*************** GTK THEME INSTALLATION";
    qDebug()<< "File to install" << this->packageTheme;

    //Verificamos si existe algo en el campo de texto
    if(this->packageTheme.isEmpty()){
        qDebug() << "ERROR: empty theme field";
        this->success = false;
        return;
    }

    //Verificamos si el archivo es valido
    if(!this->packageTheme.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: It is not a valid file";
        this->success = false;
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(this->packageTheme);
    if(!fichero.exists()){
        qDebug() << "ERROR: This file does not exist";
        this->success = false;
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: Your are trying to install a directory";
        this->success = false;
        return;
    }

    qDebug() << "** EXTRACTING ICONS TO A TEMPORAL FOLDER";
    // seguimos extrayendo el tema en la carpeta temporal
    QDir temporal(QDir::tempPath()+"/CGC/theme");

    //Si no existe nuestra carpeta temporal, creada
    if(!temporal.exists()){
        qDebug() << "TEMPORAL DOES NOT EXIST " << temporal.path();

        QStringList arguu;
        arguu << "-p" << temporal.path();

        if(QProcess::execute("mkdir", arguu)!=0){
            qDebug() << "Cannot create temporal folder";
            success = false;
            return;
        }

        qDebug() << "Temporal folder as been created";

    }

    QStringList argumentos;
    argumentos << "-xf" << this->packageTheme  << "-C"  << temporal.path();
    qDebug()<< "** Command to exec " << "tar " << argumentos.join(" ");

    if(QProcess::execute("tar", argumentos) != 0){
        qDebug() << "ERROR: executing command";
        this->success = false;
        return;
    }

    // ARCHIVOS EXTRAIDOS en la carpeta temporal ahora obten el nombre de la carpeta

    QString carpeta;

    foreach(QString i, temporal.entryList()){
        if(i == "." || i == "..")
            continue;
        else
            carpeta = i;
    }

    qDebug() << "FOUND THEME FOLDER = " << carpeta;


    qDebug() << "\n******* THEME " << temporal.path()+"/"+carpeta;

    // Tenemos el nombre de la carpeta a analizar
    QDirIterator iterador(temporal.path()+"/"+carpeta);

    bool encontrado = false;
    QString archivito;
    while(iterador.hasNext()){

        QString file = iterador.next();
        qDebug() << file;

        if(file.contains(QRegExp("(gtk-2.0)$"))){
            //archivo gtk-2.0
            qDebug() << "FILE : " << file;
            encontrado = true;
            archivito = file;
            break;
        }

    }

    qDebug() << "\n*************************\n\n\n\n";

    if(!encontrado){
        qDebug() << ">>>> Invalid file";

        //Al final borra los archivos
        qDebug() << "Deleting temps";
        QStringList args;
        args << "-rf" << temporal.path();
        if(QProcess::execute("rm", args) != 0){
            qDebug() << "There was not cleanning";
        }

        qDebug() << "Cleanning complete";
        qDebug()<< temporal.entryList();

        success = false;
        return;
    }


    qDebug() << ">>>> this is a valid theme : " << archivito;

    //Al final borra los archivos
    qDebug() << "Deleting temps";
    QStringList args;
    args << "-rf" << temporal.path();
    if(QProcess::execute("rm", args) != 0){
        qDebug() << "There was not cleanning";
    }

    qDebug() << "Cleanning complete";
    qDebug()<< temporal.entryList();

    this->success = true;
}

void ThreadAnalisysTheme::setPackageTheme(QString theme)
{
  this->packageTheme = theme;
}


bool ThreadAnalisysTheme::isSuccess()
{
    return this->success;
}


// Analisis en tema de icono
void ThreadAnalisysThemeIcon::run()
{
    qDebug()<< "*************** GTK THEME INSTALLATION";
    qDebug()<< "File to install" << this->packageTheme;

    //Verificamos si existe algo en el campo de texto
    if(this->packageTheme.isEmpty()){
        qDebug() << "ERROR: theme field is empty";
        this->success = false;
        return;
    }

    //Verificamos si el archivo es valido
    if(!this->packageTheme.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: Invalid file";
        this->success = false;
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(this->packageTheme);
    if(!fichero.exists()){
        qDebug() << "ERROR: This file does not exist";
        this->success = false;
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: You are trying to install a directory";
        this->success = false;
        return;
    }

    qDebug() << "** EXTRACTING ICONS TO A TEMPORAL FOLDER";
    // seguimos extrayendo el tema en la carpeta temporal
    QDir temporal(QDir::tempPath()+"/CGC/icon");

    //Si no existe nuestra carpeta temporal, creada
    if(!temporal.exists()){
        qDebug() << "TEMP DOES NOT EXIST " << temporal.path();

        QStringList arguu;
        arguu << "-p" << temporal.path();

        if(QProcess::execute("mkdir", arguu)!=0){
            qDebug() << "Cannot create temporal folder";
            success = false;
            return;
        }

        qDebug() << "Temporal folder created";

    }

    QStringList argumentos;
    argumentos << "-xf" << this->packageTheme  << "-C"  << temporal.path();
    qDebug()<< "** Command to exec " << "tar " << argumentos.join(" ");

    if(QProcess::execute("tar", argumentos) != 0){
        qDebug() << "ERROR: executing command";
        this->success = false;
        return;
    }

    // ARCHIVOS EXTRAIDOS en la carpeta temporal ahora obten el nombre de la carpeta

    QString carpeta;

    foreach(QString i, temporal.entryList()){
        if(i == "." || i == "..")
            continue;
        else
            carpeta = i;
    }

    qDebug() << "FOUND THEME FOLDER = " << carpeta;


    qDebug() << "\n******* THEME " << temporal.path()+"/"+carpeta;

    // Tenemos el nombre de la carpeta a analizar
    QDirIterator iterador(temporal.path()+"/"+carpeta);

    bool encontrado = false;
    QString archivito;
    while(iterador.hasNext()){

        QString file = iterador.next();
        qDebug() << file;

        if(file.contains(QRegExp("(index.theme)$"))){
            //archivo index.theme
            qDebug() << "FILE : " << file;
            encontrado = true;
            archivito = file;
            break;
        }

    }

    qDebug() << "\n*************************\n\n\n\n";

    if(!encontrado){
        qDebug() << ">>>> Invalid file";

        //Al final borra los archivos
        qDebug() << "Deleting temps";
        QStringList args;
        args << "-rf" << temporal.path();
        if(QProcess::execute("rm", args) != 0){
            qDebug() << "There was not cleanning";
        }

        qDebug() << "Cleanning complete";
        qDebug()<< temporal.entryList();

        success = false;
        return;
    }


    qDebug() << ">>>> this is a valid theme : " << archivito;

    //Al final borra los archivos
    qDebug() << "Deleting temps";
    QStringList args;
    args << "-rf" << temporal.path();
    if(QProcess::execute("rm", args) != 0){
        qDebug() << "There was not cleanning";
    }

    qDebug() << "Cleanning complete";
    qDebug()<< temporal.entryList();

    this->success = true;
}

void ThreadAnalisysThemeIcon::setPackageTheme(QString theme)
{
  this->packageTheme = theme;
}


bool ThreadAnalisysThemeIcon::isSuccess()
{
    return this->success;
}


//HILO PARA EL BORRADO

bool ThreadErase::isSuccess(){ return success; }
void ThreadErase::setThemeForErase(QString tema){ themeForErase = tema; }

void ThreadErase::run()
{

    QThread::sleep(3);

        //Ejecutar el comando rm -rfv para eliminar archivos
        QStringList argumentos;
        argumentos << "-rf" << themeForErase;

        if(QProcess::execute("rm", argumentos) != 0 ){
           this->success = false;
        }
        else{
            this->success = true;
        }


}

