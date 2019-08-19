#ifndef TTASK_H
#define TTASK_H

#include "tLib.h"
struct _tEvent;
//定义错误码
typedef enum _tError
{
  tErrorNoError = 0,
  tErrorTimeout = 1,
	tErrorResourceUnavaliable=2,
	tErrorDel = 3,
}tError;
//枚举数据类型（是否延时状态）
enum TINYOS_TASK_STATE
   {
 TINYOS_TASK_STATE_RDY = 0,
 TINYOS_TASK_STATE_DESTORYED = (1 << 1),
 TINYOS_TASK_STATE_DELAYED = (1 << 2),
 TINYOS_TASK_STATE_SUSPEND = (1 << 3)
   };
#define TINYOS_TASK_WAIT_MASK (0xFF << 16)
	 
// Cortex-M的堆栈单元类型：堆栈单元的大小为32位，所以使用uint32_t
typedef uint32_t tTaskStack;
	 
//任务控制块
typedef struct _tTask{
	tTaskStack *stack;//指向堆栈的指针
	tNode linkNode;//增加一个同优先级链表结点
	uint32_t delayTicks; //表示任务是否延时，延时多少
	tNode delayNode; //延时结点
	uint32_t prio; //优先级
	enum TINYOS_TASK_STATE state;//任务状态
	uint32_t slice;//时间片
	enum TINYOS_TASK_STATE suspendCount;
	void (*clean)(void *param);
	void * cleanParam;
	uint8_t requestDeleteFlag;
	//事件控制块
	struct _tEvent *waitEvent;
	void *eventMsg;
	uint32_t waitEventResult;
}tTask;

//消息结构体
typedef struct _taskInfo{
			uint32_t delayTicks;
			uint32_t prio;
enum TINYOS_TASK_STATE state;
			uint32_t slice;
			uint32_t suspendCount;	
}tTaskInfo;
#endif