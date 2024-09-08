#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>

//主界面控制
class server;
//实现UDP//端对端模式
class QUdpSocket;//UDP套接字
namespace Ui {
class Widget;
}

enum MsgType{Msg, UsrEnter, UsrLeft, FileName, Refuse};//聊天消息//新用户加入//用户退出//文件名//拒绝接收文件

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent,QString usrname);
    ~Widget();
protected:
    void usrEnter(QString usrname,QString ipaddr);//新用户加入
    void usrLeft(QString usrname,QString time);   //处理用户离开
    void sndMsg(MsgType type, QString srvaddr="");//广播UDP消息

    QString getIP();   //获取IP地址
    QString getUsr(); //获取用户名
    QString getMsg(); //获取聊天信息
   // 功能添加
    bool saveFile(const QString &filename);//保存聊天记录

     void closeEvent(QCloseEvent *);


 //界面控制
     void hasPendingFile(QString usrname, QString srvaddr,QString clntaddr, QString filename);
private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;  //声明UDP套接字
    qint16 port;        //端口
    QString uName;    //用户名

//界面控制

    QString fileName;
    server *srv{nullptr};

private slots:
    void processPendingDatagrams();    //接收UDP消息
    void on_sendBtn_clicked();     //ui界面生成//发送按钮
    void on_exitBtn_clicked();   // 退出

//控制界面
    void getFileName(QString); //获取服务器信号发过来的文件名


    void on_sendTBtn_clicked();
    void on_saveTBtn_clicked();
};

#endif // WIDGET_H
