#include "filesender.h"
#include "ui_filesender.h"

#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <signal.h>
#include <QDateTime>
using namespace std;
filesender::filesender(QWidget *parent,const QHostAddress &rcvip, const QString &fileName,long int len) :
    QDialog(parent),
    ui(new Ui::filesender)
{
    ui->setupUi(this);
    setFixedSize(400,207);
    tPort = 6003;
    payloadSize = 64*1024;
    totalBytes = 0;
    bytesSend=0;
    bytesTobeWrite = 0;
    sending_window_size=60;
    hostip=QHostAddress(rcvip);
    MAX_WINDOW_SIZE=128;
    input=fopen(fileName.toLatin1().data(), "rb");
    qDebug()<<fileName;
    qDebug()<<input;
    filesize=len;
    timepass=0;
    timer.start();
    totaltimer.start();
    if(!input)
    {
        QMessageBox::information(this,"提示","文件读取失败");
        close();
    }
    initSrv();
}

filesender::~filesender()
{
    delete ui;
}

void filesender::initSrv()
{
    udpsocket = new QUdpSocket(this);
    udpsocket->bind(QHostAddress::Any, tPort+1);
    connect(udpsocket, SIGNAL(readyRead()),this, SLOT(recv_acks()));
    //connect(&timer, SIGNAL(timeout()), this, SLOT(timeout_handler()));
    ui->progressBar->reset();
    ui->progressBar->setRange(0,filesize);
}


void filesender::closeEvent(QCloseEvent *)
{
    state=0;
    //timer.stop();
    fclose(input);
    free(pkts);
    close();
}

void filesender::send_pkt(int i) {
    int index = i%MAX_WINDOW_SIZE;
    qDebug("i=%i",index);
    udpsocket->writeDatagram((char *)&pkts[index],sizeof(struct packet),hostip,tPort);
    while (udpsocket->waitForBytesWritten()) {
    }
}

void filesender::timeout_handler() {
    //timer.stop();
    if(sending_window_size == 1)
        timeout_counter++;
    if(timeout_counter>=3) {
        state=0;
        timeout_counter=0;
       // timer.stop();
        QMessageBox::information(this,"提示","超时");
        close();
    }
    else
        handle_loss_event();
}

void filesender::handle_loss_event() {
    //timer.stop();
    sending_window_size = sending_window_size/2;
    if(sending_window_size == 0)
        sending_window_size = 1;
    duplicate_ack_num = 0;
    resend_base = min(base + sending_window_size, next_seq_no);
    //timer.start(timeout*1000);
    for(int i=base; i<resend_base; i++) {
        send_pkt(i);
    }
    timer.start();
}




void filesender::recv_acks() {
    /*wait for ACKs*/
    char ack_buf[8];
    qDebug()<<"enter recv_ack";
    while(udpsocket->hasPendingDatagrams()){
        struct ack_packet *ack = (struct ack_packet *) ack_buf;
        if(ack->len == 8) {
            qDebug()<<"base:"<<base;
            if(ack->ackno < (unsigned int)base) {
                qDebug()<<"ackno:"<<ack->ackno;
                if(ack->ackno != (unsigned int)last_resent) { //if duplicate ack was sent but for a newly lost pkt
                duplicate_ack_num ++;
                }
                if(duplicate_ack_num >= 3) {
                    last_resent = ack->ackno;
                    handle_loss_event();
                }
            } else {
                sending_window_size = min(MAX_WINDOW_SIZE, sending_window_size*2);//min(MAX_WINDOW_SIZE, sending_window_size+1);
                if(resend_base != 0 && resend_base < next_seq_no) {
                    int indx = sending_window_size - (resend_base - (base+1));
                    indx = min(resend_base+indx, next_seq_no);
                    for(int i=resend_base; i<indx; i++) {
                        qDebug()<<"resendbase"<<resend_base;
                        send_pkt(i);
                    }
                    resend_base = indx;
                }
            }
            bytesSend+=504;
            timeout_counter=0;
            ui->progressBar->setValue(bytesSend);
            ui->progressBar->setFormat("%p%");
            base = ack->ackno + 1;
            qDebug("base=%d",base);
            timer.start();
        }
        else if(ack->len==0)
        {
            state=0;
            //totaltimer.stop();
            QTime temp;
            temp.start();
            QString out="发送完成，平均速率为"+QString::number(filesize/(totaltimer.msecsTo(temp)))+"KB/S";
            ui->label->setText(out);
            QMessageBox::information(this,"提示","发送完成");
            close();
        }
    }
    return ;
}

int filesender::start_gbn_server() {
    int  data_size = 504;
    char buff[data_size];
    state=1;
    struct packet pkt;
    QTime curtime;
    memset(buff,0,data_size);
    int length = fread(buff, 1, data_size, input);
    char buff_next[data_size];
    memset(buff_next,0,data_size);
    int length_next = fread(buff_next, 1, data_size, input);
    remaining_chunks = length_next;
        pkts = (struct packet *)malloc(MAX_WINDOW_SIZE*sizeof(struct packet));
    while(length > 0&&state==1) {
QCoreApplication::processEvents();
       if(udpsocket->hasPendingDatagrams()) recv_acks();
       curtime.start();
       timepass=time.msecsTo(curtime);
       qDebug("%d",timepass);
       if((resend_base == next_seq_no) && next_seq_no < base+sending_window_size) {
           pkt.len = length + 8;
           pkt.seqno = next_seq_no;
           memcpy(pkt.data, buff, data_size);
           pkts[next_seq_no%MAX_WINDOW_SIZE] = pkt;
           send_pkt(next_seq_no);
           timer.start(timeout*1000);
           if(next_seq_no == base)
           {
               timeout_counter=0;
               timer.start();
           }
           next_seq_no++;
           resend_base++;
           memset(buff,0,data_size);
           memcpy(buff, buff_next, length_next);
           length = length_next;
           memset(buff_next,0,data_size);
           length_next = fread(buff_next, 1, data_size, input);
           remaining_chunks = length_next;
       }
    }
    return 0;
}
