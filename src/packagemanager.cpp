/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "packagemanager.h"

#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>

// =============================================================================
// Construction / Destruction
// =============================================================================

PackageManager::PackageManager(QObject *parent)
    : QObject(parent)
{
}

PackageManager::~PackageManager()
{
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(3000);
        delete m_process;
        m_process = nullptr;
    }
}

// =============================================================================
// Synchronous helpers
// =============================================================================

QPair<QString, int> PackageManager::runCommand(const QString &command, const QStringList &args) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(command, args);
    process.waitForFinished(10000);
    return {process.readAllStandardOutput(), process.exitCode()};
}

// =============================================================================
// Synchronous query methods (existing)
// =============================================================================

bool PackageManager::isPackageInstalled(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Q", packageName});
    Q_UNUSED(output);
    return exitCode == 0;
}

bool PackageManager::isPackageAvailable(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Si", packageName});
    Q_UNUSED(output);
    return exitCode == 0;
}

QString PackageManager::installedVersion(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Q", packageName});
    if (exitCode != 0)
        return {};

    // Output format: "package-name version"
    QStringList parts = output.trimmed().split(' ');
    if (parts.size() >= 2)
        return parts.at(1);
    return {};
}

QString PackageManager::availableVersion(const QString &packageName)
{
    auto [output, exitCode] = runCommand("pacman", {"-Si", packageName});
    if (exitCode != 0)
        return {};

    // Parse "Version         : x.y.z-r"
    for (const QString &line : output.split('\n')) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("Version")) {
            int colonPos = trimmed.indexOf(':');
            if (colonPos >= 0)
                return trimmed.mid(colonPos + 1).trimmed();
        }
    }
    return {};
}

QStringList PackageManager::filterInstalled(const QStringList &packages)
{
    QStringList installed;
    for (const QString &pkg : packages) {
        if (isPackageInstalled(pkg))
            installed.append(pkg);
    }
    return installed;
}

QStringList PackageManager::filterNotInstalled(const QStringList &packages)
{
    QStringList notInstalled;
    for (const QString &pkg : packages) {
        if (!isPackageInstalled(pkg))
            notInstalled.append(pkg);
    }
    return notInstalled;
}

QString PackageManager::findAurHelper()
{
    if (m_aurHelperDetected)
        return m_cachedAurHelper;

    // Check common AUR helpers in order of preference
    for (const QString &helper : {"paru", "yay", "pikaur", "trizen"}) {
        auto [output, exitCode] = runCommand("which", {helper});
        Q_UNUSED(output);
        if (exitCode == 0) {
            m_cachedAurHelper = helper;
            m_aurHelperDetected = true;
            return helper;
        }
    }

    m_aurHelperDetected = true;
    m_cachedAurHelper.clear();
    return {};
}

bool PackageManager::isAurPackageInstalled(const QString &packageName)
{
    // AUR packages are still tracked by pacman locally
    return isPackageInstalled(packageName);
}

// =============================================================================
// New synchronous query methods
// =============================================================================

bool PackageManager::isPacmanLocked() const
{
    return QFileInfo::exists("/var/lib/pacman/db.lck");
}

bool PackageManager::isNetworkAvailable() const
{
    // Check if a default route exists (simple and dependency-free)
    QProcess process;
    process.start("ip", {"route", "show", "default"});
    process.waitForFinished(5000);
    QString output = process.readAllStandardOutput().trimmed();
    return !output.isEmpty();
}

bool PackageManager::isOperationRunning() const
{
    return m_process != nullptr && m_process->state() != QProcess::NotRunning;
}

bool PackageManager::isKmsHookPresent() const
{
    QFile file("/etc/mkinitcpio.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString content = file.readAll();
    file.close();

    // Look for 'kms' in HOOKS=(...) line
    QRegularExpression re(R"(^HOOKS=\((.+)\))", QRegularExpression::MultilineOption);
    QRegularExpressionMatch match = re.match(content);
    if (match.hasMatch()) {
        QString hooks = match.captured(1);
        return hooks.contains(QRegularExpression(R"(\bkms\b)"));
    }
    return false;
}

QString PackageManager::pkHelperPath()
{
    // 1. Check environment variable (for development/testing)
    QString envPath = qEnvironmentVariable("RSCN_PKHELPER_PATH");
    if (!envPath.isEmpty() && QFileInfo::exists(envPath))
        return envPath;

    // 2. Check relative to application binary (build directory)
    QString appDir = QCoreApplication::applicationDirPath();
    QString localPath = appDir + "/rscn-drivers-pkhelper";
    if (QFileInfo::exists(localPath))
        return localPath;

    // 3. Check in ../lib relative to bin (standard FHS layout)
    QString libPath = appDir + "/../lib/rscn-drivers/rscn-drivers-pkhelper";
    if (QFileInfo::exists(libPath))
        return QFileInfo(libPath).canonicalFilePath();

    // 4. Standard system install path
    return "/usr/lib/rscn-drivers/rscn-drivers-pkhelper";
}

// =============================================================================
// Async process management
// =============================================================================

void PackageManager::setupProcess()
{
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(3000);
        delete m_process;
    }

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_process, &QProcess::readyRead,
            this, &PackageManager::onProcessReadyRead);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &PackageManager::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &PackageManager::onProcessError);
}

void PackageManager::cleanupProcess()
{
    if (m_process) {
        m_process->deleteLater();
        m_process = nullptr;
    }
    m_currentOperation = OperationType::None;
}

void PackageManager::startPrivilegedOperation(const QStringList &helperArgs, OperationType type)
{
    if (isOperationRunning()) {
        emit operationFinished(false, tr("Another operation is already running"));
        return;
    }

    QString helper = pkHelperPath();
    if (!QFileInfo::exists(helper)) {
        emit operationFinished(false,
            tr("Privileged helper script not found at: %1").arg(helper));
        return;
    }

    m_currentOperation = type;
    setupProcess();

    qDebug() << "Starting privileged operation: pkexec" << helper << helperArgs;

    emit operationStarted(type);
    m_process->start("pkexec", QStringList() << helper << helperArgs);
}

void PackageManager::startUserOperation(const QString &command, const QStringList &args, OperationType type)
{
    if (isOperationRunning()) {
        emit operationFinished(false, tr("Another operation is already running"));
        return;
    }

    m_currentOperation = type;
    setupProcess();

    qDebug() << "Starting user operation:" << command << args;

    emit operationStarted(type);
    m_process->start(command, args);
}

void PackageManager::onProcessReadyRead()
{
    if (!m_process)
        return;

    QByteArray data = m_process->readAll();
    if (data.isEmpty())
        return;

    QString text = QString::fromUtf8(data);
    const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
            emit operationOutput(trimmed);
        }
    }
}

void PackageManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    // Read any remaining buffered output
    if (m_process) {
        QByteArray remaining = m_process->readAll();
        if (!remaining.isEmpty()) {
            QString text = QString::fromUtf8(remaining);
            const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
            for (const QString &line : lines) {
                QString trimmed = line.trimmed();
                if (!trimmed.isEmpty())
                    emit operationOutput(trimmed);
            }
        }
    }

    bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);
    QString errorMsg;

    if (exitStatus == QProcess::CrashExit) {
        errorMsg = tr("Operation crashed unexpectedly");
    } else if (exitCode == 126) {
        // pkexec: authorization dismissed
        errorMsg = tr("Authorization was dismissed by the user");
    } else if (exitCode == 127) {
        // pkexec: command not found or not authorized
        errorMsg = tr("Authorization failed or helper not found");
    } else if (exitCode != 0) {
        errorMsg = tr("Operation failed with exit code %1").arg(exitCode);
    }

    cleanupProcess();
    emit operationFinished(success, errorMsg);
}

void PackageManager::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;

    switch (error) {
    case QProcess::FailedToStart:
        errorMsg = tr("Failed to start the operation. "
                       "Make sure pkexec is installed and the helper script is accessible.");
        break;
    case QProcess::Crashed:
        errorMsg = tr("Operation crashed unexpectedly");
        break;
    case QProcess::Timedout:
        errorMsg = tr("Operation timed out");
        break;
    case QProcess::WriteError:
        errorMsg = tr("Write error during operation");
        break;
    case QProcess::ReadError:
        errorMsg = tr("Read error during operation");
        break;
    default:
        errorMsg = tr("An unknown error occurred");
        break;
    }

    cleanupProcess();
    emit operationFinished(false, errorMsg);
}

// =============================================================================
// Async package operations
// =============================================================================

void PackageManager::installPackages(const QStringList &packages)
{
    if (packages.isEmpty()) {
        emit operationFinished(true, {});
        return;
    }

    if (isPacmanLocked()) {
        emit operationFinished(false,
            tr("Pacman database is locked. Is another package manager running?"));
        return;
    }

    if (!isNetworkAvailable()) {
        emit operationFinished(false,
            tr("No network connectivity detected. "
               "A working internet connection is required to install packages."));
        return;
    }

    QStringList args;
    args << "install" << packages;
    startPrivilegedOperation(args, OperationType::PacmanInstall);
}

void PackageManager::removePackages(const QStringList &packages)
{
    if (packages.isEmpty()) {
        emit operationFinished(true, {});
        return;
    }

    if (isPacmanLocked()) {
        emit operationFinished(false,
            tr("Pacman database is locked. Is another package manager running?"));
        return;
    }

    QStringList args;
    args << "remove" << packages;
    startPrivilegedOperation(args, OperationType::PacmanRemove);
}

void PackageManager::installAurPackages(const QStringList &packages)
{
    if (packages.isEmpty()) {
        emit operationFinished(true, {});
        return;
    }

    QString aurHelper = findAurHelper();
    if (aurHelper.isEmpty()) {
        emit operationFinished(false,
            tr("No AUR helper found on the system. "
               "Please install yay or paru to manage AUR packages."));
        return;
    }

    if (isPacmanLocked()) {
        emit operationFinished(false,
            tr("Pacman database is locked. Is another package manager running?"));
        return;
    }

    if (!isNetworkAvailable()) {
        emit operationFinished(false,
            tr("No network connectivity detected. "
               "A working internet connection is required to install packages."));
        return;
    }

    // AUR helpers must run as the current user (not as root).
    // They handle privilege escalation internally via sudo when needed.
    QStringList args;
    args << "-S" << "--noconfirm" << "--needed" << packages;
    startUserOperation(aurHelper, args, OperationType::AurInstall);
}

void PackageManager::removeAurPackages(const QStringList &packages)
{
    if (packages.isEmpty()) {
        emit operationFinished(true, {});
        return;
    }

    if (isPacmanLocked()) {
        emit operationFinished(false,
            tr("Pacman database is locked. Is another package manager running?"));
        return;
    }

    // AUR packages once installed are just regular pacman packages,
    // so we can remove them via the privileged helper using pacman.
    QStringList args;
    args << "remove" << packages;
    startPrivilegedOperation(args, OperationType::AurRemove);
}

// =============================================================================
// Async post-install operations
// =============================================================================

void PackageManager::removeKmsHook()
{
    startPrivilegedOperation({"remove-kms-hook"}, OperationType::RemoveKmsHook);
}

void PackageManager::regenerateInitramfs()
{
    startPrivilegedOperation({"regenerate-initramfs"}, OperationType::RegenerateInitramfs);
}

void PackageManager::regenerateGrubConfig()
{
    startPrivilegedOperation({"regenerate-grub"}, OperationType::RegenerateGrubConfig);
}

void PackageManager::cancelOperation()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        qDebug() << "Canceling current operation";
        m_process->kill();
        m_process->waitForFinished(3000);
        cleanupProcess();
        emit operationFinished(false, tr("Operation was canceled by the user"));
    }
}
