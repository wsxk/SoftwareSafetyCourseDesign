#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_gui.h"
#include <QFileDialog>
#include <windows.h>
#include <psapi.h>
#include <thread>

class qt_gui : public QMainWindow
{
    Q_OBJECT

public:
    qt_gui(QWidget *parent = Q_NULLPTR);
    friend void get_hook_message(qt_gui &w);
    HANDLE my_thread = NULL;
private slots:
    void get_file();
    void run_file();
    //void get_hook_info();
private:
    Ui::qt_guiClass ui;
};
