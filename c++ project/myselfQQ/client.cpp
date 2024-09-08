#include "client.h"
#include "ui_client.h"

#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>

Client::Client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    setFixedSize(400,190);

    totalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;

    tClnt = new QTcpSocket(this); //初始化套接字
    tPort = 5555;
    connect(tClnt, SIGNAL(readyRead()), this, SLOT(readMsg())); //读取数据
    //连接期间错误
    connect(tClnt, SIGNAL(error(QAbstractSocket::SocketError)),\
            this,SLOT(displayErr(QAbstractSocket::SocketError)));
}

Client::~Client()
{
    delete ui;
}
//槽函数定义//错误
void Client::displayErr(QAbstractSocket::SocketError sockErr)
{
    switch(sockErr)
    {
    case QAbstractSocket::RemoteHostClosedError : break;
    default : qDebug() << tClnt->errorString();
    }
}
//与服务器连接
void Client::newConn()
{
    blockSize = 0;
    tClnt->abort(); //重连
    tClnt->connectToHost(hostAddr, tPort); //连接客户端
    time.start(); //计时
}
//接收服务器传来的数据
void Client::readMsg()
{
    QDataStream in(tClnt);
    in.setVersion(QDataStream::Qt_5_8);

    float useTime = time.elapsed();

    if (bytesReceived <= sizeof(qint64)*2) {
        if ((tClnt->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0))
        {
            in>>totalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((tClnt->bytesAvailable() >= fileNameSize) && (fileNameSize != 0)){
            in>>fileName;
            bytesReceived +=fileNameSize;

            if(!locFile->open(QFile::WriteOnly)){
                QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件 %1:\n%2.").arg(fileName).arg(locFile->errorString()));
                return;
            }
        } else {
            return;
        }
    }
    if (bytesReceived < totalBytes) {
        bytesReceived += tClnt->bytesAvailable();
        inBlock = tClnt->readAll();
        locFile->write(inBlock);
        inBlock.resize(0);
    }
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesReceived);

    double speed = bytesReceived / useTime;
    ui->cstatusLbl->setText(tr("已接收 %1MB (%2MB/s) \n共%3MB 已用时：%4秒\n估计剩余时间：%5秒")
                                      .arg(bytesReceived / (1024*1024))
                                      .arg(speed*1000/(1024*1024),0,'f',2)
                                      .arg(totalBytes / (1024 * 1024))
                                      .arg(useTime/1000,0,'f',0)
                                      .arg(totalBytes/speed/1000 - useTime/1000,0,'f',0));

    if(bytesReceived == totalBytes)
    {
        locFile->close();
        tClnt->close();
        ui->cstatusLbl->setText(tr("接收文件 %1 完毕").arg(fileName));
    }

}
//取消按钮
void Client::on_ccanclebtn_clicked()
{
    tClnt->abort();
    if (locFile->isOpen())
        locFile->close();
}
//关闭按钮
void Client::on_cclosebtn_clicked()
{
    tClnt->abort();
    if (locFile->isOpen())
        locFile->close();
    close();
}
//系统关闭事件
void Client::closeEvent(QCloseEvent *)
{
   on_cclosebtn_clicked();
}
//主界面widget弹出文件对话框来选择发送过来的文件保存路径
void Client::setFileName(QString name)
{
     locFile = new QFile(name);
}
//主界面获取发送端的IP地址
void Client::setHostAddr(QHostAddress addr)
{
    hostAddr = addr;
    newConn();
}
