#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include "logindialog.h"
#include "filesender.h"
#include "filereceiver.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#define ip "127.0.0.1"
#define port 8000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),condition_server(false)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint));
    server = new QTcpServer(this);
    socket = new QTcpSocket(this);

    //filesender = new QUdpSocket(this);
    //filereceiver = new QUdpSocket(this);

    socket_client = new QTcpSocket(this);

    socket_file = new QTcpSocket(this);
    this->setWindowTitle("Chatting Now!");
    ui->sendFileButton->setEnabled(false);
    initSocket();//初始化客户端发送套接字
    initServer();
    init();//初始化与服务器通信套接字
    connectServer();//与服务器建立连接

    ui->led_sendText->setAlignment(Qt::AlignLeft);
    ui->led_sendText->setAlignment(Qt::AlignTop);

    ui->label_receiveText->setAlignment(Qt::AlignLeft);
    ui->label_receiveText->setAlignment(Qt::AlignTop);
    //ui->scrollArea->setWidget(ui->label_receiveText);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::newConnection() {

    socket = server->nextPendingConnection();
    //ui->label_receiveText->setText("连接成功\n-----------\n\n");
    qDebug()<<"new connection";
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));

}

void MainWindow::readData() {

    QString data(socket->readAll());
    ui->label_receiveText->setPlainText(ui->label_receiveText->toPlainText() + data);
    QStringList list = data.split("#");
    if(list[0] == "file")
    {
        QHostAddress hostip = socket->peerAddress();
        QString name ="";
        while(name=="")
        {name = QFileDialog::getSaveFileName(0,tr("保存文件"),list[1]);}
        qDebug()<<name;
        long int len=list[2].toInt();
        qDebug()<<"list2"<<list[2];
        qDebug()<<"len"<<len;
        //if(receiver != NULL) delete receiver;
        qDebug()<<"1";
        receiver = new filereceiver(this,hostip,name,len);
        qDebug()<<"2";

        receiver->setAttribute(Qt::WA_DeleteOnClose, true);
        receiver->show();
        socket_client->abort();
        socket_client->connectToHost(hostip,6000);
        socket_client->write("confirm#");
    }
    else if(list[0] == "confirm")
    {
        qDebug()<<list[0];
        long int len = file->size();
        QStringList list1 = ui->onlineList->currentItem()->text().split(" ");
        QString current_ip(online_user[list1[0]]);
        QHostAddress address(current_ip);
        //if(sender) delete sender;
        sender= new filesender(this,address,fileName,len);
        sender->setAttribute(Qt::WA_DeleteOnClose, true);
        sender->show();
        sender->time.start();
        sender->start_gbn_server();
    }
}

void MainWindow::on_actionExit_triggered() {

    MainWindow::close();

}

void MainWindow::on_actionClear_triggered() {

    ui->label_receiveText->clear();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //QString as="e";
    //QString data=as+"#"+realuserName;
    //tcpSocket_login->write(data.toLatin1());
    event->accept();
}


/*void MainWindow::on_btn_send_clicked() {

    socket->write(ui->lineEdit_sendText->text().toUtf8() + "\n");
    ui->lineEdit_sendText->clear();

}*/
void MainWindow::init()
{
    tcpSocket_login = new QTcpSocket(this);
    connect(tcpSocket_login , SIGNAL(error(QAbstractSocket::SocketError)) ,
            this,SLOT(displayError(QAbstractSocket::SocketError)));   //发生错误时执行displayError函数
}

void MainWindow::connectServer()
{
    tcpSocket_login->abort();   //取消已有的连接
    tcpSocket_login->connectToHost(ip,port);
    connect(tcpSocket_login , SIGNAL(readyRead()) , this , SLOT(readMessages()));
    QString data = "d";
    data = data + "#" + realuserName;
    tcpSocket_login->write(data.toLatin1());
}

void MainWindow::start() {

    initServer();
    //findIP();

    //ui->btn_start->setText("停止");
    ui->btn_send->setEnabled(true);

    condition_server = true;

}

void MainWindow::stop() {

    server->close();
    socket->close();

    //ui->btn_start->setText("开始");
    ui->btn_send->setEnabled(false);

    condition_server = false;

}

void MainWindow::initServer() {

    server->listen(QHostAddress::Any, 6000);

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

}

void MainWindow::initSocket() {

    connect(socket_client, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket_client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket_client, SIGNAL(readyRead()), this, SLOT(readData()));
    //connect(socket_file, SIGNAL(connected()), this, SLOT(connected()));
    //connect(socket_file, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket_file, SIGNAL(readyRead()), this, SLOT(readData()));
}

void MainWindow::connected() {

    ui->btn_send->setEnabled(true);
    //ui->btn_connect->setText("断开");
    //ui->label_receiveText->setText("连接成功\n-----------\n\n");

    condition_client = true;

}

void MainWindow::disconnected() {

    ui->btn_send->setEnabled(false);
    //ui->btn_connect->setText("连接");

    condition_client= false;

}

void MainWindow::readDataClient() {

    QString data(socket_client->readAll());
    ui->label_receiveText->setPlainText(ui->label_receiveText->toPlainText() +"对方说123:\n"+data);

}

void MainWindow::readMessages()
{
    QString data=tcpSocket_login->readAll();
    qDebug()<<data;
    QStringList list=data.split("#");
    if(list[0] == "d")
    {
        int index = 1;
        online_user.clear();
        offline_user.clear();
        ui->onlineList->clear();
        while(list[index] != "e")
        {
            ui->onlineList->addItem(list[index] + " (online)");
            online_user.insert(list[index], list[index + 1]);
            index++;
            if(list[index] == "e")
            {
                index++;
                break;
            }
            index ++;
        }//处理在线用户信息
        index ++;
        while(index < list.length())
        {
            ui->onlineList->addItem(list[index] + " (offline)");
            offline_user.insert(list[index], index);
            index++;
            qDebug()<<index;
            if(index == list.length()) break;
            else if(list[index] == "f" || list[index] == "")
            {
                index++;
                break;
            }
        }//处理离线用户信息
        index++;
        while(index < list.length())
        {
            ui->label_receiveText->setPlainText(ui->label_receiveText->toPlainText()+ list[index - 1] + ":" + list[index] + "\n");
            index = index + 2;
        }
   }
}

void MainWindow::on_btn_send_clicked() {

    /*socket_client->write(ui->led_sendText->text().toUtf8() + "\n");
    qDebug()<<ui->led_sendText->text();
    ui->led_sendText->clear();*/
    if(ui->onlineList->currentItem() == NULL)
    {
        QMessageBox::information(this,"警告","请选择发送对象",QMessageBox::Ok);
    }
    else
    {
        QStringList list = ui->onlineList->currentItem()->text().split(" ");
        if(list[1] == "(offline)")
        {
            if(offline_user.contains(list[0]))
            {
                QString current_user = list[0];
                sendOfflineMessage(current_user);
            }
            else return;
        }
        else if(list[1] == "(online)")
        {
            if(online_user.contains(list[0]))
            {
                QString current_user = list[0];
                QString current_ip(online_user[current_user]);
                sendOnlineMessage(current_user, current_ip);
            }
            else return;
        }
    }
    /*else if(ui->onlineList->currentItem() != NULL && ui->offlineList->currentItem() == NULL)
    {
        if(online_user.contains(ui->onlineList->currentItem()->text()))
        {
            QString current_user = ui->onlineList->currentItem()->text();
            QString current_ip(online_user[current_user]);
            sendOnlineMessage(current_user, current_ip);
        }
        else return;
    }
    else if(ui->onlineList->currentItem() == NULL && ui->offlineList->currentItem() != NULL)
    {
        if(offline_user.contains(ui->offlineList->currentItem()->text()))
        {
            QString current_user = ui->offlineList->currentItem()->text();
            sendOfflineMessage(current_user);
        }
        else return;
    }
    else return;*/
    return;
}

void MainWindow::sendOnlineMessage(QString user, QString user_ip)
{
    QHostAddress address(user_ip);
    qDebug()<<user_ip;
    socket_client->abort();
    socket_client->connectToHost(address, 6000);
    QString line;

    line = realuserName + " :  ";
    line = line + ui->led_sendText->text() + "\n";
    ui->label_receiveText->setPlainText(ui->label_receiveText->toPlainText()+ "To" + user + ":" + ui->led_sendText->text() + "\n");
    socket_client->write(line.toUtf8() + "\n");
    /*socket_client->write(ui->led_sendText->text().toUtf8() + "\n");
    qDebug()<<ui->led_sendText->text();
    ui->led_sendText->clear();*/
}

void MainWindow::sendOfflineMessage(QString user)
{
    QString line = "f";
    line = line + "#" + user + "#" + realuserName + "#" + ui->led_sendText->text();
    qDebug()<<line;
    tcpSocket_login->write(line.toUtf8());
    ui->label_receiveText->setText(ui->label_receiveText->toPlainText()+ "To " + user + ":(offline message)" + ui->led_sendText->text() + "\n");
    ui->led_sendText->clear();
}


void MainWindow::on_openButton_clicked()
{
    openFile();
}

void MainWindow::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    ui->fileEdit->setText(fileName);
    if(!fileName.isEmpty())
    {
        ui->sendFileButton->setEnabled(true);
    }
    file = new QFile(fileName,this);
    qDebug()<<file->fileName();
    qDebug()<<file->size();
    if(!file->open(QIODevice::ReadWrite))
    {
        return;
    }
}

void MainWindow::on_sendFileButton_clicked()
{
    if(ui->onlineList->currentItem() == NULL)
    {
        QMessageBox::information(this,"警告","请选择发送对象",QMessageBox::Ok);
    }
    else
    {
        QStringList list = ui->onlineList->currentItem()->text().split(" ");
        qDebug()<<list[0];
        qDebug()<<list[1];
        if(list[1] == "(offline)") return;
        else
        {
            QString current_user = list[0];
            QString current_ip(online_user[current_user]);
            QHostAddress address(current_ip);
            qDebug()<<current_ip;
            socket_client->abort();
            socket_client->connectToHost(address,6000);
            long int len= file->size();
            QString fileinfo = "file#"+fileName.right(fileName.size() - fileName.lastIndexOf('/')-1)+"#" + QString::number(len);
            qDebug()<<fileinfo;
            socket_client->write(fileinfo.toUtf8());
        }

    }
    //sendFile();
}

void MainWindow::sendFile()
{
    qDebug()<<"gethere";
    while(!file->atEnd())
     {
        qDebug()<<"sending";
        QByteArray line = file->read(8000);
        qDebug()<<line.size();
        QHostAddress address = QHostAddress(QHostAddress::LocalHost);
        //filesender->writeDatagram(line.data(), line.size(), address, 7758);
        qDebug()<<"asd\n";
        if(line.size() <8000){
            QMessageBox::warning(this,tr("通知"),tr("发送成功！"),QMessageBox::Yes);
        }
    }
}

/*void MainWindow::initSocket()
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 7758);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}*/


void MainWindow::on_logoutButton_clicked()
{
    this->close();
    QString as="e";
    QString data=as+"#"+realuserName;
    tcpSocket_login->write(data.toLatin1());
    server->close();
    socket->close();
    socket_client->disconnectFromHost();
    login *lg = new login;
    lg->exec();
}
