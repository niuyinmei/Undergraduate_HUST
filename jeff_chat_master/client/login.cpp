#include "login.h"
#include "ui_login.h"
#include "logindialog.h"
#include "forgetpswd.h"
#include "mainwindow.h"
#include <QMessageBox>
#define ip "127.0.0.1"
#define port 8000

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    ui->pswdEdit->setEchoMode(QLineEdit::Password);
    this->setWindowTitle("Jeff's Messenger");
    /*init();
    connectServer();*/
}

login::~login()
{
    delete ui;
}

/*void login::on_loginButton_clicked()
{
    // 判断用户名和密码是否正确，如果错误则弹出警告对话框
    if(ui->usrEdit->text().trimmed() == tr("jeffmei")
        && ui->pswdEdit->text() == tr("123456"))
    {
        accept();
    }
    else
    {
        QMessageBox::warning(this, tr("警告"),
                             tr("用户名密码输入有误"),
                             QMessageBox::Yes);
        // 清空内容并定位光标
        ui->usrEdit->clear();
        ui->pswdEdit->clear();
        ui->usrEdit->setFocus();
    }
}*/





//connect to server
void login::init()
{
    tcpSocket_login = new QTcpSocket(this);
    connect(tcpSocket_login , SIGNAL(error(QAbstractSocket::SocketError)) ,
            this,SLOT(displayError(QAbstractSocket::SocketError)));   //发生错误时执行displayError函数
}

void login::connectServer()
{
    tcpSocket_login->abort();   //取消已有的连接
    tcpSocket_login->connectToHost(ip,port);
    qDebug()<<"qwe";
    connect(tcpSocket_login , SIGNAL(readyRead()) , this , SLOT(readMessages()));
}

void login::on_loginButton_clicked()
{
    init();
    connectServer();
    QString userName=ui->usrEdit->text();
    realuserName = userName;
    QString passward=ui->pswdEdit->text();
    if(userName=="" || passward=="")
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
    QString as="b";
    QString data=as+"#"+userName+"#"+passward;
    tcpSocket_login->write(data.toLatin1());
    qDebug()<<data;
}


void login::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<tcpSocket_login->errorString();   //输出出错信息
}


void login::readMessages()
{
    QString data=tcpSocket_login->readAll();
    QStringList list=data.split("#");
    /*if(list[0]=="a" && list[1]=="true")
        QMessageBox::information(this,"信息提示","注册成功!",QMessageBox::Ok);
    else if(list[0]=="a" && list[1]=="false")
        QMessageBox::information(this,"信息提示","注册失败,用户名已经被注册!",QMessageBox::Ok);*/
    if(list[0]=="b" && list[1]=="true")
    {
        QMessageBox::information(this,"信息提示","登录成功!",QMessageBox::Ok);
        //accept();
        tcpSocket_login->disconnectFromHost();
        MainWindow *mw = new MainWindow;
        mw->show();
        this->hide();
    }
    else if(list[0]=="b" && list[1]=="false")
    {
        QMessageBox::information(this,"信息提示","登录失败,用户名或密码错误!",QMessageBox::Ok);
        ui->usrEdit->clear();
        ui->pswdEdit->clear();
        ui->usrEdit->setFocus();
    }
    else
        return;
}

void login::on_registerButton_clicked()
{
    loginDialog *widget = new loginDialog;
    //widget->show();
    widget->exec();
}

void login::on_forgetButton_clicked()
{
    forgetpswd *widget = new forgetpswd;
    widget->exec();
}
