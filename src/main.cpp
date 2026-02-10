/*
 * RSCN Drivers - Driver Manager for RSCN OS
 * Copyright (C) 2026 ReSpring Clips Neko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("RSCN Drivers");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("RSCN");
    app.setWindowIcon(QIcon(":/icons/rscn-drivers.svg"));

    MainWindow window;
    window.show();

    return app.exec();
}
