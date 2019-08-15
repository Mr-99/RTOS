#include "tSem.h"
#include "thinyOS.h"
//@��ʼ���ź���
void tSemInit(tSem *sem,uint32_t startCount,uint32_t maxCount)
{
 tEventInit(&sem->event,tEventTypeSem);
 sem->maxCount = maxCount;
	if(maxCount == 0)//maxCount=0����ζ��Դ������
	{
		sem->count = startCount;
	}
	else
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;//�����ʼֵ�������ֵ�������ֵ�����߾��ǳ�ʼֵ
	}
	
}
uint32_t tSemWait(tSem *sem,uint32_t waitTicks)//��ȡ��Դ
{
 uint32_t status = tTaskEnterCritical();
				 if(sem->count >0)//�����Դ����
						 {
						 --sem->count;//��Դ��һ
    	       tTaskExitCritical(status); 
    	       return tErrorNoError;
						 }
						 else//���û����Դ�������Ŷӣ��ȴ���Դ�ͷ�
						 { 
						  tEventWait(&sem->event,currentTask,(void*)0,tEventTypeSem,waitTicks);//�Ŷӵ���Դ
							tTaskExitCritical(status);//�ָ���ʼ״̬
							tTaskSched();
						  return currentTask->waitEventResult;
						 }
}
tError tSemNoWaitGet(tSem * sem)//��ȡ��Դ
{
 uint32_t status = tTaskEnterCritical();//�����ֳ�
					 if(sem->count >0)//�����Դ����
						 {
						--sem->count;//��Դ��һ
						tTaskExitCritical(status);//�ָ���ʼ״̬
						return tErrorNoError;	 
						 }
						 else
						 {
						tTaskExitCritical(status);//�ָ��ֳ�	
						return tErrorResourceUnavaliable;
						 }
}
void tSemNotify(tSem * sem)//�ͷ���Դ
{
 uint32_t status = tTaskEnterCritical();//�����ֳ�
	if(tEventWaitCount(&sem->event) > 0)//�����ǰ�¼���������
			{
			 tTask *task = tEventWakeUp(&sem->event,(void*)0,tErrorNoError);
			 if(task->prio < currentTask->prio)//����������ȼ��ȵ�ǰ��(Խ��Խ��)
				 {
					tTaskSched();
				 }
			}
	else
		 {
		 	++sem->count;//��Դ��1
			 if((sem->maxCount != 0)&&(sem->count > sem->maxCount))
			 {
			  sem->count = sem->maxCount;
			 }
		 }
	tTaskExitCritical(status);//�ָ��ֳ�	
}