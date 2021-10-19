#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    time = QTime::currentTime();
    pc = new process_container;

    tabs = new QTabWidget;
    sys_tab = new QWidget();
    proc_tab = new QWidget();

    host_label = new QLabel;
    user_label = new QLabel;
    uptime_label = new QLabel;
    cpu_label = new QLabel;
    cpu_load_label = new QLabel;
    freq_label = new QLabel;
    ram_label = new QLabel;
    used_ram_label = new QLabel;
    discriptors_total = new QLabel("Total discriptors under use:");

    ram_progressBar = new QProgressBar;
    cpu_progressBar = new QProgressBar;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(host_label);
    layout->addWidget(user_label);
    layout->addWidget(uptime_label);
    layout->addWidget(cpu_label);
    layout->addWidget(cpu_load_label);
    layout->addWidget(cpu_progressBar);
    layout->addWidget(freq_label);
    layout->addWidget(ram_label);
    layout->addWidget(used_ram_label);
    layout->addWidget(ram_progressBar);
    layout->addWidget(discriptors_total);
    sys_tab->setLayout(layout);



    tabs->addTab(sys_tab, "System information");
    tabs->addTab(pc, "Processes");

    QHBoxLayout *main_layout = new QHBoxLayout;
    main_layout->addWidget(tabs);
    ui->centralwidget->setLayout(main_layout);

    update();

    QTimer *update_timer = new QTimer();

    connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));

    update_timer->start(1000);
}

void MainWindow::update()
{
    ifstream stream("/proc/sys/kernel/hostname"); string str;
    QStringList temp_strList;
    QString discr_tot;
    int brake_index;
    QFile *file = new QFile("/proc/sys/kernel/hostname");
    int temp;
    if(file->open(QIODevice::ReadOnly))
    {
        host_label->setText("Hostname: " + file->readAll());
    }
    else
    {
        qDebug() << "Hostname error!" << "Unable to open /proc/sys/kernel/hostname";
        host_label->setText("Hostname: ");
    }

    QString name = qgetenv("USER");
    if (name.isEmpty())
    {
        name = qgetenv("USERNAME");
        qDebug() << "Username error!";
    }
    else
    {
        user_label->setText("User: " + name);
    }

    time = QTime::currentTime();
    QString e = QString::number(time.hour()) +  QString(" h. ") + QString::number(time.minute()) + QString(" m. ")
            + QString::number(time.second()) + QString(" s.");
    uptime_label->setText("Uptime: " + e);
    file->close();
    file->setFileName("/proc/cpuinfo");
    name.clear();
    e.clear();

    if(file->open(QIODevice::ReadOnly))
    {
        name = file->readAll();
        temp = name.indexOf("model name");
        name.remove(0, temp);
        temp = name.indexOf(':');
        name.remove(0, temp + 2);
        e = name;
        temp = e.indexOf("stepping");
        e.remove(temp, (e.size() - 1));
        cpu_label->setText("CPU: " + e);

        temp = name.indexOf("cpu MHz");
        name.remove(0, temp);
        e = name;
        temp = e.indexOf(':');
        e.remove(0, temp + 2);
        temp = e.indexOf("cache");
        e.remove(temp, (e.size() - 1));
        freq_label->setText("Frequency: " + e + "MHz");
    }
    else
    {
        qDebug() << "CPU error!" << "Unable to open /proc/cpuinfo";
    }

    cpu_progressBar->setValue(getCpuLoad(0.3));
    file->close();
    file->setFileName("/proc/meminfo");
    if(!file->open(QIODevice::ReadOnly))
        qDebug() << "MEM error!" << "Unable to open /proc/cpuinfo";
    name.clear();
    name = file->readLine();
    e.clear();
    temp_strList = name.split(' ');
    temp_strList.removeDuplicates();
    int num = temp_strList[2].toInt();
    int percent = num / 100;
    int gb = (num / 1024) / 1024;
    int mb = (num-gb*1024*1024) /1024;
    int kb = (num - (gb*1024*1024+mb*1024));
    if (gb > 0)
        e = QString::number(gb) + QString(" Gb ");
    else
        e = QString("");
    if (mb > 0)
        e += QString::number(mb) + QString(" Mb ");
    if (kb > 0)
        e += QString::number(kb) + QString(" Kb ");
    ram_label->setText("RAM: " + e);
    int free = 0;
    name.clear();
    e.clear();
    temp_strList.clear();

    for (int i = 0 ; i < 3 ; i++) {
        name = file->readLine();
        temp_strList = name.split(' ');
        temp_strList.removeDuplicates();
        free += temp_strList[2].toInt();
    }

    num -= free;
    gb = num / 1024 / 1024;
    mb = (num - gb*1024*1024) / 1024;
    kb = (num - ((mb*1024) + (gb * 1024 * 1024)));
    if (gb > 0)
        e = QString::number(gb) + QString(" Gb ");
    else
        e = QString("");
    if (mb > 0)
        e += QString::number(mb) + QString(" Mb ");
    if (kb > 0)
        e += QString::number(kb) + QString(" Kb ");
    used_ram_label->setText("RAM Load: " + e);
    percent = num / percent; ram_progressBar->setValue(percent);
    file->close();

    file->setFileName("/proc/sys/fs/file-nr");
    if(!file->open(QIODevice::ReadOnly))
        qDebug() << "PROCESS ERROR!" << "Unable to open entry";
    discr_tot = file->readAll();
    brake_index = discr_tot.size() - discr_tot.indexOf("\t", 0);
    discr_tot.chop(brake_index);
    discriptors_total->setText("Total discriptors under use: " + discr_tot);
    file->close();
}

vector<float> MainWindow::readCpuStats()
{
    vector<float> ret;
    QFile *file = new QFile("/proc/stat");
    QString temp_1;
    QStringList temp_2;

    if(!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open /proc/stat";
        return ret;
    }

    temp_1 = file->readLine();
    temp_2 = temp_1.split(' ');

    for (int i = 0; i < 4; ++i)
    {
        ret.push_back(temp_2.at(i + 2).toFloat());
    }
    return ret;
}

int MainWindow::getCpuLoad(double dt)
{
    vector<float> stats1 = readCpuStats();
    QProcess::execute("sleep", QStringList() << QString::number(dt));
    vector<float> stats2 = readCpuStats();
    int size1 = stats1.size();
    int size2 = stats2.size();
    if (!size1 || !size2 || size1 != size2) return 2;
    for (int i = 0; i < size1; ++i) stats2[i] -= stats1[i];
    int sum = 1;
    for (int i = 0; i < size1; ++i) sum += stats2[i];
    int load = 100 - (stats2[size2 - 1] * 100 / sum);
    return load;
}

MainWindow::~MainWindow()
{
    delete ui;
}

