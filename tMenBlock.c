#include "tMenBlock.h"
#include "thinyOS.h"
//@块初始化
 void tMemBlockInit(tMemBlock * memBlock,uint8_t * memStart,uint32_t blockSize,uint32_t blockCnt)
 {
    uint8_t * memBlockStart = (uint8_t*)memStart;
	  uint8_t * memBlockEnd = memBlockStart+blockSize*blockCnt;
	 
	  if(blockSize < sizeof(tNode))
		{
		  return;
		}
		
		tEventInit(&memBlock->eventy,tEventTypeMemBlock);
		memBlock->memStart = memStart;
		memBlock->blockSize = blockSize;
		memBlock->maxCount = blockCnt;
		
		tListInit(&memBlock->blockList);
		while(memBlockStart < memBlockEnd)
		{
		    tNodeInit((tNode *)memBlockStart);
			  tListAddLast(&memBlock->blockList,(tNode *)memBlockStart);
			  
			  memBlockStart +=blockSize;
		}
 }
 //@等待获取资源
 uint32_t tMemBlockWait(tMemBlock * memBlock,uint8_t **mem,uint32_t waitTicks)
 {
    uint32_t status = tTaskEnterCritical();
	  if( tListCount(&memBlock->blockList)>0 )
		{
		 *mem = (uint8_t *)tListRemoveFirst(&memBlock->blockList);
			tTaskExitCritical(status); 
			return tErrorNoError;
		}
		else
		{
		 tEventWait(&memBlock->eventy,currentTask,(void *)0,tEventTypeMemBlock,waitTicks);
		 tTaskExitCritical(status); 
		 tTaskSched();
		 *mem = currentTask->eventMsg;
		 return currentTask->waitEventResult;
		}
 }
 //@无等待获取资源
 uint32_t TmemBlockNoWaitGet(tMemBlock * memBlock,void ** mem)
 {
     uint32_t status = tTaskEnterCritical();
	  if( tListCount(&memBlock->blockList)>0 )
		{
		 *mem = (uint8_t *)tListRemoveFirst(&memBlock->blockList);
			tTaskExitCritical(status); 
		
			return tErrorNoError;
		}
		else
		{
	  tTaskExitCritical(status); 
		return tErrorResourceUnavaliable;
		}
 }
//@释放存储块
 void tMemBlockNotify(tMemBlock * memBlock,uint8_t * mem)
 {
      uint32_t status = tTaskEnterCritical();
	    if(tEventWaitCount(&memBlock->eventy)>0)
			{
			tTask * task = tEventWakeUp(&memBlock->eventy,(void *)mem,tErrorNoError); 
					if(task->prio < currentTask->prio)
							{
								tTaskSched();
							}
			}
		 else
			{
				 tListAddLast(&memBlock->blockList,(tNode*)mem);
			}
			tTaskExitCritical(status);
 }
//@获取存储块消息
void tMemBlockGetInfo(tMemBlock * memBlock,tMemBlockInfo * info)
{
      uint32_t status = tTaskEnterCritical();
						info->count = tListCount(&memBlock->blockList);
						info->maxCount = memBlock->maxCount;
						info->blockSize = memBlock->blockSize;
						info->taskCount = tEventWaitCount(&memBlock->eventy);
		  tTaskExitCritical(status);
}
//@删除存储块
 uint32_t tMemBlockDestroy(tMemBlock * memBlock)
{

      uint32_t status = tTaskEnterCritical();
      uint32_t count = tEventRemoveAll(&memBlock->eventy,(void *)0,tErrorDel);
		  tTaskExitCritical(status);
					
					 if(count > 0)
					 {
						tTaskSched();
					 }
					 
	return count;

}