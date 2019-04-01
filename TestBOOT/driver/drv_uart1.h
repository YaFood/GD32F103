#ifndef __DRV_UART1_H__
#define __DRV_UART1_H__

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
    #define GETCHAR_PROTOTYPE int __io_getchar(FILE *f)
#else
    #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */


extern VOID DRV_UART1_Init(VOID);
extern S32 DRV_UART1_GetChar(IN U32 timeout, OUT U8 *ch);
extern VOID DRV_UART1_PutChar(IN U8 ch);
extern VOID DRV_UART1_SendBuf(IN U8 *buf, IN U32 len);

#endif

