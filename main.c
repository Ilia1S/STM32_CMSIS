#include "main.h"

volatile uint32_t DelayCounterMs = 0;
volatile uint32_t SysTimerMs = 0;

void CMSIS_RCC_SystemClock_84Mhz(void);
void CMSIS_SysTick_Timer_init(void);
void CMSIS_PA5_OUTPUT_Push_Pull_init(void);
void Delay_Ms(uint32_t );

int main(void)
{
	CMSIS_RCC_SystemClock_84Mhz();
	CMSIS_SysTick_Timer_init();
	CMSIS_PA5_OUTPUT_Push_Pull_init();
	while (1)
	{
		WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5);
		Delay_Ms(1000);
		GPIOA->BSRR = GPIO_BSRR_BR5;
		Delay_Ms(1000);
	}
}

void CMSIS_RCC_SystemClock_84Mhz(void)
{
	SET_BIT(RCC->CR, RCC_CR_HSION); // internal RC oscillator ON
	while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0); // waiting until internal RC is ready
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); // PLL selected as system clock
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL); // PLL used as system clock
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); // AHB prescaler = 1
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS); // 2 wait states to the flash memory
	SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN); // prefetch enable
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2); // APB1 prescaler = 2
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1); // APB2 prescaler = 1
	CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC); // set HSI as PLL clock entry
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_4); // division factor for the main PLL input clock
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, 0x150 << RCC_PLLCFGR_PLLN_Pos); // main PLL multiplication factor for VCO
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLP, RCC_PLLCFGR_PLLP_0); // main PLL division factor for main system clock
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ, RCC_PLLCFGR_PLLQ_2); // main PLL division factor for USB OTG FS and SDIO clocks

	SET_BIT(RCC->CR, RCC_CR_PLLON); // start PLL
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0); // waiting until PLL is ready
}
void CMSIS_SysTick_Timer_init(void)
{
	CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); // counter disabled
	SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); // clock source - AHB (without the prescaler)
	SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); // counting down to zero to asserts the SysTick exception request
	MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 71999 << SysTick_LOAD_RELOAD_Pos); // 1 ms
	MODIFY_REG(SysTick->LOAD, SysTick_VAL_CURRENT_Msk, 71999 << SysTick_VAL_CURRENT_Pos); // counting from 71999 to 0
	SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); // counter enabled
}
void CMSIS_PA5_OUTPUT_Push_Pull_init(void)
{
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); //enable GPIOA clocking
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5, 1 << GPIO_MODER_MODER5_Pos); //set PA5 as an output
	CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT5); //output push-pull
	MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, 2 << GPIO_OSPEEDR_OSPEED5_Pos); //fast speed
	MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD5, 0 << GPIO_PUPDR_PUPD5_Pos); //no pull-up, no pull-down
	WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5); //high output level
}
void Delay_Ms(uint32_t Milliseconds)
{
	DelayCounterMs = Milliseconds;
	while (DelayCounterMs != 0);
}
void SysTick_Handler(void) {

	SysTimerMs++;
	if (DelayCounterMs) {
		DelayCounterMs--;
	}
}
