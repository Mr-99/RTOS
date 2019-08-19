#ifndef TSEM_H
#define TSEM_H
#include "tEvent.h"

//�ź���
typedef struct _tSem{
tEvent event;
uint32_t count;
uint32_t maxCount;
}tSem;
//@
typedef struct _tSemInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;
}tSemInfo;
//�ź�����ʼ��
void tSemInit(tSem *sem,uint32_t startCount,uint32_t maxCount);
//@��ȡ��Դ
uint32_t tSemWait(tSem *sem,uint32_t waitTicks);
//@�޵ȴ���ȡ��Դ
tError tSemNoWaitGet(tSem * sem);
//@�ͷ���Դ
void tSemNotify(tSem * sem);
//@��ѯ��Ϣ
void tSemGetInfo(tSem * sem,tSemInfo * info);
//@ɾ���ź���
uint32_t tSemDestroy(tSem * sem);
#endif