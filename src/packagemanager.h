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

class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = nullptr);
    ~PackageManager();

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

    /// Detect AUR helper (yay, paru) available on the system
    QString findAurHelper();

    /// Check if an AUR package is installed
    bool isAurPackageInstalled(const QString &packageName);

signals:
    void operationOutput(const QString &line);
    void operationFinished(bool success);

public slots:
    void installPackages(const QStringList &packages);
    void removePackages(const QStringList &packages);

private:
    /// Run a command synchronously, return (stdout, exitCode)
    QPair<QString, int> runCommand(const QString &command, const QStringList &args) const;
};

#endif // PACKAGEMANAGER_H
