#include "dialog_uninstaller.h"
DialogUninstaller::DialogUninstaller(QWidget* parent, AparienciaGTK *app): QDialog(parent), appareance(app), ui(new Ui::dialog_uninstaller)
{
    ui->setupUi(this);
    
    refresthListsForUninstall();
    
    //EVENTOS
    connect(ui->but_uninstall_theme, SIGNAL(clicked()), this, SLOT(uninstallTheme()));
    connect(ui->but_uninstall_icon, SIGNAL(clicked()), this, SLOT(uninstallIcon()));

    threadEraseIcon = new ThreadErase;
    threadEraseTheme = new ThreadErase;

    connect(threadEraseIcon, SIGNAL(started()), this, SLOT(uninstallIcon()));
    connect(threadEraseTheme, SIGNAL(started()), this, SLOT(uninstallTheme()));

    connect(threadEraseIcon, SIGNAL(finished()), this, SLOT(threadUninstalledThemeIconFinished()));
    connect(threadEraseTheme, SIGNAL(finished()), this, SLOT(threadUninstalledThemeFinished()));
}


DialogUninstaller::~DialogUninstaller()
{
    delete ui;
    delete threadEraseIcon;
    delete threadEraseTheme;
}



// DESINTALADOR

void DialogUninstaller::refresthListsForUninstall()
{

    ui->lb_notice_uninstall_icon->setText("");
    ui->lb_notice_uninstall_theme->setText("");
    
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


void DialogUninstaller::uninstallTheme()
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

    

    //Le decimos la hilo que archivos va a eliminar
    threadEraseTheme->setThemeForErase(ubicacion);
    threadEraseTheme->start();


}

void DialogUninstaller::uninstallIcon()
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


    //Le decimos la hilo que archivos va a eliminar
    threadEraseIcon->setThemeForErase(ubicacion);
    threadEraseIcon->start();

}


void DialogUninstaller::threadUninstalledThemeFinished()
{
    if(threadEraseTheme->isSuccess()){
        ui->lb_notice_uninstall_theme->setText(tr("Tema gtk desinstalado satisfactoriamente"));
    }
    else{
        ui->lb_notice_uninstall_theme->setText(tr("No se pudo desinstalar el tema gtk"));
    }

    //habilitamos gui
    ui->cb_uninstall_theme->setEnabled(true);
    ui->but_uninstall_theme->setEnabled(true);

    refresthListsForUninstall();
}

void DialogUninstaller::threadUninstalledThemeIconFinished()
{
    if(threadEraseTheme->isSuccess()){
        ui->lb_notice_uninstall_icon->setText(tr("Iconos Desinstalados satisfactoriamente ..."));
    }
    else{
        ui->lb_notice_uninstall_icon->setText(tr("No se pudo desintalar el tema de iconos"));
    }

    //habilitamos gui
    ui->cb_uninstall_icon->setEnabled(true);
    ui->but_uninstall_icon->setEnabled(true);


    refresthListsForUninstall();
}

