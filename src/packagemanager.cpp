/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "packagemanager.h"

#include <QProcess>
#include <QFileInfo>

PackageManager::PackageManager(QObject *parent)
    : QObject(parent)
{
}

PackageManager::~PackageManager()
{
}

QPair<QString, int> PackageManager::runCommand(const QString &command, const QStringList &args) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(command, args);
    process.waitForFinished(10000);
    return {process.readAllStandardOutput(), process.exitCode()};
}

bool PackageManager::isPackageInstalled(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Q", packageName});
    Q_UNUSED(output);
    return exitCode == 0;
}

bool PackageManager::isPackageAvailable(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Si", packageName});
    Q_UNUSED(output);
    return exitCode == 0;
}

QString PackageManager::installedVersion(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Q", packageName});
    if (exitCode != 0)
        return {};

    // Output format: "package-name version"
    QStringList parts = output.trimmed().split(' ');
    if (parts.size() >= 2)
        return parts.at(1);
    return {};
}

QString PackageManager::availableVersion(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Si", packageName});
    if (exitCode != 0)
        return {};

    // Parse "Version         : x.y.z-r"
    for (const QString &line : output.split('\n')) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("Version")) {
            int colonPos = trimmed.indexOf(':');
            if (colonPos >= 0)
                return trimmed.mid(colonPos + 1).trimmed();
        }
    }
    return {};
}

QStringList PackageManager::filterInstalled(const QStringList &packages)
{
    QStringList installed;
    for (const QString &pkg : packages) {
        if (isPackageInstalled(pkg))
            installed.append(pkg);
    }
    return installed;
}

QStringList PackageManager::filterNotInstalled(const QStringList &packages)
{
    QStringList notInstalled;
    for (const QString &pkg : packages) {
        if (!isPackageInstalled(pkg))
            notInstalled.append(pkg);
    }
    return notInstalled;
}

QString PackageManager::findAurHelper()
{
    // Check common AUR helpers in order of preference
    for (const QString &helper : {"paru", "yay", "pikaur", "trizen"}) {
        auto [output, exitCode] = runCommand("which", {helper});
        Q_UNUSED(output);
        if (exitCode == 0)
            return helper;
    }
    return {};
}

bool PackageManager::isAurPackageInstalled(const QString &packageName)
{
    // AUR packages are still tracked by pacman locally
    return isPackageInstalled(packageName);
}

void PackageManager::installPackages(const QStringList &packages)
{
    Q_UNUSED(packages);
    // TODO: Phase 4 - implement with pkexec and real-time output
}

void PackageManager::removePackages(const QStringList &packages)
{
    Q_UNUSED(packages);
    // TODO: Phase 4 - implement with pkexec and real-time output
}
