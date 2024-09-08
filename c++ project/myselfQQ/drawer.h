#ifndef DRAWER_H
#define DRAWER_H

#include <QToolBox>
#include <QToolButton>
#include "widget.h"

class Drawer : public QToolBox
{
    Q_OBJECT
public:
    Drawer(QWidget *parent=0,Qt::WindowFlags f=0);
     //声明聊天对象
private slots:
    void showChatWidget1();
    void showChatWidget2();
    void showChatWidget3();
    void showChatWidget4();
    void showChatWidget5();
    void showChatWidget6();
    void showChatWidget7();
    void showChatWidget8();
    void showChatWidget9();
    void showChatWidget10();

private:
    //10个按钮部件，对应10个客户
    QToolButton *toolbtn1;
    QToolButton *toolbtn2;
    QToolButton *toolbtn3;
    QToolButton *toolbtn4;
    QToolButton *toolbtn5;
    QToolButton *toolbtn6;
    QToolButton *toolbtn7;
    QToolButton *toolbtn8;
    QToolButton *toolbtn9;
    QToolButton *toolbtn10;
    //声明聊天对象
    Widget *chatWidget1;
    Widget *chatWidget2;
    Widget *chatWidget3;
    Widget *chatWidget4;
    Widget *chatWidget5;
    Widget *chatWidget6;
    Widget *chatWidget7;
    Widget *chatWidget8;
    Widget *chatWidget9;
    Widget *chatWidget10;
};

#endif // DRAWER_H
