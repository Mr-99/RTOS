#include "thinyOS.h"
//@使当前任务进入延时状态
void tTaskDelay(uint32_t delay)
								{
		 uint32_t status = tTaskEnterCritical();
		 tTimeTaskWait(currentTask,delay);
		 tTaskSchedUnRdy(currentTask);
		 tTaskSched();
		    // 退出临界区
     tTaskExitCritical(status); 							
								}