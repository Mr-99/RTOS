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
//@获得消息
uint32_t tMboxWait(tMbox *mbox,void **msg,uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();
	  if(mbox->count > 0)//如果有消息
		{
		  --mbox->count;//消息数量减1，因为后面会获取消息
			*msg = mbox->msgBuffer[mbox->read++];
			if(mbox->read >= mbox->maxCount)
			{
			   mbox->read = 0;
			}
		  tTaskExitCritical(status);
			return tErrorNoError;			
		}
		else//没有消息，加入等待列队
		{
		   tEventWait(&mbox->event,currentTask,(void *)0,tEventTypeMbox,waitTicks);//将当前任务加入等待列队
			 tTaskExitCritical(status);
			
			 tTaskSched();
			
			 *msg = currentTask->eventMsg;
			  return currentTask->waitEventResult;
			 
		}
}
//@无等待获取消息
uint32_t tMboxNoWaitGet(tMbox * mbox,void **msg)
{
    uint32_t status = tTaskEnterCritical();
		if(mbox->count > 0)//如果有消息
		{
		  --mbox->count;//消息数量减1，因为后面会获取消息
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
			return tErrorResourceUnavaliable;//返回无消息
		}
}
//@释放资源操作
uint32_t tMboxNotify(tMbox * mbox,void * msg,uint32_t notifyOption)
{
    uint32_t status = tTaskEnterCritical();
	  if(tEventWaitCount(&mbox->event) > 0)//如果有等待资源的任务
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