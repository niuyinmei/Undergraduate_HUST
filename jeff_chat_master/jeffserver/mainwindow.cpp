#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>
#define ip "127.0.0.1"
#define port 8000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    //init();
    /*if(Load())
    {
        ui->setupUi(this);
        this->setWindowTitle("Query");
        QObject::connect(ui->OKButton,SIGNAL(clicked(bool)),this,SLOT(on_OK_Button_clicked()));
    }
    else
        QMessageBox::information(this,tr("Error"),tr("Unable to load the data."));*/
    if(load())
    {
        ui->setupUi(this);
        init();
        this->setWindowTitle("Jeff's Server");
    }
    else
        QMessageBox::information(this,tr("Error"),tr("Unable to load the data."));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    this->tcpServer=new QTcpServer(this);
    //this->tcpSocket=new QTcpSocket(this);
    for(int i = 0; i < 50; i++)
    {
        this->tcpSocket[i] = new QTcpSocket(this);
        connect(tcpSocket[i],SIGNAL(error(QAbstractSocket::SocketError)),
                         this,SLOT(displayError(QAbstractSocket::SocketError)));
    }
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    //connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                     //this,SLOT(displayError(QAbstractSocket::SocketError)));
}


void MainWindow::receiveData(int current)
{
    QString data=tcpSocket[current]->readAll();
    qDebug()<<data;
    QStringList list=data.split("#");
    bool ret=0;
    if(list[0]=="a") //注册
    {
        ret = register_compare(list[1],list[2],list[3]);
        QString sendData = list[0];
        if(ret)
            sendData+="#true";
        else
            sendData+= "#false";
        tcpSocket[current]->write(sendData.toLatin1());
    }
    if(list[0]=="b")  //登录
    {
        ret = login_compare(list[1], list[2], tcpSocket[current]);
        qDebug()<<list[1];
        QString sendData = list[0];
        if(ret)
            sendData+="#true";
        else
            sendData+= "#false";
        tcpSocket[current]->write(sendData.toLatin1());
        qDebug()<<current;
        //updatelist();
    }
    if(list[0] == "c") //找回密码
    {
        QString password;
        QString sendData = list[0];
        ret = forget_password(list[1], list[2], password);
        if(ret)
            sendData+="#true#"+password;
        else
            sendData+= "#false";
        tcpSocket[current]->write(sendData.toLatin1());
    }
    if(list[0] == "d") //确认登陆
    {
        if(online_user.contains(list[1]))
        {
            online_user[list[1]] = tcpSocket[current];
        }
        updatelist();
        offlinerecord::Iterator iter;
        QString data = "";
        QString line = "f";
        for(iter = offline_record.begin(); iter != offline_record.end(); )
        {
            if(iter.value().receiver == list[1])
            {
                data = data + iter.value().sender + "#" + iter.value().message + "#";
                offline_record.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
        line = line + "#" + data;
        qDebug()<<line;
        tcpSocket[current]->write(line.toUtf8());
    }
    if(list[0] == "e")//离线
    {
        if(online_user.contains(list[1]))
        {
            online_user.remove(list[1]);
            offline_user.insert(list[1],10);
            updatelist();
        }
    }
    if(list[0] == "f")//离线消息
    {
        message_number++;
        offLineMessage newoffline;
        newoffline.receiver = list[1];
        newoffline.sender = list[2];
        newoffline.message = list[3];
        offline_record.insert(message_number, newoffline);
    }
        //ret=checkSignIn(list[1],list[2]);
    else
        return;
    //QString sendData=list[0];
    /*if(ret)
        sendData+="#true";
    else
        sendData+="#false";*/
    //tcpSocket->write(sendData.toLatin1());
}

void MainWindow::on_startBtn_clicked()
{
    ui->startBtn->setEnabled(false);
    if(!tcpServer->listen(QHostAddress::Any,8000))
    {
        //qDebug()<<tcpServer->errorString();
        close();
        return;
    }
    updatelist();
}


void MainWindow::acceptConnection()
{
    tcpSocket[user_number]=tcpServer->nextPendingConnection();
    int index = user_number;
    connect(tcpSocket[index],&QTcpSocket::readyRead,this, [=]()
    {
        receiveData(index);
        qDebug()<<index;
    });
    qDebug()<<user_number;
    user_number++;
}

void MainWindow::displayError(QAbstractSocket::SocketError, int current)
{
    //qDebug()<<tcpSocket[current]->errorString();
    tcpSocket[current]->close();
}

void MainWindow::updatelist()
{
    offlineuserList::Iterator it1;
    onlineuserList::Iterator iter1;
    ui->onlineList->clear();
    ui->offlineList->clear();
    for(it1 = offline_user.begin(); it1 != offline_user.end(); ++it1)
    {
        ui->offlineList->addItem(it1.key());
    }
    for(iter1 = online_user.begin(); iter1 != online_user.end(); ++iter1)
    {
        ui->onlineList->addItem(iter1.key());
    }
    for(iter1 = online_user.begin(); iter1 != online_user.end(); ++iter1)
    {
        QString line1 = "d";
        QString line2 = "e";
        line1 = line1 + "#";
        line2 = line2 + "#";
        onlineuserList::Iterator iter2;
        offlineuserList::Iterator it2;
        for(iter2 = online_user.begin(); iter2 != online_user.end(); ++iter2)
        {
            line1 = line1 + iter2.key() + "#";
            line1 = line1 + iter2.value()->peerAddress().toString().mid(7) + "#";
        }
        iter1.value()->write(line1.toLatin1());//发送在线用户信息

        for(it2 = offline_user.begin(); it2 != offline_user.end(); ++it2)
        {
            line2 = line2 + it2.key() + "#";
        }
        iter1.value()->write(line2.toLatin1());//发送离线用户信息
    }
}
