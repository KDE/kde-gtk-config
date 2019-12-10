/*
 *
 */

#ifndef CONFIGSAVETEST_H
#define CONFIGSAVETEST_H

#include <QObject>

class ConfigSaveTest : public QObject
{
    Q_OBJECT
public:
    ConfigSaveTest();
private Q_SLOTS:
    void testGtk2();
    void testGtk3();
};

#endif // CONFIGSAVETEST_H
