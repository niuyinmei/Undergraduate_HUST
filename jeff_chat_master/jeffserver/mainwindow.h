#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMap>
#include <stdlib.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    typedef QMap <QString, QTcpSocket *> onlineuserList;
    typedef QMap <QString, int> offlineuserList;
    onlineuserList online_user;
    offlineuserList offline_user;
    typedef struct userInfo{
        char username[50];
        char password[50];
        char email_account[50];
        int status;
    }userInfo;

    userInfo usr[50];
    int current_user;

    int message_number = 0;
    int user_number = 0;
    typedef struct offlinemessage{
        QString sender;
        QString message;
        QString receiver;
    }offLineMessage;
    typedef QMap <int, offLineMessage>  offlinerecord;
    offlinerecord offline_record;

   bool load(void)
   {
       FILE *fp;
       if(!(fp = fopen("D:\\network\\final\\jeffserver\\accounts.txt","r")))
       {
           exit(-1);
       }
       int i = 0;
       while(!feof(fp))
       {
           fscanf(fp,"%s %s %s", usr[i].username, usr[i].password, usr[i].email_account);
           decrypt(usr[i].username);
           decrypt(usr[i].password);
           decrypt(usr[i].email_account);
           usr[i].status = 0;
           qDebug()<<usr[i].username<<usr[i].password<<usr[i].email_account;
           QString user = QString(QLatin1String(usr[i].username));
           offline_user[user] = i;
           i++;
       }
       current_user = i ;
       qDebug()<<current_user;
       fclose(fp);
       return true;
   }

   bool login_compare(QString user, QString pswd, QTcpSocket *tcpsocket)
   {
       /*QString str = “hello”; //QString转char *
       QByteArray ba = str.toLatin1();
       char *mm = ba.data();  */
      QByteArray user_temp = user.toLatin1();
      QByteArray pswd_temp = pswd.toLatin1();
      char *user_cmp = user_temp.data();
      char* pswd_cmp = pswd_temp.data();
      for(int i = 0; i < current_user; i++)
          if( !strcmp(usr[i].username, user_cmp) &&  !strcmp(usr[i].password, pswd_cmp))
          {
                usr[i].status = 1;
                online_user.insert(user, tcpsocket);
                offline_user.remove(user);
                return true;
          }
     // qDebug()<<"false2";
      return false;
   }

   bool register_compare(QString user, QString pswd, QString email)
   {
       FILE *fp;
       QByteArray user_temp = user.toLatin1();
       QByteArray pswd_temp = pswd.toLatin1();
       QByteArray email_temp = email.toLatin1();
       char *user_cmp = user_temp.data();
       char* pswd_cmp = pswd_temp.data();
       char* email_cmp = email_temp.data();
       for(int i = 0; i < current_user; i++)
           if( !strcmp(usr[i].username, user_cmp))
                 return false;
        fp = fopen("D:\\network\\final\\jeffserver\\accounts.txt","a");
        qDebug()<<user_cmp;
        qDebug()<<pswd_cmp;
        qDebug()<<pswd_cmp;
        encrypt(user_cmp);
        encrypt(pswd_cmp);
        encrypt(email_cmp);
        fprintf(fp,"\n%s %s %s", user_cmp, pswd_cmp, email_cmp);
        fclose(fp);
        return load();
   }
   bool forget_password(QString user, QString email, QString &password)
   {
       QByteArray user_temp = user.toLatin1();
       QByteArray email_temp = email.toLatin1();
       char *user_cmp = user_temp.data();
       char* email_cmp = email_temp.data();
       for(int i = 0; i < current_user; i++)
           if( !strcmp(usr[i].username, user_cmp) &&  !strcmp(usr[i].email_account, email_cmp))
           {
                 password =QString(QLatin1String(usr[i].password));
                 return true;
           }
      // qDebug()<<"false2";
       return false;
   }

   void encrypt(char string[])
   {
       unsigned int i;
       for(i=0;i<strlen(string);++i)
       {
           string[i] = string[i] - strlen(string);
       }
   }

   void decrypt(char string[])
   {
       unsigned int i;
       for(i=0;i<strlen(string);++i)
       {
           string[i] = string[i] + strlen(string);
       }
   }
   void receiveData(int current);

protected:
    void init();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket[50];
    QTcpServer *tcpServer;

private slots:
    void on_startBtn_clicked();

    void acceptConnection();



    void displayError(QAbstractSocket::SocketError, int current);

    void updatelist();
};

#endif // MAINWINDOW_H
