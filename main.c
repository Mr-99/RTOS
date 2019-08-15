#include "thinyOS.h"
//�ṹ��λ�����
uint16_t num=0x1111;
#pragma pack(1)
struct _test{
	uint16_t start_h :6;//��6λ
	uint16_t start_l :10;//��10λ
};
#pragma pack()
struct _test *test=(struct _test*)&num;
//////////////////
// ��ǰ���񣺼�¼��ǰ���ĸ�������������
tTask * currentTask;

tTask *nextTask;


//���ȼ�����
tBitmap taskPrioBitmap;

tList taskTable[TINYOS_PRO_COUNT];//����32������

//������
uint8_t schedLockCount;//����������

//��ʱ����ͷ����
tList tTaskDelayedList;

tNode *node;
uint32_t highestPrio;
//@���ظ����ȼ�������
tTask * tTaskHighestReady(void)
{
 highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);//�õ���ǰ�ĸ����ȼ�
 node = tListFirst(&taskTable[highestPrio]);//�ڵ�ǰ���ȼ�������ȡ���׸����
	return tNodeParent(node,tTask,linkNode);//������������Ӧ������
}
//@��ʼ��������
void tTaskSchedInit(void)
{
	 int i;
  schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap) ;//���ȼ�λͼ��ʼ��
	for(i = 0;i<TINYOS_PRO_COUNT;i++)
	{
	tListInit(&taskTable[i]);
	}
	
	
}
//@��ֹ�������
void tTaskSchedDisable(void)
{ 
	 uint32_t status = tTaskEnterCritical();
	if(schedLockCount < 255 )
	{
	   schedLockCount++;
	}
	 tTaskExitCritical(status);
}
//@�����������
void tTaskSchedEnable(void)
{
	 uint32_t status = tTaskEnterCritical();
if( schedLockCount > 0 )
 {
	  if(--schedLockCount == 0)
		{
		     tTaskSched();
		}
 }
  tTaskExitCritical(status);
 }
//@����������Ϊ����״̬
void tTaskSchedRdy (tTask * task)
{
    tListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
    tBitmapSet(&taskPrioBitmap, task->prio);
}
//@������Ӿ����б����Ƴ�
void tTaskSchedUnRdy (tTask * task)
{
    tListRemove(&taskTable[task->prio], &(task->linkNode));

    // �����п��ܴ��ڶ������ֻ�е�û������ʱ�������λͼ���
    if (tListCount(&taskTable[task->prio]) == 0) 
    {
        tBitmapClear(&taskPrioBitmap, task->prio);
    }
}
//@������Ƚӿڡ�tinyOSͨ������ѡ����һ�����������Ȼ���л������������С�
void tTaskSched(void)
{
		 tTask *tempTask;
 uint32_t status = tTaskEnterCritical();
  if(schedLockCount > 0)//�����ǰ�������������л�����
	  {
	return;
	  }
	tempTask = tTaskHighestReady();
	if(tempTask != currentTask)
				{
			nextTask = tempTask;	

			tTaskSwitch();		
				}
 tTaskExitCritical(status);
}
//@��ʼ��������ʱ����
void tTaskDelayedInit(void)
{
  tListInit(&tTaskDelayedList);
}	
//@�����������ʱ�ж���
void tTimeTaskWait(tTask * task,uint32_t ticks)
{
			task->delayTicks = ticks;
	    tListAddLast(&tTaskDelayedList,&(task->delayNode));
	    task->state |= TINYOS_TASK_STATE_DELAYED;
}
//@����ʱ���������ʱ�����л���
void tTimeTaskWakeUp(tTask * task)
{
  tListRemove(&tTaskDelayedList,&(task->delayNode));
	task->state  = (enum TINYOS_TASK_STATE)(task->state&(~TINYOS_TASK_STATE_DELAYED));
}
//@ϵͳʱ�ӽ��Ĵ���
 void tTaskSystemTickhandler(void)
 {
     tNode * node;
	     // �����ٽ������Ա�������������������л��ڼ䣬������Ϊ�����жϵ���currentTask��nextTask���ܸ���
    uint32_t status = tTaskEnterCritical();
     for(node = tTaskDelayedList.headNode.nextNode;node != &(tTaskDelayedList.headNode);node = node->nextNode)
	    {
			   tTask *task = tNodeParent(node,tTask,delayNode);
				if( --task->delayTicks == 0 )
				      {
					 if(task->waitEvent)//�������ʱ���Ϊ�㣬�����¼���˵���¼�û�б����ѣ�˵����ʱ�ˣ���ʱ���¼���ɾ������
					   {
					    tEventRemoveTask(task,(void *)0,tErrorTimeout);//�����¼���ʱ��־
					   }
				   tTimeTaskWakeUp(task);
				   tTaskSchedRdy(task);
				      }
			}
		 if(0 == --currentTask->slice )
		 {
		     if(tListCount(&taskTable[currentTask->prio]) > 0)
				 {
				   tListRemoveFirst(&taskTable[currentTask->prio]);
					 tListAddLast(&taskTable[currentTask->prio],&(currentTask->linkNode));
					 currentTask->slice = TINYOS_SLICE_MAX;
				 }
		 }
		 //tickCounter++;
    tTaskExitCritical(status); 
		 		      tTaskSched();		      // �˳��ٽ���


 }
//@�����߳�
tTask tidleTask;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];
 void idleTask(void *para)
{
	for(;;)
	  {
	  }
}

//@������
int main()
{
	tTaskSchedInit();
	tTaskDelayedInit();
  tBitmapInit(&taskPrioBitmap);
  tTaskInit(&tidleTask,idleTask,(void *)0x33,31,&idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
	tInitApp();
	nextTask = tTaskHighestReady(); 
	tTaskRunFirst();
  return 0;
 }
//@��С�˲��Դ���
 void big_little_test(void)
 {
 unsigned int   uiA = 0xaabbccdd;
 unsigned int  *puiA = &uiA;
 unsigned char *pucA = (unsigned char *)puiA;
 static uint8_t *str;
 
        if (0xdd == *pucA)
        {
					str = "The cpu is little endian\n";
        }
        else
        {
					str =  "The cpu is big endian\n";
        }
 }