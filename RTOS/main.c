#include"thinyOS.h"
#include "ARMCM4.h"
tTask *currentTask;
tTask *nextTask;
tTask *taskTable[2];
uint32_t tickCounter;
//������
uint8_t schedLockCount;
//����
void tSetSysTickPeriod(uint32_t ms);
//˫������ʱԭ�����������
 void tTaskSystemTickhandler(void);
  //�����л�
void tTaskSched(void);

/*��ʱ*/
void tTaskDelay(uint32_t delay);
void delay(int count)
{
    while(--count > 0);
}
/*
�Ĵ�����ַ��Ĵ�����ֵ
*/
#define NVIC_INT_CTRL   0xE000Ed04
#define NVIC_PENDSVSET  0x10000000
#define NVIC_SYSPRI2    0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF
/*
��ĳ����ַ��ֵ
*/
#define MEM32(addr)    *(volatile unsigned long *)(addr)
#define MEM8(addr)     *(volatile unsigned char *)(addr)
/*
�����쳣
*/
void triggerPendSVC(void)
{
	 MEM8(NVIC_SYSPRI2) =  NVIC_PENDSV_PRI;
	 MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
/*������������صĺ�������*/
tTask tTask1;
tTask tTask2;
tTask tidleTask;
/*�����ջ*/
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack idleTaskEnv[512];
//��������ر���
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
			//ģ������̣���-��-д�룩
			temp = tickCounter;//��
			tTaskDelay(100);//�ģ�����������ٽ����������ڸĵĹ��̷����ж�,���д��Ḳ�ǻ�������
			temp=3;
			tickCounter=temp;//д
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
/*�����ʼ��,*/
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
//led��ʼ��
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
//�����л�
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
//˫������ʱԭ�����������
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
 //�����л�
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
//�ٽ������뱣������ȡ״̬�Ĵ�����
uint32_t tTaskEnterCritical(void)
{ 
uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}
//�ٽ����˳�������д��״̬�Ĵ�����
void tTaskExitCritical(uint32_t status)
{
  __set_PRIMASK(status);
} 
//������ ������ֹ�����л������ۺ�����������缴ʹʱ��Ƭ����
//��������ʼ��
void tTaskSchedInit(void)
{
  schedLockCount = 0;
}
//����������
void tTaskSchedEnable(void)
{ 
	if(schedLockCount < 255 )
	{
	   schedLockCount++;
	}
}
//����������
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
