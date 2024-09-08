#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include <QTime>

class QFile;
class QTcpServer;
class QTcpSocket;

namespace Ui {
class server;
}

class server : public QDialog
{
    Q_OBJECT

public:
    explicit server(QWidget *parent = nullptr);
    ~server();

    void initSrv();  //初始化服务器
    void refused();  //关闭服务器
protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::server *ui;
    qint16 tPort;      //端口
    QTcpServer *tSrv;   //声明服务器
    QString fileName;   //文件名
    QString theFileName;
    QFile *locFile;  //待发送文件

    qint64 totalBytes;    //总共需发送的字节数
    qint64 bytesWritten;  //已发送的字节数
    qint64 bytesTobeWrite; //待发送的字节数
    qint64 payloadSize;     //被初始化为一个常量
    QByteArray outBlock;   //缓存一次发送的文件

    QTcpSocket *clntConn;  //客户端连接套接字

    QTime time;
private slots:
    void sndMsg();   //发送数据
    void updClntProgress(qint64 numBytes);  //更新进度条

    void on_sopenbtn_clicked();

    void on_ssendbtn_clicked();

    void on_sclosebtn_clicked();

signals:
    void sndFileName(QString fileName);//发送文件信号

};

#endif // SERVER_H
