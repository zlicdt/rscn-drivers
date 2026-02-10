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

struct GpuDevice {
    QString pciSlot;       // e.g. "01:00.0"
    QString vendor;        // "Intel", "AMD", "NVIDIA"
    QString model;         // e.g. "GeForce RTX 3060"
    QString pciId;         // e.g. "10de:2504"
    QString kernelDriver;  // e.g. "nvidia", "nouveau", "amdgpu", "i915"
    QString kernelModule;  // kernel modules available
};

class HardwareDetector : public QObject
{
    Q_OBJECT

public:
    explicit HardwareDetector(QObject *parent = nullptr);
    ~HardwareDetector();

    QList<GpuDevice> detectGpus();

private:
    QList<GpuDevice> parseLspciOutput(const QString &output);
};

#endif // HARDWAREDETECTOR_H
