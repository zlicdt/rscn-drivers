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

// ---------------------------------------------------------------------------
// Intel profiles
// ---------------------------------------------------------------------------
QList<DriverProfile> DriverProfileManager::buildIntelProfiles()
{
    QList<DriverProfile> profiles;

    // Modern Intel (Broadwell+): mesa + vulkan-intel + intel-media-driver
    {
        DriverProfile p;
        p.id = "intel-modern";
        p.displayName = "Intel Mesa (Broadwell+)";
        p.requiredPackages = {"mesa", "vulkan-intel", "intel-media-driver"};
        p.optionalPackages = {"lib32-mesa", "lib32-vulkan-intel"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::OpenSource;
        p.recommended = true;
        p.vendor = "Intel";
        p.description = "Open-source Mesa/Vulkan driver for Intel HD/UHD/Iris/Arc GPUs (Broadwell and newer). Recommended.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::IntelBroadwellPlus, GpuArch::IntelArc};
        profiles.append(p);
    }

    // Legacy Intel (pre-Broadwell): mesa + libva-intel-driver
    {
        DriverProfile p;
        p.id = "intel-legacy";
        p.displayName = "Intel Mesa (Legacy)";
        p.requiredPackages = {"mesa", "libva-intel-driver"};
        p.optionalPackages = {"lib32-mesa", "xf86-video-intel"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::OpenSource;
        p.recommended = false;
        p.vendor = "Intel";
        p.description = "Open-source Mesa driver for older Intel GPUs (pre-Broadwell, GMA series).";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::IntelLegacy};
        profiles.append(p);
    }

    return profiles;
}

// ---------------------------------------------------------------------------
// AMD profiles
// ---------------------------------------------------------------------------
QList<DriverProfile> DriverProfileManager::buildAmdProfiles()
{
    QList<DriverProfile> profiles;

    // AMD open-source (GCN+): mesa + xf86-video-amdgpu + vulkan-radeon
    {
        DriverProfile p;
        p.id = "amd-opensource";
        p.displayName = "AMD Mesa / AMDGPU (Open Source)";
        p.requiredPackages = {"mesa", "xf86-video-amdgpu", "vulkan-radeon"};
        p.optionalPackages = {"lib32-mesa", "lib32-vulkan-radeon"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::OpenSource;
        p.recommended = true;
        p.vendor = "AMD";
        p.description = "Open-source AMDGPU kernel driver with Mesa Vulkan. Recommended for GCN and newer.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::AmdGcn, GpuArch::AmdRdna, GpuArch::AmdIntegrated};
        profiles.append(p);
    }

    // AMD legacy (pre-GCN): xf86-video-ati
    {
        DriverProfile p;
        p.id = "amd-legacy";
        p.displayName = "AMD ATI (Legacy)";
        p.requiredPackages = {"mesa", "xf86-video-ati"};
        p.optionalPackages = {"lib32-mesa"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::OpenSource;
        p.recommended = false;
        p.vendor = "AMD";
        p.description = "Open-source ATI driver for pre-GCN AMD/ATI GPUs.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::AmdPreGcn};
        profiles.append(p);
    }

    // AMD PRO (amdgpu-pro from AUR)
    {
        DriverProfile p;
        p.id = "amd-pro";
        p.displayName = "AMDGPU PRO (Proprietary)";
        p.requiredPackages = {"amdgpu-pro-libgl"};
        p.optionalPackages = {"opencl-amd"};
        p.source = PackageSource::AUR;
        p.type = DriverType::Proprietary;
        p.recommended = false;
        p.vendor = "AMD";
        p.description = "Proprietary AMDGPU PRO driver from AUR. For OpenCL support or professional applications.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::AmdGcn, GpuArch::AmdRdna, GpuArch::AmdIntegrated};
        profiles.append(p);
    }

    return profiles;
}

// ---------------------------------------------------------------------------
// NVIDIA profiles
// ---------------------------------------------------------------------------
QList<DriverProfile> DriverProfileManager::buildNvidiaProfiles()
{
    QList<DriverProfile> profiles;

    // NVIDIA proprietary (Maxwell+, DKMS)
    {
        DriverProfile p;
        p.id = "nvidia-proprietary";
        p.displayName = "NVIDIA Proprietary (DKMS)";
        p.requiredPackages = {"nvidia-dkms", "nvidia-utils"};
        p.optionalPackages = {"lib32-nvidia-utils", "nvidia-settings"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::Proprietary;
        p.recommended = true;
        p.vendor = "NVIDIA";
        p.description = "Proprietary NVIDIA driver using DKMS. Recommended for Maxwell (GTX 900) and newer.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::NvidiaMaxwell, GpuArch::NvidiaPascal,
                            GpuArch::NvidiaTuring, GpuArch::NvidiaAmpere,
                            GpuArch::NvidiaAdaLovelace};
        profiles.append(p);
    }

    // NVIDIA proprietary (standard, for linux kernel)
    {
        DriverProfile p;
        p.id = "nvidia-standard";
        p.displayName = "NVIDIA Proprietary (Standard)";
        p.requiredPackages = {"nvidia", "nvidia-utils"};
        p.optionalPackages = {"lib32-nvidia-utils", "nvidia-settings"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::Proprietary;
        p.recommended = false;
        p.vendor = "NVIDIA";
        p.description = "Proprietary NVIDIA driver for the standard linux kernel. Use nvidia-dkms for custom kernels.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::NvidiaMaxwell, GpuArch::NvidiaPascal,
                            GpuArch::NvidiaTuring, GpuArch::NvidiaAmpere,
                            GpuArch::NvidiaAdaLovelace};
        profiles.append(p);
    }

    // NVIDIA LTS kernel
    {
        DriverProfile p;
        p.id = "nvidia-lts";
        p.displayName = "NVIDIA Proprietary (LTS Kernel)";
        p.requiredPackages = {"nvidia-lts", "nvidia-utils"};
        p.optionalPackages = {"lib32-nvidia-utils", "nvidia-settings"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::Proprietary;
        p.recommended = false;
        p.vendor = "NVIDIA";
        p.description = "Proprietary NVIDIA driver for the linux-lts kernel.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::NvidiaMaxwell, GpuArch::NvidiaPascal,
                            GpuArch::NvidiaTuring, GpuArch::NvidiaAmpere,
                            GpuArch::NvidiaAdaLovelace};
        profiles.append(p);
    }

    // NVIDIA legacy Kepler (470xx from AUR)
    {
        DriverProfile p;
        p.id = "nvidia-470xx";
        p.displayName = "NVIDIA 470xx (Kepler Legacy)";
        p.requiredPackages = {"nvidia-470xx-dkms", "nvidia-470xx-utils"};
        p.optionalPackages = {"lib32-nvidia-470xx-utils"};
        p.source = PackageSource::AUR;
        p.type = DriverType::Proprietary;
        p.recommended = false;
        p.vendor = "NVIDIA";
        p.description = "Legacy NVIDIA driver from AUR for GeForce 600/700 series (Kepler).";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {GpuArch::NvidiaKepler};
        profiles.append(p);
    }

    // Nouveau (open-source) — works with all NVIDIA GPUs as fallback
    {
        DriverProfile p;
        p.id = "nvidia-nouveau";
        p.displayName = "Nouveau (Open Source)";
        p.requiredPackages = {"mesa", "xf86-video-nouveau"};
        p.optionalPackages = {"lib32-mesa"};
        p.source = PackageSource::Pacman;
        p.type = DriverType::OpenSource;
        p.recommended = false;
        p.vendor = "NVIDIA";
        p.description = "Open-source Nouveau driver. Lower performance, no advanced GPU features. Fallback option.";
        p.active = false;
        p.installStatus = InstallStatus::NotInstalled;
        p.supportedArchs = {};  // empty = all NVIDIA architectures
        profiles.append(p);
    }

    return profiles;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------
QList<DriverProfile> DriverProfileManager::getProfilesForVendor(const QString &vendor)
{
    if (vendor == "Intel")
        return buildIntelProfiles();
    if (vendor == "AMD")
        return buildAmdProfiles();
    if (vendor == "NVIDIA")
        return buildNvidiaProfiles();
    return {};
}

QList<DriverProfile> DriverProfileManager::getAllProfiles()
{
    QList<DriverProfile> all;
    all.append(buildIntelProfiles());
    all.append(buildAmdProfiles());
    all.append(buildNvidiaProfiles());
    return all;
}

QList<DriverProfile> DriverProfileManager::getProfilesForDevice(
    const GpuDevice &device,
    PackageManager &packageManager)
{
    QList<DriverProfile> allProfiles = getProfilesForVendor(device.vendor);
    QList<DriverProfile> filtered;

    // Filter profiles by supported architecture
    for (auto &profile : allProfiles) {
        // Empty supportedArchs means the profile applies to all architectures of that vendor
        if (!profile.supportedArchs.isEmpty() &&
            !profile.supportedArchs.contains(device.architecture)) {
            continue;
        }

        profile.installStatus = checkInstallStatus(profile, packageManager);
        profile.active = isDriverActive(profile, device);
        filtered.append(profile);
    }

    // If the legacy profile is the right match, mark it as recommended instead of the modern one
    // (e.g., Intel legacy GPU should recommend intel-legacy, not intel-modern)
    bool hasRecommendedActive = false;
    for (const auto &p : filtered) {
        if (p.recommended)
            hasRecommendedActive = true;
    }

    // If no recommended profile matched the architecture filter,
    // promote the first available profile to recommended
    if (!hasRecommendedActive && !filtered.isEmpty()) {
        filtered[0].recommended = true;
    }

    return filtered;
}

InstallStatus DriverProfileManager::checkInstallStatus(
    const DriverProfile &profile,
    PackageManager &packageManager)
{
    if (profile.requiredPackages.isEmpty())
        return InstallStatus::NotInstalled;

    int installedCount = 0;
    for (const QString &pkg : profile.requiredPackages) {
        if (packageManager.isPackageInstalled(pkg))
            ++installedCount;
    }

    if (installedCount == 0)
        return InstallStatus::NotInstalled;
    if (installedCount == profile.requiredPackages.size())
        return InstallStatus::FullyInstalled;
    return InstallStatus::PartiallyInstalled;
}

bool DriverProfileManager::isDriverActive(
    const DriverProfile &profile,
    const GpuDevice &device)
{
    const QString &driver = device.kernelDriver;

    // For NVIDIA proprietary profiles, the kernel driver is "nvidia".
    // We need to disambiguate by checking which package is actually installed.
    if (profile.id == "nvidia-proprietary" || profile.id == "nvidia-standard" || profile.id == "nvidia-lts") {
        // Active only if the kernel driver is nvidia AND this specific profile is fully installed
        return driver == "nvidia" && profile.installStatus == InstallStatus::FullyInstalled;
    }
    if (profile.id == "nvidia-470xx") {
        return driver == "nvidia" && profile.installStatus == InstallStatus::FullyInstalled;
    }
    if (profile.id == "nvidia-nouveau") {
        return driver == "nouveau";
    }

    // For AMD, amdgpu is used by both open-source and PRO.
    // PRO is only active if its packages are installed.
    if (profile.id == "amd-pro") {
        return driver == "amdgpu" && profile.installStatus == InstallStatus::FullyInstalled;
    }
    if (profile.id == "amd-opensource") {
        return driver == "amdgpu";
    }
    if (profile.id == "amd-legacy") {
        return driver == "radeon";
    }

    if (profile.id.startsWith("intel-")) {
        return driver == "i915";
    }

    return false;
}
