/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "mainwindow.h"

#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_detector(new HardwareDetector(this))
    , m_packageManager(new PackageManager(this))
{
    setWindowTitle(tr("RSCN Drivers"));
    setMinimumSize(680, 480);
    resize(780, 520);

    // Trigger hardware scan after the event loop starts
    QTimer::singleShot(0, this, &MainWindow::scanHardware);
}

MainWindow::~MainWindow()
{
}

void MainWindow::scanHardware()
{
    qDebug() << "=== Scanning GPU hardware ===";

    m_gpuDevices = m_detector->detectGpus();

    if (m_gpuDevices.isEmpty()) {
        qDebug() << "No GPU devices detected.";
        return;
    }

    for (const GpuDevice &gpu : m_gpuDevices) {
        qDebug() << "";
        qDebug() << "GPU:" << gpu.vendor << gpu.model;
        qDebug() << "  PCI Slot:      " << gpu.pciSlot;
        qDebug() << "  PCI ID:        " << gpu.pciId;
        qDebug() << "  Kernel Driver: " << gpu.kernelDriver;
        qDebug() << "  Kernel Modules:" << gpu.kernelModules.join(", ");

        // Get matching driver profiles with install status
        QList<DriverProfile> profiles =
            DriverProfileManager::getProfilesForDevice(gpu, *m_packageManager);

        qDebug() << "  Available driver profiles:" << profiles.size();
        for (const DriverProfile &p : profiles) {
            QString status;
            switch (p.installStatus) {
            case InstallStatus::FullyInstalled:
                status = "[Installed]";
                break;
            case InstallStatus::PartiallyInstalled:
                status = "[Partial]";
                break;
            case InstallStatus::NotInstalled:
                status = "[Not Installed]";
                break;
            }

            QString activeStr = p.active ? " (IN USE)" : "";
            QString recStr = p.recommended ? " *Recommended*" : "";
            QString typeStr = (p.type == DriverType::Proprietary) ? "proprietary" : "open-source";

            qDebug().noquote() << "    -" << p.displayName
                               << status << activeStr << recStr
                               << "(" << typeStr << ")";
            qDebug().noquote() << "      Packages:" << p.requiredPackages.join(", ");
        }
    }

    qDebug() << "";
    qDebug() << "=== Scan complete ===";
}
