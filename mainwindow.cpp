/******************************************************************************
** Copyright (C) 2015 Laird
**
** Project: smartBASIC File Includer
**
** Module: mainwindow.cpp
**
** Notes:
**
*******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDropEvent>
#include <QMimeData>
#include <QRegularExpression>

/******************************************************************************/
// Local Functions or Private Members
/******************************************************************************/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //Enable file drops
    setAcceptDrops(true);

    //Setup the GUI
    ui->setupUi(this);

    //Enable file drops
    setAcceptDrops(true);
}

//=============================================================================
//=============================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

//=============================================================================
//=============================================================================
void
MainWindow::dragEnterEvent
    (
    QDragEnterEvent *event
    )
{
    //A file is being dragged onto the window
    if (event->mimeData()->urls().count() >= 1)
    {
        //Nothing is running, serial handle is open and a single file is being dragged - accept action
        event->acceptProposedAction();
    }
}

//=============================================================================
//=============================================================================
void
MainWindow::dropEvent
    (
    QDropEvent *event
    )
{
    //A file has been dragged onto the window - send this file if possible
    QList<QUrl> lstURLs = event->mimeData()->urls();
    if (lstURLs.isEmpty())
    {
        //No files
        ui->edit_Text->appendPlainText("No files were dropped.");
        return;
    }

    //Create output directory
    QFileInfo fiTmpFileInfo(lstURLs.first().toLocalFile());
    QString strOutDir(fiTmpFileInfo.path().append("/Done/"));
    QDir NewDir;
    NewDir.mkdir(strOutDir);

    unsigned int i = 0;
    while (i < lstURLs.count())
    {
        QString strFileName = lstURLs[i].toLocalFile();
        if (strFileName.isEmpty())
        {
            //Invalid filename
            ui->edit_Text->appendPlainText("A file dropped is invalid.");
        }
        else if (!(strFileName.right(3).toLower() == ".sb") && !(strFileName.right(4).toLower() == ".txt"))
        {
            //Not a smartBASIC file
            ui->edit_Text->appendPlainText(QString("Not a sB file: ").append(strFileName));
        }
        else if (!QFile::exists(strFileName))
        {
            //File doesn't exist
            ui->edit_Text->appendPlainText(QString("File does not exist: ").append(strFileName));
        }
        else
        {
            //Open file and read in contents
            QFile file(strFileName);
            if (!file.open(QFile::ReadOnly))
            {
                //Failed to open
                ui->edit_Text->appendPlainText(QString("Unable to open file: ").append(strFileName));
            }
            else
            {
                //Read in file contents
                QByteArray baTmpBA = file.readAll();

                //Close file handle
                file.close();

                //Split the filename path up
                QFileInfo fiFileInfo(strFileName);

                //Parse through the data
                QRegularExpression reTempRE("(^|:)(\\s{0,})#(\\s{0,})include(\\s{1,})\"(.*?)\"");
                reTempRE.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
                bool bChangedState = true;
                bool bFailed = false;
                while (bChangedState == true)
                {
                    bChangedState = false;
                    QRegularExpressionMatchIterator rx1match = reTempRE.globalMatch(baTmpBA);
                    while (rx1match.hasNext())
                    {
                        //Found an include, add the file data
                        QRegularExpressionMatch ThisMatch = rx1match.next();

                        file.setFileName(QString(fiFileInfo.path()).append("/").append(ThisMatch.captured(5)));
                        if (!file.open(QIODevice::ReadOnly))
                        {
                            //Failed to open include file
                            ui->edit_Text->appendPlainText(QString("Invalid include: ").append(ThisMatch.captured(5).append(" in ").append(strFileName)));
                            bFailed = true;
                            break;
                        }

                        bChangedState = true;

                        QByteArray tmpData2;
                        while (!file.atEnd())
                        {
                            tmpData2.append(file.readAll());
                        }
                        file.close();
                        unsigned int i = baTmpBA.indexOf(ThisMatch.captured(0));
                        baTmpBA.remove(i, ThisMatch.captured(0).length());
                        baTmpBA.insert(i, "\r\n");
                        baTmpBA.insert(i+2, tmpData2);
                    }
                }

                if (bFailed == false)
                {
                    //Remove all extra #include statments
                    baTmpBA.replace("#include", "");

                    //
                    ui->edit_Text->appendPlainText(QString("Successfully parsed ").append(strFileName));

                    //Save to new file
                    //strFileName.insert(strFileName.lastIndexOf("."), "_");
                    file.setFileName(QString(strOutDir).append(fiFileInfo.fileName()));
                    if (!file.open(QFile::WriteOnly))
                    {
                        //Failed to open file for writing
                        ui->edit_Text->appendPlainText(QString("Failed to open output file ").append(QString(strOutDir).append(fiFileInfo.fileName())));
                    }
                    else
                    {
                        //Write the data
                        file.write(baTmpBA);
                        file.flush();
                        file.close();

                        //Written data!
                        ui->edit_Text->appendPlainText(QString("Output saved to ").append(QString(strOutDir).append(fiFileInfo.fileName())));
                    }
                }
            }
        }

        //
        ++i;
    }
}

//=============================================================================
//=============================================================================
void
MainWindow::on_btn_Clear_clicked
    (
    )
{
    //Clears the edit box
    ui->edit_Text->clear();
}
