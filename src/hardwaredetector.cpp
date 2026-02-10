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

#include <QProcess>
#include <QRegularExpression>

HardwareDetector::HardwareDetector(QObject *parent)
    : QObject(parent)
{
}

HardwareDetector::~HardwareDetector()
{
}

QList<GpuDevice> HardwareDetector::detectGpus()
{
    QString output = runCommand("lspci", {"-nn", "-k"});
    return parseLspciOutput(output);
}

QString HardwareDetector::identifyVendor(const QString &rawVendor)
{
    QString lower = rawVendor.toLower();
    if (lower.contains("nvidia"))
        return "NVIDIA";
    if (lower.contains("amd") || lower.contains("ati") || lower.contains("radeon"))
        return "AMD";
    if (lower.contains("intel"))
        return "Intel";
    return "Unknown";
}

QString HardwareDetector::extractModel(const QString &rawDescription, const QString &vendor)
{
    // The raw description looks like:
    //   "NVIDIA Corporation AD107M [GeForce RTX 4060 Max-Q / Mobile] [10de:28e0] (rev a1)"
    //   "Advanced Micro Devices, Inc. [AMD/ATI] Raphael [1002:164e] (rev d8)"
    // We want the model part.

    QString desc = rawDescription;

    // Remove PCI ID bracket like [10de:28e0]
    static QRegularExpression pciIdRe(R"(\s*\[\w{4}:\w{4}\])");
    desc.remove(pciIdRe);

    // Remove revision info like (rev a1)
    static QRegularExpression revRe(R"(\s*\(rev \w+\))");
    desc.remove(revRe);

    // Remove class code like [0300]
    static QRegularExpression classRe(R"(\s*\[\w{4}\])");
    desc.remove(classRe);

    desc = desc.trimmed();

    if (vendor == "NVIDIA") {
        // Try to extract the bracketed model name like [GeForce RTX 4060 ...]
        static QRegularExpression bracketModel(R"(\[([^\]]+)\]\s*$)");
        auto match = bracketModel.match(desc);
        if (match.hasMatch()) {
            return match.captured(1).trimmed();
        }
        // Fall back: remove "NVIDIA Corporation" prefix
        desc.remove("NVIDIA Corporation", Qt::CaseInsensitive);
        return desc.trimmed();
    }

    if (vendor == "AMD") {
        // Remove the vendor prefix
        static QRegularExpression amdPrefix(R"(^Advanced Micro Devices,?\s*Inc\.?\s*\[AMD/?ATI?\]\s*)");
        desc.remove(amdPrefix);
        return desc.trimmed();
    }

    if (vendor == "Intel") {
        // Remove "Intel Corporation" prefix
        desc.remove("Intel Corporation", Qt::CaseInsensitive);
        return desc.trimmed();
    }

    return desc;
}

QString HardwareDetector::runCommand(const QString &command, const QStringList &args) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(command, args);
    process.waitForFinished(5000);
    return process.readAllStandardOutput();
}

QList<GpuDevice> HardwareDetector::parseLspciOutput(const QString &output)
{
    QList<GpuDevice> gpus;

    // lspci -nn -k output is structured as blocks separated by empty lines.
    // The first line of each block has the format:
    //   SLOT CLASS: VENDOR DESCRIPTION [VVVV:DDDD] (rev XX)
    // Followed by indented lines:
    //   \tSubsystem: ...
    //   \tKernel driver in use: ...
    //   \tKernel modules: ...

    // We match VGA / 3D / Display controller entries
    static QRegularExpression gpuHeaderRe(
        R"(^(\S+)\s+((?:VGA compatible|3D|Display)\s+controller)\s+\[(\w{4})\]:\s+(.+)$)",
        QRegularExpression::CaseInsensitiveOption
    );

    // PCI ID: [VVVV:DDDD]
    static QRegularExpression pciIdRe(R"(\[(\w{4}):(\w{4})\])");

    QStringList lines = output.split('\n');

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines[i];
        auto headerMatch = gpuHeaderRe.match(line);
        if (!headerMatch.hasMatch())
            continue;

        GpuDevice gpu;
        gpu.pciSlot = headerMatch.captured(1);               // e.g. "01:00.0"
        gpu.deviceClass = headerMatch.captured(2);            // e.g. "VGA compatible controller"
        QString fullDesc = headerMatch.captured(4);           // full description after class

        // Extract PCI vendor:device ID
        auto pciIdMatch = pciIdRe.match(fullDesc);
        if (pciIdMatch.hasMatch()) {
            gpu.vendorId = pciIdMatch.captured(1);
            gpu.deviceId = pciIdMatch.captured(2);
            gpu.pciId = gpu.vendorId + ":" + gpu.deviceId;
        }

        // Identify vendor
        gpu.vendor = identifyVendor(fullDesc);

        // Extract model name
        gpu.model = extractModel(fullDesc, gpu.vendor);

        // Parse subsequent indented lines for this device
        for (int j = i + 1; j < lines.size(); ++j) {
            const QString &subLine = lines[j];
            if (!subLine.startsWith('\t') && !subLine.startsWith("  ")) {
                break;  // end of this block
            }

            QString trimmed = subLine.trimmed();

            if (trimmed.startsWith("Subsystem:")) {
                gpu.subsystem = trimmed.mid(10).trimmed();
            } else if (trimmed.startsWith("Kernel driver in use:")) {
                gpu.kernelDriver = trimmed.mid(21).trimmed();
            } else if (trimmed.startsWith("Kernel modules:")) {
                QString modulesStr = trimmed.mid(15).trimmed();
                gpu.kernelModules = modulesStr.split(',', Qt::SkipEmptyParts);
                for (auto &m : gpu.kernelModules)
                    m = m.trimmed();
            }
        }

        gpus.append(gpu);
    }

    return gpus;
}
