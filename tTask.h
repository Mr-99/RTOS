#ifndef TTASK_H
#define TTASK_H

#include "tLib.h"
struct _tEvent;
//���������
typedef enum _tError
{
  tErrorNoError = 0,
  tErrorTimeout = 1,
	tErrorResourceUnavaliable=2,
	tErrorDel = 3,
}tError;
//ö���������ͣ��Ƿ���ʱ״̬��
enum TINYOS_TASK_STATE
   {
 TINYOS_TASK_STATE_RDY = 0,
 TINYOS_TASK_STATE_DESTORYED = (1 << 1),
 TINYOS_TASK_STATE_DELAYED = (1 << 2),
 TINYOS_TASK_STATE_SUSPEND = (1 << 3)
   };
#define TINYOS_TASK_WAIT_MASK (0xFF << 16)
	 
// Cortex-M�Ķ�ջ��Ԫ���ͣ���ջ��Ԫ�Ĵ�СΪ32λ������ʹ��uint32_t
typedef uint32_t tTaskStack;
	 
//������ƿ�
typedef struct _tTask{
	tTaskStack *stack;//ָ���ջ��ָ��
	tNode linkNode;//����һ��ͬ���ȼ�������
	uint32_t delayTicks; //��ʾ�����Ƿ���ʱ����ʱ����
	tNode delayNode; //��ʱ���
	uint32_t prio; //���ȼ�
	enum TINYOS_TASK_STATE state;//����״̬
	uint32_t slice;//ʱ��Ƭ
	enum TINYOS_TASK_STATE suspendCount;
	void (*clean)(void *param);
	void * cleanParam;
	uint8_t requestDeleteFlag;
	//�¼����ƿ�
	struct _tEvent *waitEvent;
	void *eventMsg;
	uint32_t waitEventResult;
}tTask;

//��Ϣ�ṹ��
typedef struct _taskInfo{
			uint32_t delayTicks;
			uint32_t prio;
enum TINYOS_TASK_STATE state;
			uint32_t slice;
			uint32_t suspendCount;	
}tTaskInfo;
#endif