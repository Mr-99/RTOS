#include "tSem.h"
//@��ʼ���ź���
void tSemInit(tSem *sem,uint32_t startCount,uint32_t maxCount)
{
 tEventInit(&sem->event,tEventTypeSem);
 sem->maxCount = maxCount;
	if(maxCount == 0)//maxCount=0����ζ��Դ������
	{
		sem->count = startCount;
	}
	else
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;//�����ʼֵ�������ֵ�������ֵ�����߾��ǳ�ʼֵ
	}
	
}