#include "thread.h"

Thread::Thread(QString accion): action(accion)
{

}

void Thread::run()
{

    if(urlPackage.isEmpty()){
        qDebug() << "*** ERROR: No hay nada que hacer";
        return;
    }

    if(action == "icon"){
        qDebug() << "Instalando tema de iconos";
        // Instalamos el Tema de Icono
        success = Installer::installIconTheme(urlPackage);
    }

    if(action == "theme"){
        qDebug() << "Instalando tema GTK";
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
    qDebug()<< "*************** INSTALACION DE TEMA GTK";
    qDebug()<< "Archivo a instalar" << this->packageTheme;

    //Verificamos si existe algo en el campo de texto
    if(this->packageTheme.isEmpty()){
        qDebug() << "ERROR: campo tema vacio";
        this->success = false;
        return;
    }

    //Verificamos si el archivo es valido
    if(!this->packageTheme.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: El archivo no es valido";
        this->success = false;
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(this->packageTheme);
    if(!fichero.exists()){
        qDebug() << "ERROR: Ese fichero no existe";
        this->success = false;
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: Lo que estas tratando de instalar es un directorio";
        this->success = false;
        return;
    }

    qDebug() << "** EXTRAYENDO ICONOS EN UNA CARPETA TEMPORAL";
    // seguimos extrayendo el tema en la carpeta temporal
    QDir temporal(QDir::tempPath()+"/CGC/theme");

    //Si no existe nuestra carpeta temporal, creada
    if(!temporal.exists()){
        qDebug() << "TEMPORAL NO EXISTE " << temporal.path();

        QStringList arguu;
        arguu << "-p" << temporal.path();

        if(QProcess::execute("mkdir", arguu)!=0){
            qDebug() << "No se pudo crear la carpeta temporal";
            success = false;
            return;
        }

        qDebug() << "CArpeta temporal creada";

    }

    QStringList argumentos;
    argumentos << "-xf" << this->packageTheme  << "-C"  << temporal.path();
    qDebug()<< "** Comando a ejecutar " << "tar " << argumentos.join(" ");

    if(QProcess::execute("tar", argumentos) != 0){
        qDebug() << "ERROR: error ejecutando el comando";
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

    qDebug() << "CARPETA DEL TEMA ENCONTRADO = " << carpeta;


    qDebug() << "\n******* TEMA " << temporal.path()+"/"+carpeta;

    // Tenemos el nombre de la carpeta a analizar
    QDirIterator iterador(temporal.path()+"/"+carpeta);

    bool encontrado = false;
    QString archivito;
    while(iterador.hasNext()){

        QString file = iterador.next();
        qDebug() << file;

        if(file.contains(QRegExp("(gtk-2.0)$"))){
            //archivo gtk-2.0
            qDebug() << "ARCHIVO : " << file;
            encontrado = true;
            archivito = file;
            break;
        }

    }

    qDebug() << "\n*************************\n\n\n\n";

    if(!encontrado){
        qDebug() << ">>>> Archivo no valido";

        //Al final borra los archivos
        qDebug() << "Borrando Temporales";
        QStringList args;
        args << "-rf" << temporal.path();
        if(QProcess::execute("rm", args) != 0){
            qDebug() << "No hubo limpieza";
        }

        qDebug() << "Limpieza completa";
        qDebug()<< temporal.entryList();

        success = false;
        return;
    }


    qDebug() << ">>>> este si es un tema valido : " << archivito;

    //Al final borra los archivos
    qDebug() << "Borrando Temporales";
    QStringList args;
    args << "-rf" << temporal.path();
    if(QProcess::execute("rm", args) != 0){
        qDebug() << "No hubo limpieza";
    }

    qDebug() << "Limpieza completa";
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
    qDebug()<< "*************** INSTALACION DE TEMA GTK";
    qDebug()<< "Archivo a instalar" << this->packageTheme;

    //Verificamos si existe algo en el campo de texto
    if(this->packageTheme.isEmpty()){
        qDebug() << "ERROR: campo tema vacio";
        this->success = false;
        return;
    }

    //Verificamos si el archivo es valido
    if(!this->packageTheme.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: El archivo no es valido";
        this->success = false;
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(this->packageTheme);
    if(!fichero.exists()){
        qDebug() << "ERROR: Ese fichero no existe";
        this->success = false;
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: Lo que estas tratando de instalar es un directorio";
        this->success = false;
        return;
    }

    qDebug() << "** EXTRAYENDO ICONOS EN UNA CARPETA TEMPORAL";
    // seguimos extrayendo el tema en la carpeta temporal
    QDir temporal(QDir::tempPath()+"/CGC/icon");

    //Si no existe nuestra carpeta temporal, creada
    if(!temporal.exists()){
        qDebug() << "TEMPORAL NO EXISTE " << temporal.path();

        QStringList arguu;
        arguu << "-p" << temporal.path();

        if(QProcess::execute("mkdir", arguu)!=0){
            qDebug() << "No se pudo crear la carpeta temporal";
            success = false;
            return;
        }

        qDebug() << "CArpeta temporal creada";

    }

    QStringList argumentos;
    argumentos << "-xf" << this->packageTheme  << "-C"  << temporal.path();
    qDebug()<< "** Comando a ejecutar " << "tar " << argumentos.join(" ");

    if(QProcess::execute("tar", argumentos) != 0){
        qDebug() << "ERROR: error ejecutando el comando";
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

    qDebug() << "CARPETA DEL TEMA ENCONTRADO = " << carpeta;


    qDebug() << "\n******* TEMA " << temporal.path()+"/"+carpeta;

    // Tenemos el nombre de la carpeta a analizar
    QDirIterator iterador(temporal.path()+"/"+carpeta);

    bool encontrado = false;
    QString archivito;
    while(iterador.hasNext()){

        QString file = iterador.next();
        qDebug() << file;

        if(file.contains(QRegExp("(index.theme)$"))){
            //archivo index.theme
            qDebug() << "ARCHIVO : " << file;
            encontrado = true;
            archivito = file;
            break;
        }

    }

    qDebug() << "\n*************************\n\n\n\n";

    if(!encontrado){
        qDebug() << ">>>> Archivo no valido";

        //Al final borra los archivos
        qDebug() << "Borrando Temporales";
        QStringList args;
        args << "-rf" << temporal.path();
        if(QProcess::execute("rm", args) != 0){
            qDebug() << "No hubo limpieza";
        }

        qDebug() << "Limpieza completa";
        qDebug()<< temporal.entryList();

        success = false;
        return;
    }


    qDebug() << ">>>> este si es un tema valido : " << archivito;

    //Al final borra los archivos
    qDebug() << "Borrando Temporales";
    QStringList args;
    args << "-rf" << temporal.path();
    if(QProcess::execute("rm", args) != 0){
        qDebug() << "No hubo limpieza";
    }

    qDebug() << "Limpieza completa";
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

