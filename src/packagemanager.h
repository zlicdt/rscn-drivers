/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>

/// Type of package operation currently running
enum class OperationType {
    None,
    PacmanInstall,
    PacmanRemove,
    AurInstall,
    AurRemove,
    RemoveKmsHook,
    RegenerateInitramfs,
    RegenerateGrubConfig
};

class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = nullptr);
    ~PackageManager();

    // ===== Synchronous query methods =====

    /// Check if a single package is installed locally
    bool isPackageInstalled(const QString &packageName);

    /// Check if a single package is available in the sync repos
    bool isPackageAvailable(const QString &packageName);

    /// Get the installed version of a package, or empty string
    QString installedVersion(const QString &packageName);

    /// Get the available (repo) version of a package, or empty string
    QString availableVersion(const QString &packageName);

    /// Check which packages from a list are installed
    QStringList filterInstalled(const QStringList &packages);

    /// Check which packages from a list are NOT installed
    QStringList filterNotInstalled(const QStringList &packages);

    /// Detect AUR helper (yay, paru, etc.) available on the system
    QString findAurHelper();

    /// Check if an AUR package is installed
    bool isAurPackageInstalled(const QString &packageName);

    /// Check if pacman database is locked (another instance is running)
    bool isPacmanLocked() const;

    /// Check if network connectivity is available
    bool isNetworkAvailable() const;

    /// Check if an async operation is currently running
    bool isOperationRunning() const;

    /// Check if 'kms' hook is present in mkinitcpio.conf HOOKS
    bool isKmsHookPresent() const;

    /// Get the path to the privileged helper script
    static QString pkHelperPath();

signals:
    /// Emitted for each line of output from an async operation
    void operationOutput(const QString &line);

    /// Emitted when an async operation completes
    void operationFinished(bool success, const QString &errorMessage);

    /// Emitted when an async operation starts
    void operationStarted(OperationType type);

public slots:
    // ===== Async pacman operations (with privilege escalation) =====

    /// Install packages via pacman (uses pkexec for root access)
    void installPackages(const QStringList &packages);

    /// Remove packages via pacman (uses pkexec for root access)
    void removePackages(const QStringList &packages);

    // ===== Async AUR operations (runs as current user) =====

    /// Install packages via the detected AUR helper
    void installAurPackages(const QStringList &packages);

    /// Remove AUR packages (falls back to pacman if no AUR helper)
    void removeAurPackages(const QStringList &packages);

    // ===== Async post-install operations (privileged) =====

    /// Remove 'kms' from mkinitcpio.conf HOOKS
    void removeKmsHook();

    /// Regenerate initramfs via mkinitcpio -P
    void regenerateInitramfs();

    /// Regenerate bootloader configuration (GRUB / systemd-boot)
    void regenerateGrubConfig();

    /// Cancel the currently running operation
    void cancelOperation();

private slots:
    void onProcessReadyRead();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    /// Run a command synchronously, return (stdout, exitCode)
    QPair<QString, int> runCommand(const QString &command, const QStringList &args) const;

    /// Start a privileged operation via pkexec + helper script
    void startPrivilegedOperation(const QStringList &helperArgs, OperationType type);

    /// Start a user-level operation (e.g., AUR helper)
    void startUserOperation(const QString &command, const QStringList &args, OperationType type);

    /// Set up the QProcess for a new async operation
    void setupProcess();

    /// Clean up after an operation completes
    void cleanupProcess();

    QProcess *m_process = nullptr;
    OperationType m_currentOperation = OperationType::None;
    QString m_cachedAurHelper;
    bool m_aurHelperDetected = false;
};

#endif // PACKAGEMANAGER_H
