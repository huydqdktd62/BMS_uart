#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QDebug>
#include <QFileDialog>

QSerialPort *serial;
QTimer *timer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->bt_send->setEnabled(false);
    serial = new QSerialPort(this);
    ui->bt_send->setEnabled(false);
    QList<QSerialPortInfo> port_info = QSerialPortInfo::availablePorts();

    for(uint16_t i = 0;i < port_info.count();i++){
        ui->cb_port->addItem(port_info.at(i).portName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_bt_connect_clicked()
{
    if(!serial->isOpen()){
        serial->setPortName(ui->cb_port->currentText());
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setParity(QSerialPort::NoParity);
        bool is_open = serial->open(QIODevice::ReadWrite);
        if(is_open == true){
            qDebug()<<"Open "<<ui->cb_port->currentText();
            ui->cb_port->setEnabled(false);
            ui->bt_connect->setText("Connected");
            connect(serial,SIGNAL(readyRead()),this,SLOT(readyReadSlot()));
            ui->bt_send->setEnabled(true);
            this->file_write.setFileName("Log.txt");
            this->file_write.open(QIODevice::Append|QIODevice::Text);
        }
        else{
            qDebug()<<"Can't open "<<ui->cb_port->currentText();
        }
    }
    else{
        serial->close();
        ui->cb_port->setEnabled(true);
        qDebug()<<"Close "<<ui->cb_port->currentText();
        ui->bt_connect->setText("Connect");
        this->file_write.close();
    }
}


void MainWindow::on_bt_file_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                             tr("Open File"), "data_test/", tr("Files (*.txt *.csv)"));

    this->file.setFileName(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Can't open"<<file_name;
        return;
    }
    ui->l_file->setText(file_name);
    ui->bt_send->setEnabled(true);
    qDebug()<<"Open"<<file_name;
}


void MainWindow::on_bt_send_clicked()
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_irq()));

    timer->start(ui->lineEdit->text().toInt());
    ui->bt_send->setEnabled(false);
}

void MainWindow::readyReadSlot()
{
    QByteArray data = serial->readAll();
    ui->log_panel->append(data);
    this->file_write.write(data);
}

void MainWindow::timer_irq()
{
    if(this->file.isOpen() && !this->file.atEnd()){
        QByteArray data = this->file.readLine();
        if(serial->isOpen()){
            qDebug()<<"Data:"<<data;
            serial->write(data);
        }
    }
    else{
        qDebug()<<"End";
        ui->bt_send->setEnabled(false);
        this->file.close();
        timer->stop();
    }
}

