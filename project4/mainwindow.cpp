#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"
#include <QFile>
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QStandardItemModel>
#include <QProcess>
#include <sys/stat.h>
#include <QPen>
#include <QFont>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    showTab1();
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChange()));

    showTime();
    timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(showTab1()));
    connect(timer1, SIGNAL(timeout()), this, SLOT(showTime()));
    timer1->start(1000);    //refreshing by 1 sec


    timer2 = new QTimer(this);
    timer2->start(50);
    connect(timer1, SIGNAL(timeout()), this, SLOT(getCPURate()));

    timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(calcCPURate()));
    connect(timer3, SIGNAL(timeout()), this, SLOT(getAndCalcMemoryRate()));
    timer3->start(100);

    model1 = new QStandardItemModel();
    model1->setColumnCount(5);
    model1->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Process ID"));
    model1->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Name"));
    model1->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("Status"));
    model1->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Priority"));
    model1->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("Meomory Used"));

    ui->tableView->setModel(model1);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    model2 = new QStandardItemModel();
    model2->setColumnCount(5);
    model2->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Process ID"));
    model2->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Name"));
    model2->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("Status"));
    model2->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Priority"));
    model2->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("Meomory Used"));

    ui->killView->setModel(model2);
    ui->killView->horizontalHeader()->setStretchLastSection(true);
    ui->killView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->killView->setSelectionBehavior(QAbstractItemView::SelectRows);

    timerElapsed = new QTime(QTime::currentTime());
    setCPUDisplay();
    setMemoryDisplay();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tabChange()    //switching tabs
{
    int pageNum = ui->tabWidget->currentIndex();
    switch (pageNum) {
    case 0:
        showTab1();
        break;
    case 1:
        showTab2();
        break;
    case 2:
        showTab3();
        break;
    case 3:
        showTab4();
        break;
    default:
        break;
    }
}

void MainWindow::showTab1()     //displaying basic system info
{
    QFile tempFile;
    QString hostNameString;
    QString uptimeString;
    QStringList timeRunningString;
    QString osTypeString;
    QString osReleaseString;
    QString systemVersionString;
    QString cpuInfoString;
    QString cpuMhzString;

    tempFile.setFileName("/proc/sys/kernel/hostname");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The cpuinfo file can not open!"), QMessageBox::Yes);
        return;
    }
    hostNameString = tempFile.readLine().trimmed();
    ui->hostName->setText(hostNameString);
    tempFile.close();


    tempFile.setFileName("/proc/uptime");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The cpuinfo file can not open!"), QMessageBox::Yes);
        return;
    }
    uptimeString = tempFile.readAll();
    timeRunningString = uptimeString.split(" ",QString::SkipEmptyParts);
    double runningtime = timeRunningString[0].toDouble();
    int runninghours = runningtime/3600;
    int runningmins = (runningtime - runninghours*3600)/60;
    int runningsec = runningtime - runninghours*3600 - runningmins*60;
    QString hh = QString::number(runninghours, 10);
    QString mm = QString::number(runningmins, 10);
    QString ss = QString::number(runningsec, 10);
    QString totaltime = hh.append("h ").append(mm).append("mins ").append(ss).append("s");
    ui->timeRunning->setText(totaltime);


    int i=QDateTime::currentDateTime().toTime_t() - runningtime;
    QString startTimeString = QDateTime::fromTime_t(i).toString("yyyy-MM-dd hh:mm:ss");
    ui->startTime->setText(startTimeString);
    tempFile.close();


    tempFile.setFileName("/proc/sys/kernel/ostype");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The cpuinfo file can not open!"), QMessageBox::Yes);
        return;
    }
    osTypeString = tempFile.readAll().trimmed();
    tempFile.close();
    tempFile.setFileName("/proc/sys/kernel/osrelease");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The cpuinfo file can not open!"), QMessageBox::Yes);
        return;
    }
    osReleaseString = tempFile.readAll().trimmed();
    tempFile.close();
    systemVersionString = osTypeString.append(" ").append(osReleaseString);
    ui->systemVersion->setText(systemVersionString);


    tempFile.setFileName("/proc/cpuinfo");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The cpuinfo file can not open!"), QMessageBox::Yes);
        return;
    }
    int pos;
    QString tempStr;
    while(1)
    {
        tempStr = tempFile.readLine();
        if (tempStr == NULL)    break;
        if (tempStr.indexOf("model name") != -1)
        {
            pos = tempStr.indexOf("model name") + 13; //跳过前面的"model name："所占用的字符
            cpuInfoString = tempStr.mid(pos, tempStr.length()-13).trimmed();
        }
        else if(tempStr.indexOf("cpu MHz") != -1)
        {
            pos = tempStr.indexOf("cpu MHz") + 11;
            cpuMhzString = tempStr.mid(pos, tempStr.length() - 11).trimmed();
        }
    }
    ui->cpuModelInfo->setText(cpuInfoString);
    ui->cpuMhz->setText(cpuMhzString);
    tempFile.close();
}

void MainWindow::showTab2()
{
    QDir procDir("/proc");
    QStringList pidList = procDir.entryList();
    QString pidString = pidList.join("\n");
    QFile tempFile;
    QString tempStr;

    int find_start = 3;
    int startOfPid;
    int endOfPid;
    int totalProNum = 0;
    QString proID;
    bool ok;
    int pidInt;

    QString proName;
    QString proStatus;
    QString proPriority;
    QString proMemory;

    while(1)
    {
        startOfPid = pidString.indexOf("\n", find_start);
        endOfPid = pidString.indexOf("\n", startOfPid + 1);
        find_start = endOfPid;
        proID = pidString.mid(startOfPid + 1, endOfPid - startOfPid - 1);
        totalProNum++;
        pidInt = proID.toInt(&ok, 10);
        if(!ok)
        {
            break;
        }

        tempFile.setFileName("/proc/" + proID + "/stat");
        if ( !tempFile.open(QIODevice::ReadOnly) )
        {
            QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
            return;
        }
        tempStr = tempFile.readLine();
        if (tempStr.length() == 0)
        {
            break;
        }
        startOfPid = tempStr.indexOf("(");
        endOfPid = tempStr.indexOf(")");
        proName = tempStr.mid(startOfPid + 1, endOfPid - startOfPid - 1);
        proName.trimmed();
        proStatus = tempStr.section(" ", 2, 2);
        proPriority = tempStr.section(" ", 17, 17);
        proMemory = tempStr.section(" ", 22, 22);
        tempFile.close();

        model1->setItem(totalProNum - 1, 0, new QStandardItem(proID));
        model1->setItem(totalProNum - 1, 1, new QStandardItem(proName));
        model1->setItem(totalProNum - 1, 2, new QStandardItem(proStatus));
        model1->setItem(totalProNum - 1, 3, new QStandardItem(proPriority));
        model1->setItem(totalProNum - 1, 4, new QStandardItem(proMemory));
    }
}

void MainWindow::showTab3()
{
    //dummy function
}

void MainWindow::showTab4()
{
    //dummy function
}


void MainWindow::on_searchButton_clicked()
{
    QString killpid = ui->killEdit->text();
    QDir procDir("/proc");
    QStringList pidList = procDir.entryList();
    QList<QString>::Iterator it = pidList.begin();
    bool found = false;

    for(;it != pidList.end(); it++)
    {
        if(!killpid.compare(*it))
        {
            QFile tempFile;
            QString tempStr;
            int startOfPid;
            int endOfPid;

            QString proName;
            QString proStatus;
            QString proPriority;
            QString proMemory;


            tempFile.setFileName("/proc/" + killpid + "/stat");
            if ( !tempFile.open(QIODevice::ReadOnly) )
            {
                QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
                return;
            }
            tempStr = tempFile.readLine();
            if (tempStr.length() == 0)
            {
                break;
            }
            startOfPid = tempStr.indexOf("(");
            endOfPid = tempStr.indexOf(")");
            proName = tempStr.mid(startOfPid + 1, endOfPid - startOfPid - 1);
            proName.trimmed();
            proStatus = tempStr.section(" ", 2, 2);
            proPriority = tempStr.section(" ", 17, 17);
            proMemory = tempStr.section(" ", 22, 22);
            tempFile.close();

            model2->setItem(0, 0, new QStandardItem(killpid));
            model2->setItem(0, 1, new QStandardItem(proName));
            model2->setItem(0, 2, new QStandardItem(proStatus));
            model2->setItem(0, 3, new QStandardItem(proPriority));
            model2->setItem(0, 4, new QStandardItem(proMemory));
            found = true;
            ui->noticeText->setText("");
            break;
        }
    }
    if (found == false)
    {
        ui->noticeText->setText("The process does not exist.");
        model2->removeRows(0, model2->rowCount());
    }
}

void MainWindow::on_killButton_clicked()
{
    if(model2->item(0,0) == NULL)   return;
    else
    {
        QString kill = "kill";
        kill.append(" " + model2->item(0,0)->text());
        char*  ch;
        QByteArray ba = kill.toLatin1();
        ch = ba.data();
        system(ch);
        model2->removeRows(0, model2->rowCount());
    }
}

void MainWindow::on_runButton_clicked()
{
    QProcess *pro=new QProcess;
    QString newProc;
    newProc=ui->runEdit->text();
    pro->start(newProc);
}

void MainWindow::showTime()
{
    QTime timecurrent = QTime::currentTime();
    QString strtime = timecurrent.toString("hh:mm:ss AP");
    QDate datecurrent = QDate::currentDate();
    QString strdate = datecurrent.toString("dddd, MMMM d, yyyy");
    strdate.append(" "+ strtime);
    ui->timeLabel->setText(strdate.append(" Shanghai"));
}

void MainWindow::getCPURate()
{
    QFile tempFile;
    QString tempStr;
    QStringList dataFromFile;
    tempFile.setFileName("/proc/stat");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
        return;
    }
    tempStr = tempFile.readLine();
    tempStr = tempStr.mid(5);
    dataFromFile = tempStr.split(" ");

    pastData.user = currentData.user;
    pastData.nice = currentData.nice;
    pastData.system = currentData.system;
    pastData.idle = currentData.idle;
    pastData.iowait = currentData.iowait;
    pastData.irq = currentData.irq;
    pastData.softirq = currentData.softirq;

    currentData.user = dataFromFile[0].toInt();
    currentData.nice = dataFromFile[1].toInt();
    currentData.system = dataFromFile[2].toInt();
    currentData.idle = dataFromFile[3].toInt();
    currentData.iowait = dataFromFile[4].toInt();
    currentData.irq = dataFromFile[5].toInt();
    currentData.softirq = dataFromFile[6].toInt();
    tempFile.close();
    return;
}

void MainWindow::calcCPURate()
{
    cpuRate = 1 - (double)(currentData.idle - pastData.idle)/
                ((currentData.user - pastData.user) + (currentData.nice - pastData.nice) + (currentData.system - pastData.system)
                 + (currentData.idle - pastData.idle) + (currentData.irq - pastData.irq) + (currentData.iowait - pastData.iowait)
                 + (currentData.softirq - pastData.softirq));
}

void MainWindow::on_shutButton_clicked()
{
    QString passWord;
    QString sudo;
    char* command;
    bool OK;
    QByteArray ba;
    passWord=QInputDialog::getText(this,"Enter Password","Password:",QLineEdit::Password,"",&OK);
    if(OK)
    {
        sudo=QString("echo %1 | sudo -S shutdown -h now").arg(passWord);
        ba=sudo.toLatin1();
        command=ba.data();
        system(command);
    }
}

void MainWindow::on_rebootButton_clicked()
{
    QString passWord;
    QString sudo;
    char* command;
    bool OK;
    QByteArray ba;
    passWord=QInputDialog::getText(this,"Enter Password","Password:",QLineEdit::Password,"",&OK);
    if(OK)
    {
        sudo=QString("echo %1 | sudo -S reboot -h now").arg(passWord);
        ba=sudo.toLatin1();
        command=ba.data();
        system(command);
    }
}

void MainWindow::setCPUDisplay()
{
    ui->cpuDisplay->addGraph();
    ui->cpuDisplay->graph(0)->setPen(QPen(Qt::blue));
    ui->cpuDisplay->yAxis->setLabel("CPU %");
    ui->cpuDisplay->yAxis->setRange(0, 100);


    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->cpuDisplay->xAxis->setTicker(timeTicker);
    ui->cpuDisplay->axisRect()->setupFullAxesBox();
    ui->cpuDisplay->xAxis->setRange(0, 60, Qt::AlignRight);

    connect(ui->cpuDisplay->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->cpuDisplay->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->cpuDisplay->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->cpuDisplay->yAxis2, SLOT(setRange(QCPRange)));

    QTimer  *timerCPUDisplay;
    timerCPUDisplay = new QTimer(this);
    connect(timerCPUDisplay, SIGNAL(timeout()), this, SLOT(drawCPURate()));
    timerCPUDisplay->start(200);

    ui->cpuBar->setRange(0, 100);
}

void MainWindow::drawCPURate()
{
    double key = timerElapsed->elapsed()/1000.0;
    ui->cpuDisplay->graph(0)->addData(key, cpuRate * 100);
    ui->cpuDisplay->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->cpuDisplay->replot();
    ui->cpuLabel->setStyleSheet("color:blue");
    ui->cpuLabel->setText("CPU: " + QString::number(cpuRate * 100) + "%");
    ui->cpuBar->setValue(cpuRate * 100);
}

void MainWindow::getAndCalcMemoryRate()
{
    QFile tempFile;
    QString tempStr;
    int pos;
    tempFile.setFileName("/proc/meminfo");
    if ( !tempFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
        return;
    }
    while(1)
    {
        tempStr = tempFile.readLine();
        if (tempStr == NULL)    break;
        if (tempStr.indexOf("MemTotal:") != -1)
        {
            pos = tempStr.indexOf("MemTotal:") + 9;
            memoryData.memTotal = tempStr.mid(pos,tempStr.length() - 12).trimmed().toInt();
        }
        else if (tempStr.indexOf("MemFree:") != -1)
        {
            pos = tempStr.indexOf("MemFree:") + 8;
            memoryData.memFree = tempStr.mid(pos,tempStr.length() - 11).trimmed().toInt();
        }
        else if (tempStr.indexOf("Buffers:") != -1)
        {
            pos = tempStr.indexOf("Buffers:") + 8;
            memoryData.buffers = tempStr.mid(pos,tempStr.length() - 11).trimmed().toInt();
        }
        else if (tempStr.indexOf("Cached:") != -1 && tempStr.indexOf("SwapCached:") == -1)
        {
            pos = tempStr.indexOf("Cached:") + 7;
            memoryData.cached = tempStr.mid(pos,tempStr.length() - 10).trimmed().toInt();
        }
        else if (tempStr.indexOf("SwapTotal:") != -1)
        {
            pos = tempStr.indexOf("SwapTotal:") + 10;
            swapData.swapTotal = tempStr.mid(pos,tempStr.length() - 13).trimmed().toInt();
        }
        else if (tempStr.indexOf("SwapFree") != -1)
        {
            pos = tempStr.indexOf("SwapFree:") + 9;
            swapData.swapFree = tempStr.mid(pos,tempStr.length() - 12).trimmed().toInt();
        }
    }
    tempFile.close();
    memoryRate = (double)(memoryData.memTotal - memoryData.memFree - memoryData.buffers - memoryData.cached)/(memoryData.memTotal);
    swapRate = 1 - (double)(swapData.swapFree)/(swapData.swapTotal);
}

void MainWindow::setMemoryDisplay()
{
    ui->memoryDisplay->addGraph();
    ui->memoryDisplay->graph(0)->setPen(QPen(Qt::red));
    ui->memoryDisplay->graph(0)->setName("Memory");

    ui->memoryDisplay->addGraph();
    ui->memoryDisplay->graph(1)->setPen(QPen(Qt::green));
    ui->memoryDisplay->graph(1)->setName("Swap");

    ui->memoryDisplay->yAxis->setLabel("%");
    ui->memoryDisplay->yAxis->setRange(0, 100);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    ui->memoryDisplay->legend->setFont(font);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->memoryDisplay->xAxis->setTicker(timeTicker);
    ui->memoryDisplay->axisRect()->setupFullAxesBox();
    ui->memoryDisplay->xAxis->setRange(0, 60, Qt::AlignRight);

    connect(ui->memoryDisplay->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->memoryDisplay->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->memoryDisplay->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->memoryDisplay->yAxis2, SLOT(setRange(QCPRange)));

    QTimer  *timerCPUDisplay;
    timerCPUDisplay = new QTimer(this);
    connect(timerCPUDisplay, SIGNAL(timeout()), this, SLOT(drawMemoryRate()));
    timerCPUDisplay->start(200);

    ui->memoryBar->setRange(0, 100);
}

void MainWindow::drawMemoryRate()
{
    double key = timerElapsed->elapsed()/1000.0;
    ui->memoryDisplay->graph(0)->addData(key, memoryRate * 100);
    ui->memoryDisplay->graph(1)->addData(key, swapRate * 100 + 1);
    ui->memoryDisplay->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->memoryDisplay->replot();
    ui->memoryLabel->setStyleSheet("color:red");
    ui->memoryLabel->setText("Memory: " + QString::number(memoryRate * 100) + "%");
    ui->swapLabel->setStyleSheet("color:green");
    ui->swapLabel->setText("Swap: " + QString::number(swapRate * 100) + "%");

    ui->memoryBar->setValue(memoryRate * 100);
}

void MainWindow::on_refreshButton_clicked()
{
    showTab2();
}
