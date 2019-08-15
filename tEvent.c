#include "thinyOS.h"
void tEventInit(tEvent * event,tEventType type)
{
event->type = type;
tListInit(&(event->waitList));
}
//@������ȥ�ȴ��¼�����
void tEventWait(tEvent * event,tTask * task,void * msg,uint32_t state,uint32_t timeout)
{
	 uint32_t status = tTaskEnterCritical();
   task->state |= state; //��������״̬
	 task->waitEvent = event;//�ȴ����¼�����
	 task->eventMsg = msg;//�¼�����Ϣ
	 task->waitEventResult =  tErrorNoError;
	 tTaskSchedUnRdy(task);//������Ӿ����������Ƴ�
	 tListAddLast(&event->waitList,&task->linkNode);//���뵽�ȴ�����
	 if(timeout)
	  {
	     tTimeTaskWait(task,timeout);//����timeoutʱ�䳤�ȵĵȴ������û���¼���������,�ȴ���ʱ
	  }
	 tTaskExitCritical(status);
}
//@�¼�����,��������
tTask * tEventWakeUp(tEvent * event,void * msg,uint32_t result)
{
 tNode * node;
 tTask * task  = (tTask*)0;
	 uint32_t status = tTaskEnterCritical();
    if((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
		{
		   task = (tTask*)tNodeParent(node,tTask,linkNode);
			 task->waitEvent = (tEvent*)0;//����û���¼��ȴ�
			 task->eventMsg = msg;
			 task->waitEventResult =  result;
			 task->state &= ~TINYOS_TASK_WAIT_MASK;
			 if(task->delayTicks != 0)
			 {
			  tTimeTaskWakeUp(task);
			 }
			  tTaskSchedRdy(task);//�������񣬼�������б�
		}
	 tTaskExitCritical(status);
	return task;
}
//@��������¼����ƿ����Ƴ�
void tEventRemoveTask(tTask *task,void *msg,uint32_t result)
{
	 uint32_t status = tTaskEnterCritical();
	 tListRemove(&task->waitEvent->waitList,&task->linkNode);//�����������ڵ��¼��ȴ��ж���ɾ��
	 task->waitEvent = (tEvent*)0;//����û���¼��ȴ�
	 task->eventMsg = msg;
	 task->waitEventResult =  result;
	 task->state &= ~TINYOS_TASK_WAIT_MASK;
	 tTaskExitCritical(status);
}
//@ɾ���¼�����������
uint32_t tEventRemoveAll(tEvent *event,void *msg,uint32_t result)
{
	 tNode * node;
	 uint32_t count = 0;
	 uint32_t status = tTaskEnterCritical();
	 count = tListCount(&event->waitList);//������¼��е�����ĸ���
	 //���¼���ѭ��ɾ������
	 while((node = tListRemoveFirst(&event->waitList)) != (tNode*)0)
	  {
		 tTask * task = (tTask*)tNodeParent(node,tTask,linkNode);
		 task->waitEvent = (tEvent *)0;
		 task->eventMsg = msg;
		 task->waitEventResult = result;
		 task->state &= ~TINYOS_TASK_WAIT_MASK;
		 if(task->delayTicks != 0)//��������г�ʱʱ�䣬��ʱʱ�䲻Ϊ��
		    {
		 tTimeTaskWakeUp(task);
		    }
		 tTaskSchedRdy(task);
		}
	 tTaskExitCritical(status);
	 return count;
}
//@�����¼����ж��ٸ�����
uint32_t tEventWaitCount(tEvent * event)
{
 uint32_t count = 0;
	 uint32_t status = tTaskEnterCritical();
	 count = tListCount(&event->waitList);
	 tTaskExitCritical(status);
	return count;
}

