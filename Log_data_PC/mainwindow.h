#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QFile>
#include <QTextStream>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void readyReadSlot();
    void timer_irq();
private slots:
    void on_bt_connect_clicked();

    void on_bt_file_clicked();

    void on_bt_send_clicked();


private:
    Ui::MainWindow *ui;
    QFile file;
    QFile file_write;
    QTextStream strem;

};
#endif // MAINWINDOW_H
