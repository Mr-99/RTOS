#ifndef TMUTEX_H
#define TMUTEX_H
#include "tEvent.h"
typedef struct _tMutex{
			tEvent event;
			uint32_t lockedCount;
			tTask *owner;
			uint32_t ownerOriginalPrio;
}tMutex;
//��ѯ�ṹ��Ϣ�ṹ��
typedef struct _tMutexInfo{
  uint32_t taskCount;
	uint32_t ownerPrio;
	uint32_t inheritedPrio;
	tTask * owner;
	uint32_t lockedCount;
}tMutexInfo;
//��������ʼ��
void tMutexInit(tMutex * mutex);
//�����Դ
uint32_t tMutexWait(tMutex * mutex,uint32_t waitTicks);
//�޵ȴ������Դ
uint32_t tMutexNoWaitGet(tMutex *mutex);
//�ͷ���Դ
uint32_t tMutexNotify(tMutex * mutex);
//@ɾ���ӿ�
uint32_t tMutexDestroy(tMutex * mutex);
//@״̬��ѯ�ӿ�
void tMutexGetInfo(tMutex * mutex,tMutexInfo * info);
#endif