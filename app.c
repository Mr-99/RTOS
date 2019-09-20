#include "thinyOS.h"
#include "tMenBlock.h"
#include <string.h>
/*������������صĺ�������*/
tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;
/*�����ջ*/
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];
tTaskStack task4Env[1024];
//�¼����ƿ�
tEvent eventWaitNormal;

//��������ر���
int task_flag1;
int task_flag2;
int task_flag3;
int task_flag4;
//�ź���
tSem sem1;//�����һ���ź���
tSem sem2;//����ڶ����ź���
//����1��Ϣ������
tMbox mbox1;
void * mbox1MsgBuffer[20];//�����Ϣ����
uint32_t msg1[20];
//�洢�����
typedef uint8_t (*tBlock)[100];
uint8_t mem1[20][100];
tMemBlock memBlock1;

//�¼���־��
tFlagGroup flagGroup1;
//�����ź�
tMutex mutex;
void task1DestroyFunc(void * param)
{
 task_flag1 = 0;
}
void task1(void *para)
{
	uint8_t i;
	uint8_t *mem;
	tMutexInfo info;
	tSetSysTickPeriod(10);//������Сʱ�䵥λ
	
  tMutexInit(&mutex);//��ʼ�������ź�
	tMutexWait(&mutex,0);
	tTaskDelay(2);
	
	tMutexGetInfo(&mutex,&info);
	
	tMutexDestroy(&mutex);
	
	for(;;)
	  { 
			tMutexWait(&mutex,0);
			tMutexWait(&mutex,0);
			
			task_flag1 = 0;
			tTaskDelay(1);;//�ģ�����������ٽ����������ڸĵĹ��̷����ж�,���д��Ḳ�ǻ�������
			task_flag1 = 1;
			tTaskDelay(1);
			
			tMutexNotify(&mutex);
			tMutexNotify(&mutex);
			
		}

}
void task2(void *para)
{
	int destroy = 0;
	uint32_t resultFlags;
	tFlagGroupInfo info;

	tMutexWait(&mutex,0);
	
	for(;;)
	  {

			task_flag2 = 0;
		  tTaskDelay(1);
			task_flag2 = 1;
		  tTaskDelay(1);
						
			tMutexNotify(&mutex);
			tMutexNotify(&mutex);
//			 if(!destroy)
//				 {
//					 tMemBlockDestroy(&memBlock1);
//					 destroy = 1;
//				 }
	  }
}
void task3(void *para)
{
	for(;;)
	  {
			task_flag3 = 0;
			tTaskDelay(1);
			task_flag3 = 1;
		  tTaskDelay(1);
	  }
}
void task4(void *para)
{
	for(;;)
	  {
			task_flag4 = 0;
			tTaskDelay(1);
			task_flag4 = 1;
		  tTaskDelay(1);
	  }
}
void tInitApp(void)
{
	tTaskInit(&tTask1,task1,(void *)0x11,0,&task1Env[1024]);
	tTaskInit(&tTask2,task2,(void *)0x22,2,&task2Env[1024]);
	tTaskInit(&tTask3,task3,(void *)0x33,2,&task3Env[1024]);
	tTaskInit(&tTask4,task4,(void *)0x44,2,&task4Env[1024]);
}