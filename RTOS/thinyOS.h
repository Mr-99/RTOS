#ifndef TINYOS_H
#define TINYOS_H
#include<stdint.h>
typedef uint32_t tTaskStack;//�����ջ��������
typedef struct _tTask{

tTaskStack *stack;//ָ���ջ��ָ��
uint32_t delayTicks; //��ʾ�����Ƿ���ʱ����ʱ����
}tTask;


extern tTask *currentTask;
extern tTask *nextTask;
//��������
/*
�ٽ�������
*/
//�ٽ������뱣������ȡ״̬�Ĵ�����
uint32_t tTaskEnterCritical(void);
//�ٽ����˳�������д��״̬�Ĵ�����
void tTaskExitCritical(uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);
/*
������
*/
//����������
void tTaskSchedDisable(void);
//����������
void tTaskSchedEnable(void);
//��ֹ����
void tTaskSched(void);
void tTaskSchedInit(void);
#endif