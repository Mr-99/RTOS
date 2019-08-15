#include "tSem.h"
#include "thinyOS.h"
//@初始化信号量
void tSemInit(tSem *sem,uint32_t startCount,uint32_t maxCount)
{
 tEventInit(&sem->event,tEventTypeSem);
 sem->maxCount = maxCount;
	if(maxCount == 0)//maxCount=0，意味资源无限制
	{
		sem->count = startCount;
	}
	else
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;//如果初始值大于最大值，给最大值。否者就是初始值
	}
	
}
uint32_t tSemWait(tSem *sem,uint32_t waitTicks)//获取资源
{
 uint32_t status = tTaskEnterCritical();
				 if(sem->count >0)//如果资源还有
						 {
						 --sem->count;//资源减一
    	       tTaskExitCritical(status); 
    	       return tErrorNoError;
						 }
						 else//如果没有资源，进行排队，等待资源释放
						 { 
						  tEventWait(&sem->event,currentTask,(void*)0,tEventTypeSem,waitTicks);//排队等资源
							tTaskExitCritical(status);//恢复开始状态
							tTaskSched();
						  return currentTask->waitEventResult;
						 }
}
tError tSemNoWaitGet(tSem * sem)//获取资源
{
 uint32_t status = tTaskEnterCritical();//保护现场
					 if(sem->count >0)//如果资源还有
						 {
						--sem->count;//资源减一
						tTaskExitCritical(status);//恢复开始状态
						return tErrorNoError;	 
						 }
						 else
						 {
						tTaskExitCritical(status);//恢复现场	
						return tErrorResourceUnavaliable;
						 }
}
void tSemNotify(tSem * sem)//释放资源
{
 uint32_t status = tTaskEnterCritical();//保护现场
	if(tEventWaitCount(&sem->event) > 0)//如果当前事件中有任务
			{
			 tTask *task = tEventWakeUp(&sem->event,(void*)0,tErrorNoError);
			 if(task->prio < currentTask->prio)//如果任务优先级比当前高(越低越高)
				 {
					tTaskSched();
				 }
			}
	else
		 {
		 	++sem->count;//资源加1
			 if((sem->maxCount != 0)&&(sem->count > sem->maxCount))
			 {
			  sem->count = sem->maxCount;
			 }
		 }
	tTaskExitCritical(status);//恢复现场	
}