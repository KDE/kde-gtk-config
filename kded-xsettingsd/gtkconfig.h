#pragma once

#include <KConfigWatcher>
#include <KDEDModule>

#include "configvalueprovider.h"

class GSDXSettingsManager;

class Q_DECL_EXPORT GtkConfigXSettings : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.GtkConfig.XSettings")

public:
    GtkConfigXSettings(QObject *parent, const QVariantList &args);
    ~GtkConfigXSettings();

    void setGtk2Theme(const QString &themeName, const bool preferDarkTheme) const;
    void setFont() const;
    void setIconTheme() const;
    void setSoundTheme() const;
    void setEventSoundsEnabled() const;
    void setCursorTheme() const;
    void setCursorSize() const;
    void setIconsOnButtons() const;
    void setIconsInMenus() const;
    void setToolbarStyle() const;
    void setScrollbarBehavior() const;
    void setDoubleClickInterval() const;
    void setCursorBlinkRate() const;
    void setWindowDecorationsButtonsOrder() const;
    void setEnableAnimations() const;
    void setGlobalScale() const;
    void setTextScale() const;
    void setColors() const;

    void applyAllSettings() const;

public Q_SLOTS:
    void onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;

private:
    QScopedPointer<ConfigValueProvider> configValueProvider;
    KConfigWatcher::Ptr kdeglobalsConfigWatcher;
    KConfigWatcher::Ptr kwinConfigWatcher;
    KConfigWatcher::Ptr kcminputConfigWatcher;
    GSDXSettingsManager *m_gsdXsettingsManager = nullptr;
};
