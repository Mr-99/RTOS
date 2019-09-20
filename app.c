#include "thinyOS.h"
#include "tMenBlock.h"
#include <string.h>
/*包含与任务相关的核心数据*/
tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;
/*任务堆栈*/
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];
tTaskStack task4Env[1024];
//事件控制块
tEvent eventWaitNormal;

//与任务相关变量
int task_flag1;
int task_flag2;
int task_flag3;
int task_flag4;
//信号量
tSem sem1;//定义第一个信号量
tSem sem2;//定义第二个信号量
//邮箱1消息缓冲区
tMbox mbox1;
void * mbox1MsgBuffer[20];//存放消息缓冲
uint32_t msg1[20];
//存储块操作
typedef uint8_t (*tBlock)[100];
uint8_t mem1[20][100];
tMemBlock memBlock1;

//事件标志组
tFlagGroup flagGroup1;
//互斥信号
tMutex mutex;
void task1DestroyFunc(void * param)
{
 task_flag1 = 0;
}
void task1(void *para)
{
	uint8_t i;
	uint8_t *mem;
	tMutexInfo info;
	tSetSysTickPeriod(10);//设置最小时间单位
	
  tMutexInit(&mutex);//初始化互斥信号
	tMutexWait(&mutex,0);
	tTaskDelay(2);
	
	tMutexGetInfo(&mutex,&info);
	
	tMutexDestroy(&mutex);
	
	for(;;)
	  { 
			tMutexWait(&mutex,0);
			tMutexWait(&mutex,0);
			
			task_flag1 = 0;
			tTaskDelay(1);;//改（（如果不加临界区保护）在改的过程发生中断,最后写入会覆盖汇编操作）
			task_flag1 = 1;
			tTaskDelay(1);
			
			tMutexNotify(&mutex);
			tMutexNotify(&mutex);
			
		}

}
void task2(void *para)
{
	int destroy = 0;
	uint32_t resultFlags;
	tFlagGroupInfo info;

	tMutexWait(&mutex,0);
	
	for(;;)
	  {

			task_flag2 = 0;
		  tTaskDelay(1);
			task_flag2 = 1;
		  tTaskDelay(1);
						
			tMutexNotify(&mutex);
			tMutexNotify(&mutex);
//			 if(!destroy)
//				 {
//					 tMemBlockDestroy(&memBlock1);
//					 destroy = 1;
//				 }
	  }
}
void task3(void *para)
{
	for(;;)
	  {
			task_flag3 = 0;
			tTaskDelay(1);
			task_flag3 = 1;
		  tTaskDelay(1);
	  }
}
void task4(void *para)
{
	for(;;)
	  {
			task_flag4 = 0;
			tTaskDelay(1);
			task_flag4 = 1;
		  tTaskDelay(1);
	  }
}
void tInitApp(void)
{
	tTaskInit(&tTask1,task1,(void *)0x11,0,&task1Env[1024]);
	tTaskInit(&tTask2,task2,(void *)0x22,2,&task2Env[1024]);
	tTaskInit(&tTask3,task3,(void *)0x33,2,&task3Env[1024]);
	tTaskInit(&tTask4,task4,(void *)0x44,2,&task4Env[1024]);
}