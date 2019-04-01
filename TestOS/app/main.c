#include "app.h"

U32 gTask2EventBitMap = 0;

void task1(void)
{
    U16 taskCount = 0;
    while (1)
    {
        OS_TASK_TaskDelay(1000);
        LED_Light(LED2, LED_OFF);
        LED_Light(LED3, LED_ON);
        taskCount++;
        
        //OS_TASK_WaitForEvent();
        printf("task1 is runing 1...,%u\n", taskCount);
        OS_TASK_TaskDelay(500);
        printf("task1 is runing 2...,%u\n", taskCount);
        gTask2EventBitMap = 1;
        
    }
}

void task2(void)
{
    U16 taskCount = 0;
    while (1)
    {
        OS_TASK_WaitForEvent();
        gTask2EventBitMap = 0;
        //OS_TASK_TaskDelay(2000);
        LED_Light(LED2, LED_ON);
        LED_Light(LED3, LED_OFF);
        taskCount++;
        printf("task2 is runing...%u\n", taskCount);
        
    }
}

void taskIdle(void)
{
    while (1)
    {
    }
}



void SysTick_Configuration(void)
{
    /* Setup SysTick Timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock/1000))
    {
        /* Capture error */
        while (1);
    }
}

void os_test(void)
{
    LED_Init();
    DRV_UART_Uart1Init();
    SysTick_Configuration();

    printf("task create...\n");


    OS_TASK_CreateTask(0, task1, 200, NULL);
    OS_TASK_CreateTask(1, task2, 200, &gTask2EventBitMap);
    OS_TASK_CreateTask(2, taskIdle, 20, NULL); /* ±ØÐë´æÔÚ */
    
    OS_TASK_SchedulerTask();
    
    for (;;){}
}

int add(int a, int b)
{
	return a+b;
}

int main()
{
	os_test();
}
