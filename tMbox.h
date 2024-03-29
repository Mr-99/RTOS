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
//信息查询
typedef struct _tMboxInfo{
 uint32_t count;
 uint32_t maxCount;
 uint32_t taskCount;	
}tMboxInfo;
void tMboxInit(tMbox * mbox,void **msgBuffer,uint32_t maxCount);
//@获得消息
uint32_t tMboxWait(tMbox *mbox,void **msg,uint32_t waitTicks);
//@无等待获取消息
uint32_t tMboxNoWaitGet(tMbox * mbox,void **msg);
//@释放资源操作
uint32_t tMboxNotify(tMbox * mbox,void * msg,uint32_t notifyOption);
//
void tMboxFlush(tMbox * mbox);
//@删除邮箱
uint32_t tMboxDestroy(tMbox * mbox);
//@获取消息
void tMboxGetInfo(tMbox * mbox,tMboxInfo * info);
#endif