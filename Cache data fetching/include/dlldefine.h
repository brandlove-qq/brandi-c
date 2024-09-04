//#include <QtCore/qglobal.h>
#ifndef DLLDEFINE_H
#define DLLDEFINE_H

#endif // DLLDEFINE_H
#include "zskjExportType.h"
typedef struct c_Point
{
int x;
int y;
}c_Point;

typedef struct c_Rect           //区域
{

      int left;
      int top;
      int right;
      int bottom;
}c_Rect;

typedef struct c_ObjectData
{
    c_Rect rect;                      //< 物体所在矩形区域
    c_Point center;                   //< 物体中心点坐标
    unsigned int objectNo;           //< 物体编号
    MoveDirection move;              //< 物体对象的移动方向
    bool isBeyondBoundary;           //< 物体是否超出边界
    unsigned int pointCnt;           //< 物体块的像素点数
    std::string name;                     //< 物体名称，以utf-8格式进行编码
}c_ObjectData,*c_pObjectData;

#if 0
ObjectData c_to_q(c_ObjectData c) //C++类型转QT类型
{
  ObjectData q;

  q.rect.setLeft(c.rect.left);
  q.rect.setTop(c.rect.top);
  q.rect.setRight(c.rect.right);
  q.rect.setBottom(c.rect.bottom);


  q.center.setX(c.center.x);
  q.center.setY(c.center.y);

  q.objectNo=c.objectNo;

  q.move=c.move;

  q.isBeyondBoundary=c.isBeyondBoundary;

  q.pointCnt=c.pointCnt;

  q.name=c.name;

  return q;
}

c_ObjectData q_to_c(ObjectData q) //QT类型转C++类型
{
  c_ObjectData c;

  c.rect.left=q.rect.left();
  c.rect.right=q.rect.right();
  c.rect.top=q.rect.top();
  c.rect.bottom=q.rect.bottom();

  c.center.x=q.center.x();
  c.center.y=q.center.y();

  c.objectNo=q.objectNo;

  c.move=q.move;

  c.isBeyondBoundary=q.isBeyondBoundary;

  c.pointCnt=q.pointCnt;

  c.name=q.name;

  return c;
}
#endif
