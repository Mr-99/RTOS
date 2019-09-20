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
 //��Ϣ�ṹ��
 typedef struct _tMemBlockInfo{
				uint32_t count;
				uint32_t maxCount;
				uint32_t blockSize;
				uint32_t taskCount;
 }tMemBlockInfo;
 //END
 void tMemBlockInit(tMemBlock * memBlcok,uint8_t * memStart,uint32_t blockSize,uint32_t blockCnt);
 //@�ȴ���ȡ��Դ
 uint32_t tMemBlockWait(tMemBlock * memBlock,uint8_t **mem,uint32_t waitTicks);
 //@�޵ȴ���ȡ��Դ
 uint32_t TmemBlockNoWaitGet(tMemBlock * memBlock,void ** mem);
//@�ͷŴ洢��
 void tMemBlockNotify(tMemBlock * memBlock,uint8_t * mem);
//@��ȡ�洢����Ϣ
void tMemBlockGetInfo(tMemBlock * memBlock,tMemBlockInfo * info);
//@ɾ���洢��
 uint32_t tMemBlockDestroy(tMemBlock * memBlock);
#endif