#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QHostAddress>
#include <QFile>
#include <QTime>

class QTcpSocket;
namespace Ui {
class Client;
}

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();
    void setHostAddr(QHostAddress addr); //获取发送端的IP地址
    void setFileName(QString name);  //获取文件保存路径

protected:
    void closeEvent(QCloseEvent *); //系统关闭事件
private:
   Ui::Client *ui;

    QTcpSocket *tClnt;    //客户端套接字
    quint16 blockSize;
    QHostAddress hostAddr;
    qint16 tPort;

    qint64 totalBytes;  //总共需接收的字节数
    qint64 bytesReceived;  //已接收的字节数
    qint64 fileNameSize;
    QString fileName;
    QFile *locFile;     //待接收文件
    QByteArray inBlock;  //缓存一次接收文件

    QTime time;

private slots:

    void newConn();  //连接到服务器
    void readMsg();  //读取数据
    void displayErr(QAbstractSocket::SocketError); //连接期间错误

    void on_ccanclebtn_clicked();
    void on_cclosebtn_clicked();
};

#endif // CLIENT_H
