#ifndef __OS_TASK_H__
#define __OS_TASK_H__

typedef void (*TaskFunction_t)(void);

/*==================================================================
* Function	: OS_TASK_CreateTask
* Description	: 创建新任务
* Input Para	:     
    IN U8 id,  任务号，同任务优先级，每个任务唯一
    IN TaskFunction_t taskHandle,  任务函数，任务入口
    IN U16 taskStackDeep,  任务的最大堆栈深度，sizeof(StackSize_t)*taskStackDeep=实际占内存字节数
    IN U32 *eventBitMap  任务事件位图，用于任务通信，不需要可以填NULL
* Output Para	: 无
* Return Value: 
    OS_OK    创建成功
    OS_ERROR 创建失败
==================================================================*/
extern S32 OS_TASK_CreateTask
(
    IN U8 id, 
    IN TaskFunction_t taskHandle, 
    IN U16 taskStackDeep, 
    IN U32 *eventBitMap
);

/*==================================================================
* Function	: OS_TASK_SchedulerTask
* Description	: 启动任务调度
* Input Para	: 无
* Output Para	: 无 
* Return Value: 无
==================================================================*/
extern VOID OS_TASK_SchedulerTask(VOID);

/*==================================================================
* Function	: OS_TASK_TaskDelay
* Description	: 用于阻塞任务等待超时
* Input Para	: IN U16 ms  阻塞毫秒数
* Output Para	: 无
* Return Value: 无
==================================================================*/
extern VOID OS_TASK_TaskDelay(IN U16 ms);

/*==================================================================
* Function	: OS_TASK_WaitForEvent
* Description	: 用于阻塞任务等待事件
* Input Para	: 无
* Output Para	: 无
* Return Value: 无
==================================================================*/
extern VOID OS_TASK_WaitForEvent(VOID);

#endif

