#include "forgetpswd.h"
#include "ui_forgetpswd.h"
#define ip "127.0.0.1"
#define port 8000

forgetpswd::forgetpswd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::forgetpswd)
{
    ui->setupUi(this);
    ui->confirmButton->setFocus();
    this->setWindowTitle("Forget your password?");
    init();
    connectServer();
}

forgetpswd::~forgetpswd()
{
    delete ui;
}

void forgetpswd::init()
{
    tcpSocket_find = new QTcpSocket(this);
    connect(tcpSocket_find , SIGNAL(error(QAbstractSocket::SocketError)) ,
            this,SLOT(displayError(QAbstractSocket::SocketError)));   //发生错误时执行displayError函数
}

void forgetpswd::connectServer()
{
    tcpSocket_find->abort();   //取消已有的连接
    tcpSocket_find->connectToHost(ip,port);
    connect(tcpSocket_find , SIGNAL(readyRead()) , this , SLOT(readMessages()));
}

void forgetpswd::on_cancelButton_clicked()
{
    tcpSocket_find->disconnectFromHost();
    this->close();
}

void forgetpswd::on_confirmButton_clicked()
{
    QString userName=ui->usrlineEdit->text();
    QString email=ui->maillineEdit->text();
    if(userName=="" || email=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }
    else
    {
        QString as="c";
        QString data=as+"#"+userName+"#"+ email;
        tcpSocket_find->write(data.toLatin1());
    }
    //login::tcpSocket_login->write(data.toLatin1());
}


void forgetpswd::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<tcpSocket_find->errorString();   //输出出错信息
}


void forgetpswd::readMessages()
{
    QString data=tcpSocket_find->readAll();
    //QString data=login::tcpSocket_login->readAll();
    QStringList list=data.split("#");
    /*if(list[0]=="a" && list[1]=="true")
        QMessageBox::information(this,"信息提示","注册成功!",QMessageBox::Ok);
    else if(list[0]=="a" && list[1]=="false")
        QMessageBox::information(this,"信息提示","注册失败,用户名已经被注册!",QMessageBox::Ok);*/
    if(list[0]=="c" && list[1] =="true")
    {
        QMessageBox::information(this,"信息提示","您的密码为" + list[2], QMessageBox::Ok);
        tcpSocket_find->disconnectFromHost();
        this->close();
    }
    else if(list[0]=="c" && list[1]=="false")
    {
        QMessageBox::information(this,"信息提示","该用户不存在或邮箱输入有误",QMessageBox::Ok);
        return;
    }
    else
        return;
}
