#include "widget.h"
#include "ui_widget.h"
#include <QUdpSocket>
#include <QHostInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>
#include <QString>
#include "server.h"
#include "client.h"
#include <QFileDialog>
//界面控制
//udp实现基本聊天会话功能
//对应drawer//记得加QT     += network
//保存聊天记录

//用户退出没有信息显示？？？？？？还有错误（关闭退出没有设置退出发送给udp)
Widget::Widget(QWidget *parent,QString usrname) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    //发送UDP广播
    uName = usrname;      //用户名
    udpSocket = new QUdpSocket(this);  //创建UDP套接字，初始化
    port = 23232;   //默认端口
    udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);//绑定
    //接收来自其他用户的UDP广播消息
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    sndMsg(UsrEnter);   //发送UDP广播消息//发送数据
    //界面控制
   // srv=new server(this);
     srv = new server(this);   //获取文件名
    connect(srv, SIGNAL(sndFileName(QString)), this, SLOT(getFileName(QString)));


}
//界面控制
//槽连接
void  Widget::getFileName(QString name)
{
    fileName = name;//获取文件名
    sndMsg(FileName);//发送给广播

}



Widget::~Widget()
{
    delete ui;
}

//发送UDP广播消息
void Widget::sndMsg(MsgType type, QString srvaddr)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP(); //IP地址
    out<<type<<getUsr();  //消息类//用户名

    switch (type)
    {
    case Msg:
        if (ui->msgTxtEdit->toPlainText() == "") {
            QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
            return;
        }
        out << address << getMsg();
        ui->msgBrowser->verticalScrollBar()->setValue(ui->msgBrowser->verticalScrollBar()->maximum());
        break;
    case UsrEnter :
        out << address;
        break;
    case UsrLeft :
        break;
    case FileName:
    { //选择该文件要发送给的用户，从列表获取当前选中用户的IP地址
        //将其与文件名一起写入udp数据报文中
        int row = ui->usrTblWidget->currentRow();
        QString clntaddr = ui->usrTblWidget->item(row, 1)->text();
        out << address << clntaddr << fileName;
        break;
    }
    case Refuse:
        out << srvaddr;
        break;

    }
  udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);
}
//接收UDP消息
void Widget::processPendingDatagrams()
{//要同时也接收UDP广播发来的消息
    while(udpSocket->hasPendingDatagrams())//判断是否有可供读取的数据
    {
        QByteArray datagrame;
        datagrame.resize(udpSocket->pendingDatagramSize());//数据报文大小
        udpSocket->readDatagram(datagrame.data(),datagrame.size());//读取数据
        QDataStream in(&datagrame,QIODevice::ReadOnly);//
        int msgtype;
        in>>msgtype;
         QString usrName,ipAddr,msg;

         QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");//获取系统当前日期时间

         switch (msgtype)  //信息类型
         {
          case Msg:

                in>>usrName>>ipAddr>>msg;//普通的聊天消息//用户名//IP//消息内容
                ui->msgBrowser->setTextColor(Qt::blue); //显示颜色
                ui->msgBrowser->setCurrentFont(QFont("time new roman",12));//
                ui->msgBrowser->append("[" +usrName+ "]" +time);//用户//时间
                ui->msgBrowser->append(msg); //显示消息
                 break;

          case UsrEnter:

                in>>usrName>>ipAddr; //用户名//IP地址
                usrEnter(usrName,ipAddr);
                 break;

          case UsrLeft:

                in>>usrName;  //用户名
                usrLeft(usrName,time);
                break;

          case FileName:
                 { //接收发送过来的文件名信息时，使用函数判断是否接收该文件
                in >> usrName >> ipAddr;
                 QString clntAddr, fileName;
                in >> clntAddr >> fileName;
                hasPendingFile(usrName, ipAddr, clntAddr, fileName);

                  break;
                 }
          case Refuse:
              {
             in >> usrName;
             QString srvAddr;
             in >> srvAddr;
             QString ipAddr = getIP();

             if(ipAddr == srvAddr)
             {
                 srv->refused();//服务器拒绝函数
             }
                 break;
              }
         }

    }

}
//处理新用户加入
void Widget::usrEnter(QString usrname, QString ipaddr)
{
    bool isEmpty=ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
    QTableWidgetItem *usr=new QTableWidgetItem(usrname);
    QTableWidgetItem *ip= new QTableWidgetItem(ipaddr);

    ui->usrTblWidget->insertRow(0);
    ui->usrTblWidget->setItem(0,0,usr);
    ui->usrTblWidget->setItem(0,1,ip);
   //又加入就显示在消息框中
    ui->msgBrowser->setTextColor(Qt::gray);
    ui->msgBrowser->setCurrentFont(QFont("time new roman!",10));
    ui->msgBrowser->append(tr("在线人数：%1").arg(usrname));
    ui->usrnumlbl->setText(tr("在线人数：%1").arg(ui->usrTblWidget->rowCount()));
    sndMsg(UsrEnter);
    //发送新用户登陆消息//已经在线的端点要告知新加入用户端点信息，如果没有新用户没法显示在人数的信息

    }
    // 使用用户名usrname来判断该用户是否加入用户列表中，加入
}
//处理用户离开
void Widget::usrLeft(QString usrname, QString time)
{                                                    //？？？？？？
    int rownum=ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).first()->row();
    ui->usrTblWidget->removeRow(rownum);
    ui->msgBrowser->setTextColor(Qt::gray);
    ui->msgBrowser->setCurrentFont(QFont("time now roman",10));
    ui->msgBrowser->append(tr("%1于%2离开!").arg(usrname).arg(time));
    ui->usrnumlbl->setText(tr("在线人数:%1").arg(ui->usrTblWidget->rowCount()));
    //将离开的用户的信息删除，然后进行提示
}
//获取IP地址
QString Widget::getIP()
{

 QList<QHostAddress> list=QNetworkInterface::allAddresses();//列表形式列出所有IP地址
 foreach (QHostAddress addr, list)  //遍历找到ipv4地址
 {      //???
   if(addr.protocol()==QAbstractSocket::IPv4Protocol)
   {
       return addr.toString(); //返回
   }
 }
   return 0;
}
//获取用户名
QString Widget::getUsr()
{
return uName;
}
//获取用户聊天消息
QString Widget::getMsg()
{   //从界面的消息文本编辑框获取用户输入的消息，然后对文本编辑框进行清空
                                   //?????
    QString msg=ui->msgTxtEdit->toHtml();
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();
    return msg;

}
//发送按钮
void Widget::on_sendBtn_clicked()
{
 sndMsg(Msg);
}
//退出
void Widget::on_exitBtn_clicked()
{
    close();
}
void Widget::closeEvent(QCloseEvent *e)
{
    sndMsg(UsrLeft);
    QWidget::closeEvent(e);
}
//界面控制
//槽连接，传输文件
void Widget::on_sendTBtn_clicked()
{
    //选择该文件要发送给的用户，从列表获取当前选中用户的IP地址
    if(ui->usrTblWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0, tr("选择用户"),tr("请先选择目标用户！"), QMessageBox::Ok);
        return;
    }
    srv->show(); //显示对话框
    srv->initSrv(); //初始化

}

void Widget::hasPendingFile(QString usrname, QString srvaddr, QString clntaddr, QString filename)
{//首先弹出对话框，判断用户是否接收该文件，接收；创建客户端对象来接收文件//拒绝，发回消息给udp广播
    QString ipAddr = getIP();
    if(ipAddr == clntaddr)  //改地址与客户端地址一样
    {
        int btn = QMessageBox::information(this,tr("接受文件"),tr("来自%1(%2)的文件：%3,是否接收？").arg(usrname).arg(srvaddr).arg(filename),QMessageBox::Yes,QMessageBox::No);
        if (btn == QMessageBox::Yes)
        {
            QString name = QFileDialog::getSaveFileName(0,tr("保存文件"),filename);
            if(!name.isEmpty())
            {
                Client *clnt = new Client(this);//创建客户端
                clnt->setFileName(name);
                clnt->setHostAddr(QHostAddress(srvaddr));
                clnt->show();
            }
        } else
        {
            sndMsg(Refuse, srvaddr);//拒绝udp广播
        }
    }


}

//保存聊天记录按钮
void Widget::on_saveTBtn_clicked()
{
  if(ui->msgBrowser->document()->isEmpty())
  {
      QMessageBox::warning(0,tr("警告"),tr("聊天记录为空，无法保存"),QMessageBox::Ok);
  }
  else
  {
   QString fname=QFileDialog::getSaveFileName(this,tr("保存聊天记录"),tr("聊天记录"),tr("文本(*.txt);;所有文件(*.*)"));

  if(!fname.isEmpty())
    saveFile(fname);
   }
}
//保存聊天记录
bool Widget::saveFile(const QString &filename )
{
  QFile file(filename);
  if(!file.open(QFile::WriteOnly|QFile::ReadOnly))
  {
      QMessageBox::warning(this,tr("保存文件"),tr("无法保存文件%1：\n%2").arg(filename).arg(file.errorString()));
      return false;
  }
  QTextStream out(&file);
  out<<ui->msgBrowser->toPlainText();
  return  true;

}

