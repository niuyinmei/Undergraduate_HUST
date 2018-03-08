#ifndef FILERECEIVER_H
#define FILERECEIVER_H

#include <QUdpSocket>
#include <QDialog>
#include <QTimer>
#include <cstring>
#include <string.h>
#include <QTimer>

namespace Ui {
class filereceiver;
}

class filereceiver : public QDialog
{
    Q_OBJECT

public:
    explicit filereceiver(QWidget *parent = 0, const QHostAddress &senderip=QHostAddress("127.0.0.1"),const QString &filename="",long int len=0);
    ~filereceiver();
    long int filesize;
        //void setHostAddr(QHostAddress addr);
       // void setFileName(QString name);
    void initcli();

    protected:
        void closeEvent(QCloseEvent *);
    private:
        Ui::filereceiver *ui;
    QUdpSocket *udpsocket;
        //QTcpSocket *tClnt;
        quint16 blockSize;
        QHostAddress hostAddr;
        qint16 tPort;
        struct packet {
            uint16_t cksum;
            uint16_t len;
            uint32_t seqno;
            char data[504];
        };

        struct ack_packet {
            uint16_t cksum;
            uint16_t len;
            uint32_t ackno;
        };
        qint64 totalBytes;
        qint64 bytesReceived;
        qint64 fileNameSize;
        QString fileName;
        QByteArray inBlock;
    QHostAddress senderip;
    FILE* output;
        QTimer timer;
        int expected_seq_no = 0, timeout = 4;
        struct packet file_name_pkt;
        int state;
    private slots:
        void rcvpkt();
        void timeout_handler();
};

#endif // FILERECEIVER_H
