#ifndef TMEMBLOCK_H
#define TMEMBLOCK_H

#include "tEvent.h"
 typedef struct _tMenBlock{
	 
   tEvent eventy;
	 void *memStart;
	 uint32_t blockSize;
	 uint32_t maxCount;
	 tList blockList;
 
 }tMemBlock;
 //消息结构体
 typedef struct _tMemBlockInfo{
				uint32_t count;
				uint32_t maxCount;
				uint32_t blockSize;
				uint32_t taskCount;
 }tMemBlockInfo;
 //END
 void tMemBlockInit(tMemBlock * memBlcok,uint8_t * memStart,uint32_t blockSize,uint32_t blockCnt);
 //@等待获取资源
 uint32_t tMemBlockWait(tMemBlock * memBlock,uint8_t **mem,uint32_t waitTicks);
 //@无等待获取资源
 uint32_t TmemBlockNoWaitGet(tMemBlock * memBlock,void ** mem);
//@释放存储块
 void tMemBlockNotify(tMemBlock * memBlock,uint8_t * mem);
//@获取存储块消息
void tMemBlockGetInfo(tMemBlock * memBlock,tMemBlockInfo * info);
//@删除存储块
 uint32_t tMemBlockDestroy(tMemBlock * memBlock);
#endif