#ifndef _OPERAT_H_
#define _OPERAT_H_

struct tagstu//描述学生
{
	int no;
	char name[16];
        char sex;
        int age;
	float score;
};
struct taglink
{
	struct tagstu student;//数据域
	struct taglink *pnext;        //指针域
};
int link_add(struct taglink *p);
void student_init(struct tagstu *p);
int link_Del(struct taglink *p);
int Student_Seek(struct taglink *p);
int Student_Change(struct taglink *p);
void link_Show(struct taglink *p);
void menu_Show(void);
struct taglink *link_create(void);
void student_sum(struct taglink *p);

#endif