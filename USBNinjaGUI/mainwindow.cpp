/*
 * Copyright (C) by Grant Foudree <grant907@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */
 
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList columnTitles;
    columnTitles << "ID" << "Action" << "Date" << "User" << "Label" << "Manufacturer" << "Size";
    ui->treeWidget->setHeaderLabels(columnTitles);

    QMenu *contextMenu = new QMenu(ui->treeWidget);
    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *expandAll = new QAction("Expand all", contextMenu);
    QAction *collapseAll = new QAction("Collapse all", contextMenu);
    ui->treeWidget->addAction(expandAll);
    ui->treeWidget->addAction(collapseAll);
    connect(expandAll, SIGNAL(triggered()), this, SLOT(expandAllHandler()));
    connect(collapseAll, SIGNAL(triggered()), this, SLOT(collapseAllHandler()));

    loadData();
    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->sortItems(0, Qt::AscendingOrder);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearData()
{
    ui->treeWidget->clear();
}

void MainWindow::loadData()
{
    std::vector<logUSB> usb;
    Sql sql;
    sql.dbConnect(LOG_FILE);
    sql.queryLog(&usb);
    sql.dbDisconnect();

    std::vector<logUSB>::iterator usbIt = usb.begin();

    for (int i = usb.size(); usbIt != usb.end(); usbIt++, i--)
    {
        QStringList parentData, childData, childHeader;
        parentData << QString::number(i) << "REJECTED" << usbIt->date.c_str();
        parentData << usbIt->user.c_str() << usbIt->driveLabel.c_str() << usbIt->driveName.c_str();
        parentData << QString::number(usbIt->driveSize) + " MB";

        childHeader << "" << "Drive Letter" << "Serial" << "GUID" << "USBNinja Serial";

        childData << "" << QString(usbIt->driveLetter) << usbIt->driveSerial.c_str();
        childData << usbIt->driveGUID.c_str() << usbIt->usbninjaSerial.c_str();

        QTreeWidgetItem *parentItm = new QTreeWidgetItem(parentData);
        QTreeWidgetItem *childItmHeader = new QTreeWidgetItem(childHeader);
        QTreeWidgetItem *childItm = new QTreeWidgetItem(childData);

        for (int i = 0; i < childItmHeader->columnCount(); i++)
            childItmHeader->setForeground(i, Qt::lightGray);

        parentItm->addChild(childItmHeader);
        parentItm->addChild(childItm);
        ui->treeWidget->insertTopLevelItem(0, parentItm);
    }
    ui->treeWidget->setColumnWidth(0, 40);
    ui->treeWidget->setColumnWidth(3, 150);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(5);
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainWindow::on_actionClear_Log_triggered()
{
    if (QMessageBox::information(this, "Confirm", "Are you sure you want to clear the logs?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        if (!DeleteFileA(LOG_FILE))
        {
            ErrorLog::logErrorToFile(NULL, "Error deleteing logfile", ErrorLog::winErrToStr(GetLastError()));
            QMessageBox::critical(this, "Error", "There was an error deleteing the log file.");
        }
    }
    clearData();
    loadData();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog;
    aboutDialog.setModal(true);
    aboutDialog.exec();
}

void MainWindow::on_actionManage_Devices_triggered()
{
    ManageDrivesDialog manageDrivesDialog;
    manageDrivesDialog.setModal(true);
    manageDrivesDialog.exec();
}

void MainWindow::expandAllHandler()
{
    ui->treeWidget->expandAll();
}

void MainWindow::collapseAllHandler()
{
    ui->treeWidget->collapseAll();
}

void MainWindow::on_actionUnauthorize_All_Devices_triggered()
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Warning!");
    msgBox->setText("You are about to unauthorize ALL devices. No device can " \
                    "access this computer until you authorize it! Are you sure you want to do this?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::No);
    msgBox->setIcon(QMessageBox::Warning);

    if (msgBox->exec() == QMessageBox::Yes)
    {
        Sql sql;
        sql.dbConnect(AUTH_FILE);
        if (sql.dbExecSql("DELETE FROM authDrives;"))
        {
            QMessageBox::information(this, "Success", "All devices have been unauthorized.");
        }
        else
        {
            QMessageBox::critical(this, "Error", "There was an error unauthorizing devices from the local database. Please check the log files.");
        }
        sql.dbDisconnect();
    }
    delete msgBox;
}

void MainWindow::on_actionGraph_Data_triggered()
{
    GraphDialog graphDialog;
    graphDialog.setModal(true);
    graphDialog.exec();
}


void MainWindow::on_actionExport_to_CSV_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as...", NULL, QString("CSV (*.csv);; All files (*.*)"));

    CSVExport csvExp;
    csvExp.csvData();

    if (!csvExp.writeData((char*)fileName.toStdString().c_str()))
    {
        QMessageBox::critical(this, "Error", "There was an error exporting the data to the CSV file.");
    }
}

void MainWindow::on_actionReload_Log_triggered()
{
    clearData();
    loadData();
}
