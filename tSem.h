#ifndef TSEM_H
#define TSEM_H
#include "tEvent.h"

//信号量
typedef struct _tSem{
tEvent event;
uint32_t count;
uint32_t maxCount;
}tSem;
//信号量初始化
void tSemInit(tSem *sem,uint32_t startCount,uint32_t maxCount);
//@获取资源
uint32_t tSemWait(tSem *sem,uint32_t waitTicks);
//@无等待获取资源
tError tSemNoWaitGet(tSem * sem);
//@释放资源
void tSemNotify(tSem * sem);
#endif