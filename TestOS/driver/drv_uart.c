#include "os_public.h"
#include "drv_uart.h"

void DRV_UART_Uart1Init(void)
{
    GPIO_InitPara GPIO_InitStructure;
    USART_InitPara USART_InitStructure;
    
    /* 配置时钟 */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_GPIOA , ENABLE );
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_USART1 , ENABLE );

    /* 配置GPIO */
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_9 ;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_Init( GPIOA , &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN_FLOATING;;
    GPIO_Init( GPIOA , &GPIO_InitStructure); 

    /* 配置串口参数 */
    USART_DeInit( USART1 );
    USART_InitStructure.USART_BRR           = 115200;
    USART_InitStructure.USART_WL            = USART_WL_8B;
    USART_InitStructure.USART_STBits            = USART_STBITS_1;
    USART_InitStructure.USART_Parity                = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx                = USART_RXORTX_RX | USART_RXORTX_TX;
    USART_Init(USART1, &USART_InitStructure);
    
    /* 使能 */
    USART_Enable(USART1, ENABLE);
    
    return;
}

PUTCHAR_PROTOTYPE
{
    while (USART_GetBitState( USART1 , USART_FLAG_TBE) == RESET)
    {
    }
    USART_DataSend( USART1 , (uint8_t) ch );
    return ch;
}

