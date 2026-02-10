/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "driverprofile.h"

QList<DriverProfile> DriverProfileManager::getProfilesForVendor(const QString &vendor)
{
    Q_UNUSED(vendor);
    // TODO: implement vendor-based profile lookup
    return {};
}

QList<DriverProfile> DriverProfileManager::getAllProfiles()
{
    // TODO: return all driver profiles
    return {};
}
