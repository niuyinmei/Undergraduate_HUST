#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QStandardItemModel>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    typedef struct cpu_usage{
        int user;
        int nice;
        int system;
        int idle;
        int iowait;
        int irq;
        int softirq;
    }cpuUsage;

    typedef struct memory_usage{
        int memTotal;
        int memFree;
        int buffers;
        int cached;
    }memoryUsage;

    typedef struct swap_usage{
        int swapTotal;
        int swapFree;
    }swapUsage;

private slots:
    void tabChange();

    void showTab1();

    void showTab2();

    void showTab3();

    void showTab4();

    void on_searchButton_clicked();

    void on_killButton_clicked();

    void on_runButton_clicked();

    void showTime();

    void on_rebootButton_clicked();

    void on_shutButton_clicked();

    void getCPURate();

    void calcCPURate();

    void setCPUDisplay();

    void drawCPURate();

    void getAndCalcMemoryRate();

    void setMemoryDisplay();

    void drawMemoryRate();

    void on_refreshButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer1;
    QTimer *timer2;
    QTimer* timer3;
    QTime *timerElapsed;
    QStandardItemModel  *model1;
    QStandardItemModel  *model2;
    cpuUsage pastData;
    cpuUsage currentData;
    memoryUsage memoryData;
    swapUsage swapData;
    double cpuRate;
    double memoryRate;
    double swapRate;


};
#endif // MAINWINDOW_H
