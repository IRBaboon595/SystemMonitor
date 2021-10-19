#ifndef PROCESS_CONTAINER_H
#define PROCESS_CONTAINER_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#include <iostream>
#include <fstream>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


using namespace std;

class process_container : public QWidget
{
    Q_OBJECT
public:
    explicit process_container(QWidget *parent = nullptr);
private:
    QTableWidget *table;
    QHBoxLayout* H_layout_1;
    QPushButton* button;
    QVBoxLayout* layout;
public slots:
    void update();
    void kill();
signals:

};

#endif // PROCESS_CONTAINER_H
