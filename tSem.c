#include "tSem.h"
//@初始化信号量
void tSemInit(tSem *sem,uint32_t startCount,uint32_t maxCount)
{
 tEventInit(&sem->event,tEventTypeSem);
 sem->maxCount = maxCount;
	if(maxCount == 0)//maxCount=0，意味资源无限制
	{
		sem->count = startCount;
	}
	else
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;//如果初始值大于最大值，给最大值。否者就是初始值
	}
	
}