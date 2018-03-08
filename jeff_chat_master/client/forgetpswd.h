#ifndef FORGETPSWD_H
#define FORGETPSWD_H

#include <QDialog>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class forgetpswd;
}

class forgetpswd : public QDialog
{
    Q_OBJECT

public:
    explicit forgetpswd(QWidget *parent = 0);
    ~forgetpswd();

private:
    Ui::forgetpswd *ui;
    QTcpSocket *tcpSocket_find;

    //QTcpSocket *tcpSocket_register;

private slots:
    void displayError(QAbstractSocket::SocketError);
    void readMessages();

    void on_cancelButton_clicked();

    void on_confirmButton_clicked();

protected:
    void init();
    void connectServer();
};

#endif // FORGETPSWD_H
