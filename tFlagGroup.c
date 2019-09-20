#include "tFlagGroup.h"
#include "thinyOS.h"
void tFlagGroupInit(tFlagGroup * falgGroup,uint32_t flags)
{
  tEventInit(&falgGroup->event,tEventTypeFlagGroup);
	falgGroup->falg = flags;
	
}
static uint32_t tFlagGroupCheckAndConsume(tFlagGroup * flagGroup,uint32_t type,uint32_t * flags)
{
  uint32_t srcFlag = *flags;
	uint32_t isSet = type & TFLAGGROUP_SET;//是否需要置位
	uint32_t isALL = type & TFLAGGROUP_ALL;//是否等待所有
	uint32_t isConsume = type & TFLAGGROUP_CONSUME;//是否消耗
	//srcFlag为指定检查的标志位
	//type为检查标志位是否置位或者是否检查全部
	uint32_t calFlag = isSet ? (flagGroup->falg & srcFlag) : (~flagGroup->falg & srcFlag);
	
	if(((isALL != 0)&&(calFlag == srcFlag)) || ((isALL == 0) && (calFlag != 0)))
							{
							    if(isConsume)
									{
										if(isSet)//标志为1的消耗是0
										{
									 flagGroup->falg &= ~srcFlag;
										}
										else//标志为0的消耗是1
										{
										flagGroup->falg |= srcFlag;
										}
									}
			 *flags = calFlag;
			 return tErrorNoError;	
							}
	     *flags = calFlag;
				return tErrorResourceUnavaliable;

}
/**********************************************************************************************************
** Function name        :   tFlagGroupWait
** Descriptions         :   等待事件标志组中特定的标志
** parameters           :   flagGroup 等待的事件标志组
** parameters           :   waitType 等待的事件类型
** parameters           :   requstFlag 请求的事件标志
** parameters           :   resultFlag 等待标志结果
** parameters           :   waitTicks 当等待的标志没有满足条件时，等待的ticks数，为0时表示永远等待
** Returned value       :   等待结果,tErrorResourceUnavaliable.tErrorNoError,tErrorTimeout
***********************************************************************************************************/
uint32_t tFlagGroupWait (tFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag,
						uint32_t * resultFlag, uint32_t waitTicks)
{
    uint32_t result;
    uint32_t flags = requestFlag;

    uint32_t status = tTaskEnterCritical();
    result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
    if (result != tErrorNoError)
    {
	    // 如果事件标志不满足条件，则插入到等待队列中
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		tEventWait(&flagGroup->event, currentTask, (void *)0,  tEventTypeFlagGroup, waitTicks);

        tTaskExitCritical(status);

		// 再执行一次事件调度，以便于切换到其它任务
		tTaskSched();

        *resultFlag = currentTask->eventFlags;
        result = currentTask->waitEventResult;
	}
    else
    {
        *resultFlag = flags;
        tTaskExitCritical(status);
    }

	return result;
}

/**********************************************************************************************************
** Function name        :   tFlagGroupNoWaitGet
** Descriptions         :   获取事件标志组中特定的标志
** parameters           :   flagGroup 获取的事件标志组
** parameters           :   waitType 获取的事件类型
** parameters           :   requstFlag 请求的事件标志
** parameters           :   resultFlag 等待标志结果
** Returned value       :   获取结果,tErrorResourceUnavaliable.tErrorNoError
***********************************************************************************************************/
uint32_t tFlagGroupNoWaitGet (tFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag)
{
    uint32_t flags = requestFlag;

    uint32_t status = tTaskEnterCritical();
    uint32_t result = tFlagGroupCheckAndConsume(flagGroup, waitType, &flags);
	tTaskExitCritical(status);

	*resultFlag = flags;
	return status;
}

/**********************************************************************************************************
** Function name        :   tFlagGroupNotify
** Descriptions         :   通知事件标志组中的任务有新的标志发生
** parameters           :   flagGroup 事件标志组
** parameters           :   isSet 是否是设置事件标志
** parameters           :   flags 产生的事件标志
***********************************************************************************************************/
void tFlagGroupNotify (tFlagGroup * flagGroup, uint8_t isSet, uint32_t flags)
{
    tList *waitList;
    tNode * node;
    tNode * nextNode;
    uint8_t sched = 0;

    uint32_t status = tTaskEnterCritical();

    if (isSet) {
        flagGroup->falg |= flags;     // 置1事件
    } else {
        flagGroup->falg &= ~flags;    // 清0事件
    }

    // 遍历所有的等待任务, 获取满足条件的任务，加入到待移除列表中
    waitList = &flagGroup->event.waitList;
    for (node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode) {
        uint32_t result;
        tTask *task = tNodeParent(node, tTask, linkNode);
        uint32_t flags = task->eventFlags;
        nextNode = node->nextNode;

        // 检查标志
        result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
        if (result == tErrorNoError) {
            // 唤醒任务
            task->eventFlags = flags;
            tEventWakeUpTask(&flagGroup->event, task, (void *)0, tErrorNoError);
            sched = 1;
        }
    }

    // 如果有任务就绪，则执行一次调度
    if (sched)
    {
        tTaskSched();
    }

    tTaskExitCritical(status);
}
//@消息查询
void tFlagGroupGetInfo(tFlagGroup * flagGroup,tFlagGroupInfo * info)
			{
    uint32_t status = tTaskEnterCritical();
				info->flags = flagGroup->falg;
				info->taskCount = tEventWaitCount(&flagGroup->event);				
		tTaskExitCritical(status);
				
			}
//@事件标志组的删除
uint32_t tFlagGroupDestroy(tFlagGroup * falgGroup)
					{
   uint32_t status = tTaskEnterCritical();
				uint32_t count = tEventRemoveAll(&falgGroup->event,(void*)0,tErrorDel);				
	 tTaskExitCritical(status);				
			
     if(count > 0)	
		 {
		  tTaskSched();
		 }		

		return count;		 
					}