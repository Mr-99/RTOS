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
#endif