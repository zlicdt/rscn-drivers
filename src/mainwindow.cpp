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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("RSCN Drivers"));
    setMinimumSize(680, 480);
    resize(780, 520);
}

MainWindow::~MainWindow()
{
}
