//#ifndef
//#define
//#endif
#ifndef TMBOX_H
#define TMBOX_H
#include "tEvent.h"
#define tMBOXSendNormal 0x00
#define tMBoxSendFront  0x01
typedef struct _tMbox
{
    tEvent event;
	  uint32_t count;
	  uint32_t read;
	  uint32_t write;
	  uint32_t maxCount;
	  void ** msgBuffer;
}tMbox;
//��Ϣ��ѯ
typedef struct _tMboxInfo{
 uint32_t count;
 uint32_t maxCount;
 uint32_t taskCount;	
}tMboxInfo;
void tMboxInit(tMbox * mbox,void **msgBuffer,uint32_t maxCount);
//@�����Ϣ
uint32_t tMboxWait(tMbox *mbox,void **msg,uint32_t waitTicks);
//@�޵ȴ���ȡ��Ϣ
uint32_t tMboxNoWaitGet(tMbox * mbox,void **msg);
//@�ͷ���Դ����
uint32_t tMboxNotify(tMbox * mbox,void * msg,uint32_t notifyOption);
//
void tMboxFlush(tMbox * mbox);
//@ɾ������
uint32_t tMboxDestroy(tMbox * mbox);
//@��ȡ��Ϣ
void tMboxGetInfo(tMbox * mbox,tMboxInfo * info);
#endif