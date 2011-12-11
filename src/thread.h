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
    
    ///sets the icon path
    void setUrlPackage(const QString& );
    
    ///@returns whether it was successful
    bool isSuccess() const;
private:
    QString action;
    QString urlPackage;
    bool success;
};

class ThreadAnalisysTheme: public QThread
{
public:
    virtual void run();
    
    ///@returns whether the theme is correct
    bool isSuccess() const;
    
    ///sets the theme to analyze
    void setPackageTheme(const QString& );
private:
    bool success;
    QString packageTheme;
};

class ThreadAnalisysThemeIcon: public QThread
{
public:
    virtual void run();
    
    ///@returns whether the theme is correct
    bool isSuccess();
    
    ///sets what theme to use
    void setPackageTheme(const QString& );
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


