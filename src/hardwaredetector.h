/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef HARDWAREDETECTOR_H
#define HARDWAREDETECTOR_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>

/// GPU architecture generation for filtering driver profiles
enum class GpuArch {
    Unknown,
    // Intel
    IntelLegacy,        // GMA, pre-Broadwell
    IntelBroadwellPlus, // Broadwell, Skylake, Kaby Lake, Coffee Lake, Ice Lake, etc.
    IntelArc,           // Arc Alchemist+
    // AMD
    AmdPreGcn,          // TeraScale, pre-GCN (Radeon HD 2000-6000, some 7000)
    AmdGcn,             // GCN 1.0-5.0 (Radeon HD 7700+, R7/R9, RX 400-500, Vega)
    AmdRdna,            // RDNA 1/2/3/4 (RX 5000+)
    AmdIntegrated,      // APU integrated (Vega/RDNA iGPU)
    // NVIDIA
    NvidiaKepler,       // GeForce 600/700 series
    NvidiaMaxwell,      // GeForce 900 series
    NvidiaPascal,       // GeForce 10 series
    NvidiaTuring,       // GeForce 16/20 series
    NvidiaAmpere,       // GeForce 30 series
    NvidiaAdaLovelace,  // GeForce 40 series
};

struct GpuDevice {
    QString pciSlot;       // e.g. "01:00.0"
    QString vendor;        // "Intel", "AMD", "NVIDIA"
    QString model;         // e.g. "GeForce RTX 4060 Max-Q / Mobile"
    QString pciId;         // vendor:device e.g. "10de:28e0"
    QString vendorId;      // e.g. "10de"
    QString deviceId;      // e.g. "28e0"
    QString kernelDriver;  // e.g. "nvidia", "nouveau", "amdgpu", "i915"
    QStringList kernelModules; // available kernel modules
    QString subsystem;     // e.g. "Lenovo Device"
    QString deviceClass;   // e.g. "VGA compatible controller", "3D controller"
    GpuArch architecture;  // detected GPU architecture generation
};

class HardwareDetector : public QObject
{
    Q_OBJECT

public:
    explicit HardwareDetector(QObject *parent = nullptr);
    ~HardwareDetector();

    /// Run lspci and detect all GPU devices
    QList<GpuDevice> detectGpus();

    /// Identify the vendor string from a raw lspci vendor description
    static QString identifyVendor(const QString &rawVendor);

    /// Extract the model name from the full lspci device description
    static QString extractModel(const QString &rawDescription, const QString &vendor);

    /// Detect architecture generation from vendor + PCI device ID + model name
    static GpuArch detectArchitecture(const QString &vendor, const QString &deviceId, const QString &model);

    /// Get a human-readable name for a GPU architecture
    static QString archToString(GpuArch arch);

private:
    /// Run a command and return its stdout
    QString runCommand(const QString &command, const QStringList &args) const;

    /// Parse the full output of `lspci -nn -k` to find GPU entries
    QList<GpuDevice> parseLspciOutput(const QString &output);

    // Architecture detection helpers
    static GpuArch detectNvidiaArch(const QString &deviceId, const QString &model);
    static GpuArch detectAmdArch(const QString &deviceId, const QString &model);
    static GpuArch detectIntelArch(const QString &deviceId, const QString &model);
};

#endif // HARDWAREDETECTOR_H
