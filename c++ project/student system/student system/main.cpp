#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "operat.h"
#include "menushow.h"
struct taglink *phead = NULL;//指向链表头
void link_StoreMsg(struct taglink *p);
void link_GetMsg(struct taglink *p);
int main()
{   phead=link_create();
     int num=0;
p1:
	menu_Show();
        printf("请输入选项：");
	scanf("%d", &num);
	switch (num)
	{
		case 1:if(link_add(phead)==1) printf("添加成功");
                           else
                                 printf("添加失败\n");  break;
		case 2:if (link_Del(phead) == 0) printf("删除失败！\n");
			   else
				   printf("删除成功！\n"); break;
		case 3:if (Student_Seek(phead) == 0) printf("查询失败！\n");
			   else
				   printf("查询成功！\n"); break;
                case 4:if(Student_Change(phead)==0) printf("修改失败！\n");
                          else
                                 printf("修改成功！\n"); break;
                case 5: student_sum(phead);break;
		case 6:link_Show(phead);break;
		case 0:printf("谢谢使用！\n");break;
	}
	goto p1;
}



void link_StoreMsg(struct taglink *p)
{
	struct taglink *ptemp = p->pnext;
	FILE *fp = NULL;
	fp = fopen("student.txt","a+");
	
	while (ptemp != NULL)
	{
		fwrite(ptemp,sizeof(ptemp->student),1,fp);
		ptemp = ptemp->pnext;
	}
	fclose(fp);
}

void link_GetMsg(struct taglink *p)
{
	struct taglink *ptemp = p;
	struct taglink *pnew = NULL;
	FILE *fp = NULL;
	fp = fopen("student.txt", "a+");
	rewind(fp);//回到文件头
	while (feof(fp) == 0)
	{
		pnew = (struct taglink*)malloc(sizeof(struct taglink));
		pnew->pnext = NULL;
		if (fread(&pnew->student, sizeof(pnew->student), 1, fp) == 0)//读取不成功
		{
			free(pnew);
		}
		else //读取成功
		{ 
			ptemp->pnext = pnew;//添加到链表中去
			ptemp = ptemp->pnext;//
		}
	}
	fclose(fp);
}










