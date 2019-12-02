/*
 *
 */
#include "configsavetest.h"
#include <appearancegtk2.h>
#include <appearancegtk3.h>

#include <QtTest>

QTEST_GUILESS_MAIN(ConfigSaveTest);

ConfigSaveTest::ConfigSaveTest()
{
    QStandardPaths::setTestModeEnabled(true);
}

static void fillValues(QScopedPointer<AbstractAppearance>& a)
{
    a->setTheme("a");
    a->setPrimaryButtonWarpsSlider(true);

    auto a3 = dynamic_cast<AppearanceGTK3*>(a.data());
    if (a3) {
        a3->setApplicationPreferDarkTheme(false);
    }
}

void compareAppearances(QScopedPointer<AbstractAppearance>& reloaded, QScopedPointer<AbstractAppearance>& instance)
{
    QCOMPARE(reloaded->getTheme(), instance->getTheme());
    QCOMPARE(reloaded->getPrimaryButtonWarpsSlider(), instance->getPrimaryButtonWarpsSlider());
}

QByteArray readFile(const QString& path)
{
    QFile f(path);
    if(!f.open(QFile::ReadOnly | QFile::Text))
        return QByteArray();
    return f.readAll();
}

void ConfigSaveTest::testGtk2()
{
    const QString pathA = QDir::current().absoluteFilePath("test-gtk2")
                , pathB = QDir::current().absoluteFilePath("testB-gtk2");

    QScopedPointer<AbstractAppearance> instance(new AppearanceGTK2);
    fillValues(instance);
    QVERIFY(instance->saveSettings(pathA));

    QScopedPointer<AbstractAppearance> reloaded(new AppearanceGTK2);
    QVERIFY(reloaded->loadSettings(pathA));
    compareAppearances(reloaded, instance);
    QVERIFY(reloaded->saveSettings(pathB));
    QCOMPARE(readFile(pathA), readFile(pathB));
}

void ConfigSaveTest::testGtk3()
{
    QScopedPointer<AbstractAppearance> instance(new AppearanceGTK3);
    fillValues(instance);
    const QString pathA = QDir::current().absoluteFilePath("test-gtk3")
                , pathB = QDir::current().absoluteFilePath("testB-gtk3");
    QVERIFY(instance->saveSettings(pathA));

    QScopedPointer<AbstractAppearance> reloaded(new AppearanceGTK3);
    QVERIFY(QFile::exists(pathA));
    QVERIFY(reloaded->loadSettings(pathA));
    compareAppearances(reloaded, instance);
    QVERIFY(reloaded->saveSettings(pathB));

    QCOMPARE(readFile(pathA), readFile(pathB));
}
