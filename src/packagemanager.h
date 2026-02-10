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

class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = nullptr);
    ~PackageManager();

    bool isPackageInstalled(const QString &packageName);
    bool isPackageAvailable(const QString &packageName);

signals:
    void operationOutput(const QString &line);
    void operationFinished(bool success);

public slots:
    void installPackages(const QStringList &packages);
    void removePackages(const QStringList &packages);

private:
    QString findAurHelper();
};

#endif // PACKAGEMANAGER_H
