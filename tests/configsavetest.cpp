/*
 *
 */
#include "configsavetest.h"
#include <appearancegtk2.h>
#include <appearancegtk3.h>

#include <QtTest>

QTEST_GUILESS_MAIN(ConfigSaveTest);

void ConfigSaveTest::fillValues(AbstractAppearance* a)
{
    a->setFont("a");
    a->setIcon("a");
    a->setTheme("a");
    a->setToolbarStyle("a");
    a->setIconFallback("a");
    a->setShowIconsInButtons(true);
    a->setShowIconsInMenus(true);
    a->setPrimaryButtonWarpsSlider(true);

    auto a3 = dynamic_cast<AppearanceGTK3*>(a);
    if (a3) {
        a3->setApplicationPreferDarkTheme(false);
    }
}

bool compareAppearances(AbstractAppearance* a, AbstractAppearance* b)
{
    return a->getFont() == b->getFont()
        && a->getIcon() == b->getIcon()
        && a->getTheme() == b->getTheme()
        && a->getToolbarStyle() == b->getToolbarStyle()
        && a->getIconFallback() == b->getIconFallback()
        && a->getShowIconsInButtons() == b->getShowIconsInButtons()
        && a->getShowIconsInMenus() == b->getShowIconsInMenus()
        && a->getPrimaryButtonWarpsSlider() == b->getPrimaryButtonWarpsSlider();
}

QByteArray readFile(const QString& path)
{
    QFile f(path);
    if(!f.open(QFile::ReadOnly | QFile::Text))
        return QByteArray();
    return f.readAll();
}

void ConfigSaveTest::testOpen()
{
    QVector<AbstractAppearance*> instances;
    instances << new AppearanceGTK2 << new AppearanceGTK3;
    fillValues(instances[0]);
    fillValues(instances[1]);
    QVERIFY(instances[0]->saveSettings("test-gtk2"));
    QVERIFY(instances[1]->saveSettings("test-gtk3"));

    QVector<AbstractAppearance*> reloaded;
    reloaded << new AppearanceGTK2 << new AppearanceGTK3;
    QVERIFY(reloaded[0]->loadSettings("test-gtk2"));
    QVERIFY(reloaded[1]->loadSettings("test-gtk3"));
    QVERIFY(compareAppearances(reloaded[0], instances[0]));
    QVERIFY(compareAppearances(reloaded[1], instances[1]));
    QVERIFY(reloaded[0]->saveSettings("testB-gtk2"));
    QVERIFY(reloaded[1]->saveSettings("testB-gtk3"));
    QCOMPARE(readFile("test-gtk2"), readFile("testB-gtk2"));
    QCOMPARE(readFile("test-gtk3"), readFile("testB-gtk3"));
}
