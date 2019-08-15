#include "thinyOS.h"
//结构体位域操作
uint16_t num=0x1111;
#pragma pack(1)
struct _test{
	uint16_t start_h :6;//低6位
	uint16_t start_l :10;//高10位
};
#pragma pack()
struct _test *test=(struct _test*)&num;
//////////////////
// 当前任务：记录当前是哪个任务正在运行
tTask * currentTask;

tTask *nextTask;


//优先级部分
tBitmap taskPrioBitmap;

tList taskTable[TINYOS_PRO_COUNT];//定义32个链表

//调度锁
uint8_t schedLockCount;//调度锁计数

//延时队列头链表
tList tTaskDelayedList;

tNode *node;
uint32_t highestPrio;
//@返回高优先级的任务
tTask * tTaskHighestReady(void)
{
 highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);//得到当前的高优先级
 node = tListFirst(&taskTable[highestPrio]);//在当前优先级链表下取出首个结点
	return tNodeParent(node,tTask,linkNode);//计算出结点所对应的任务
}
//@初始化调度器
void tTaskSchedInit(void)
{
	 int i;
  schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap) ;//优先级位图初始化
	for(i = 0;i<TINYOS_PRO_COUNT;i++)
	{
	tListInit(&taskTable[i]);
	}
	
	
}
//@禁止任务调度
void tTaskSchedDisable(void)
{ 
	 uint32_t status = tTaskEnterCritical();
	if(schedLockCount < 255 )
	{
	   schedLockCount++;
	}
	 tTaskExitCritical(status);
}
//@允许任务调度
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
//@将任务设置为就绪状态
void tTaskSchedRdy (tTask * task)
{
    tListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
    tBitmapSet(&taskPrioBitmap, task->prio);
}
//@将任务从就绪列表中移除
void tTaskSchedUnRdy (tTask * task)
{
    tListRemove(&taskTable[task->prio], &(task->linkNode));

    // 队列中可能存在多个任务。只有当没有任务时，才清除位图标记
    if (tListCount(&taskTable[task->prio]) == 0) 
    {
        tBitmapClear(&taskPrioBitmap, task->prio);
    }
}
//@任务调度接口。tinyOS通过它来选择下一个具体的任务，然后切换至该任务运行。
void tTaskSched(void)
{
		 tTask *tempTask;
 uint32_t status = tTaskEnterCritical();
  if(schedLockCount > 0)//如果当前任务上锁，不切换任务
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
//@初始化任务延时机制
void tTaskDelayedInit(void)
{
  tListInit(&tTaskDelayedList);
}	
//@将任务加入延时列队中
void tTimeTaskWait(tTask * task,uint32_t ticks)
{
			task->delayTicks = ticks;
	    tListAddLast(&tTaskDelayedList,&(task->delayNode));
	    task->state |= TINYOS_TASK_STATE_DELAYED;
}
//@将延时的任务从延时队列中唤醒
void tTimeTaskWakeUp(tTask * task)
{
  tListRemove(&tTaskDelayedList,&(task->delayNode));
	task->state  = (enum TINYOS_TASK_STATE)(task->state&(~TINYOS_TASK_STATE_DELAYED));
}
//@系统时钟节拍处理
 void tTaskSystemTickhandler(void)
 {
     tNode * node;
	     // 进入临界区，以保护在整个任务调度与切换期间，不会因为发生中断导致currentTask和nextTask可能更改
    uint32_t status = tTaskEnterCritical();
     for(node = tTaskDelayedList.headNode.nextNode;node != &(tTaskDelayedList.headNode);node = node->nextNode)
	    {
			   tTask *task = tNodeParent(node,tTask,delayNode);
				if( --task->delayTicks == 0 )
				      {
					 if(task->waitEvent)//如果任务时间减为零，还有事件，说明事件没有被唤醒，说明超时了，超时从事件中删除任务
					   {
					    tEventRemoveTask(task,(void *)0,tErrorTimeout);//传入事件超时标志
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
		 		      tTaskSched();		      // 退出临界区


 }
//@空闲线程
tTask tidleTask;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];
 void idleTask(void *para)
{
	for(;;)
	  {
	  }
}

//@主函数
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
//@大小端测试代码
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