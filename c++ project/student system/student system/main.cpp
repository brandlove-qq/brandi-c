#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "operat.h"
#include "menushow.h"
struct taglink *phead = NULL;//ָ������ͷ
void link_StoreMsg(struct taglink *p);
void link_GetMsg(struct taglink *p);
int main()
{   phead=link_create();
     int num=0;
p1:
	menu_Show();
        printf("������ѡ�");
	scanf("%d", &num);
	switch (num)
	{
		case 1:if(link_add(phead)==1) printf("��ӳɹ�");
                           else
                                 printf("���ʧ��\n");  break;
		case 2:if (link_Del(phead) == 0) printf("ɾ��ʧ�ܣ�\n");
			   else
				   printf("ɾ���ɹ���\n"); break;
		case 3:if (Student_Seek(phead) == 0) printf("��ѯʧ�ܣ�\n");
			   else
				   printf("��ѯ�ɹ���\n"); break;
                case 4:if(Student_Change(phead)==0) printf("�޸�ʧ�ܣ�\n");
                          else
                                 printf("�޸ĳɹ���\n"); break;
                case 5: student_sum(phead);break;
		case 6:link_Show(phead);break;
		case 0:printf("ллʹ�ã�\n");break;
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
	rewind(fp);//�ص��ļ�ͷ
	while (feof(fp) == 0)
	{
		pnew = (struct taglink*)malloc(sizeof(struct taglink));
		pnew->pnext = NULL;
		if (fread(&pnew->student, sizeof(pnew->student), 1, fp) == 0)//��ȡ���ɹ�
		{
			free(pnew);
		}
		else //��ȡ�ɹ�
		{ 
			ptemp->pnext = pnew;//��ӵ�������ȥ
			ptemp = ptemp->pnext;//
		}
	}
	fclose(fp);
}










