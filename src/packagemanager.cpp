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

PackageManager::PackageManager(QObject *parent)
    : QObject(parent)
{
}

PackageManager::~PackageManager()
{
}

bool PackageManager::isPackageInstalled(const QString &packageName)
{
    Q_UNUSED(packageName);
    // TODO: implement pacman -Q check
    return false;
}

bool PackageManager::isPackageAvailable(const QString &packageName)
{
    Q_UNUSED(packageName);
    // TODO: implement pacman -Si check
    return false;
}

void PackageManager::installPackages(const QStringList &packages)
{
    Q_UNUSED(packages);
    // TODO: implement package installation
}

void PackageManager::removePackages(const QStringList &packages)
{
    Q_UNUSED(packages);
    // TODO: implement package removal
}

QString PackageManager::findAurHelper()
{
    // TODO: detect yay or paru
    return {};
}
