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
#include <QList>

#include "hardwaredetector.h"
#include "packagemanager.h"

enum class DriverType {
    OpenSource,
    Proprietary
};

enum class PackageSource {
    Pacman,
    AUR
};

enum class InstallStatus {
    NotInstalled,
    PartiallyInstalled,
    FullyInstalled
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
    bool active;                   // whether this driver is currently in use
    InstallStatus installStatus;   // current install state
};

class DriverProfileManager
{
public:
    /// Get all predefined profiles for a given GPU vendor
    static QList<DriverProfile> getProfilesForVendor(const QString &vendor);

    /// Get all predefined driver profiles
    static QList<DriverProfile> getAllProfiles();

    /// Match and return applicable profiles for a detected GPU, with
    /// install status and active flag populated
    static QList<DriverProfile> getProfilesForDevice(
        const GpuDevice &device,
        PackageManager &packageManager
    );

private:
    /// Build all Intel driver profiles
    static QList<DriverProfile> buildIntelProfiles();

    /// Build all AMD driver profiles
    static QList<DriverProfile> buildAmdProfiles();

    /// Build all NVIDIA driver profiles
    static QList<DriverProfile> buildNvidiaProfiles();

    /// Determine install status of a profile
    static InstallStatus checkInstallStatus(
        const DriverProfile &profile,
        PackageManager &packageManager
    );

    /// Check if this profile's driver is the currently active kernel driver
    static bool isDriverActive(
        const DriverProfile &profile,
        const GpuDevice &device
    );
};

#endif // DRIVERPROFILE_H
