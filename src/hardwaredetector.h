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

private:
    /// Run a command and return its stdout
    QString runCommand(const QString &command, const QStringList &args) const;

    /// Parse the full output of `lspci -nn -k` to find GPU entries
    QList<GpuDevice> parseLspciOutput(const QString &output);
};

#endif // HARDWAREDETECTOR_H
