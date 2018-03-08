#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = 0);
    ~loginDialog();
    QTcpSocket *tcpSocket_register;

private:
    Ui::loginDialog *ui;
    //QTcpSocket *tcpSocket_register;

private slots:
    void displayError(QAbstractSocket::SocketError);
    void readMessages();
    void on_confirmButton_clicked();

    void on_cancelButton_clicked();

protected:
    void init();
    void connectServer();
};

#endif // LOGINDIALOG_H
