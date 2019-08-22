#include "thinyOS.h"
void tMboxInit(tMbox * mbox,void **msgBuffer,uint32_t maxCount)
{
   tEventInit(&mbox->event,tEventTypeMbox);
	  mbox->msgBuffer = msgBuffer;
	  mbox->maxCount = maxCount;
	  mbox->read = 0;
	  mbox->write = 0;
	  mbox->count = 0;
}
//@�����Ϣ
uint32_t tMboxWait(tMbox *mbox,void **msg,uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();
	  if(mbox->count > 0)//�������Ϣ
		{
		  --mbox->count;//��Ϣ������1����Ϊ������ȡ��Ϣ
			*msg = mbox->msgBuffer[mbox->read++];
			if(mbox->read >= mbox->maxCount)
			{
			   mbox->read = 0;
			}
		  tTaskExitCritical(status);
			return tErrorNoError;			
		}
		else//û����Ϣ������ȴ��ж�
		{
		   tEventWait(&mbox->event,currentTask,(void *)0,tEventTypeMbox,waitTicks);//����ǰ�������ȴ��ж�
			 tTaskExitCritical(status);
			
			 tTaskSched();
			
			 *msg = currentTask->eventMsg;
			  return currentTask->waitEventResult;
			 
		}
}
//@�޵ȴ���ȡ��Ϣ
uint32_t tMboxNoWaitGet(tMbox * mbox,void **msg)
{
    uint32_t status = tTaskEnterCritical();
		if(mbox->count > 0)//�������Ϣ
		{
		  --mbox->count;//��Ϣ������1����Ϊ������ȡ��Ϣ
			*msg = mbox->msgBuffer[mbox->read++];
			if(mbox->read >= mbox->maxCount)
			{
			   mbox->read = 0;
			}
		  tTaskExitCritical(status);
			return tErrorNoError;			
		}
		else
		{
		  tTaskExitCritical(status);
			return tErrorResourceUnavaliable;//��������Ϣ
		}
}
//@�ͷ���Դ����
uint32_t tMboxNotify(tMbox * mbox,void * msg,uint32_t notifyOption)
{
    uint32_t status = tTaskEnterCritical();
	  if(tEventWaitCount(&mbox->event) > 0)//����еȴ���Դ������
		{
		  tTask *task = tEventWakeUp(&mbox->event,(void *)msg,tErrorNoError);
			if(task->prio < currentTask->prio)
			{
			   tTaskSched();
			}
		}
		else
		{
		   if(mbox->count >= mbox->maxCount)
			 {
			   tTaskExitCritical(status);
				 return tErrorResourceFull;
			 }
    if(notifyOption & tMBoxSendFront)
				{
				if(mbox->read <= 0)
					{
					  mbox->read = mbox->maxCount - 1;
					}
					else
					{
					  --mbox->read;
					}
					mbox->msgBuffer[mbox->read] = msg;
				}
				else
				{
				   mbox->msgBuffer[mbox->write++] = msg;
					 if(mbox->write >= mbox->maxCount)
					 {
					    mbox->write = 0;
					 }
				}
				mbox->count++;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
}