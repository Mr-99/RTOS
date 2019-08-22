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
tSem sem1;//�����һ���ź���
tSem sem2;//����ڶ����ź���
//������Ϣ������
tMbox mbox1;
void * mbox1MsgBuffer[20];//�����Ϣ����
//������Ϣ������
tMbox mbox2;
void * mbox2MsgBuffer[20];//�����Ϣ����
uint32_t msg[20];
void task1DestroyFunc(void * param)
{
 task_flag1 = 0;
}
void task1(void *para)
{
	tSetSysTickPeriod(10);//������Сʱ�䵥λ
//  tSemInit(&sem1,0,10);
//	tSemWait(&sem1,0);//�ȴ���λ�ճ���
	tMboxInit(&mbox1,(void**)mbox1MsgBuffer,20);//��ʼ������1
	for(;;)
	  { 
			uint32_t i = 0;
			for(i = 0;i<20; i++)
			{
			msg[i] = i;
			tMboxNotify(&mbox1,&msg[i],tMBOXSendNormal);//���ŵ�����,��ͨ��ʽ
			}
			tTaskDelay(100);
			for(i = 0;i<20; i++)
			{
			msg[i] = i;
			tMboxNotify(&mbox1,&msg[i],tMBoxSendFront);//���ŵ�����
			}
			tTaskDelay(100);
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
			tTaskDelay(1);
			
		}

}
void task2(void *para)
{
	tError error;
	for(;;)
	  {
       void *msg;
			 uint32_t err = tMboxWait(&mbox1,&msg,14);//����
			 //�ȴ���ɺ�����ϴν����ط�ִ�У����Ǵ�ͷ��ʼ
			 if(err == tErrorNoError)
			 {
			  uint32_t value = *(uint32_t *)msg;
				task_flag2 = value;
				tTaskDelay(1);
			 }
//			task_flag2 = 0;
//		  tTaskDelay(1);
//			//tTaskWakeUp(&tTask1);
//			task_flag2 = 1;
//		  tTaskDelay(1);
			//tTaskWakeUp(&tTask1);
//     tSemNotify(&sem1);//�ͷ�һ����Դ
//		 error = tSemNoWaitGet(&sem1);//�޵ȴ���ȡ��Դ
	  }
}
void task3(void *para)
{
	tSemInit(&sem2,0,0);
	tMboxInit(&mbox2,(void**)mbox2MsgBuffer,20);//��ʼ������2
	for(;;)
	  {
			void * msg;
			tMboxWait(&mbox2,&msg,100);
      tSemWait(&sem2,10);
			task_flag3 = 0;
			tTaskDelay(1);
			task_flag3 = 1;
		  tTaskDelay(1);

	  }
}
void task4(void *para)
{
	tSemInfo semInfo;
	int destroyed = 0;
	for(;;)
	  {

			task_flag4 = 0;
			tTaskDelay(1);
			task_flag4 = 1;
		  tTaskDelay(1);
			if(!destroyed)//���û�б�ɾ��
			{
			tSemGetInfo(&sem1,&semInfo);
			tSemDestroy(&sem1);
			destroyed = 1;//����ɾ�����
			}
	  }
}
void tInitApp(void)
{
	tTaskInit(&tTask1,task1,(void *)0x11,0,&task1Env[1024]);
	tTaskInit(&tTask2,task2,(void *)0x22,2,&task2Env[1024]);
	tTaskInit(&tTask3,task3,(void *)0x33,2,&task3Env[1024]);
	tTaskInit(&tTask4,task4,(void *)0x44,2,&task4Env[1024]);
}