/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "hardwaredetector.h"

HardwareDetector::HardwareDetector(QObject *parent)
    : QObject(parent)
{
}

HardwareDetector::~HardwareDetector()
{
}

QList<GpuDevice> HardwareDetector::detectGpus()
{
    // TODO: implement lspci parsing
    return {};
}

QList<GpuDevice> HardwareDetector::parseLspciOutput(const QString &output)
{
    Q_UNUSED(output);
    // TODO: implement parsing
    return {};
}
