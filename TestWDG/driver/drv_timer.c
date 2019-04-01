#include "os_public.h"
#include "drv_led.h"
#include "drv_timer.h"

static U8 gTimerLedFlag = 0;

VOID TIMER2_IRQHandler(VOID)
{
    if(TIMER_GetIntBitState(TIMER2,TIMER_INT_UPDATE) != RESET)
    {   
        TIMER_ClearIntBitState(TIMER2,TIMER_INT_UPDATE);
        if (gTimerLedFlag != 0)
        {
            DRV_LED_On(DRV_LED1);
            gTimerLedFlag = 0;
            return;
        }
        DRV_LED_Off(DRV_LED1);
        gTimerLedFlag++;
    }
    
}

VOID DRV_TIMER_Timer2Init(VOID)
{
    TIMER_BaseInitPara TIMER_BaseInitParaStructure;
    NVIC_InitPara NVIC_InitStructure;

    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2,ENABLE); 
	
    TIMER_DeInit(TIMER2);
    TIMER_BaseInitParaStructure.TIMER_Prescaler = 10800-1; /* 10KHz */
    TIMER_BaseInitParaStructure.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIMER_BaseInitParaStructure.TIMER_Period = 10000-1; /* 10000*10KHz = 1s */
    TIMER_BaseInitParaStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER2,&TIMER_BaseInitParaStructure);

    TIMER_INTConfig(TIMER2, TIMER_INT_UPDATE, ENABLE);
    NVIC_InitStructure.NVIC_IRQ = TIMER2_IRQn;   
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0; 
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;       
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;        
    NVIC_Init(&NVIC_InitStructure);
    
    TIMER_SinglePulseMode(TIMER2, TIMER_SP_MODE_REPETITIVE);
    TIMER_Enable(TIMER2,ENABLE);
}

VOID DRV_TIMER_Timer2PwmInit(VOID)
{
    GPIO_InitPara GPIO_InitStructure;
    TIMER_BaseInitPara TIMER_BaseInitParaStructure;
    TIMER_OCInitPara   TIMER_OCInitStructure;

    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2,ENABLE); 
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_GPIOA|RCC_APB2PERIPH_AF, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	
    TIMER_DeInit(TIMER2);
    TIMER_BaseInitParaStructure.TIMER_Prescaler = 108-1; /* 1MHz */
    TIMER_BaseInitParaStructure.TIMER_CounterMode = TIMER_COUNTER_UP;
    TIMER_BaseInitParaStructure.TIMER_Period = 10000-1; /* 10000*1MHz = 10ms */
    TIMER_BaseInitParaStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_BaseInit(TIMER2,&TIMER_BaseInitParaStructure);
    
    TIMER_OCInitStructure.TIMER_OCMode      = TIMER_OC_MODE_PWM1;
    TIMER_OCInitStructure.TIMER_OCPolarity  = TIMER_OC_POLARITY_HIGH;
    TIMER_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;
    TIMER_OCInitStructure.TIMER_OCIdleState = TIMER_OC_IDLE_STATE_RESET;
    TIMER_OCInitStructure.TIMER_Pulse = 4999; /* 5000*1MHz=5ms */
    TIMER_OC3_Init(TIMER2, &TIMER_OCInitStructure);
    
    TIMER_Enable(TIMER2,ENABLE);
}
