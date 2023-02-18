// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "xsettings.h"

#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

#include <csignal>

#include "config_editor/utils.h"

namespace XSettingsEditor
{
namespace
{
void replaceValueInXSettingsdContents(QString &xSettingsdContents, const QString &paramName, const QVariant &paramValue)
{
    const QRegularExpression regExp(paramName + QStringLiteral(" [^\n]*($|\n)"));

    QString newConfigString;
    if (paramValue.type() == QVariant::Type::String) {
        newConfigString = QStringLiteral("%1 \"%2\"\n").arg(paramName, paramValue.toString());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        // XSettigsd does not support 'true' and 'false' as values
        newConfigString = QStringLiteral("%1 %2\n").arg(paramName, QString::number(paramValue.toInt()));
    } else if (paramValue.canConvert(QMetaType::QString)) {
        newConfigString = QStringLiteral("%1 %2\n").arg(paramName, paramValue.toString());
    } else {
        return;
    }

    if (paramValue.isNull()) {
        // unset value
        xSettingsdContents.replace(regExp, QString());
    } else if (xSettingsdContents.contains(regExp)) {
        xSettingsdContents.replace(regExp, newConfigString);
    } else {
        xSettingsdContents = newConfigString + xSettingsdContents;
    }
}

pid_t pidOfXSettingsd()
{
    QProcess pidof;
    pidof.start(QStringLiteral("pidof"), QStringList() << QStringLiteral("-s") << QStringLiteral("xsettingsd"));
    pidof.waitForFinished();
    QString xsettingsdPid = QString(pidof.readAllStandardOutput()).remove('\n');
    return xsettingsdPid.toInt();
}

void reloadXSettingsd()
{
    pid_t xSettingsdPid = pidOfXSettingsd();
    if (xSettingsdPid == 0) {
        QProcess::startDetached(QStandardPaths::findExecutable(QStringLiteral("xsettingsd")), QStringList());
    } else {
        kill(xSettingsdPid, SIGHUP);
    }
}

}

void setValue(const QString &paramName, const QVariant &paramValue)
{
    QString configLocation = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

    QDir xsettingsdPath = configLocation + QStringLiteral("/xsettingsd");
    if (!xsettingsdPath.exists()) {
        xsettingsdPath.mkpath(QStringLiteral("."));
    }

    QString xSettingsdConfigPath = xsettingsdPath.path() + QStringLiteral("/xsettingsd.conf");

    QFile xSettingsdConfig(xSettingsdConfigPath);
    QString xSettingsdConfigContents = Utils::readFileContents(xSettingsdConfig);
    replaceValueInXSettingsdContents(xSettingsdConfigContents, paramName, paramValue);
    xSettingsdConfig.remove();
    xSettingsdConfig.open(QIODevice::WriteOnly | QIODevice::Text);
    xSettingsdConfig.write(xSettingsdConfigContents.toUtf8());
    reloadXSettingsd();
}

void unsetValue(const QString &paramName)
{
    setValue(paramName, QVariant());
}
}
