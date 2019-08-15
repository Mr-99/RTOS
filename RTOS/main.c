#include"thinyOS.h"
#include "ARMCM4.h"
tTask *currentTask;
tTask *nextTask;
tTask *taskTable[2];
uint32_t tickCounter;
//调度锁
uint8_t schedLockCount;
//声明
void tSetSysTickPeriod(uint32_t ms);
//双任务延时原理与空闲任务
 void tTaskSystemTickhandler(void);
  //任务切换
void tTaskSched(void);

/*延时*/
void tTaskDelay(uint32_t delay);
void delay(int count)
{
    while(--count > 0);
}
/*
寄存器地址与寄存器的值
*/
#define NVIC_INT_CTRL   0xE000Ed04
#define NVIC_PENDSVSET  0x10000000
#define NVIC_SYSPRI2    0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF
/*
给某个地址赋值
*/
#define MEM32(addr)    *(volatile unsigned long *)(addr)
#define MEM8(addr)     *(volatile unsigned char *)(addr)
/*
触发异常
*/
void triggerPendSVC(void)
{
	 MEM8(NVIC_SYSPRI2) =  NVIC_PENDSV_PRI;
	 MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
/*包含与任务相关的核心数据*/
tTask tTask1;
tTask tTask2;
tTask tidleTask;
/*任务堆栈*/
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack idleTaskEnv[512];
//与任务相关变量
int task_flag1;
int task_flag2;
void task1(void *para)
{
	char temp = 0;
	uint32_t status = 0;
	tSetSysTickPeriod(10);
	for(;;)
	  {
			task_flag1 = 1;
tTaskSchedEnable();
			//模拟汇编过程（读-改-写入）
			temp = tickCounter;//读
			tTaskDelay(100);//改（（如果不加临界区保护）在改的过程发生中断,最后写入会覆盖汇编操作）
			temp=3;
			tickCounter=temp;//写
			//end
tTaskSchedDisable();
			task_flag1 = 0;
			tTaskDelay(10);
			
		}

}
void task2(void *para)
{
	for(;;)
	  {
			task_flag2 = 1;
			tTaskDelay(10);
			task_flag2 = 0;
		  tTaskDelay(10);

	  }
}
void idleTask(void *para)
{
	for(;;)
	  {
	  }
}
/*任务初始化,*/
void tTaskInit(tTask *task,void (*entry)(void *),void *param,tTaskStack *stack)
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
		}
//led初始化
void led_init(void)
			{

			}
int main()
{
	tTaskSchedInit();
	tTaskInit(&tTask1,task1,(void *)0x11,&task1Env[1024]);
	tTaskInit(&tTask2,task2,(void *)0x22,&task2Env[1024]);
	tTaskInit(&tidleTask,idleTask,(void *)0x33,&idleTaskEnv[512]);
	
  taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();
	
  return 0;
	
 }
//任务切换
void tSetSysTickPeriod(uint32_t ms)
{
SysTick->LOAD = ms*SystemCoreClock / 1000-1;
NVIC_SetPriority(SysTick_IRQn,(1<<__NVIC_PRIO_BITS)-1);
SysTick->VAL = 0;
SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk	 | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
void tTaskDelay(uint32_t delay)
								{
     currentTask->delayTicks = delay;
		 tTaskSched();
								}
void SysTick_Handler()
{
		tTaskSystemTickhandler();
}
//双任务延时原理与空闲任务
 void tTaskSystemTickhandler()
 {
		int i;
	  for(i = 0;i<2;i++)
	   {
			if(taskTable[i]->delayTicks>0)
						{
				 taskTable[i]->delayTicks--;
						}	
		 }
		 tickCounter++;
    tTaskSched();
 }
 //任务切换
void tTaskSched()
{
	{
  if(schedLockCount > 0)
	  {
	return;
	  }
		if(currentTask == &tidleTask)
				{
				  if(taskTable[0]->delayTicks == 0)
					    {
							    nextTask = taskTable[0];
							}
					else if(taskTable[1]->delayTicks == 0)
					    {
							    nextTask = taskTable[1];
							}
					else
					    {
					        return;
					    }
				}
			else if(currentTask == taskTable[0])	
			{
								if(taskTable[1]->delayTicks == 0)
					          {
							    nextTask = taskTable[1];
							      }	
            else if(currentTask->delayTicks != 0)
									{
									nextTask = &tidleTask;
									}
						else  
									{
									return;
									}			
			}									
		else if(currentTask == taskTable[1])
				{
				    if(taskTable[0]->delayTicks == 0)	
									{
							    nextTask = taskTable[0];
									}	    
						 else if(currentTask->delayTicks != 0)
									{
									nextTask = &tidleTask;
									}
						 else
									{
									return; 
									}
				}
		tTaskSwitch();
}
	}
//临界区进入保护（读取状态寄存器）
uint32_t tTaskEnterCritical(void)
{ 
uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}
//临界区退出保护（写入状态寄存器）
void tTaskExitCritical(uint32_t status)
{
  __set_PRIMASK(status);
} 
//调度锁 上锁禁止任务切换，无论何种情况，例如即使时间片用完
//调度锁初始化
void tTaskSchedInit(void)
{
  schedLockCount = 0;
}
//调度器上锁
void tTaskSchedEnable(void)
{ 
	if(schedLockCount < 255 )
	{
	   schedLockCount++;
	}
}
//调度器解锁
void tTaskSchedDisable(void)
{
if( schedLockCount > 0 )
 {
	  if(--schedLockCount == 0)
		{
		     tTaskSched();
		}
 }
}
