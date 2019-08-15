#ifndef TEVENT_H
#define TEVENT_H

#include "tLib.h"
#include "tTask.h"
typedef enum _tEventType{
tEventTypeUnknow = 0,
tErrorTimeout = 1,
tEventTypeSem = 2
}tEventType;
typedef struct _tEvent{
  tEventType type;
	tList waitList;
}tEvent;
void tEventInit(tEvent * event,tEventType type);
//������ȥ�ȴ��¼�����
void tEventWait(tEvent * event,tTask * task,void * msg,uint32_t state,uint32_t timeout);
//�¼�����,��������
tTask * tEventWakeUp(tEvent * event,void * msg,uint32_t result);
//��������¼����ƿ����Ƴ�
void tEventRemoveTask(tTask *task,void *msg,uint32_t result);
//@ɾ���¼�����������
uint32_t tEventRemoveAll(tEvent *event,void *msg,uint32_t result);
//@�����¼����ж��ٸ�����
uint32_t tEventWaitCount(tEvent * event);
#endif