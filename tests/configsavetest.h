/*
 *
 */

#ifndef CONFIGSAVETEST_H
#define CONFIGSAVETEST_H

#include <QObject>

class AbstractAppearance;
class ConfigSaveTest : public QObject
{
    Q_OBJECT
private slots:
    void testOpen();

private:
    void fillValues(AbstractAppearance* a);
};

#endif // CONFIGSAVETEST_H
