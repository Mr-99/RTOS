#ifndef TINYOS_H
#define TINYOS_H
// ��׼ͷ�ļ�����������˳��õ����Ͷ��壬��uint32_t
#include<stdint.h>
// tinyOS���ں˿��ļ�
#include "tLib.h"
// TinyOS�������ļ�
#include "tConfig.h"
//�¼����ƿ�ͷ�ļ�
#include "tEvent.h"
#include "tTask.h"
#include "tSem.h"
//����ͷ�ļ�
#include "tMbox.h"

// ��ǰ���񣺼�¼��ǰ���ĸ�������������
extern tTask *currentTask;
// ��һ���������е������ڽ��������л�ǰ�������úø�ֵ��Ȼ�������л������л���ж�ȡ��һ������Ϣ
extern tTask *nextTask;
//��ʱ����ͷ����
extern tList tTaskDelayedList;
//����״̬��Ϣ�ṹ��
//@��ѯ����״̬����
void tTaskGetInfo(tTask * task,tTaskInfo * info);
/**********************************************************************************************************
** Function name        :   tTaskEnterCritical
** Descriptions         :   �����ٽ���
** parameters           :   ��
** Returned value       :   ����֮ǰ���ٽ���״ֵ̬
***********************************************************************************************************/
uint32_t tTaskEnterCritical (void);

/**********************************************************************************************************
** Function name        :   tTaskExitCritical
** Descriptions         :   �˳��ٽ���,�ָ�֮ǰ���ٽ���״̬
** parameters           :   status �����ٽ���֮ǰ��CPU
** Returned value       :   �����ٽ���֮ǰ���ٽ���״ֵ̬
***********************************************************************************************************/
void tTaskExitCritical (uint32_t status);

/**********************************************************************************************************
** Function name        :   tTaskRunFirst
** Descriptions         :   ������tinyOSʱ�����øú��������л�����һ����������
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskRunFirst (void); 

/**********************************************************************************************************
** Function name        :   tTaskSwitch
** Descriptions         :   ����һ�������л���tinyOS��Ԥ�����ú�currentTask��nextTask, Ȼ����øú������л���
**                          nextTask����
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSwitch (void);

/**********************************************************************************************************
** Function name        :   tTaskHighestReady
** Descriptions         :   ��ȡ��ǰ������ȼ��ҿ����е�����
** parameters           :   ��
** Returned value       :   ���ȼ���ߵ��ҿ����е�����
***********************************************************************************************************/
tTask * tTaskHighestReady (void);

/**********************************************************************************************************
** Function name        :   ��ʼ��������
** Descriptions         :   ��
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSchedInit (void);

/**********************************************************************************************************
** Function name        :   tTaskSchedDisable
** Descriptions         :   ��ֹ�������
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSchedDisable (void);

/**********************************************************************************************************
** Function name        :   tTaskSchedEnable
** Descriptions         :   �����������
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSchedEnable (void);

/**********************************************************************************************************
** Function name        :   tTaskSchedRdy
** Descriptions         :   ����������Ϊ����״̬
** input parameters     :   task    �ȴ�����Ϊ����״̬������
** output parameters    :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSchedRdy (tTask * task);

/************************************************************************************************************ Function name        :   tSchedulerUnRdyTask
** Descriptions         :   tTaskSchedUnRdy
** Descriptions         :   ������Ӿ����б����Ƴ�
** input parameters     :   task    ��a��?3y��?��????��
** output parameters    :   None
** Returned value       :   None
***********************************************************************************************************/
void tTaskSchedUnRdy (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskSched
** Descriptions         :   ������Ƚӿڡ�tinyOSͨ������ѡ����һ�����������Ȼ���л������������С�
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSched (void);

/**********************************************************************************************************
** Function name        :   tTimeTaskWait
** Descriptions         :   �����������ʱ������
** input parameters     :   task    ��Ҫ��ʱ������
**                          ticks   ��ʱ��ticks
** output parameters    :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTimeTaskWait (tTask * task, uint32_t ticks);

/**********************************************************************************************************
** Function name        :   tTimeTaskWakeUp
** Descriptions         :   ����ʱ���������ʱ�����л���
** input parameters     :   task  ��Ҫ���ѵ�����
** output parameters    :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTimeTaskWakeUp (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskDelay
** Descriptions         :   ʹ��ǰ���������ʱ״̬��
** parameters           :   delay ��ʱ���ٸ�ticks
** Returned value       :   ��
***********************************************************************************************************/
void tTaskDelay (uint32_t delay);

/**********************************************************************************************************
** Function name        :   tTaskSystemTickHandler
** Descriptions         :   ϵͳʱ�ӽ��Ĵ���
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSystemTickHandler (void);

/**********************************************************************************************************
** Function name        :   tTaskInit
** Descriptions         :   ��ʼ������ṹ
** parameters           :   task        Ҫ��ʼ��������ṹ
** parameters           :   entry       �������ں���
** parameters           :   param       ���ݸ���������в���
** Returned value       :   ��
***********************************************************************************************************/
void tTaskInit (tTask * task, void (*entry)(void *), void *param, uint32_t prio, uint32_t * stack);

/**********************************************************************************************************
** Function name        :   tTaskSuspend
** Descriptions         :   ����ָ��������
** parameters           :   task        �����������
** Returned value       :   ��
***********************************************************************************************************/
void tTaskSuspend (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskWakeUp
** Descriptions         :   ���ѱ����������
** parameters           :   task        �����ѵ�����
** Returned value       :   ��
***********************************************************************************************************/
void tTaskWakeUp (tTask * task);

/**********************************************************************************************************
** Function name        :   tInitApp
** Descriptions         :   ��ʼ��Ӧ�ýӿ�
** parameters           :   ��
** Returned value       :   ��
***********************************************************************************************************/
void tInitApp (void);

/*********************************************************************************************************
** ϵͳʱ�ӽ��Ķ�ʱ��System Tick����
** ������Ŀǰ�Ļ�����ģ�������У�ϵͳʱ�ӽ���Ϊ12MHz
** ����ذ��ձ��̳��Ƽ����ã�����systemTick��ֵ�ͻ��б仯����Ҫ�鿴�����ֲ���˽�
**********************************************************************************************************/
void tSetSysTickPeriod(uint32_t ms);
//@��ʼ��������ʱ����
void tTaskDelayedInit(void);
//@ϵͳʱ�ӽ��Ĵ���
 void tTaskSystemTickhandler(void);
 //@����ָ��������
void tTaskSuspend (tTask *task);
//@���ѱ����������
void tTaskWakeUp(tTask * task);
//@������Ӿ����б���ɾ��
void tTaskSchedRemove(tTask *task);
//@����ʱ�ж���ɾ��
void tTimeTaskRemove(tTask *task);
//@�����������
void tTaskSetCleanCallFunc(tTask *task,void (*clean)(void *param),void *param);
//@ǿ��ɾ��
void tTaskForceDelete(tTask *task);
//@����ɾ��
void tTaskRequestDelete(tTask *task);
uint8_t tTaskIsRequesteDeleted(void);
//@ɾ���Լ�
void tTaskDeleteSelf(void);
#endif