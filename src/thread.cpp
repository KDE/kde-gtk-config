#include "thread.h"
#include <kdebug.h>

Thread::Thread(const QString& accion)
    : action(accion)
{}

void Thread::run()
{
    if(urlPackage.isEmpty()) {
        kDebug() << "*** ERROR: There's nothing to do";
        return;
    }

    if(action == "icon") {
        kDebug() << "Installing icons theme";
        success = Installer::installIconTheme(urlPackage);
    } else if(action == "theme") {
        kDebug() << "Installing GTK theme";
        success = Installer::installTheme(urlPackage);
    }
}

void Thread::setUrlPackage(const QString& package)
{
    urlPackage = package;
}

bool Thread::isSuccess() const
{
    return success;
}

void ThreadAnalisysTheme::run()
{
    kDebug()<< "File to install" << packageTheme;

    //TODO: port to KArchive
    QFileInfo file(packageTheme);
    if(!file.exists() || file.isDir() || !(file.completeSuffix()=="tar" || file.completeSuffix()=="tar.gz")) {
        kDebug() << "ERROR: " << packageTheme << "is not a valid theme.";
        success = false;
        return;
    }

    kDebug() << "** EXTRACTING ICONS TO A TEMPORAL FOLDER";
    //We proceed unpacking the package in a temporal directory
    QDir temporal(QDir::tempPath()+"/CGC/theme");

    //Make sure it's already created
    temporal.mkpath(temporal.path());
    
    QStringList argus;
    argus << "-xf" << packageTheme  << "-C"  << temporal.path();
    kDebug()<< "** Command to exec " << "tar " << argus.join(" ");

    //TODO: port to KArchive
    if(QProcess::execute("tar", argus) != 0) {
        kDebug() << "ERROR: executing command";
        success = false;
        return;
    }

    // Package extracted in the temp dir. Now we want to know the name
    QString folder=temporal.entryList(QDir::AllDirs|QDir::NoDotAndDotDot).first();

    kDebug() << "FOUND THEME FOLDER = " << folder;
    kDebug() << "\n******* THEME " << temporal.path()+'/'+folder;

    //We know the path of the folder to analyze
    QDirIterator it(temporal.path()+'/'+folder);

    bool found = false;
    while(it.hasNext()) {

        QString file = it.next();
        kDebug() << "trying file" << file;

        if(it.fileName()=="gtkrc") {
            kDebug() << "FILE : " << file;
            found = true;
            break;
        }
    }

    kDebug() << "\n*************************\n\n\n\n";
    success = found;

    //TODO: really? looks to me the if does the same as the else
    if(!found) {
        kDebug() << ">>>> Invalid file";

        //Al final borra los archivos
//TODO: Use KIO
        kDebug() << "Deleting temps";
        QStringList args;
        args << "-rf" << temporal.path();
        if(QProcess::execute("rm", args) != 0) {
            kDebug() << "There was not cleanning";
        }

        kDebug() << "Cleanning complete"<< temporal.entryList();
        return;
    }

    //Al final borra los archivos
    kDebug() << "Deleting temps";
    QStringList args;
    args << "-rf" << temporal.path();
    if(QProcess::execute("rm", args) != 0) {
        kDebug() << "There was not cleanning";
    }

    kDebug() << "Cleanning complete";
    kDebug()<< temporal.entryList();
}

void ThreadAnalisysTheme::setPackageTheme(const QString& theme)
{
  packageTheme = theme;
}

bool ThreadAnalisysTheme::isSuccess() const
{
    return success;
}

void ThreadAnalisysThemeIcon::run()
{
    kDebug()<< "*************** GTK THEME INSTALLATION";
    kDebug()<< "File to install" << packageTheme;

//     TODO: port to KArchive
    QFileInfo file(packageTheme);
    if(!file.exists() || file.isDir() || !(file.completeSuffix()=="tar" || file.completeSuffix()=="tar.gz")) {
        kDebug() << "ERROR: " << packageTheme << "is not a valid theme.";
        success = false;
        return;
    }

    kDebug() << "** EXTRACTING ICONS TO A TEMPORAL FOLDER";
    QDir temporal(QDir::tempPath()+"/CGC/icon");
    temporal.mkpath(temporal.path());

//     TODO: port to KArchive
    QStringList argus;
    argus << "-xf" << packageTheme  << "-C"  << temporal.path();
    kDebug()<< "** Command to exec " << "tar " << argus.join(" ");

    if(QProcess::execute("tar", argus) != 0) {
        kDebug() << "ERROR: executing command";
        success = false;
        return;
    }

    //archive extracted in the temp directory
    QString folder= temporal.entryList(QDir::AllDirs|QDir::NoDotAndDotDot).first();
    kDebug() << "FOUND THEME FOLDER = " << folder;
    kDebug() << "\n******* THEME " << temporal.path()+'/'+folder;

    QDirIterator it(temporal.path()+'/'+folder);

    bool found = false;
    while(it.hasNext()) {

        QString file = it.next();

        if(it.fileName()=="index.theme") {
            //archivo index.theme
            kDebug() << "FILE : " << file;
            found = true;
            break;
        }

    }

    kDebug() << "\n*************************\n\n\n\n";

    if(!found) {
        kDebug() << ">>>> Invalid file";

        //Al final borra los archivos
        kDebug() << "Deleting temps";
        QStringList args;
        args << "-rf" << temporal.path();
        if(QProcess::execute("rm", args) != 0) {
            kDebug() << "There was not cleanning";
        }

        kDebug() << "Cleanning complete";
        kDebug()<< temporal.entryList();

        return;
    }

    //Clean it up
//     TODO: port to KIO
    kDebug() << "Deleting temps";
    QStringList args;
    args << "-rf" << temporal.path();
    if(QProcess::execute("rm", args) != 0) {
        kDebug() << "There was not cleanning";
    }

    kDebug() << "Cleanning complete";
    kDebug()<< temporal.entryList();
    success=found;
}

void ThreadAnalisysThemeIcon::setPackageTheme(const QString& theme)
{
    packageTheme = theme;
}

bool ThreadAnalisysThemeIcon::isSuccess()
{
    return success;
}

bool ThreadErase::isSuccess()
{
    return success;
}

void ThreadErase::setThemeForErase(const QString& theme)
{
    themeForErase = theme;
}

void ThreadErase::run()
{
    QThread::sleep(3);

//  TODO: use kio
    QStringList argumentos;
    argumentos << "-rf" << themeForErase;

    success = QProcess::execute("rm", argumentos) != 0;
}
