#ifndef __DRV_UART1_H__
#define __DRV_UART1_H__

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern VOID DRV_UART1_Init(VOID);
extern VOID DRV_UART1_PollInit(VOID);
extern VOID DRV_UART1_PollTest(VOID);
extern VOID DRV_UART1_InterruptInit(VOID);
extern VOID DRV_UART1_InterruptTest(VOID);
extern VOID DRV_UART1_DmaInit(VOID);
extern VOID DRV_UART1_DmaTest(VOID);

#endif

