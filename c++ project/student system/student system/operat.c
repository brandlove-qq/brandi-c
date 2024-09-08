#include<stdio.h>
#include"operat.h"
//创建一个链表头
struct taglink *link_create(void)
{struct taglink *pnew=NULL;//临时变量
pnew= (struct taglink *)malloc(sizeof(struct tagstu));
pnew->pnext=NULL;
return pnew;
}
//给链表中添加节点
int link_add(struct taglink *p)
{       char str[8];
        struct taglink *Pclearr=NULL;
        struct taglink *ptemp = p;
        Pclearr=ptemp->pnext;
	struct taglink *pnew = NULL;
	    pnew=(struct taglink *)malloc(sizeof(struct taglink));
        pnew->pnext = NULL;
        printf("是否继续（yes or no）:\n");
        scanf("%s",str);
        printf("\n");
if(strcmp(str,"yes")==0)
  {   
p1:
        student_init(&pnew->student);
        while (ptemp->pnext != NULL)
           {if (ptemp->pnext->student.no == pnew->student.no)//查找相同学号
            {Pclearr = ptemp->pnext;
             printf("有相同学号请修改\n");
     goto p1;
     
           break;
            }
          else
              ptemp = ptemp->pnext;//向后移动
          }
        pnew->pnext = ptemp->pnext;//头插
	    ptemp->pnext=pnew;
	return 1;
}
else if(strcmp(str,"no")==0)
       return 0;
}


/*int link_add(struct taglink *p)//相同学号也会添加进去bug
{  char str[8];
        struct taglink *ptemp = p;
	struct taglink *pnew = NULL;
	pnew = (struct taglink *)malloc(sizeof(struct taglink));
        pnew->pnext = NULL;
printf("是否继续（yes or no）:\n");
scanf("%s",str);
printf("\n");
if(strcmp(str,"yes")==0)
{
       student_init(&pnew->student);
        pnew->pnext = ptemp->pnext;//头插
	ptemp->pnext = pnew;
	return 1;}
else if(strcmp(str,"no")==0)
       return 0;
}*/
void student_init(struct tagstu *p)
{

        printf("请输入学号:");
	scanf("%d",&p->no);
	printf("请输入姓名:");
	scanf("%s",p->name);
     printf("请输入性别：");
        scanf("%s",&p->sex);
        printf("请输入年龄：");
        scanf("%d",&p->age);
	printf("请输入成绩:");
	scanf("%f", &p->score);
}
//删除链表某个节点
int link_Del(struct taglink *p)
{char str[8];
        struct taglink *ptemp = p;
	struct taglink *pdel = NULL;
	int del_no;
        printf("请输入您要删除的学号:");
	scanf("%d", &del_no);
        printf("是否继续（yes or no）:\n");
       scanf("%s",str);
       printf("\n");
if(strcmp(str,"yes")==0)
{
	while (ptemp->pnext != NULL)
	{
		if (ptemp->pnext->student.no == del_no)
		{
			pdel = ptemp->pnext;
			ptemp->pnext = pdel->pnext;
			free(pdel);
			return 1;
		}
		ptemp = ptemp->pnext;
	}
	return 0;
}
else if(strcmp(str,"no")==0)
       return 0;
}
//查询链表某个节点
int Student_Seek(struct taglink *p)
{      struct taglink *ptemp =p;//保存要查询的学号
       struct taglink *pseek=NULL;   
        int seek_no;                  
	printf("请输入您要查询的学号:");
	scanf("%d",&seek_no);
while (ptemp->pnext!= NULL)
	{
		if (ptemp->pnext->student.no == seek_no)
		{pseek = ptemp->pnext;
                printf("%d\t", ptemp->pnext->student.no);
		printf("%s\t", ptemp->pnext->student.name);
                printf("%c\t", ptemp->pnext->student.sex);
                printf("%d\t", ptemp->pnext->student.age);
		printf("%.2f\n", ptemp->pnext->student.score);
           return 1;
                 }
                ptemp = ptemp->pnext;
        }
         return 0;
}

//显示整个链表
void link_Show(struct taglink *p)
{
	struct taglink *ptemp = p;
	printf("学号\t姓名\t成绩\n");
	while (ptemp->pnext != NULL)
	{
		printf("%d\t", ptemp->pnext->student.no);
		printf("%s\t", ptemp->pnext->student.name);
                printf("%c\t", ptemp->pnext->student.sex);
                printf("%d\t", ptemp->pnext->student.age);
		printf("%.2f\n", ptemp->pnext->student.score);
		ptemp = ptemp->pnext;
	}
}

int Student_Change(struct taglink *p)
{     struct taglink *ptemp =p;//保存要修改的学号
      struct taglink *pchange=NULL;
	int change_no;
	printf("请输入您要修改的学号:");     
	scanf("%d", &change_no);
       printf("请重新填写学生信息\n");
while (ptemp->pnext!= NULL)
	{
		if (ptemp->pnext->student.no == change_no)
		{pchange= ptemp->pnext;
                 student_init(&pchange->student);
	          return 1;
                }
      ptemp = ptemp->pnext;
          }
return 0;
}

void student_sum(struct taglink *p)
{  int n=0;
  struct taglink *ptemp =p;
while(ptemp->pnext!=NULL)
   {ptemp = ptemp->pnext;
   n++;
   }   
    printf("学生的数量为：\n%d\n",n);   
}