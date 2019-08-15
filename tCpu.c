#include"thinyOS.h"
#include "ARMCM4.h"
//@统时钟节拍定时器System Tick配置
void tSetSysTickPeriod(uint32_t ms)
{
SysTick->LOAD = ms*SystemCoreClock / 1000-1;
NVIC_SetPriority(SysTick_IRQn,(1<<__NVIC_PRIO_BITS)-1);
SysTick->VAL = 0;
SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk	 | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
//@SystemTick的中断处理函数
void SysTick_Handler()
{
		tTaskSystemTickhandler();
}