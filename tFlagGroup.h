#ifndef TFLAGGROUP_H
#define TFLAGGROUP_H

#include "tEvent.h"
//消息查询
typedef struct _tFlagGroupInfo{

		uint32_t flags;
		uint32_t taskCount;
	
}tFlagGroupInfo;
//END
typedef struct _tFlagGroup
{
			tEvent event;
			uint32_t falg;
}tFlagGroup;

#define TFLAGGROUP_CLEAR (0x0 << 0)
#define TFLAGGROUP_SET (0x1 << 0)
#define TFLAGGROUP_ANY (0x0 << 1)
#define TFLAGGROUP_ALL (0x1 << 1)

#define TFLAGGROUP_SET_ALL (TFLAGGROUP_SET|TFLAGGROUP_ALL)
#define TFLAGGROUP_SET_ANY (TFLAGGROUP_SET|TFLAGGROUP_ANY)
#define TFLAGGROUP_CLEAR_ALL (TFLAGGROUP_CLEAR|TFLAGGROUP_ALL)
#define TFLAGGROUP_CLEAR_ANY (TFLAGGROUP_CLEAR|TFLAGGROUP_ANY)

#define TFLAGGROUP_CONSUME (1 << 7)

//@初始化
void tFlagGroupInit(tFlagGroup * falgGroup,uint32_t falgs);

uint32_t tFlagGroupWait(tFlagGroup * flagGroup,uint32_t waitType,uint32_t requestFlag,uint32_t *requestFla,uint32_t waitTicks);
uint32_t tFlagGroupNoWaitGet(tFlagGroup * flagGroup,uint32_t waitType,uint32_t requestFlag,uint32_t *requestFla);
void tFlagGroupNotify(tFlagGroup * flagGroup,uint8_t isSet,uint32_t flag);
//@消息查询
void tFlagGroupGetInfo(tFlagGroup * flagGroup,tFlagGroupInfo * info);
//@事件标志组的删除
uint32_t tFlagGroupDestroy(tFlagGroup * falgGroup);

#endif