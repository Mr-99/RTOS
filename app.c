#include "thinyOS.h"
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
tSem sem;//����һ���ź���
void task1DestroyFunc(void * param)
{
 task_flag1 = 0;
}
void task1(void *para)
{
	tSetSysTickPeriod(10);//������Сʱ�䵥λ
  tSemInit(&sem,1,10);
	for(;;)
	  { 

			task_flag1 = 0;
//tTaskSchedEnable();
//			//ģ������̣���-��-д�룩
//			temp = tickCounter;//��
			  tTaskDelay(1);;//�ģ�����������ٽ����������ڸĵĹ��̷����ж�,���д��Ḳ�ǻ�������
//			temp=3;
//			tickCounter=temp;//д
//			//end
//tTaskSchedDisable();
			task_flag1 = 1;
			  tTaskDelay(1);;
			
		}

}
void task2(void *para)
{
	for(;;)
	  {

			task_flag2 = 0;
		  tTaskDelay(1);
			//tTaskWakeUp(&tTask1);
			task_flag2 = 1;
		  tTaskDelay(1);
			//tTaskWakeUp(&tTask1);

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