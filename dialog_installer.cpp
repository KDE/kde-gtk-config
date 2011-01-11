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
    qDebug() << "SELECCIONANDO TEMA GTK";

    QString archivo = QFileDialog::getOpenFileName(
            this,
            tr("Selecciona un archivo de temas gtk valido"),
            QDir::homePath(),
            tr("Archivo Comprimido (*.tar.gz *.tar)")
            );

    qDebug() << "TEMA GTK SELECCIONADO : " << archivo;

    // NO hay nada que hacer si no se selecciono un archivo
    if(archivo.isEmpty())
        return;

    ui->txt_theme_file->setText(archivo);

    //emitimos la senial de que se escogio un archivo
    emit selectedTheme();
}

void DialogInstaller::selectIconThemeFile()
{
    qDebug() << "SELECCIONANDO TEMA DE ICONO";
    QString archivo = QFileDialog::getOpenFileName(
            this,
            tr("Selecciona un archivo de temas de iconos valido"),
            QDir::homePath(),
            tr("Archivo Comprimido (*.tar.gz *.tar)")
            );

    qDebug() << "TEMA DE ICONO SELECCIONADO : " << archivo;

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

    qDebug()<< "*************** INSTALACION DE TEMA GTK";
    qDebug()<< "Archivo a instalar" << archivo;

    //Verificamos si existe algo en el campo de texto
    if(archivo.isEmpty()){
        qDebug() << "ERROR: campo de texto vacio";
        return;
    }

    //Verificamos si el archivo es valido
    if(!archivo.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: El archivo no es valido";
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(archivo);
    if(!fichero.exists()){
        qDebug() << "ERROR: Ese fichero no existe";
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: Lo que estas tratando de instalar es un directorio";
        return;
    }

    //TODO VALIDADO :D

    threadForTheme->setUrlPackage(archivo);
    threadForTheme->start();

}

void DialogInstaller::installThemeIcon()
{
    QString archivo = ui->txt_icon_file->text();

    qDebug()<< "*************** INSTALACION DE TEMA GTK";
    qDebug()<< "Archivo a instalar" << archivo;

    //Verificamos si existe algo en el campo de texto
    if(archivo.isEmpty()){
        qDebug() << "ERROR: campo de texto vacio";
        return;
    }

    //Verificamos si el archivo es valido
    if(!archivo.contains(QRegExp("(.tar.gz|tar)"))){
        qDebug() << "ERROR: El archivo no es valido";
        return;
    }

    //Por si las dudas verifica si el archivo existe
    QFileInfo fichero(archivo);
    if(!fichero.exists()){
        qDebug() << "ERROR: Ese fichero no existe";
        return;
    }

    //Tambien por si las dudas verifica si es un directorio
    if(fichero.isDir()){
        qDebug() << "ERROR: Lo que estas tratando de instalar es un directorio";
        return;
    }

    //TODO VALIDADO :D
    threadForIcon->setUrlPackage(archivo);
    threadForIcon->start();


}


//Analisys de los temas a instalar
void DialogInstaller::themeAnalisys()
{

    ui->lb_theme_notice->setText(tr("Analizando tema ... "));

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
        ui->lb_theme_notice->setText(tr("El tema gtk se puede instalar"));
        ui->but_theme_install->setEnabled(true);
    }
    else{
        ui->lb_theme_notice->setText(tr("El tema gtk que se quiere instalar no es valido"));
        //desahibilitamos el boton de instalar para que no se pueda instalar
        ui->but_theme_install->setEnabled(false);
    }

}

void DialogInstaller::themeIconAnalisys()
{
    ui->lb_icon_notice->setText(tr("Analizando tema ... "));

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
        ui->lb_icon_notice->setText(tr("El tema de iconos se puede instalar"));
        ui->but_icon_install->setEnabled(true);
    }
    else{
        ui->lb_icon_notice->setText(tr("El tema de iconos que se quiere instalar no es valido"));
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
    ui->lb_theme_notice->setText("Instalando el tema Gtk ...");
    ui->but_theme_filechoice->setEnabled(false);
    ui->but_theme_install->setEnabled(false);
    ui->txt_theme_file->setEnabled(false);
    
}
void DialogInstaller::disableGUIThemeIconInstaller(){
    ui->lb_icon_notice->setText("Instalando los iconos ...");
    ui->but_icon_install->setEnabled(false);
    ui->but_icon_filechoice->setEnabled(false);
    ui->txt_icon_file->setEnabled(false);
    
}

void DialogInstaller::refreshGUIIconTheme(){
    if(threadForIcon->isSuccess()){
        ui->lb_icon_notice->setText(tr("Iconos instalados Satisfactoriamente"));
        ui->txt_icon_file->setText(tr(""));
    }
    else{
        ui->lb_icon_notice->setText(tr("No se Pudo instalar el tema de iconos"));
        ui->txt_icon_file->setText(tr(""));
    }
}

// Esto se ejecuta cuando un tema es tratado a instalar
void DialogInstaller::refreshGUITheme(){
    if(threadForTheme->isSuccess()){
        ui->lb_theme_notice->setText(tr("Tema Gtk instalado Satifactoriamente"));
        ui->txt_theme_file->setText("");
    }
    else{
        ui->lb_theme_notice->setText(tr("No se Pudo instalar el tema gtk"));
        ui->txt_theme_file->setText("");
    }
}








