#ifndef APPEARANCEGTK3_H
#define APPEARANCEGTK3_H

#include "abstractappearance.h"

class AppearanceGTK3 : public AbstractAppearance
{

public:
    virtual QStringList installedThemes() const;
    virtual bool saveSettings(const QString& file);
    virtual bool loadSettings(const QString& path);
    virtual QString defaultConfigFile() const;
};

#endif // APPEARANCEGTK3_H
