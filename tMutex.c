#include "tMutex.h"
#include "thinyOS.h"
void tMutexInit(tMutex * mutex)
{

	tEventInit(&mutex->event,tEventTypeMutex);
	mutex->owner = (tTask*)0;
	mutex->ownerOriginalPrio = TINYOS_PRO_COUNT;
}
uint32_t tMutexWait(tMutex * mutex,uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();
	 if(mutex ->lockedCount <= 0)
	 {
	 mutex->owner = currentTask;
	 mutex->ownerOriginalPrio = currentTask->prio;
	 mutex->lockedCount++;
	 tTaskExitCritical(status);
	 return tErrorNoError;
	 }
	 else
	 {
					 if(mutex->owner == currentTask)
					  {
						mutex->lockedCount++;
						tTaskExitCritical(status);
						return tErrorNoError;
					  }
					 else//如果其他任务要求占用资源,并且优先级比现在占用的任务高，设置为高优先级加快运行
					  {
							 if(currentTask->prio < mutex->owner->prio)//将占有的任务设置为高优先级，加快运行
							 {
												 tTask * owner = mutex->owner;
												 if(owner->state == TINYOS_TASK_STATE_RDY)
												 {
														tTaskSchedUnRdy(owner);
														owner->prio = currentTask->prio;
														tTaskSchedRdy(owner);
												 }
												 else//如果不是就绪状态
												 {
														owner->prio = currentTask->prio;
												 }
							 }
					 tEventWait(&mutex->event,currentTask,(void*)0,tEventTypeMutex,waitTicks);
					 tTaskExitCritical(status);
						 
						 tTaskSched();
						 
					 return currentTask->waitEventResult;
					  }
	 }
}
uint32_t tMutexNoWaitGet(tMutex *mutex)
{
    uint32_t status = tTaskEnterCritical();
		 if(mutex ->lockedCount <= 0)
		 {
	 mutex->owner = currentTask;
	 mutex->ownerOriginalPrio = currentTask->prio;
	 mutex->lockedCount++;
		tTaskExitCritical(status);
    return tErrorNoError;
		 }
		 else
		 {
					if(mutex->owner == currentTask)
					{
					   mutex->lockedCount++;
								tTaskExitCritical(status);
								return tErrorNoError;
					}
					tTaskExitCritical(status);
					return tErrorResourceUnavaliable;
		 }
}
uint32_t tMutexNotify(tMutex * mutex)
{
    uint32_t status = tTaskEnterCritical();
	  if(mutex->lockedCount <= 0)
		{
			tTaskExitCritical(status);
			return tErrorNoError;
		}
    if(mutex->owner != currentTask)//不是占用者释放则退出
		{
			tTaskExitCritical(status);
			return tErrorOwner;
		}
		if(--mutex->lockedCount > 0)//如果是当前任务，就释放资源，并且判断是否还有占据
		{
			tTaskExitCritical(status);
			return tErrorNoError;
		}
	  if(mutex->ownerOriginalPrio != mutex->owner->prio)
		{
		  if(mutex->owner->state == TINYOS_TASK_STATE_RDY)
			{
			 tTaskSchedUnRdy(mutex->owner);
       currentTask->prio = mutex->ownerOriginalPrio;
			 tTaskSchedRdy(mutex->owner);
			}
			else
			{
			 currentTask->prio = mutex->ownerOriginalPrio;
			}
		}
		
		if( tEventWaitCount(&mutex->event) > 0 )
		{
		    tTask * task = tEventWakeUp(&mutex->event,(void*)0,tErrorNoError);
			
			mutex->owner = task;
			mutex->ownerOriginalPrio = task->prio;
			mutex->lockedCount++;
			
			if(task->prio < currentTask->prio)
				{
				 tTaskSched();
				}
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	
}
//@删除接口
uint32_t tMutexDestroy(tMutex * mutex)
{  
	uint32_t count = 0; 
  uint32_t status = tTaskEnterCritical();
	if(mutex->lockedCount > 0)
	{
	   if(mutex->ownerOriginalPrio != mutex->owner->prio)
		 {
		    if(mutex->owner->state == TINYOS_TASK_STATE_RDY)
				{
				   tTaskSchedUnRdy(mutex->owner);
					 mutex->owner->prio = mutex->ownerOriginalPrio;
					 tTaskSchedRdy(mutex->owner);
				}
				else
			  {
			 	  mutex->owner->prio = mutex->ownerOriginalPrio;
			  }
		 }
		count = tEventRemoveAll(&mutex->event,(void*)0,tErrorDel);
    if(count > 0)
		{
		  tTaskSched();
		}
	}
		tTaskExitCritical(status);
    return count;
}
//@状态查询接口
void tMutexGetInfo(tMutex * mutex,tMutexInfo * info)
{
  uint32_t status = tTaskEnterCritical();
	
	info->taskCount = tEventWaitCount(&mutex->event);
	info->ownerPrio = mutex->ownerOriginalPrio;
	if( mutex->owner != (tTask*)0 )
	{
		info->inheritedPrio = mutex->owner->prio;
	}
	else
	{
	info->inheritedPrio = TINYOS_PRO_COUNT;
	}

	info->owner = mutex->owner;
	info->lockedCount = mutex->lockedCount;
	
	
	tTaskExitCritical(status);
	
}