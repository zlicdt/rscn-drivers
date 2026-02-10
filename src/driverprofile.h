/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef DRIVERPROFILE_H
#define DRIVERPROFILE_H

#include <QString>
#include <QStringList>

enum class DriverType {
    OpenSource,
    Proprietary
};

enum class PackageSource {
    Pacman,
    AUR
};

struct DriverProfile {
    QString id;                    // unique identifier
    QString displayName;           // human-readable name
    QStringList requiredPackages;   // packages that must be installed
    QStringList optionalPackages;   // packages that are optional
    PackageSource source;          // pacman or AUR
    DriverType type;               // open-source or proprietary
    bool recommended;              // whether this is the recommended driver
    QString vendor;                // "Intel", "AMD", "NVIDIA"
    QString description;           // brief description
};

class DriverProfileManager
{
public:
    static QList<DriverProfile> getProfilesForVendor(const QString &vendor);
    static QList<DriverProfile> getAllProfiles();
};

#endif // DRIVERPROFILE_H
