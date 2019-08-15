#ifndef TINYOS_H
#define TINYOS_H
#include<stdint.h>
typedef uint32_t tTaskStack;//任务堆栈数据类型
typedef struct _tTask{

tTaskStack *stack;//指向堆栈的指针
uint32_t delayTicks; //表示任务是否延时，延时多少
}tTask;


extern tTask *currentTask;
extern tTask *nextTask;
//函数声明
/*
临界区保护
*/
//临界区进入保护（读取状态寄存器）
uint32_t tTaskEnterCritical(void);
//临界区退出保护（写入状态寄存器）
void tTaskExitCritical(uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);
/*
调度锁
*/
//调度器上锁
void tTaskSchedDisable(void);
//调度器解锁
void tTaskSchedEnable(void);
//禁止调度
void tTaskSched(void);
void tTaskSchedInit(void);
#endif