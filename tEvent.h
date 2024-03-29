#ifndef TEVENT_H
#define TEVENT_H
#include "tLib.h"
#include "tTask.h"
typedef enum _tEventType{
 tEventTypeUnknow = 0,
 tEventTypeSem = 1,
	tEventTypeMbox = 2,
	tEventTypeMemBlock,
	tEventTypeFlagGroup,
	tEventTypeMutex,
}tEventType;
//定义错误码
typedef enum _tError
{
  tErrorNoError = 0,
  tErrorTimeout = 1,
	tErrorResourceUnavaliable=2,
	tErrorDel = 3,
	tErrorResourceFull,
	tErrorOwner,
}tError;
typedef struct _tEvent{
  tEventType type;
	tList waitList;
}tEvent;
void tEventInit(tEvent * event,tEventType type);
//@让任务去等待事件到来
void tEventWait(tEvent * event,tTask * task,void * msg,uint32_t state,uint32_t timeout);
//@事件到来,唤醒任务
tTask * tEventWakeUp(tEvent * event,void * msg,uint32_t result);
//@将任务从事件控制块中移除
void tEventRemoveTask(tTask *task,void *msg,uint32_t result);
//@删除事件中所有任务
uint32_t tEventRemoveAll(tEvent *event,void *msg,uint32_t result);
//@计算事件中有多少个任务
uint32_t tEventWaitCount(tEvent * event);
//@从事件控制块中唤醒指定任务
void tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result);
#endif