#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QBoxLayout>
#include <QTime>
#include <QDebug>
#include <QGroupBox>
#include <QTabWidget>
#include <QFile>

#include <QtGui>
#include <iostream>
#include <fstream>
#include <pwd.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <vector>
#include "process_container.h"
#include <QStyleFactory>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTime time;
    QTimer *update_timer;
    QGroupBox *group;

private:
    vector<float> readCpuStats();
    int getCpuLoad(double dt);

    QLabel *host_label;
    QLabel *user_label;
    QLabel *uptime_label;
    QLabel *cpu_label;
    QLabel *cpu_load_label;
    QLabel *freq_label;
    QLabel *ram_label;
    QLabel *used_ram_label;
    QLabel *discriptors_total;

    QProgressBar *ram_progressBar;
    QProgressBar *cpu_progressBar;

    QTabWidget *tabs;
    QWidget *sys_tab;
    QWidget *proc_tab;

    process_container *pc;

    Ui::MainWindow          *ui;

public slots:
    void update();

};
#endif // MAINWINDOW_H
