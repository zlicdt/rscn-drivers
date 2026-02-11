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
    QString desc = rawDescription;

    static QRegularExpression pciIdRe(R"(\s*\[\w{4}:\w{4}\])");
    desc.remove(pciIdRe);

    static QRegularExpression revRe(R"(\s*\(rev \w+\))");
    desc.remove(revRe);

    static QRegularExpression classRe(R"(\s*\[\w{4}\])");
    desc.remove(classRe);

    desc = desc.trimmed();

    if (vendor == "NVIDIA") {
        static QRegularExpression bracketModel(R"(\[([^\]]+)\]\s*$)");
        auto match = bracketModel.match(desc);
        if (match.hasMatch())
            return match.captured(1).trimmed();
        desc.remove("NVIDIA Corporation", Qt::CaseInsensitive);
        return desc.trimmed();
    }

    if (vendor == "AMD") {
        static QRegularExpression amdPrefix(R"(^Advanced Micro Devices,?\s*Inc\.?\s*\[AMD/?ATI?\]\s*)");
        desc.remove(amdPrefix);
        return desc.trimmed();
    }

    if (vendor == "Intel") {
        desc.remove("Intel Corporation", Qt::CaseInsensitive);
        return desc.trimmed();
    }

    return desc;
}

// ---------------------------------------------------------------------------
// Architecture detection
// ---------------------------------------------------------------------------

GpuArch HardwareDetector::detectArchitecture(const QString &vendor, const QString &deviceId, const QString &model)
{
    if (vendor == "NVIDIA")
        return detectNvidiaArch(deviceId, model);
    if (vendor == "AMD")
        return detectAmdArch(deviceId, model);
    if (vendor == "Intel")
        return detectIntelArch(deviceId, model);
    return GpuArch::Unknown;
}

GpuArch HardwareDetector::detectNvidiaArch(const QString &deviceId, const QString &model)
{
    // NVIDIA PCI device IDs by architecture generation:
    //   0x0xxx-0x0fff: Tesla/Fermi and older
    //   0x1000-0x133f: Kepler (GK1xx) — GeForce 600/700
    //   0x1340-0x1aff: Maxwell (GM1xx/GM2xx) — GeForce 900
    //   0x1b00-0x1dff: Pascal (GP1xx) — GeForce 10 series
    //   0x1e00-0x21ff: Turing (TU1xx) — GeForce 16/20 series
    //   0x2200-0x25ff: Ampere (GA1xx) — GeForce 30 series
    //   0x2600-0x2fff: Ada Lovelace (AD1xx) — GeForce 40 series

    bool ok = false;
    uint id = deviceId.toUInt(&ok, 16);
    if (ok) {
        if (id >= 0x2600) return GpuArch::NvidiaAdaLovelace;
        if (id >= 0x2200) return GpuArch::NvidiaAmpere;
        if (id >= 0x1e00) return GpuArch::NvidiaTuring;
        if (id >= 0x1b00) return GpuArch::NvidiaPascal;
        if (id >= 0x1340) return GpuArch::NvidiaMaxwell;
        if (id >= 0x1000) return GpuArch::NvidiaKepler;
    }

    // Model name fallback
    QString m = model.toLower();
    if (m.contains("rtx 40") || m.contains("ad1"))   return GpuArch::NvidiaAdaLovelace;
    if (m.contains("rtx 30") || m.contains("ga1"))   return GpuArch::NvidiaAmpere;
    if (m.contains("rtx 20") || m.contains("gtx 16")) return GpuArch::NvidiaTuring;
    if (m.contains("gtx 10") || m.contains("gp1"))   return GpuArch::NvidiaPascal;
    if (m.contains("gtx 9") || m.contains("gm1"))    return GpuArch::NvidiaMaxwell;
    if (m.contains("gtx 7") || m.contains("gtx 6") ||
        m.contains("gt 7")  || m.contains("gt 6"))   return GpuArch::NvidiaKepler;

    return GpuArch::Unknown;
}

GpuArch HardwareDetector::detectAmdArch(const QString &deviceId, const QString &model)
{
    bool ok = false;
    uint id = deviceId.toUInt(&ok, 16);
    QString m = model.toLower();

    // Integrated APUs by codename
    static const QStringList apuCodenames = {
        "raphael", "renoir", "cezanne", "barcelo", "phoenix", "rembrandt",
        "lucienne", "picasso", "raven", "mendocino", "hawk point",
        "strix", "granite ridge"
    };
    for (const QString &name : apuCodenames) {
        if (m.contains(name))
            return GpuArch::AmdIntegrated;
    }

    // RDNA: RX 5000+, Navi
    if (m.contains("navi") || m.contains("rx 5") || m.contains("rx 6") ||
        m.contains("rx 7") || m.contains("rx 8") || m.contains("rx 9"))
        return GpuArch::AmdRdna;

    if (ok) {
        if (id >= 0x7300 && id <= 0x7fff)                     return GpuArch::AmdRdna;
        if ((id >= 0x1500 && id <= 0x16ff) || (id >= 0x1900 && id <= 0x19ff))
            return GpuArch::AmdIntegrated;
    }

    // GCN: Radeon HD 7700+, R7, R9, RX 400/500, Vega
    if (m.contains("vega") || m.contains("rx 4") ||
        m.contains("r9 ") || m.contains("r7 ") ||
        m.contains("hd 77") || m.contains("hd 78") || m.contains("hd 79"))
        return GpuArch::AmdGcn;

    if (ok && id >= 0x6600 && id <= 0x73ff)
        return GpuArch::AmdGcn;

    // Pre-GCN
    if (m.contains("hd ") || m.contains("terascale"))
        return GpuArch::AmdPreGcn;

    return GpuArch::AmdGcn;
}

GpuArch HardwareDetector::detectIntelArch(const QString &deviceId, const QString &model)
{
    bool ok = false;
    uint id = deviceId.toUInt(&ok, 16);
    QString m = model.toLower();

    // Arc discrete
    if (m.contains("arc") || m.contains("alchemist") || m.contains("battlemage"))
        return GpuArch::IntelArc;
    if (ok && id >= 0x5690 && id <= 0x56ff)
        return GpuArch::IntelArc;

    // Broadwell+ keywords
    static const QStringList modernKeywords = {
        "uhd", "iris", "hd 5", "hd 6",
        "skylake", "kaby", "coffee", "comet", "ice lake",
        "tiger", "alder", "raptor", "meteor", "lunar", "arrow", "xe"
    };
    for (const QString &kw : modernKeywords) {
        if (m.contains(kw))
            return GpuArch::IntelBroadwellPlus;
    }

    if (ok && id >= 0x1600)
        return GpuArch::IntelBroadwellPlus;

    // Legacy
    if (m.contains("gma") || m.contains("hd 4") || m.contains("hd 3") || m.contains("hd 2"))
        return GpuArch::IntelLegacy;

    return GpuArch::IntelBroadwellPlus;
}

QString HardwareDetector::archToString(GpuArch arch)
{
    switch (arch) {
    case GpuArch::Unknown:              return "Unknown";
    case GpuArch::IntelLegacy:          return "Intel Legacy (pre-Broadwell)";
    case GpuArch::IntelBroadwellPlus:   return "Intel Broadwell+";
    case GpuArch::IntelArc:             return "Intel Arc";
    case GpuArch::AmdPreGcn:            return "AMD pre-GCN";
    case GpuArch::AmdGcn:              return "AMD GCN";
    case GpuArch::AmdRdna:             return "AMD RDNA";
    case GpuArch::AmdIntegrated:       return "AMD Integrated (APU)";
    case GpuArch::NvidiaKepler:        return "NVIDIA Kepler";
    case GpuArch::NvidiaMaxwell:       return "NVIDIA Maxwell";
    case GpuArch::NvidiaPascal:        return "NVIDIA Pascal";
    case GpuArch::NvidiaTuring:        return "NVIDIA Turing";
    case GpuArch::NvidiaAmpere:        return "NVIDIA Ampere";
    case GpuArch::NvidiaAdaLovelace:   return "NVIDIA Ada Lovelace";
    }
    return "Unknown";
}

// ---------------------------------------------------------------------------
// Command execution & lspci parsing
// ---------------------------------------------------------------------------

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

    static QRegularExpression gpuHeaderRe(
        R"(^(\S+)\s+((?:VGA compatible|3D|Display)\s+controller)\s+\[(\w{4})\]:\s+(.+)$)",
        QRegularExpression::CaseInsensitiveOption
    );

    static QRegularExpression pciIdRe(R"(\[(\w{4}):(\w{4})\])");

    QStringList lines = output.split('\n');

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines[i];
        auto headerMatch = gpuHeaderRe.match(line);
        if (!headerMatch.hasMatch())
            continue;

        GpuDevice gpu;
        gpu.pciSlot = headerMatch.captured(1);
        gpu.deviceClass = headerMatch.captured(2);
        QString fullDesc = headerMatch.captured(4);

        auto pciIdMatch = pciIdRe.match(fullDesc);
        if (pciIdMatch.hasMatch()) {
            gpu.vendorId = pciIdMatch.captured(1);
            gpu.deviceId = pciIdMatch.captured(2);
            gpu.pciId = gpu.vendorId + ":" + gpu.deviceId;
        }

        gpu.vendor = identifyVendor(fullDesc);
        gpu.model = extractModel(fullDesc, gpu.vendor);

        for (int j = i + 1; j < lines.size(); ++j) {
            const QString &subLine = lines[j];
            if (!subLine.startsWith('\t') && !subLine.startsWith("  "))
                break;

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

        // Detect architecture after all device info is gathered
        gpu.architecture = detectArchitecture(gpu.vendor, gpu.deviceId, gpu.model);

        gpus.append(gpu);
    }

    return gpus;
}
