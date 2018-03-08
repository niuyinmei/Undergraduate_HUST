#include "logindialog.h"
#include "ui_logindialog.h"
#include "login.h"
#include <QMessageBox>
#define ip "127.0.0.1"
#define port 8000

loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    ui->pswdEdit->setEchoMode(QLineEdit::Password);
    ui->confirmEdit->setEchoMode(QLineEdit::Password);
    ui->confirmButton->setFocus();
    this->setWindowTitle("Register");
    init();
    connectServer();
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::init()
{
    tcpSocket_register = new QTcpSocket(this);
    connect(tcpSocket_register , SIGNAL(error(QAbstractSocket::SocketError)) ,
            this,SLOT(displayError(QAbstractSocket::SocketError)));   //发生错误时执行displayError函数
}

void loginDialog::connectServer()
{
    tcpSocket_register->abort();   //取消已有的连接
    tcpSocket_register->connectToHost(ip,port);
    connect(tcpSocket_register , SIGNAL(readyRead()) , this , SLOT(readMessages()));
}



void loginDialog::on_confirmButton_clicked()
{
    QString userName=ui->usrEdit->text();
    QString passward=ui->pswdEdit->text();
    QString confirm_password = ui->confirmEdit->text();
    QString email = ui->emailEdit->text();
    if(userName=="" || passward==""|| email=="")
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
    else if(QString::compare(passward, confirm_password) != 0)
    {
        QMessageBox::information(this,"警告","密码与确认密码不一致",QMessageBox::Ok);
        ui->usrEdit->clear();
        ui->pswdEdit->clear();
        ui->confirmEdit->clear();
        ui->emailEdit->clear();
        ui->usrEdit->setFocus();
    }
    else
    {
        QString as="a";
        QString data=as+"#"+userName+"#"+passward + "#" + email;
        tcpSocket_register->write(data.toLatin1());
    }
    //login::tcpSocket_login->write(data.toLatin1());
}

void loginDialog::on_cancelButton_clicked()
{
    tcpSocket_register->disconnectFromHost();
    this->close();
}

void loginDialog::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<tcpSocket_register->errorString();   //输出出错信息
}


void loginDialog::readMessages()
{
    QString data=tcpSocket_register->readAll();
    //QString data=login::tcpSocket_login->readAll();
    QStringList list=data.split("#");
    /*if(list[0]=="a" && list[1]=="true")
        QMessageBox::information(this,"信息提示","注册成功!",QMessageBox::Ok);
    else if(list[0]=="a" && list[1]=="false")
        QMessageBox::information(this,"信息提示","注册失败,用户名已经被注册!",QMessageBox::Ok);*/
    if(list[0]=="a" && list[1]=="true")
    {
        QMessageBox::information(this,"信息提示","注册成功!",QMessageBox::Ok);
        tcpSocket_register->disconnectFromHost();
        this->close();
    }
    else if(list[0]=="a" && list[1]=="false")
    {
        QMessageBox::information(this,"信息提示","注册失败，此用户名已被使用",QMessageBox::Ok);
        ui->usrEdit->clear();
        ui->pswdEdit->clear();
        ui->confirmEdit->clear();
        ui->emailEdit->clear();
        ui->usrEdit->setFocus();
    }
    else
        return;
}
