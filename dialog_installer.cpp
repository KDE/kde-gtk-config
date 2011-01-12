#include "dialog_installer.h"

DialogInstaller::DialogInstaller(QWidget *parent): QDialog(parent), ui(new Ui::dialog_installer)
{
    ui->setupUi(this);
    
    //Hilos para el programa
    threadForTheme = new Thread("theme");
    threadForIcon = new Thread("icon");
    threadAnalisysTheme = new ThreadAnalisysTheme;
    threadAnalisysThemeIcon = new ThreadAnalisysThemeIcon;
    
    
    //EVENTOS
    //botones de seleccion de ficheros
    connect(ui->but_theme_filechoice, SIGNAL(clicked()), this, SLOT(selectThemeFile()));
    connect(ui->but_icon_filechoice, SIGNAL(clicked()), this, SLOT(selectIconThemeFile()));

    //botones para instalar los ficheros
    connect(ui->but_icon_install, SIGNAL(clicked()), this, SLOT(installThemeIcon()));
    connect(ui->but_theme_install, SIGNAL(clicked()), this, SLOT(installTheme()));

    //senial para activar boton de instalacion

    //Estas senales son para analizar
    connect(this, SIGNAL(selectedTheme()), this, SLOT(themeAnalisys()));
    connect(this, SIGNAL(selectedIconTheme()), this, SLOT(themeIconAnalisys()));
    connect(threadAnalisysTheme, SIGNAL(finished()), this, SLOT(checkThemeAnalisys()));
    connect(threadAnalisysThemeIcon, SIGNAL(finished()), this, SLOT(checkThemeIconAnalisys()));


    connect(threadForTheme, SIGNAL(started()), this, SLOT(disableGUIThemeInstaller()));
    connect(threadForIcon, SIGNAL(started()), this, SLOT(disableGUIThemeIconInstaller()));
    connect(threadForTheme, SIGNAL(finished()), this, SLOT(enableGUIThemeInstaller()));
    connect(threadForIcon, SIGNAL(finished()), this, SLOT(enableGUIThemeIconInstaller()));

    //conexion cuando se termina de ejecutar la instalacion
    connect(threadForTheme, SIGNAL(finished()), this, SLOT(refreshGUITheme()));
    connect(threadForIcon, SIGNAL(finished()), this, SLOT(refreshGUIIconTheme()));

}

DialogInstaller::~DialogInstaller()
{
    delete ui;
    delete threadAnalisysTheme;
    delete threadAnalisysThemeIcon;
    delete threadForIcon;
    delete threadForTheme;
    
}


// SELECCIONA UN FICHERO TAR.GZ VALIDO
void DialogInstaller::selectThemeFile(){
    qDebug() << "SELECTING GTK THEME";

    QString archivo = QFileDialog::getOpenFileName(
            this,
            i18n("Select a valid gtk theme file"),
            QDir::homePath(),
            i18n("Compressed file (*.tar.gz *.tar)")
            );

    qDebug() << "SELECTED GTK THEME : " << archivo;

    // NO hay nada que hacer si no se selecciono un archivo
    if(archivo.isEmpty())
        return;

    ui->txt_theme_file->setText(archivo);

    //emitimos la senial de que se escogio un archivo
    emit selectedTheme();
}

void DialogInstaller::selectIconThemeFile()
{
    qDebug() << "SELECTING ICONS THEME";
    QString archivo = QFileDialog::getOpenFileName(
            this,
            i18n("Select a vaild icons theme file"),
            QDir::homePath(),
            i18n("Compressed file (*.tar.gz *.tar)")
            );

    qDebug() << "SELECTED ICONS THEME : " << archivo;

    // NO hay nada que hacer si no se selecciono un archivo
    if(archivo.isEmpty())
        return;

    ui->txt_icon_file->setText(archivo);

    //emitimos la senial de que se escogio un archivo
    emit selectedIconTheme();


}

// INSTALA UN TEMA VALIDO
void DialogInstaller::installTheme()
{

    QString archivo = ui->txt_theme_file->text();

    qDebug()<< "*************** GTK THEME INSTALLATION";
    qDebug()<< "File to install" << archivo;

    //Verificamos si existe algo en el campo de texto
    if(archivo.isEmpty()){
        qDebug() << "ERROR: empty text field";
        return;
    }

    //Verificamos si el archivo es valido
    if(!archivo.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: Is not a valid file";
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(archivo);
    if(!fichero.exists()){
        qDebug() << "ERROR: File does not exist";
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: Trying to install a directory";
        return;
    }

    //TODO VALIDADO :D

    threadForTheme->setUrlPackage(archivo);
    threadForTheme->start();

}

void DialogInstaller::installThemeIcon()
{
    QString archivo = ui->txt_icon_file->text();

    qDebug()<< "*************** GTK THEME INSTALLATION";
    qDebug()<< "File to install" << archivo;

    //Verificamos si existe algo en el campo de texto
    if(archivo.isEmpty()){
        qDebug() << "ERROR: empty text field";
        return;
    }

    //Verificamos si el archivo es valido
    if(!archivo.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: it is not a valid file";
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(archivo);
    if(!fichero.exists()){
        qDebug() << "ERROR: This fie does not exist";
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: You try to install a directory";
        return;
    }

    //TODO VALIDADO :D
    threadForIcon->setUrlPackage(archivo);
    threadForIcon->start();


}


//Analisys de los temas a instalar
void DialogInstaller::themeAnalisys()
{

    ui->lb_theme_notice->setText(i18n("Parsing theme ... "));

    //desabilitamos los controles
    ui->but_theme_filechoice->setEnabled(false);
    ui->txt_theme_file->setEnabled(false);
    ui->but_icon_install->setEnabled(false);

    // Verificamos que el archivo a instalar es valido
    QString archivo = ui->txt_theme_file->text();
    threadAnalisysTheme->setPackageTheme(archivo);
    threadAnalisysTheme->start();
}

//metodo que se ejecuta cuando termina el analisis del tema
void DialogInstaller::checkThemeAnalisys()
{

    //habilitamos los controles
    ui->but_theme_filechoice->setEnabled(true);
    ui->txt_theme_file->setEnabled(true);

    if(threadAnalisysTheme->isSuccess()){
        ui->lb_theme_notice->setText(i18n("This Gtk theme can be installed"));
        ui->but_theme_install->setEnabled(true);
    }
    else{
        ui->lb_theme_notice->setText(i18n("This Gtk theme cannot be installed"));
        //desahibilitamos el boton de instalar para que no se pueda instalar
        ui->but_theme_install->setEnabled(false);
    }

}

void DialogInstaller::themeIconAnalisys()
{
    ui->lb_icon_notice->setText(i18n("Parsing theme ... "));

    //desabilitamos los controles
    ui->but_icon_filechoice->setEnabled(false);
    ui->txt_icon_file->setEnabled(false);
    ui->but_icon_install->setDisabled(true);

    // Verificamos que el archivo a instalar es valido
    QString archivo = ui->txt_icon_file->text();
    threadAnalisysThemeIcon->setPackageTheme(archivo);
    threadAnalisysThemeIcon->start();

}

void DialogInstaller::checkThemeIconAnalisys()
{
    //habilitamos los controles
    ui->but_icon_filechoice->setEnabled(true);
    ui->txt_icon_file->setEnabled(true);

    if(threadAnalisysThemeIcon->isSuccess()){
        ui->lb_icon_notice->setText(i18n("This icons theme can be installed"));
        ui->but_icon_install->setEnabled(true);
    }
    else{
        ui->lb_icon_notice->setText(i18n("This icons theme cannot be installed"));
        //desahibilitamos el boton de instalar para que no se pueda instalar
        ui->but_icon_install->setEnabled(false);
    }

}

void DialogInstaller::enableGUIThemeInstaller(){
    ui->but_theme_filechoice->setEnabled(true);
    ui->but_theme_install->setEnabled(true);
    ui->txt_theme_file->setEnabled(true);
   
}

//Cuando un tema de icono se acabo de instalar habilitamos los componentes para una
// nueva instalacion
void DialogInstaller::enableGUIThemeIconInstaller(){
    ui->but_icon_filechoice->setEnabled(true);
    ui->but_icon_install->setEnabled(true);
    ui->txt_icon_file->setEnabled(true);
    
}

void DialogInstaller::disableGUIThemeInstaller(){
    ui->lb_theme_notice->setText("Installing Gtk theme ...");
    ui->but_theme_filechoice->setEnabled(false);
    ui->but_theme_install->setEnabled(false);
    ui->txt_theme_file->setEnabled(false);
    
}
void DialogInstaller::disableGUIThemeIconInstaller(){
    ui->lb_icon_notice->setText("Installing icons ...");
    ui->but_icon_install->setEnabled(false);
    ui->but_icon_filechoice->setEnabled(false);
    ui->txt_icon_file->setEnabled(false);
    
}

void DialogInstaller::refreshGUIIconTheme(){
    if(threadForIcon->isSuccess()){
        ui->lb_icon_notice->setText(i18n("Icons Successfully Installed"));
        ui->txt_icon_file->setText(i18n(""));
    }
    else{
        ui->lb_icon_notice->setText(i18n("The icons theme cannot be installed"));
        ui->txt_icon_file->setText(i18n(""));
    }
}

// Esto se ejecuta cuando un tema es tratado a instalar
void DialogInstaller::refreshGUITheme(){
    if(threadForTheme->isSuccess()){
        ui->lb_theme_notice->setText(i18n("Gtk Theme Sucessfully Installed"));
        ui->txt_theme_file->setText("");
    }
    else{
        ui->lb_theme_notice->setText(i18n("The Gtk theme cannot be installed"));
        ui->txt_theme_file->setText("");
    }
}








