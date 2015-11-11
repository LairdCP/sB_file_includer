/******************************************************************************
** Copyright (C) 2015 Laird
**
** Project: smartBASIC File Includer
**
** Module: mainwindow.h
**
** Notes:
**
*******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/******************************************************************************/
// Include Files
/******************************************************************************/
#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void
    dragEnterEvent
        (
        QDragEnterEvent *event
        );
    void
    dropEvent
        (
        QDropEvent *event
        );

private slots:
    void
    on_btn_Clear_clicked
        (
        );

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
