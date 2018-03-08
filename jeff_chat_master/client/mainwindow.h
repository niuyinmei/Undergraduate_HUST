#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetWork>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QMap>
#include <QTimer>
#include <filereceiver.h>
#include <filesender.h>

namespace Ui {
class MainWindow;
}

extern QString realuserName;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *socket;
    QTcpSocket *tcpSocket_login;
    bool condition_server;

    QTcpSocket *socket_client;
    bool condition_client;
    void initSocket();
    QString fileName;
    QFile *file;
    QTcpSocket *socket_file;

    filereceiver* receiver;
    filesender* sender;

    typedef QMap<QString, QString> onlineuserlist;
    typedef QMap<QString, int> offlineuserlist;
    onlineuserlist online_user;
    offlineuserlist offline_user;
    QTimer timer;

    void start();
    void stop();
    void initServer();
    void init();
    void connectServer();

    void sendOnlineMessage(QString user, QString user_ip);
    void sendOfflineMessage(QString user);

    void openFile();
    void sendFile();

    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionExit_triggered();
    void on_actionClear_triggered();

    void newConnection();
    void readData();

    void on_btn_send_clicked();

    void connected();
    void disconnected();
    void readDataClient();
    void on_openButton_clicked();

    void on_sendFileButton_clicked();
    void on_logoutButton_clicked();
    void readMessages();
    //void processServer();
};

#endif // MAINWINDOW_H
