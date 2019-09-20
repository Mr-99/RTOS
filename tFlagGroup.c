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
	uint32_t isSet = type & TFLAGGROUP_SET;//�Ƿ���Ҫ��λ
	uint32_t isALL = type & TFLAGGROUP_ALL;//�Ƿ�ȴ�����
	uint32_t isConsume = type & TFLAGGROUP_CONSUME;//�Ƿ�����
	//srcFlagΪָ�����ı�־λ
	//typeΪ����־λ�Ƿ���λ�����Ƿ���ȫ��
	uint32_t calFlag = isSet ? (flagGroup->falg & srcFlag) : (~flagGroup->falg & srcFlag);
	
	if(((isALL != 0)&&(calFlag == srcFlag)) || ((isALL == 0) && (calFlag != 0)))
							{
							    if(isConsume)
									{
										if(isSet)//��־Ϊ1��������0
										{
									 flagGroup->falg &= ~srcFlag;
										}
										else//��־Ϊ0��������1
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
** Descriptions         :   �ȴ��¼���־�����ض��ı�־
** parameters           :   flagGroup �ȴ����¼���־��
** parameters           :   waitType �ȴ����¼�����
** parameters           :   requstFlag ������¼���־
** parameters           :   resultFlag �ȴ���־���
** parameters           :   waitTicks ���ȴ��ı�־û����������ʱ���ȴ���ticks����Ϊ0ʱ��ʾ��Զ�ȴ�
** Returned value       :   �ȴ����,tErrorResourceUnavaliable.tErrorNoError,tErrorTimeout
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
	    // ����¼���־����������������뵽�ȴ�������
		currentTask->waitFlagsType = waitType;
		currentTask->eventFlags = requestFlag;
		tEventWait(&flagGroup->event, currentTask, (void *)0,  tEventTypeFlagGroup, waitTicks);

        tTaskExitCritical(status);

		// ��ִ��һ���¼����ȣ��Ա����л�����������
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
** Descriptions         :   ��ȡ�¼���־�����ض��ı�־
** parameters           :   flagGroup ��ȡ���¼���־��
** parameters           :   waitType ��ȡ���¼�����
** parameters           :   requstFlag ������¼���־
** parameters           :   resultFlag �ȴ���־���
** Returned value       :   ��ȡ���,tErrorResourceUnavaliable.tErrorNoError
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
** Descriptions         :   ֪ͨ�¼���־���е��������µı�־����
** parameters           :   flagGroup �¼���־��
** parameters           :   isSet �Ƿ��������¼���־
** parameters           :   flags �������¼���־
***********************************************************************************************************/
void tFlagGroupNotify (tFlagGroup * flagGroup, uint8_t isSet, uint32_t flags)
{
    tList *waitList;
    tNode * node;
    tNode * nextNode;
    uint8_t sched = 0;

    uint32_t status = tTaskEnterCritical();

    if (isSet) {
        flagGroup->falg |= flags;     // ��1�¼�
    } else {
        flagGroup->falg &= ~flags;    // ��0�¼�
    }

    // �������еĵȴ�����, ��ȡ�������������񣬼��뵽���Ƴ��б���
    waitList = &flagGroup->event.waitList;
    for (node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode) {
        uint32_t result;
        tTask *task = tNodeParent(node, tTask, linkNode);
        uint32_t flags = task->eventFlags;
        nextNode = node->nextNode;

        // ����־
        result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
        if (result == tErrorNoError) {
            // ��������
            task->eventFlags = flags;
            tEventWakeUpTask(&flagGroup->event, task, (void *)0, tErrorNoError);
            sched = 1;
        }
    }

    // ����������������ִ��һ�ε���
    if (sched)
    {
        tTaskSched();
    }

    tTaskExitCritical(status);
}
//@��Ϣ��ѯ
void tFlagGroupGetInfo(tFlagGroup * flagGroup,tFlagGroupInfo * info)
			{
    uint32_t status = tTaskEnterCritical();
				info->flags = flagGroup->falg;
				info->taskCount = tEventWaitCount(&flagGroup->event);				
		tTaskExitCritical(status);
				
			}
//@�¼���־���ɾ��
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