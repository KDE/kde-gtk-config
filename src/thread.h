#ifndef THREAD_H
#define THREAD_H
#include <QtCore>
#include <installer.h>
/**
 Used so that we don't block the GUI when we install the themes
  */
class Thread: public QThread
{
public:
    Thread(const QString& );
    virtual void run();
    void setUrlPackage(const QString& ); //Establece la url del icono
    bool isSuccess() const; //Es true, si completo el trabajo satisfactoriamente
private:
    QString action;
    QString urlPackage;
    bool success;
};

class ThreadAnalisysTheme: public QThread
{
public:
    virtual void run();
    bool isSuccess() const; //Es true, si el tema es correcto
    void setPackageTheme(const QString& ); //Estableces el tema a analizar
private:
    bool success;
    QString packageTheme;
};

class ThreadAnalisysThemeIcon: public QThread
{
public:
    virtual void run();
    bool isSuccess(); //Es true, si el tema es correcto
    void setPackageTheme(const QString& ); //Estableces el tema a analizar
private:
    bool success;
    QString packageTheme;
};

/**
  Thread to erase the icons
  */
class ThreadErase: public QThread
{
public:
    virtual void run();
    bool isSuccess();
    void setThemeForErase(const QString& theme );
private:
    bool success;
    QString themeForErase;

};

#endif // THREAD_H


