#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class login;
}

extern QString  realuserName;
class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();
    //QTcpSocket *tcpSocket_login;
    //QString  realuserName;

private slots:
    void on_loginButton_clicked();
    void displayError(QAbstractSocket::SocketError);
    void readMessages();
    void on_registerButton_clicked();

    void on_forgetButton_clicked();

protected:
    void init();
    void connectServer();
private:
    Ui::login *ui;
    QTcpSocket *tcpSocket_login;
};

#endif // LOGIN_H
