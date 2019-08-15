#include "thinyOS.h"
extern tBitmap taskPrioBitmap;
extern tList taskTable[TINYOS_PRO_COUNT];//定义32个链表
/*任务初始化,*/
void tTaskInit(tTask *task,void (*entry)(void *),void *param,uint32_t prio,tTaskStack *stack)
		{
		*(--stack) = (unsigned long)(1<<24);
		*(--stack) = (unsigned long)entry;	
		*(--stack) = (unsigned long)0x14;	
    *(--stack) = (unsigned long)0x12;	
    *(--stack) = (unsigned long)0x3;	 
    *(--stack) = (unsigned long)0x2;
		*(--stack) = (unsigned long)0x1;
		*(--stack) = (unsigned long)param;
			
		*(--stack) = (unsigned long)0x11;
    *(--stack) = (unsigned long)0x10;
		*(--stack) = (unsigned long)0x9;
		*(--stack) = (unsigned long)0x8;
		*(--stack) = (unsigned long)0x7;
		*(--stack) = (unsigned long)0x6;
		*(--stack) = (unsigned long)0x5;
		*(--stack) = (unsigned long)0x4;
			
		  task->stack = stack;
			task->delayTicks = 0;
			task->prio = prio; 
			//任务状态初始化
			task->state = TINYOS_TASK_STATE_RDY;

		  //同优先级时间片运行
			tNodeInit(&(task->linkNode));//初始化结点
						//延时结点初始化
			tNodeInit(&(task->delayNode));//初始化延时结点
			tListAddFirst(&taskTable[prio],&(task->linkNode));//在该优先级下的链表中增加结点
			tBitmapSet(&taskPrioBitmap,prio);
			task->slice = TINYOS_SLICE_MAX;
			task->suspendCount = TINYOS_TASK_STATE_RDY;
			//初始化清理函数部分
			task->clean = (void (*)(void *))0;//清理函数指针指向空
			task->cleanParam = (void *)0;
			task->requestDeleteFlag = 0;
		}
//@挂起指定的任务
void tTaskSuspend (tTask *task)
{
uint32_t status = tTaskEnterCritical();
 if(!(task->state & (uint32_t)TINYOS_TASK_STATE_DELAYED))
 {
   if(++task->suspendCount <= 1)
	    {
	       task->state |= (uint32_t)TINYOS_TASK_STATE_SUSPEND;
				 tTaskSchedUnRdy(task);
				 if(task == currentTask)
				    {
				      tTaskSched();
				    }
	    }
 }
tTaskExitCritical(status);

}
//@唤醒被挂起的任务
void tTaskWakeUp(tTask * task)
{
  uint32_t status = tTaskEnterCritical();
	if( task->state & (uint32_t)TINYOS_TASK_STATE_SUSPEND )
	     {
	    if(--task->suspendCount == 0)
			    {
			      task->state &= ~(uint32_t)TINYOS_TASK_STATE_SUSPEND;
						tTaskSchedRdy(task);
						tTaskSched();
			    }
	     }
	tTaskExitCritical(status);
}
//@将任务从就绪列表中删除
void tTaskSchedRemove(tTask *task)
{
  tListRemove(&taskTable[task->prio],&(task->linkNode));
	if(tListCount(&taskTable[task->prio]) == 0)
	{
	   tBitmapClear(&taskPrioBitmap,task->prio);
	}
}
//@从延时列队中删除
void tTimeTaskRemove(tTask *task)
{
  tListRemove(&tTaskDelayedList,&(task->delayNode));
}
//@设置清除函数
void tTaskSetCleanCallFunc(tTask *task,void (*clean)(void *param),void *param)
{
  task->clean = clean;
	task->cleanParam = param;
}
//@强制删除
void tTaskForceDelete(tTask *task)
{
  uint32_t status = tTaskEnterCritical();
  if(task->state & (uint32_t)TINYOS_TASK_STATE_DELAYED)
	{
	  tTimeTaskRemove(task);
	}
	else if(!(task->state & TINYOS_TASK_STATE_SUSPEND))
	{
	   tTaskSchedRemove(task);
	}
	if(task->clean)
	{
	  task->clean(task->cleanParam);
	}
	if(currentTask == task)
	{
	 tTaskSched();
	}
	tTaskExitCritical(status);
}
//@请求删除
void tTaskRequestDelete(tTask *task)
{
  uint32_t status = tTaskEnterCritical();
  task->requestDeleteFlag = 1;
	tTaskExitCritical(status);
}
uint8_t tTaskIsRequesteDeleted(void)
{
	uint32_t t_delete;
  uint32_t status = tTaskEnterCritical();
  t_delete = currentTask->requestDeleteFlag;
	tTaskExitCritical(status);
	return t_delete;
}
//@删除自己
void tTaskDeleteSelf(void)
{
   uint32_t status = tTaskEnterCritical();
   tTaskSchedRemove(currentTask);
	if(currentTask->clean)
	{
	  currentTask->clean(currentTask->cleanParam);
	}
	tTaskSched();
	tTaskExitCritical(status);

}
//@查询任务状态函数
void tTaskGetInfo(tTask * task,tTaskInfo * info)
{
   uint32_t status = tTaskEnterCritical();
		info->delayTicks = task->delayTicks;
		info->prio = task->prio;
		info->state = task->state; 
		info->slice =  task->slice;
		info->suspendCount = task->suspendCount;
	tTaskExitCritical(status);
}