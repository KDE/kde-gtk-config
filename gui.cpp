#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);

    // ESTABLECEMOS ICONOS
    //Este es el icono que aparace en el instalador, es un caja
    iconInstaller = new QPixmap("/usr/share/icons/oxygen/64x64/apps/utilities-file-archiver.png");
    ui->lb_for_icon->setPixmap(*iconInstaller);



    // EVENTOS PARA EL LADO DE LA CONFIGURACION

    appareance = new AparienciaGTK;
    font = new QFont("Sans Serif");


    //cargamos por primera vez los temas
    refreshLists();
    makePreviewIconTheme();


    //evento para aplicar cambios
    connect(ui->but_apply, SIGNAL(clicked()), this, SLOT(applyChanges()));

    //evento que notifican que hubo cambios en la interfaz grafica
    connect(ui->cb_theme, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_icon_fallback ,SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_font, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->cb_font_style, SIGNAL(currentIndexChanged(int)), this, SLOT(appChanged()));
    connect(ui->spin_font_tam, SIGNAL(valueChanged(int)), this, SLOT(appChanged()));
    connect(ui->but_reset, SIGNAL(clicked()), this, SLOT(resetValues()));
    connect(ui->but_reset, SIGNAL(clicked()), this, SLOT(appChanged()));

    //cuando se activa la pestania hay que refrescar la ventana
    connect(ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    //eventos para hacer previews
    connect(ui->cb_icon_fallback, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));
    connect(ui->cb_icon, SIGNAL(activated(QString)), this, SLOT(makePreviewIconTheme()));



    // VINCULO LOS EVENTOS DE LA APLICACION

    threadForTheme = new Thread("theme");
    threadForIcon = new Thread("icon");
    threadAnalisysTheme = new ThreadAnalisysTheme;
    threadAnalisysThemeIcon = new ThreadAnalisysThemeIcon;

    // EVENTOS PARA EL INSTALADOR

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

    //DESINTALADOR
    connect(ui->but_uninstall_theme, SIGNAL(clicked()), this, SLOT(uninstallTheme()));
    connect(ui->but_uninstall_icon, SIGNAL(clicked()), this, SLOT(uninstallIcon()));

    threadEraseIcon = new ThreadErase;
    threadEraseTheme = new ThreadErase;

    connect(threadEraseIcon, SIGNAL(started()), this, SLOT(uninstallIcon()));
    connect(threadEraseTheme, SIGNAL(started()), this, SLOT(uninstallTheme()));

    connect(threadEraseIcon, SIGNAL(finished()), this, SLOT(threadUninstalledThemeIconFinished()));
    connect(threadEraseTheme, SIGNAL(finished()), this, SLOT(threadUninstalledThemeFinished()));


}

GUI::~GUI()
{
    delete ui;
    delete threadForTheme;
    delete threadForIcon;
    delete appareance;
    delete iconInstaller;
    delete threadAnalisysTheme;
    delete font;
    delete iconoUno;
    delete iconoDos;
    delete iconoTres;
    delete threadEraseIcon;
    delete threadEraseTheme;
}

// SLOTS

// se activa cuando se cambia de pestania
void GUI::tabChanged(int tab)
{
    if(tab == 0){

        refreshLists();
    }

    if(tab == 1){

    }

    if(tab == 2){
        if(ui->tab_uninstall->isEnabled())
            refresthListsForUninstall();
    }
}

// SELECCIONA UN FICHERO TAR.GZ VALIDO
void GUI::selectThemeFile(){
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

void GUI::selectIconThemeFile()
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
void GUI::installTheme()
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

void GUI::installThemeIcon()
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
void GUI::themeAnalisys()
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
void GUI::checkThemeAnalisys()
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

void GUI::themeIconAnalisys()
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

void GUI::checkThemeIconAnalisys()
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

void GUI::enableGUIThemeInstaller(){
    ui->but_theme_filechoice->setEnabled(true);
    ui->but_theme_install->setEnabled(true);
    ui->txt_theme_file->setEnabled(true);
    ui->tab_uninstall->setEnabled(true);
}

//Cuando un tema de icono se acabo de instalar habilitamos los componentes para una
// nueva instalacion
void GUI::enableGUIThemeIconInstaller(){
    ui->but_icon_filechoice->setEnabled(true);
    ui->but_icon_install->setEnabled(true);
    ui->txt_icon_file->setEnabled(true);
    ui->tab_uninstall->setEnabled(true);
}

void GUI::disableGUIThemeInstaller(){
    ui->lb_theme_notice->setText("Instalando el tema Gtk ...");
    ui->but_theme_filechoice->setEnabled(false);
    ui->but_theme_install->setEnabled(false);
    ui->txt_theme_file->setEnabled(false);
    ui->tab_uninstall->setEnabled(false);
}
void GUI::disableGUIThemeIconInstaller(){
    ui->lb_icon_notice->setText("Instalando los iconos ...");
    ui->but_icon_install->setEnabled(false);
    ui->but_icon_filechoice->setEnabled(false);
    ui->txt_icon_file->setEnabled(false);
    ui->tab_uninstall->setEnabled(false);
}

void GUI::refreshGUIIconTheme(){
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
void GUI::refreshGUITheme(){
    if(threadForTheme->isSuccess()){
        ui->lb_theme_notice->setText(tr("Tema Gtk instalado Satifactoriamente"));
        ui->txt_theme_file->setText("");
    }
    else{
        ui->lb_theme_notice->setText(tr("No se Pudo instalar el tema gtk"));
        ui->txt_theme_file->setText("");
    }
}





// ***************** SLOTS PARA EL AREA DE CONFIGURACION

void GUI::refreshLists()
{

    //Alcualizamos el combobox de los temas
    ui->cb_theme->clear(); //limpiamos checkbox
    ui->cb_theme->addItems(appareance->getAvaliableThemes());

    //Actualizamos el combox de los temas de iconos disponibles
    ui->cb_icon->clear(); // limpiamos checkbox
    ui->cb_icon->addItems(appareance->getAvaliableIcons());

    //Actualizamos el combobox de los temas de iconos fallback disponibles
    ui->cb_icon_fallback->clear();
    ui->cb_icon_fallback->addItems(appareance->getAvaliableIcons());

    //Obtenemos el tema instalado


    //Acualizamos los comboboxs con el tema que esta definido desde el archiovo .gtkrc-2.0

    //combobox del tema
    if(!appareance->getTheme().isEmpty()){
        int pos= ui->cb_theme->findText(appareance->getTheme());
        ui->cb_theme->setCurrentIndex(pos);
    }

    //combobox de icono
    if(!appareance->getIcon().isEmpty()){
        int  pos = ui->cb_icon->findText(appareance->getIcon());
        ui->cb_icon->setCurrentIndex(pos);
    }

    //combobox de icono fallback
    if(!appareance->getIconFallBack().isEmpty()){
        int pos = ui->cb_icon_fallback->findText(appareance->getIconFallBack());
        ui->cb_icon_fallback->setCurrentIndex(pos);
    }


    //Trabajamos con la fuente

    QString fuente = appareance->getFont();

    if(!fuente.isEmpty()){

        bool negrita = false, cursiva = false;


        //buscamos bold en la cadena y la reemplazamos
        if(fuente.contains(QRegExp("bold "))){
            fuente.replace(QRegExp("bold "), "");
            negrita = true;
        }

        if(fuente.contains(QRegExp("italic "))){
            fuente.replace(QRegExp("italic "), "");
            cursiva = true;
        }

        //Buscamos la posicion  de algun valor numerico
        int pos = fuente.lastIndexOf(QRegExp(QString("([0-9][0-9]|[0-9])")))-1;
        QString num = fuente.mid(pos); //encontramos los pixeles de la fuente
        QString nombre = fuente.mid(0, pos-1); //Encontramos el nombre de la fuente

        ui->cb_font->setCurrentFont(QFont(nombre));

        //Establecemos el estilo de la fuente
        if(negrita && !cursiva)
            pos = 1;
        else if(!negrita && cursiva)
            pos = 2;
        else if(negrita && cursiva)
            pos = 3;
        else
            pos = 0;

        ui->cb_font_style->setCurrentIndex(pos);

        //Establecemos el tamanio
        ui->spin_font_tam->setValue(num.toInt());


        qDebug() << "FUENTE ACTUALITA:" << fuente << "\n"
               << "num : " << num << " , nombre : " << nombre;

    }

    ui->but_apply->setEnabled(false);



}

//Ofrece una vista previa de iconos
void GUI::makePreviewIconTheme()
{



    //Buscar primero los iconos del tema fallback
    //file:///usr/share/icons/oxygen-refit-2-2.5.0/48x48/places/folder_home.png
    //file:///usr/share/icons/oxygen-refit-2-2.5.0/48x48/places/user-trash.png
    //file:///usr/share/icons/oxygen-refit-2-2.5.0/48x48/actions/document-print.png

    //Borramos iconos anteriores
    ui->lb_prev_1->setPixmap(QPixmap());
    ui->lb_prev_2->setPixmap(QPixmap());
    ui->lb_prev_3->setPixmap(QPixmap());



    //BUSCAMOS LA RUTA DEL TEMA DE ICONOS INSTALADO
    QString icono_fallback = ui->cb_icon_fallback->currentText();
    QString path_fallback;

    foreach(QString i, appareance->getAvaliableIconsPaths()){
        if(i.contains(icono_fallback)){
            path_fallback   = i;
            break;
        }
    }

    qDebug() << "-- URL PATH ICON FALLBACK " << path_fallback;


    //PRIMER ICONO

    QFileInfo temp(path_fallback+"/48x48/places/folder_home.png");
    qDebug() << "******************************** ICONOS FALLBACK"
             << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();

    if(temp.exists()) ui->lb_prev_1->setPixmap(QPixmap(temp.absoluteFilePath()));

    temp.setFile(path_fallback+"/places/48/folder_home.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_1->setPixmap(QPixmap(temp.absoluteFilePath()));

    //SEGUNDO ICONO

    temp.setFile(path_fallback+"/places/48/user-trash.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_2->setPixmap(QPixmap(temp.absoluteFilePath()));

    temp.setFile(path_fallback+"/48x48/places/user-trash.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_2->setPixmap(QPixmap(temp.absoluteFilePath()));

    //TERCER ICONO

    temp.setFile(path_fallback+"/48x48/actions/document-print.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_3->setPixmap(QPixmap(temp.absoluteFilePath()));

    temp.setFile(path_fallback+"/actions/48/document-print.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_3->setPixmap(QPixmap(temp.absoluteFilePath()));
    qDebug() << "\n***********************************************";


    //AHORA TOCA EL TURNO DE LOS ICONOS
    //BUSCAMOS LA RUTA DEL TEMA DE ICONOS INSTALADO
    QString icono = ui->cb_icon->currentText();
    QString path_icon;

    foreach(QString i, appareance->getAvaliableIconsPaths()){
        if(i.contains(icono)){
            path_icon  = i;
            break;
        }
    }

    qDebug() << "-- URL PATH ICON " << path_icon;


    //PRIMER ICONO

    temp.setFile(path_icon+"/48x48/places/folder_home.png");
    qDebug() << "******************************** ICONOS FALLBACK"
             << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();

    if(temp.exists()) ui->lb_prev_1->setPixmap(QPixmap(temp.absoluteFilePath()));

    temp.setFile(path_icon+"/places/48/folder_home.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_1->setPixmap(QPixmap(temp.absoluteFilePath()));

    //SEGUNDO ICONO

    temp.setFile(path_icon+"/places/48/user-trash.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_2->setPixmap(QPixmap(temp.absoluteFilePath()));

    temp.setFile(path_icon+"/48x48/places/user-trash.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_2->setPixmap(QPixmap(temp.absoluteFilePath()));

    //TERCER ICONO

    temp.setFile(path_icon+"/48x48/actions/document-print.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_3->setPixmap(QPixmap(temp.absoluteFilePath()));

    temp.setFile(path_icon+"/actions/48/document-print.png");
    qDebug() << "\nExiste : " << temp.absoluteFilePath() << " : " << temp.exists();
    if(temp.exists()) ui->lb_prev_3->setPixmap(QPixmap(temp.absoluteFilePath()));
    qDebug() << "\n***********************************************";

}

void GUI::applyChanges()
{

    //Establecemos los valores para guardarlo en el archivo .gtkrc-2.0
    appareance->setTheme(ui->cb_theme->currentText());


    //hay que encontrar el path del tema
    foreach(QString i, appareance->getAvaliableThemesPaths()){

        if(i.contains(ui->cb_theme->currentText())){
            appareance->setThemePath(i);
            break;
        }

    }


    appareance->setIcon(ui->cb_icon->currentText());
    appareance->setIconFallBack(ui->cb_icon_fallback->currentText());

    //Establecemos valores den negrita o cursuva
    QString estilo;
    int pos = ui->cb_font_style->currentIndex();

    if(pos == 1)
        estilo = "bold";
    else if(pos == 2)
        estilo = "italic";
    else if(pos == 3)
        estilo = "bold italic";
    else
        estilo = "";

    QString fuente;
    if(estilo.isEmpty()){

        //Esto hace que tenga menos espacios en blanco entre la fuente y el tamaño
        fuente = ui->cb_font->currentFont().family() + " " + QVariant(ui->spin_font_tam->value()).toString();
    }
    else{
        fuente = ui->cb_font->currentFont().family() + " " + estilo + " " + QVariant(ui->spin_font_tam->value()).toString();
    }
    appareance->setFont( fuente );



    qDebug() << "******************************************* INSTALACION :\n"
            << "tema : " << appareance->getTheme() << "\n"
            << "tema path : " << appareance->getThemePath() << "\n"
            << "icono : " << appareance->getIcon() << "\n"
            << "icono fallback : " << appareance->getIconFallBack() << "\n"
            << "font : " << appareance->getFont() << "\n"
            << "********************************************************";

    //Deshabilitamos el boton de aplicar
    ui->but_apply->setEnabled(false);

    //Guardamos configuracion
    if(appareance->saveFileConfig()){

    }
    else{
        QMessageBox::warning(this, "ERROR", "No se pudo guardar sus configuraciones");
    }

}

void GUI::appChanged()
{
   //Habilitamos el boton de instalar
   ui->but_apply->setEnabled(true);
}

void GUI::resetValues()
{
    //Establecemos los valores predeterminados de chakra linux

    ui->cb_theme->setCurrentIndex(0);
    ui->cb_icon->setCurrentIndex(0);
    ui->cb_icon_fallback->setCurrentIndex(0);

    ui->cb_font->setCurrentFont(QFont("Sans Serif"));
    ui->cb_font_style->setCurrentIndex(0);
    ui->spin_font_tam->setValue(12);
}

// DESINTALADOR

void GUI::refresthListsForUninstall()
{



    //Rellenamos campos en los combobox
    QStringList themes = appareance->getAvaliableThemesPaths();

    //validamos que contenga algo la lista themes
    if(themes.length() > 0){

        //Filtramos los temas que estan ubicados en /usr/share/themes
        foreach(QString i, themes){
            if(i.contains("/usr/share/theme"))themes.removeAll(i);
        }
        //Removemos las ruta completa para dejar el puro nombre de los iconos
        for(int i=0; i<themes.length(); i++){
            QDir temp(themes[i]);
            themes[i] = temp.dirName();
        }

        ui->cb_uninstall_theme->clear();
        ui->cb_uninstall_theme->addItems(themes);
    }



    QStringList icons = appareance->getAvaliableIconsPaths();

    //validamos si la lista contiene algo de valor
    if(icons.length() > 0){

        //Filtramos los temas que estan ubicados en /usr/share/icons
        foreach(QString i, icons){
            if(i.contains("/usr/share/icon"))icons.removeAll(i);
        }
        //Removemos las ruta completa para dejar el puro nombre de los iconos
        for(int i=0; i<icons.length(); i++){
            QDir temp(icons[i]);
            icons[i] = temp.dirName();
        }

        ui->cb_uninstall_icon->clear();
        ui->cb_uninstall_icon->addItems(icons);

    }
}


void GUI::uninstallTheme()
{

    //Si el combobox esta vacio no hacer algo
    if(ui->cb_uninstall_theme->count() == 0)
        return;

    QString tema = ui->cb_uninstall_theme->currentText();


    QString ubicacion;
    foreach(QString i, appareance->getAvaliableThemesPaths()){
        if(i.contains(QRegExp("/"+tema+"$"))){
            qDebug() << "Encontrado, elimina : " << i;
            ubicacion = i;
            break;
        }
    }

    if(ubicacion.isEmpty())
        return;


    //Desahibilitamos la GUI
    ui->cb_uninstall_theme->setEnabled(false);
    ui->but_uninstall_theme->setEnabled(false);

    ui->lb_notice_uninstall_theme->setText(tr("Desinstalando el tema gtk ..."));

    ui->tab_install->setDisabled(true);

    //Le decimos la hilo que archivos va a eliminar
    threadEraseIcon->setThemeForErase(ubicacion);
    threadEraseIcon->start();


}

void GUI::uninstallIcon()
{

    //Si el combobox esta vacio no hacer algo
    if(ui->cb_uninstall_icon->count() == 0)
        return;

    QString icono = ui->cb_uninstall_icon->currentText();


    QString ubicacion;
    foreach(QString i, appareance->getAvaliableIconsPaths()){
        if(i.contains(QRegExp("/"+icono+"$"))){
            qDebug() << "Encontrado, elimina : " << i;
            ubicacion = i;
            break;
        }
    }

    if(ubicacion.isEmpty())
        return;

    //Desahibilitamos la GUI
    ui->cb_uninstall_icon->setEnabled(false);
    ui->but_uninstall_icon->setEnabled(false);

    ui->lb_notice_uninstall_icon->setText(tr("Desinstalando iconos ..."));

    ui->tab_install->setDisabled(true);

    //Le decimos la hilo que archivos va a eliminar
    threadEraseIcon->setThemeForErase(ubicacion);
    threadEraseIcon->start();

}


void GUI::threadUninstalledThemeFinished()
{
    if(threadEraseTheme->isSuccess()){
        ui->lb_notice_uninstall_theme->setText(tr("Tema gtk desintalar satisfactoriamente"));
    }
    else{
        ui->lb_notice_uninstall_theme->setText(tr("No se pudo desintalar el tema gtk"));
    }

    //habilitamos gui
    ui->cb_uninstall_theme->setEnabled(true);
    ui->but_uninstall_theme->setEnabled(true);

    ui->tab_install->setDisabled(false);
    refresthListsForUninstall();
}

void GUI::threadUninstalledThemeIconFinished()
{
    if(threadEraseTheme->isSuccess()){
        ui->lb_notice_uninstall_icon->setText(tr("Iconos Desinstalados satisfactoriamente"));
    }
    else{
        ui->lb_notice_uninstall_icon->setText(tr("No se pudo desintalar el tema de iconos"));
    }

    //habilitamos gui
    ui->cb_uninstall_icon->setEnabled(true);
    ui->but_uninstall_icon->setEnabled(true);

    ui->tab_install->setDisabled(false);
    refresthListsForUninstall();
}

