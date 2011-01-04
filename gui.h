#ifndef GUI_H
#define GUI_H

#include <QtGui>
#include <QtCore>
#include <installer.h>
#include <thread.h>
#include <aparienciagtk.h>

namespace Ui {
    class GUI;
}

class GUI : public QWidget
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();


signals:
    void selectedTheme();
    void selectedIconTheme();


public slots:

    //GENERAL
    void tabChanged(int);


    // PARA EL INSTALADOR

    /**
      Abre un cuadro de dialogo, para seleccionar un fichero *.tar.gz de temas.
      La url lo guarda en la etiqueta del grupo correspondiente
      */
    void selectIconThemeFile();
    /**
      Abre un cuadro de dialogo, para seleccionar un fichero *.tar.gz de temas.
      La url lo guarda en la etiqueta del grupo correspondiente
      */
    void selectThemeFile();
    /**
      Instala el tema de icono a traves del objeto Installer
        Para ello lanza un hilo para que no se bloqué la GUI
      */
    void installThemeIcon();
    /**
      Instala el tema de icono a traves del objeto Installer
      Para ello lanza un hilo para que no se bloqué la GUI
      */
    void installTheme();


    void enableGUIThemeInstaller();
    void disableGUIThemeInstaller();
    void refreshGUITheme();
    void themeAnalisys();
    void checkThemeAnalisys();


    void enableGUIThemeIconInstaller();
    void disableGUIThemeIconInstaller();
    void refreshGUIIconTheme();
    void themeIconAnalisys();
    void checkThemeIconAnalisys();


    //****************************************************
    // METODOS PARA EL AREA DE CONFIGURACION

    void refreshLists();
    void makePreviewIconTheme();
    void applyChanges();
    void appChanged(); //Metodo que se ejecuta cuando un cambio en la gui se ha registrado
    void resetValues(); //Establece los valores por default


    //METODOS PARA EL DESINSTALADOR
    void refresthListsForUninstall();
    void uninstallTheme();
    void uninstallIcon();
    void threadUninstalledThemeFinished();
    void threadUninstalledThemeIconFinished();




private:
    Ui::GUI *ui;
    QPixmap *iconInstaller;
    AparienciaGTK *appareance;

    //PROPIEDADES PARA EL AREA DE INSTALADOR
    Thread *threadForTheme;
    Thread *threadForIcon;
    ThreadAnalisysTheme *threadAnalisysTheme; //hilo para analizar los temas a instalar
    ThreadAnalisysThemeIcon *threadAnalisysThemeIcon; //hilo para analizar los temas a instalar

    //PROPIEDADES PARA EL AREA DE CONGIFURACION
    QFont *font;
    QPixmap *iconoUno;
    QPixmap *iconoDos;
    QPixmap *iconoTres;

    //PROPIEDADES PARA EL DESINSTALADOR
    ThreadErase *threadEraseIcon;
    ThreadErase *threadEraseTheme;
};



#endif // GUI_H
