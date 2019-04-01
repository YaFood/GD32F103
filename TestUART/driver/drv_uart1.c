#include "os_public.h"
#include "drv_systick.h"
#include "drv_uart1.h"

#define DRV_UART1_BUFLEN 128
static U8 gUart1RxBuf[DRV_UART1_BUFLEN] = {0};
volatile static U8 gUart1RxCount = 0;
volatile static U8 gUart1RxBufFlag = 0;

static VOID UART1_GpioInit(VOID)
{
    GPIO_InitPara GPIO_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA , ENABLE);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_9 ;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
    GPIO_Init( GPIOA , &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN_FLOATING;;
    GPIO_Init( GPIOA , &GPIO_InitStructure); 
}

static VOID UART1_Config(VOID)
{
    USART_InitPara USART_InitStructure;
    
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_USART1 , ENABLE);
    
    USART_DeInit( USART1 );
    USART_InitStructure.USART_BRR = 115200;  /* 波特率 */
    USART_InitStructure.USART_WL = USART_WL_8B; /* 数据位 */
    USART_InitStructure.USART_STBits = USART_STBITS_1; /* 停止位 */
    USART_InitStructure.USART_Parity = USART_PARITY_RESET; /* 校验位 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE; /* 流控 */
    USART_InitStructure.USART_RxorTx = USART_RXORTX_RX | USART_RXORTX_TX; /* 收发使能 */
    USART_Init(USART1, &USART_InitStructure);
}

static VOID UART1_NvicConfiguration(VOID)
{
    NVIC_InitPara NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQ = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
} 

VOID DRV_UART1_Init(VOID)
{
    UART1_GpioInit();
    UART1_Config();
    USART_Enable(USART1, ENABLE);
}

PUTCHAR_PROTOTYPE
{
    /* 等待发送完成 */
    while (USART_GetBitState(USART1, USART_FLAG_TBE) == RESET)
    {
    }
    
    USART_DataSend(USART1 , (U8)ch);
    
    while (USART_GetBitState(USART1, USART_FLAG_TC) == RESET)
    {
    }
    return ch;
}

static VOID UART1_SendChar(IN U8 ch)
{
    /* 等待发送完成 */
    while (USART_GetBitState(USART1, USART_FLAG_TBE) == RESET)
    {
    }

    USART_DataSend(USART1 , (U8)ch);

    while (USART_GetBitState(USART1, USART_FLAG_TC) == RESET)
    {
    }
}

VOID DRV_UART1_PollInit(VOID)
{
    UART1_GpioInit();
    UART1_Config();
    USART_Enable(USART1, ENABLE);
}


VOID DRV_UART1_PollTest(VOID)
{
    U8 ch = 0;
    
    while (1)
    {
        if (USART_GetBitState(USART1, USART_FLAG_RBNE) != RESET)
        {
            ch = (U8)USART_DataReceive(USART1);
            UART1_SendChar(ch);
        }
    }
}

VOID USART1_IRQHandler(VOID)
{
    if (USART_GetIntBitState(USART1, USART_INT_RBNE) != RESET)
    {
        if (gUart1RxCount >= DRV_UART1_BUFLEN)
        {
            memset(gUart1RxBuf, 0, sizeof(gUart1RxBuf));
            gUart1RxCount = 0;
        }
        gUart1RxBuf[gUart1RxCount] = (U8)USART_DataReceive(USART1);
        gUart1RxCount++;
    }

    if (USART_GetIntBitState(USART1, USART_INT_IDLEF) != RESET)
    {
        gUart1RxBufFlag++;
    }
}

VOID DRV_UART1_InterruptTest(VOID)
{
    U8 rxCount = 0;

    while (1)
    {
        if (gUart1RxBufFlag > 0)
        {
            for (rxCount = 0; rxCount < gUart1RxCount; rxCount++)
            {
                UART1_SendChar(gUart1RxBuf[rxCount]);
            }
            memset(gUart1RxBuf, 0, sizeof(gUart1RxBuf));
            gUart1RxCount = 0;
            gUart1RxBufFlag = 0;
        }
    }
}

VOID DRV_UART1_InterruptInit(VOID)
{
    UART1_GpioInit();
    UART1_Config();
    UART1_NvicConfiguration();
    USART_Enable(USART1, ENABLE);
    USART_INT_Set(USART1, USART_INT_RBNE, ENABLE);
    USART_INT_Set(USART1, USART_INT_IDLEF, ENABLE);
}

static VOID UART1_DmaRxConfig(IN U8 *buf, IN U32 len)
{
    DMA_InitPara DMA_InitStructure;
    
    DMA_Enable(DMA1_CHANNEL5, DISABLE);
    
    /* USART1 RX DMA1 Channel (triggered by USART1 Rx event) Config */
    DMA_DeInit(DMA1_CHANNEL5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (U32) &(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (U32)buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALSRC;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_BYTE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL5, &DMA_InitStructure);
    DMA_Enable(DMA1_CHANNEL5, ENABLE);
}

static VOID UART1_DmaSend(IN U8 *buf, IN U32 len)
{
    DMA_InitPara DMA_InitStructure;

    DMA_Enable(DMA1_CHANNEL4, DISABLE);

    /* USART1_Tx_DMA_Channel (triggered by USART1 Tx event) Config */
    DMA_DeInit(DMA1_CHANNEL4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (U32) &(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (U32)buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PERIPHERALDST;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PERIPHERALDATASIZE_BYTE;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MEMORYDATASIZE_BYTE;
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_VERYHIGH;
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
    DMA_Init(DMA1_CHANNEL4, &DMA_InitStructure);
    
    DMA_Enable(DMA1_CHANNEL4, ENABLE);
    while (DMA_GetBitState(DMA1_FLAG_TC4) == RESET)
    {
    }
}

VOID DRV_UART1_DmaTest(VOID)
{
    while (1)
    {
        if (USART_GetBitState(USART1, USART_FLAG_IDLEF) != RESET)
        {
            UART1_DmaSend(gUart1RxBuf, DRV_UART1_BUFLEN);
            memset(gUart1RxBuf, 0, DRV_UART1_BUFLEN);
            UART1_DmaRxConfig(gUart1RxBuf, DRV_UART1_BUFLEN);
            USART_DataReceive(USART1); /* 清除USART_FLAG_IDLEF */
        }
    }
}

VOID DRV_UART1_DmaInit(VOID)
{
    UART1_GpioInit();
    UART1_Config();
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_DMA1, ENABLE);
    UART1_DmaRxConfig(gUart1RxBuf, DRV_UART1_BUFLEN);
    USART_Enable(USART1, ENABLE);
    USART_DMA_Enable(USART1, (USART_DMAREQ_TX | USART_DMAREQ_RX), ENABLE);
}
