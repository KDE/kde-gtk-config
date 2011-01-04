#include <QtGui/QApplication>
#include <QtCore>
#include <aparienciagtk.h>
#include <installer.h>
#include <gui.h>
//Prototipos
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    GUI gui;

    gui.show();
    return a.exec();



}



