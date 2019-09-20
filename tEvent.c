#include "thinyOS.h"
void tEventInit(tEvent * event,tEventType type)
{
event->type = type;
tListInit(&(event->waitList));
}
//@让任务去等待事件到来
void tEventWait(tEvent * event,tTask * task,void * msg,uint32_t state,uint32_t timeout)
{
	 uint32_t status = tTaskEnterCritical();
   task->state |= state << 16; //更新任务状态
	 task->waitEvent = event;//等待的事件类型
	 task->eventMsg = msg;//事件的消息
	 task->waitEventResult =  tErrorNoError;
	 tTaskSchedUnRdy(task);//将任务从就绪队列中移除
	 tListAddLast(&event->waitList,&task->linkNode);//插入到等待队列
	 if(timeout)
	  {
	     tTimeTaskWait(task,timeout);//进行timeout时间长度的等待，如果没有事件将任务唤醒,等待超时
	  }
	 tTaskExitCritical(status);
}
//@事件到来,唤醒任务
tTask * tEventWakeUp(tEvent * event,void * msg,uint32_t result)
{
 tNode * node;
 tTask * task  = (tTask*)0;
	 uint32_t status = tTaskEnterCritical();
    if((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
		{
		   task = (tTask*)tNodeParent(node,tTask,linkNode);
			 task->waitEvent = (tEvent*)0;//任务没有事件等待
			 task->eventMsg = msg;
			 task->waitEventResult =  result;
			 task->state &= ~TINYOS_TASK_WAIT_MASK;
			 if(task->delayTicks != 0)
			 {
			  tTimeTaskWakeUp(task);
			 }
			  tTaskSchedRdy(task);//唤醒任务，加入就绪列表
		}
	 tTaskExitCritical(status);
	return task;
}
//@将任务从事件控制块中移除
void tEventRemoveTask(tTask *task,void *msg,uint32_t result)
{
	 uint32_t status = tTaskEnterCritical();
	 tListRemove(&task->waitEvent->waitList,&task->linkNode);//将任务在所在的事件等待列队中删除
	 task->waitEvent = (tEvent*)0;//任务没有事件等待
	 task->eventMsg = msg;
	 task->waitEventResult =  result;
	 task->state &= ~TINYOS_TASK_WAIT_MASK;
	 tTaskExitCritical(status);
}
//@删除事件中所有任务
uint32_t tEventRemoveAll(tEvent *event,void *msg,uint32_t result)
{
	 tNode * node;
	 uint32_t count = 0;
	 uint32_t status = tTaskEnterCritical();
	 count = tListCount(&event->waitList);//计算出事件中的任务的个数
	 //从事件中循环删除任务
	 while((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
	  {
		 tTask * task = (tTask*)tNodeParent(node,tTask,linkNode);
		 task->waitEvent = (tEvent *)0;
		 task->eventMsg = msg;
		 task->waitEventResult = result;
		 task->state &= ~TINYOS_TASK_WAIT_MASK;
		 if(task->delayTicks != 0)//如果设置有超时时间，超时时间不为零
		    {
		 tTimeTaskWakeUp(task);
		    }
		 tTaskSchedRdy(task);
		}
	 tTaskExitCritical(status);
	 return count;
}
//@计算事件中有多少个任务
uint32_t tEventWaitCount(tEvent * event)
{
 uint32_t count = 0;
	 uint32_t status = tTaskEnterCritical();
	 count = tListCount(&event->waitList);
	 tTaskExitCritical(status);
	return count;
}
/**********************************************************************************************************
** Function name        :   tEventWakeUpTask
** Descriptions         :   从事件控制块中唤醒指定任务
** parameters           :   event 事件控制块
** parameters           :   task 等待唤醒的任务
** parameters           :   msg 事件消息
** parameters           :   result 告知事件的等待结果
** Returned value       :   首个等待的任务，如果没有任务等待，则返回0
***********************************************************************************************************/
void tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result)
{
    // 进入临界区
    uint32_t status = tTaskEnterCritical();

    tListRemove(&event->waitList, &task->linkNode);

    // 设置收到的消息、结构，清除相应的等待标志位
    task->waitEvent = (tEvent *)0;
    task->eventMsg = msg;
    task->waitEventResult = result;
    task->state &= ~TINYOS_TASK_WAIT_MASK;

    // 任务申请了超时等待，这里检查下，将其从延时队列中移除
    if (task->delayTicks != 0)
    {
        tTimeTaskWakeUp(task);
    }

    // 将任务加入就绪队列
    tTaskSchedRdy(task);

    // 退出临界区
    tTaskExitCritical(status);
}
