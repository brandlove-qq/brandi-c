#include<stdio.h>
#include"operat.h"
//����һ������ͷ
struct taglink *link_create(void)
{struct taglink *pnew=NULL;//��ʱ����
pnew= (struct taglink *)malloc(sizeof(struct tagstu));
pnew->pnext=NULL;
return pnew;
}
//����������ӽڵ�
int link_add(struct taglink *p)
{       char str[8];
        struct taglink *Pclearr=NULL;
        struct taglink *ptemp = p;
        Pclearr=ptemp->pnext;
	struct taglink *pnew = NULL;
	    pnew=(struct taglink *)malloc(sizeof(struct taglink));
        pnew->pnext = NULL;
        printf("�Ƿ������yes or no��:\n");
        scanf("%s",str);
        printf("\n");
if(strcmp(str,"yes")==0)
  {   
p1:
        student_init(&pnew->student);
        while (ptemp->pnext != NULL)
           {if (ptemp->pnext->student.no == pnew->student.no)//������ͬѧ��
            {Pclearr = ptemp->pnext;
             printf("����ͬѧ�����޸�\n");
     goto p1;
     
           break;
            }
          else
              ptemp = ptemp->pnext;//����ƶ�
          }
        pnew->pnext = ptemp->pnext;//ͷ��
	    ptemp->pnext=pnew;
	return 1;
}
else if(strcmp(str,"no")==0)
       return 0;
}


/*int link_add(struct taglink *p)//��ͬѧ��Ҳ����ӽ�ȥbug
{  char str[8];
        struct taglink *ptemp = p;
	struct taglink *pnew = NULL;
	pnew = (struct taglink *)malloc(sizeof(struct taglink));
        pnew->pnext = NULL;
printf("�Ƿ������yes or no��:\n");
scanf("%s",str);
printf("\n");
if(strcmp(str,"yes")==0)
{
       student_init(&pnew->student);
        pnew->pnext = ptemp->pnext;//ͷ��
	ptemp->pnext = pnew;
	return 1;}
else if(strcmp(str,"no")==0)
       return 0;
}*/
void student_init(struct tagstu *p)
{

        printf("������ѧ��:");
	scanf("%d",&p->no);
	printf("����������:");
	scanf("%s",p->name);
     printf("�������Ա�");
        scanf("%s",&p->sex);
        printf("���������䣺");
        scanf("%d",&p->age);
	printf("������ɼ�:");
	scanf("%f", &p->score);
}
//ɾ������ĳ���ڵ�
int link_Del(struct taglink *p)
{char str[8];
        struct taglink *ptemp = p;
	struct taglink *pdel = NULL;
	int del_no;
        printf("��������Ҫɾ����ѧ��:");
	scanf("%d", &del_no);
        printf("�Ƿ������yes or no��:\n");
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
//��ѯ����ĳ���ڵ�
int Student_Seek(struct taglink *p)
{      struct taglink *ptemp =p;//����Ҫ��ѯ��ѧ��
       struct taglink *pseek=NULL;   
        int seek_no;                  
	printf("��������Ҫ��ѯ��ѧ��:");
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

//��ʾ��������
void link_Show(struct taglink *p)
{
	struct taglink *ptemp = p;
	printf("ѧ��\t����\t�ɼ�\n");
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
{     struct taglink *ptemp =p;//����Ҫ�޸ĵ�ѧ��
      struct taglink *pchange=NULL;
	int change_no;
	printf("��������Ҫ�޸ĵ�ѧ��:");     
	scanf("%d", &change_no);
       printf("��������дѧ����Ϣ\n");
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
    printf("ѧ��������Ϊ��\n%d\n",n);   
}