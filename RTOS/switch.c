#include "thinyOS.h"
#include "ARMCM4.h"

/*
�Ĵ�����ַ��Ĵ�����ֵ
*/
#define NVIC_INT_CTRL   0xE000Ed04
#define NVIC_PENDSVSET  0x10000000
#define NVIC_SYSPRI2    0xE000ED22
#define NVIC_PENDSV_PRI 0x000000FF
/*
��ĳ����ַ��ֵ
*/
#define MEM32(addr)    *(volatile unsigned long *)(addr)
#define MEM8(addr)     *(volatile unsigned char *)(addr)
	
/*
PendSV_Handler�жϺ��������û��д��
*/
/*����switch.c(27): warning: A1581W: Added 2 bytes of padding at address 0x1e
����취:��NOPָ����޸Ķ��뷽ʽ
MRS  R0��CRSR  ����CPSR�е����ݴ�����R0
*/
//����:���浱ǰ����״̬���ָ���һ������״̬
__asm void PendSV_Handler(void)
{
		IMPORT currentTask
		IMPORT nextTask
	  
	  MRS R0, PSP
	  CBZ R0, PendSVHander_nosave
	  NOP
	  STMDB R0!,{R4-R11}
		LDR R1, =currentTask
		LDR R1, [R1]
		STR R0, [R1]

PendSVHander_nosave
	  LDR R0, =currentTask
	  LDR R1, =nextTask
	  LDR R2, [R1]
	  STR R2, [R0]
	
	  LDR R0, [R2]
	  LDMIA R0!, {R4-R11}
		
		MSR PSP, R0
		ORR LR, LR, #0x04
		BX LR
	  NOP
	
} 
void tTaskRunFirst()
{
	 __set_PSP(0);
	 MEM8(NVIC_SYSPRI2) =  NVIC_PENDSV_PRI;
	 MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;

}
void tTaskSwitch()
{
	 MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
