#include "thinyOS.h"
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
void task1DestroyFunc(void * param)
{
 task_flag1 = 0;
}
void task1(void *para)
{
	tSetSysTickPeriod(10);//设置最小时间单位
  tSemInit(&sem1,0,10);
	tSemWait(&sem1,0);//等待车位空出来
	for(;;)
	  { 
			task_flag1 = 0;
//tTaskSchedEnable();
//			//模拟汇编过程（读-改-写入）
//			temp = tickCounter;//读
			  tTaskDelay(1);;//改（（如果不加临界区保护）在改的过程发生中断,最后写入会覆盖汇编操作）
//			temp=3;
//			tickCounter=temp;//写
//			//end
//tTaskSchedDisable();
			task_flag1 = 1;
			tTaskDelay(1);;
			
		}

}
void task2(void *para)
{
	tError error;
	for(;;)
	  {

			task_flag2 = 0;
		  tTaskDelay(1);
			//tTaskWakeUp(&tTask1);
			task_flag2 = 1;
		  tTaskDelay(1);
			//tTaskWakeUp(&tTask1);
//     tSemNotify(&sem1);//释放一个资源
//		 error = tSemNoWaitGet(&sem1);//无等待获取资源
	  }
}
void task3(void *para)
{
	tSemInit(&sem2,0,0);
	for(;;)
	  {
      tSemWait(&sem2,10);
			task_flag3 = 0;
			tTaskDelay(1);
			task_flag3 = 1;
		  tTaskDelay(1);

	  }
}
void task4(void *para)
{
	tSemInfo semInfo;
	int destroyed = 0;
	for(;;)
	  {

			task_flag4 = 0;
			tTaskDelay(1);
			task_flag4 = 1;
		  tTaskDelay(1);
			if(!destroyed)//如果没有被删除
			{
			tSemGetInfo(&sem1,&semInfo);
			tSemDestroy(&sem1);
			destroyed = 1;//设置删除标记
			}
	  }
}
void tInitApp(void)
{
	tTaskInit(&tTask1,task1,(void *)0x11,0,&task1Env[1024]);
	tTaskInit(&tTask2,task2,(void *)0x22,2,&task2Env[1024]);
	tTaskInit(&tTask3,task3,(void *)0x33,2,&task3Env[1024]);
	tTaskInit(&tTask4,task4,(void *)0x44,2,&task4Env[1024]);
}