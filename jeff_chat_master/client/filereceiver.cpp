#include "filereceiver.h"
#include "ui_filereceiver.h"
#include "QMessageBox"
#include <QElapsedTimer>
#include <QUdpSocket>
#include <QTimer>
#include <QFile>
#include "qregexp.h"

#include <stdio.h>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>

filereceiver::filereceiver(QWidget *parent,const QHostAddress &hostip,const QString &filename,long int len) :
    QDialog(parent),
    ui(new Ui::filereceiver)
{
    ui->setupUi(this);

    setFixedSize(400,190);
    filesize=len;
    totalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    senderip=QHostAddress(hostip);
    output=fopen(filename.toLatin1(),"wb");
    tPort = 6003;
    udpsocket = new QUdpSocket(this);
    udpsocket->bind(QHostAddress::Any, tPort);
    initcli();
}

filereceiver::~filereceiver()
{
    delete ui;
}

void filereceiver::initcli(){
    connect(udpsocket, SIGNAL(readyRead()),this, SLOT(rcvpkt()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout_handler()));
    ui->progressBar->reset();
    state=1;
    timer.stop();
    ui->progressBar->setRange(0,filesize);
}

void filereceiver::closeEvent(QCloseEvent *)
{
    if(output) fclose(output);
    output=0;
    state=0;
    timer.stop();
    delete udpsocket;
    udpsocket=0;
    close();
}


void filereceiver::timeout_handler() {
    timer.stop();
    state=0;
    QMessageBox::information(this,"提示","接收超时");
    close();
}

void filereceiver::rcvpkt(){
    char pkt_buf[sizeof(struct packet)];
    QHostAddress senderaddr;
    quint16 senderport;
    struct ack_packet ack;
    struct packet *pkt=NULL;
    while(udpsocket->hasPendingDatagrams()&&state==1)
    {
        udpsocket->readDatagram((char*)&pkt_buf,(qint64)sizeof(struct packet),&senderaddr,&senderport);
        if(senderaddr!=senderip)
            continue;
        pkt = (struct packet *) pkt_buf;
        if(pkt->len > sizeof(struct packet)) {
            fclose(output);
            QMessageBox::information(this,"提示","接收出错");
            close();
            break;
        }
        if(pkt->seqno == (unsigned int)expected_seq_no) {
            int data_size = pkt->len - 8;
            fwrite(pkt->data,data_size,1,output);
            expected_seq_no++;
        }
        /*send ACK*/
        ack.ackno = expected_seq_no-1;
        ack.len = 8;
        //ack.cksum = compute_ack_checksum(&ack);
        udpsocket->writeDatagram((char *)&ack,sizeof(struct ack_packet),senderip,tPort+1);
        timer.start(8000);
    }
    bytesReceived=ftell(output);
    ui->progressBar->setValue(bytesReceived);
    ui->progressBar->setFormat("%p%");
    if(bytesReceived>=filesize){
        udpsocket->readAll();
        if(output)
        {
            fclose(output);
            output=0;
        }
        ack.len = 0;
        state=0;
        udpsocket->writeDatagram((char *)&ack,sizeof(struct ack_packet),senderip,tPort+1);
        QMessageBox::information(this,"提示","接收完成");
        close();
    }
}

