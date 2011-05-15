#ifndef THREAD_H
#define THREAD_H
#include <QtCore>
#include <installer.h>
/**
 Usaremos hilos para que no se bloquee la GUI a la hora de instalar temas
  */
class Thread: public QThread
{

public:
    Thread(QString);
    virtual void run();
    void setUrlPackage(QString); //Establece la url del icono
    bool isSuccess(); //Es true, si completo el trabajo satisfactoriamente
private:
    QString action;
    QString urlPackage;
    bool success;
};

/**
  hilo para el analisys del Tema
  */
class ThreadAnalisysTheme: public QThread
{
public:
    virtual void run();
    bool isSuccess(); //Es true, si el tema es correcto
    void setPackageTheme(QString); //Estableces el tema a analizar
private:
    bool success;
    QString packageTheme;
};

/**
  hilo para el analisys del Tema de Icono
  */
class ThreadAnalisysThemeIcon: public QThread
{
public:
    virtual void run();
    bool isSuccess(); //Es true, si el tema es correcto
    void setPackageTheme(QString); //Estableces el tema a analizar
private:
    bool success;
    QString packageTheme;
};

/**
  Hilo para eliminar los iconos
  */
class ThreadErase: public QThread
{
public:
    virtual void run();
    bool isSuccess();
    void setThemeForErase(QString);
private:
    bool success;
    QString themeForErase;

};

#endif // THREAD_H


