#include "process_container.h"

process_container::process_container(QWidget *parent) : QWidget(parent)
{
    H_layout_1 = new QHBoxLayout;
    button = new QPushButton("Завершить");
    button->setToolTip("Для завершения процесса вы должны выделить его PID и нажать на кнопку \"Завершить\"");

    H_layout_1->addStretch();
    H_layout_1->addWidget(button);
    layout = new QVBoxLayout;
    table = new QTableWidget;
    update();
    layout->addWidget(table);
    layout->addLayout(H_layout_1);

    this->setLayout(layout);

    QTimer *timer = new QTimer;
    timer->start(5000);

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(button, SIGNAL(clicked()), this, SLOT(kill()));

}

void process_container::update()
{
    table->setColumnCount(4);
    table->setRowCount(0);
    QStringList list;
    QStringList temp_list;
    QString temp;
    QString vms;
    QString rss;
    double temp_long = 0;
    int bracket_1_index = 0;
    int bracket_2_index = 0;
    int brake_index = 0;
    temp.clear();
    list << "Name" << "PID" << "VMS" << "RSS";
    table->setHorizontalHeaderLabels(list);
    QDir *dir = new QDir("/proc");
    list = dir->entryList(QStringList("*"), QDir::AllDirs);

    foreach(QString str, list) {
        if(str.toInt()) {
            QFile file = {"/proc/" + str.toUtf8() + "/stat"};               //23-vms 24-rss pages (one page - 4kB)
            if(!file.open(QIODevice::ReadOnly))
                qDebug() << "PROCESS ERROR!" << "Unable to open entry";  
            vms = file.readAll();
            bracket_1_index = vms.indexOf('(', 0);
            bracket_2_index = vms.indexOf(')', 0);
            brake_index = vms.indexOf(' ', bracket_1_index);
            if(brake_index < bracket_2_index)
            {
                vms.remove(brake_index, 1);
            }
            temp_list = vms.split(' ');
            vms = temp_list.value(22);
            temp_long = vms.toDouble() / 1000;
            vms = QString::number(uint64_t(temp_long));
            rss = temp_list.value(23);
            temp_long = rss.toDouble() * 4;
            rss = QString::number(uint64_t(temp_long));
            file.close();

            file.setFileName("/proc/" + str.toUtf8() + "/comm");
            if(!file.open(QIODevice::ReadOnly))
                qDebug() << "PROCESS ERROR!" << "Unable to open entry";
            temp = file.readAll();
            int lastRow = table->rowCount();
            QString icon = "/usr/share/icons/hicolor/32x32/apps/" + temp + ".png";
            QFile file_icon(icon);
            table->insertRow(lastRow);
            table->setColumnWidth(0,150);

            if(!file_icon.exists()) {
                icon = "./binary.png";
            }
            table->setItem(lastRow, 0, new QTableWidgetItem(QPixmap(icon), temp));
            table->setItem(lastRow, 1, new QTableWidgetItem(str));
            table->setItem(lastRow, 2, new QTableWidgetItem(vms));
            table->setItem(lastRow, 3, new QTableWidgetItem(rss));
        }
        else
        {
            continue;
        }
    }
}

void process_container::kill()
{
    QList<QTableWidgetItem*> list = table->selectedItems();
    QTableWidgetItem* item = list.value(0);
    QString str = item->text();
    qDebug() << str;
    QProcess::execute("kill", QStringList() << str);
    update();
}
