#include "thinyOS.h"
//@ʹ��ǰ���������ʱ״̬
void tTaskDelay(uint32_t delay)
								{
		 uint32_t status = tTaskEnterCritical();
		 tTimeTaskWait(currentTask,delay);
		 tTaskSchedUnRdy(currentTask);
		 tTaskSched();
		    // �˳��ٽ���
     tTaskExitCritical(status); 							
								}