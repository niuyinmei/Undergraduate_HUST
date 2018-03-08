#ifndef FILESENDER_H
#define FILESENDER_H

#include <QDialog>
#include <QTimer>
#include <QUdpSocket>
#include <QFile>
#include "QMessageBox"
#include <QElapsedTimer>
#include <QTime>
#include "qregexp.h"

namespace Ui {
class filesender;
}

class filesender : public QDialog
{
    Q_OBJECT

public:
    explicit filesender(QWidget *parent,const QHostAddress &rcvip, const QString &fileName,long int len);
    ~filesender();
    QTime time;
        void initSrv();
        void refused();
        int start_gbn_server();
    protected:
        void ansmit();
    void error(const char *msg) ;
    void send_invalid_pkt(int len);

    void handle_loss_event();

    int expected_seq_no = 0;

        void closeEvent(QCloseEvent *);
        void send_pkt(int i);
        struct packet {
            /* Header */
            uint16_t cksum; /* Optional bonus part */
            uint16_t len;
            uint32_t seqno;
            /* Data */
            char data[504]; /* Not always 500 bytes, can be less */
        };

        /* Ack-only packets are only 8 bytes */
        struct ack_packet {
            uint16_t cksum; /* Optional bonus part */
            uint16_t len;
            uint32_t ackno;
        };
    private:
        Ui::filesender *ui;
        quint16 tPort;
        QString theFileName;
        QFile *locFile;
        int sending_window_size = 50;
        int remaining_chunks;
        int base=0,  next_seq_no=0;
        int duplicate_ack_num=0, resend_base=0;
        int timeout=2;
        int timepass;
        int last_resent=0, timeout_counter=0;
        struct packet *pkts;
        int MAX_WINDOW_SIZE;
        int state;
    long int filesize;
        qint64 totalBytes;
        qint64 bytesSend;
        qint64 bytesTobeWrite;
        qint64 payloadSize;
        QByteArray outBlock;
        QHostAddress hostip;
        FILE *input;
        //QTcpSocket *clntConn;
        QUdpSocket *udpsocket;
        QTimer timer;
        QTime totaltimer;

        //struct packet curr_pkt;

    private slots:
    void recv_acks();
        //void sndMsg();
       // void updClntProgress(qint64 numBytes);
      //  void on_sOpenBtn_clicked();
    void timeout_handler();
      //  void on_sSendBtn_clicked();

    signals:
        void sndFileName(QString fileName);
};

#endif // FILESENDER_H
