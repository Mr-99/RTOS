#ifndef TMUTEX_H
#define TMUTEX_H
#include "tEvent.h"
typedef struct _tMutex{
			tEvent event;
			uint32_t lockedCount;
			tTask *owner;
			uint32_t ownerOriginalPrio;
}tMutex;
//查询结构消息结构体
typedef struct _tMutexInfo{
  uint32_t taskCount;
	uint32_t ownerPrio;
	uint32_t inheritedPrio;
	tTask * owner;
	uint32_t lockedCount;
}tMutexInfo;
//互斥量初始化
void tMutexInit(tMutex * mutex);
//获得资源
uint32_t tMutexWait(tMutex * mutex,uint32_t waitTicks);
//无等待获得资源
uint32_t tMutexNoWaitGet(tMutex *mutex);
//释放资源
uint32_t tMutexNotify(tMutex * mutex);
//@删除接口
uint32_t tMutexDestroy(tMutex * mutex);
//@状态查询接口
void tMutexGetInfo(tMutex * mutex,tMutexInfo * info);
#endif