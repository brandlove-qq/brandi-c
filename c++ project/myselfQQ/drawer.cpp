#include "drawer.h"
#include <QGroupBox>
#include <QVBoxLayout>

//定义抽屉盒需要的各种按钮控件
//界面设计与开发
//构造函数
Drawer::Drawer(QWidget *parent,Qt::WindowFlags f):QToolBox (parent,f)
{
    //一个抽屉盒
    setWindowTitle(tr("Myself QQ 2019"));                   //设置主窗体的标题
    setWindowIcon(QPixmap(":/images/qq.png"));              //设置主窗体标题栏图标

    toolbtn1 =new QToolButton; //创建按钮部件 //类实例
    toolbtn1->setText(tr("1"));  //设置按钮文字
    toolbtn1->setIcon(QPixmap(":/images/Cherry.png")); //设置按钮图标//路径
    toolbtn1->setIconSize(QPixmap(":/images/Cherry.png").size());//设置按钮大小与图标大小相同
    toolbtn1->setAutoRaise(true);  //当鼠标离开时，自动恢复弹起状态
    //设置按钮的文字显示在图标旁边。每一个按钮的设置相同
    toolbtn1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn1,SIGNAL(clicked()),this,SLOT(showChatWidget1())); //信号与槽连接//显示

    toolbtn2 =new QToolButton;
    toolbtn2->setText(tr("2"));
    toolbtn2->setIcon(QPixmap(":/images/dr.png"));
    toolbtn2->setIconSize(QPixmap(":/images/dr.png").size());
    toolbtn2->setAutoRaise(true);
    toolbtn2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn2,SIGNAL(clicked()),this,SLOT(showChatWidget2()));

    toolbtn3 =new QToolButton;
    toolbtn3->setText(tr("3"));
    toolbtn3->setIcon(QPixmap(":/images/ii_wps.png"));
    toolbtn3->setIconSize(QPixmap(":/images/ii_wps.png").size());
    toolbtn3->setAutoRaise(true);
    toolbtn3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn3,SIGNAL(clicked()),this,SLOT(showChatWidget3()));

    toolbtn4 =new QToolButton;
    toolbtn4->setText(tr("4"));
    toolbtn4->setIcon(QPixmap(":/images/jj.png"));
    toolbtn4->setIconSize(QPixmap(":/images/jj.png").size());
    toolbtn4->setAutoRaise(true);
    toolbtn4->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn4,SIGNAL(clicked()),this,SLOT(showChatWidget4()));

    toolbtn5 =new QToolButton;
    toolbtn5->setText(tr("5"));
    toolbtn5->setIcon(QPixmap(":/images/lswh.png"));
    toolbtn5->setIconSize(QPixmap(":/images/lswh.png").size());
    toolbtn5->setAutoRaise(true);
    toolbtn5->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn5,SIGNAL(clicked()),this,SLOT(showChatWidget5()));

    toolbtn6 =new QToolButton;
    toolbtn6->setText(tr("6"));
    toolbtn6->setIcon(QPixmap(":/images/qmnn.png"));
    toolbtn6->setIconSize(QPixmap(":/images/qmnn.png").size());
    toolbtn6->setAutoRaise(true);
    toolbtn6->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn6,SIGNAL(clicked()),this,SLOT(showChatWidget6()));

    toolbtn7 =new QToolButton;
    toolbtn7->setText(tr("7"));
    toolbtn7->setIcon(QPixmap(":/images/tt_wps.png"));
    toolbtn7->setIconSize(QPixmap(":/images/tt——wps.png").size());
    toolbtn7->setAutoRaise(true);
    toolbtn7->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn7,SIGNAL(clicked()),this,SLOT(showChatWidget7()));

    toolbtn8 =new QToolButton;
    toolbtn8->setText(tr("8"));
    toolbtn8->setIcon(QPixmap(":/images/uu_wps.png"));
    toolbtn8->setIconSize(QPixmap(":/images/uu_wps.png").size());
    toolbtn8->setAutoRaise(true);
    toolbtn8->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn8,SIGNAL(clicked()),this,SLOT(showChatWidget8()));

    toolbtn9 =new QToolButton;
    toolbtn9->setText(tr("9"));
    toolbtn9->setIcon(QPixmap(":/images/ymrl.png"));
    toolbtn9->setIconSize(QPixmap(":/images/ymrl.png").size());
    toolbtn9->setAutoRaise(true);
    toolbtn9->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn9,SIGNAL(clicked()),this,SLOT(showChatWidget9()));

    toolbtn10 =new QToolButton;
    toolbtn10->setText(tr("10"));
    toolbtn10->setIcon(QPixmap(":/images/yy_wps.png"));
    toolbtn10->setIconSize(QPixmap(":/images/yy_wps.png").size());
    toolbtn10->setAutoRaise(true);
    toolbtn10->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolbtn10,SIGNAL(clicked()),this,SLOT(showChatWidget9()));

    QGroupBox *groupBox=new QGroupBox; //QGroupBox类实例（对象）
    QVBoxLayout *layout=new QVBoxLayout(groupBox); //布局管理
    layout->setMargin(20);//布局中各窗体的显示间距
    layout->setAlignment(Qt::AlignLeft);//布局中各窗体的显示位置
    layout->addWidget(toolbtn1); //将按钮添加到布局中
    layout->addWidget(toolbtn2);
    layout->addWidget(toolbtn3);
    layout->addWidget(toolbtn4);
    layout->addWidget(toolbtn5);
    layout->addWidget(toolbtn6);
    layout->addWidget(toolbtn7);
    layout->addWidget(toolbtn8);
    layout->addWidget(toolbtn9);
    layout->addWidget(toolbtn10);
    layout->addStretch();//插入一个占位符

    this->addItem((QWidget*)groupBox,tr("群成员"));


}

//槽定义

void Drawer::showChatWidget1()
{
    chatWidget1 = new Widget(nullptr,toolbtn1->text()); //以按钮部件为用户名创建一个widget对象（一个聊天窗口）
    chatWidget1->setWindowTitle(toolbtn1->text());//设置聊天窗口标题文字
    chatWidget1->setWindowIcon(toolbtn1->icon());  //设置聊天串口图标（对应头像）
    chatWidget1->show();  //显示聊天窗口

}
void Drawer::showChatWidget2()
{
    chatWidget2 = new Widget(nullptr,toolbtn2->text());
    chatWidget2->setWindowTitle(toolbtn2->text());
    chatWidget2->setWindowIcon(toolbtn2->icon());
    chatWidget2->show();
}
void Drawer::showChatWidget3()
{
    chatWidget3 = new Widget(nullptr,toolbtn3->text());
    chatWidget3->setWindowTitle(toolbtn3->text());
    chatWidget3->setWindowIcon(toolbtn3->icon());
    chatWidget3->show();
}
void Drawer::showChatWidget4()
{
    chatWidget4 = new Widget(nullptr,toolbtn4->text());
    chatWidget4->setWindowTitle(toolbtn4->text());
    chatWidget4->setWindowIcon(toolbtn4->icon());
    chatWidget4->show();

}
void Drawer::showChatWidget5()
{
    chatWidget5 = new Widget(nullptr,toolbtn5->text());
    chatWidget5->setWindowTitle(toolbtn5->text());
    chatWidget5->setWindowIcon(toolbtn5->icon());
    chatWidget5->show();

}
void Drawer::showChatWidget6()
{
    chatWidget6 = new Widget(nullptr,toolbtn6->text());
    chatWidget6->setWindowTitle(toolbtn6->text());
    chatWidget6->setWindowIcon(toolbtn6->icon());
    chatWidget6->show();
}
void Drawer::showChatWidget7()
{
    chatWidget7 = new Widget(nullptr,toolbtn7->text());
    chatWidget7->setWindowTitle(toolbtn7->text());
    chatWidget7->setWindowIcon(toolbtn7->icon());
    chatWidget7->show();
}
void Drawer::showChatWidget8()
{
    chatWidget8 = new Widget(nullptr,toolbtn8->text());
    chatWidget8->setWindowTitle(toolbtn8->text());
    chatWidget8->setWindowIcon(toolbtn8->icon());
    chatWidget8->show();
}
void  Drawer::showChatWidget9()
{
    chatWidget9 = new Widget(nullptr,toolbtn9->text());
    chatWidget9->setWindowTitle(toolbtn9->text());
    chatWidget9->setWindowIcon(toolbtn9->icon());
    chatWidget9->show();
}
void Drawer::showChatWidget10()
{
    chatWidget10 = new Widget(nullptr,toolbtn10->text());
    chatWidget10->setWindowTitle(toolbtn10->text());
    chatWidget10->setWindowIcon(toolbtn10->icon());
    chatWidget10->show();
}
