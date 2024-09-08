#include "server.h"
#include "ui_server.h"
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>


server::server(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::server)
{
    ui->setupUi(this);
    setFixedSize(400,207); //对话框大小

    tPort = 5555;  //端口
    tSrv = new QTcpServer(this);  //服务器对象
    connect(tSrv, SIGNAL(newConnection()), this, SLOT(sndMsg())); //连接//发送数据

    initSrv(); //初始化服务器

}

server::~server()
{
    delete ui;
}

void server::initSrv()
{  //变量初始化
    payloadSize = 64*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesTobeWrite = 0;
   //设置按钮状态
    ui->sstatuslabel->setText(tr("请选择要传送的文件"));
    ui->progressBar->reset();
    ui->sopenbtn->setEnabled(true);
    ui->ssendbtn->setEnabled(false);
    //关闭对话框
    tSrv->close();
}
//发送数据
void server::sndMsg()
{
    ui->ssendbtn->setEnabled(false);
    clntConn = tSrv->nextPendingConnection();//客户端连接套接字 //服务器
    connect(clntConn,SIGNAL(bytesWritten(qint64)),this,SLOT(updClntProgress(qint64)));

    ui->sstatuslabel->setText(tr("开始传送文件 %1 ！").arg(theFileName));

    locFile = new QFile(fileName); //待发送文件初始化
    if(!locFile->open((QFile::ReadOnly))) //只读方式，选中文件
    {
        QMessageBox::warning(this, tr("应用程序"), tr("无法读取文件 %1:\n%2").arg(fileName).arg(locFile->errorString()));
        return;
    }
    totalBytes = locFile->size();//总共需发送的字节数大小，存在着变量中
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);//将outBlock（缓存文件）封装在这个类

    sendOut.setVersion(QDataStream::Qt_5_8);//？？？？？
    time.start();  // 开始计时
    //right（）去掉文件的路径部分，仅将文件部分保存在curfile
    QString curFile = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    sendOut << qint64(0) << qint64(0) << curFile;//创建一个临时文件头，将该值追加在totalBytes字段，从而完成实际的发送字节数记录
    totalBytes += outBlock.size();
    sendOut.device()->seek(0); //读写操作的指向从头开始
    sendOut << totalBytes << qint64((outBlock.size() - sizeof(qint64)*2));//填写实际的总长度和文件长度
    bytesTobeWrite = totalBytes - clntConn->write(outBlock);//发出
    outBlock.resize(0); //清空

}
//更新进度条
void server::updClntProgress(qint64 numBytes)
{
    qApp->processEvents(); //传输大文件界面不会冻结
    bytesWritten += (int)numBytes;
    if (bytesTobeWrite > 0)
    {                        //读取
        outBlock = locFile->read(qMin(bytesTobeWrite, payloadSize));
        bytesTobeWrite -= (int)clntConn->write(outBlock);
        outBlock.resize(0);//清空
    } else
    {
        locFile->close();
    }
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesWritten);

    float useTime = time.elapsed();//从开始到现在的时间
    double speed = bytesWritten / useTime; //传输速率
    ui->sstatuslabel->setText(tr("已发送 %1MB (%2MB/s) \n共%3MB 已用时:%4秒\n估计剩余时间：%5秒")
                   .arg(bytesWritten / (1024*1024))
                   .arg(speed*1000 / (1024*1024), 0, 'f', 2)
                   .arg(totalBytes / (1024 * 1024))
                   .arg(useTime/1000, 0, 'f', 0)
                   .arg(totalBytes/speed/1000 - useTime/1000, 0, 'f', 0));

    if(bytesWritten == totalBytes)
    {
        locFile->close();
        tSrv->close();
        ui->sstatuslabel->setText(tr("传送文件 %1 成功").arg(theFileName));
    }

}
//打开按钮
void server::on_sopenbtn_clicked()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {                  //right()去掉文件路径部分
        theFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        ui->sstatuslabel->setText(tr("要传送的文件为：%1 ").arg(theFileName));
        ui->ssendbtn->setEnabled(true);
        ui->sopenbtn->setEnabled(false);
    }
}
//发送按钮

void server::on_ssendbtn_clicked()
{
    if(!tSrv->listen(QHostAddress::Any,tPort))//开始监听
    {
        qDebug() << tSrv->errorString();
        close();
        return;
    }

    ui->sstatuslabel->setText(tr("等待对方接收... ..."));
    emit sndFileName(theFileName);
}
//关闭按钮

void server::on_sclosebtn_clicked()
{
    if(tSrv->isListening())
    {
        tSrv->close(); //关闭服务器
        if (locFile->isOpen())
            locFile->close();//关闭文件
        clntConn->abort();
    }
    close();  //关闭系统事件
}
//系统关闭事件
void server::closeEvent(QCloseEvent *)
{
 on_sclosebtn_clicked();
}
//拒接接收，关闭服务器
void server::refused()
{
    tSrv->close();
    ui->sstatuslabel->setText(tr("对方拒绝接收！"));
}
