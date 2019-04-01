#include "os_public.h"
#include "os_task.h"

#define OS_TASK_MAX 3
#define OS_TASK_SWITCH_INTERVAL 10 /* 单位ms */

#define TASK_NVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define TASK_NVIC_PENDSVSET_BIT		( 1UL << 28UL )
/* ICSR寄存器bit28置1，触发PendSV中断 */
#define OS_TASK_SWITCH  TASK_NVIC_INT_CTRL_REG = TASK_NVIC_PENDSVSET_BIT

typedef S32 StackSize_t;

/* PSR中EPSR执行状态寄存器，其中T位即bit24必须为1 */
#define OS_TASK_INITIAL_XPSR			( 0x01000000 ) 
#define OS_TASK_START_ADDRESS_MASK				( ( StackSize_t ) 0xfffffffeUL )

typedef enum
{
    TASK_INIT = 0,
    TASK_READY,    
    TASK_RUNNING,    
    TASK_SUSPENDED,
    TASK_DELETED,
    TASK_BUTT,
}TaskState_E;

typedef struct
{
    volatile StackSize_t *topStack; /* 栈顶，向下生长 */
    TaskState_E state;
    U16 delay;
    U16 delayMax;
    U32 *eventBitMap;
    U8 id;
}TCB_S;

__attribute__((section("privileged_data"))) volatile TCB_S *gCurrentTCB = NULL;

static TCB_S *gTaskTcbList[OS_TASK_MAX] = {0};

static volatile  U32 gTaskSysTickCount = 0;

/* 初始化进程堆栈内容 */
static StackSize_t* TASK_TaskStackFirstInit(IN StackSize_t *topStack, IN TaskFunction_t func)
{
    /* 按堆栈地址顺序入栈，而非寄存器入栈顺序
     * PSR,PC,LR,R12,R3,R2,R1,R0 以上是芯片自动入栈的
     * R4,R5,R6,R7,R8,R9,R10,R11 以上手工入栈，入出栈顺序注意保持一致
     * 此处也可以增加计数，用于堆栈溢出检查
     */
    topStack--;
    *topStack = OS_TASK_INITIAL_XPSR;
    topStack--;
    *topStack = (((StackSize_t)func) & OS_TASK_START_ADDRESS_MASK);
    topStack--; /* 任务栈初次初始化，已是最上层了，返回即错，因此可以增加返回函数用户调试 */
    topStack -= 5; /* 可用于函数入参 */
    topStack -= 8;
    return topStack;
}

VOID TASK_GetCurrentTask(VOID)
{
    volatile TCB_S *tmpTcb = NULL;
    U8 id = 0;

    for (id = 0; id < OS_TASK_MAX; id++)
    {
        tmpTcb = gTaskTcbList[id];
        if ((TASK_READY == tmpTcb->state) || (TASK_RUNNING == tmpTcb->state))
        {
            tmpTcb->state = TASK_RUNNING;
            gCurrentTCB = tmpTcb;
            break;
        }
    }
    
    return;
}

__asm static VOID TASK_StartFirstTask(VOID)
{
    /* 触发svc，在svc中断中通过修改LD寄存器值的方式进入线程模式 */
    svc 0
    nop
    nop
}

__asm static VOID TASK_SvcHandler(VOID)
{
    extern gCurrentTCB;

    /* 任务相关内容映射到入线程栈 */
	ldr	r3, =gCurrentTCB
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
    msr psp, r0
    isb

    /* 利用LR寄存器异常返回进入线程模式特性 */
	mov r14, #0xfffffffd
	bx r14
    nop
}

void SVC_Handler(void)
{
    TASK_GetCurrentTask();
    TASK_SvcHandler();
}

__asm VOID PendSV_Handler(VOID)
{
    extern gCurrentTCB;
    extern TASK_GetCurrentTask;
            
    /* 把当前任务入栈,主要是R4-R11，因为其它已自动入栈 */
    mrs r0, psp
    isb
    stmdb r0!, {r4-r11}
    dsb
    isb

    /* 把堆栈地址映射到TCB */
    ldr r3, =gCurrentTCB
    ldr r2, [r3]  /* r2 = gCurrentTCB*/
    str r0, [r2]  /* 把r0赋值给gCurrentTCB->topStack */

    /* 切换任务上下文，注意堆栈保存，R3, r14需要重新恢复*/
    stmdb sp!, {r3,r14}
    dsb
    isb
    bl TASK_GetCurrentTask
	ldmia sp!, {r3,r14}
    dsb
    isb

    /* 获取新任务栈 */
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
    dsb
    isb
    msr psp, r0
    isb

	bx r14
    nop
}

static VOID TASK_DelayList(VOID)
{
    volatile TCB_S *tmpTcb = NULL;
    U8 id = 0;

    for (id = 0; id < OS_TASK_MAX; id++)
    {
        tmpTcb = gTaskTcbList[id];
        if (NULL == tmpTcb)
        {
            continue;
        }
        
        if (tmpTcb->delayMax != 0)
        {
            if ((gTaskSysTickCount - tmpTcb->delay) >= tmpTcb->delayMax)
            {
                tmpTcb->delay = 0;
                tmpTcb->delayMax = 0;
                tmpTcb->state = TASK_READY;
                OS_TASK_SWITCH;
            }
            else
            {
                tmpTcb->state = TASK_SUSPENDED;
            }
        }
    }
    
    return;
}

static VOID TASK_WaitForEventList(VOID)
{
    volatile TCB_S *tmpTcb = NULL;
    U8 id = 0;

    for (id = 0; id < OS_TASK_MAX; id++)
    {
        tmpTcb = gTaskTcbList[id];
        if (NULL == tmpTcb)
        {
            continue;
        }
        
        if (NULL == tmpTcb->eventBitMap)
        {
            continue;
        }
        
        if (*tmpTcb->eventBitMap != 0)
        {
            tmpTcb->state = TASK_READY;
            OS_TASK_SWITCH;
        }
        else
        {
            tmpTcb->state = TASK_SUSPENDED;
        }
    }
}

VOID SysTick_Handler(VOID)
{
    TASK_DelayList();
    TASK_WaitForEventList();
    gTaskSysTickCount++;
    if ((gTaskSysTickCount%OS_TASK_SWITCH_INTERVAL) != 0)
    {
        return;
    }

    OS_TASK_SWITCH;
}

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
S32 OS_TASK_CreateTask
(
    IN U8 id, 
    IN TaskFunction_t taskHandle, 
    IN U16 taskStackDeep, 
    IN U32 *eventBitMap
)
{
    TCB_S *newTcb = NULL;
    StackSize_t *topStack = NULL;
    
    if (id >= OS_TASK_MAX)
    {
        return OS_ERROR;
    }
    
    newTcb = (TCB_S*)malloc(sizeof(TCB_S));
    if (NULL == newTcb)
    {
        return OS_ERROR;
    }
    
    newTcb->state = TASK_INIT;
    topStack = (StackSize_t *)malloc(sizeof(StackSize_t)*taskStackDeep);
    if (NULL == topStack)
    {
        return OS_ERROR;
    }
    topStack += sizeof(StackSize_t)*taskStackDeep;

    newTcb->topStack = TASK_TaskStackFirstInit(topStack, taskHandle);
    
    newTcb->state = TASK_READY;
    newTcb->delay = 0;
    newTcb->delayMax = 0;
    newTcb->eventBitMap = eventBitMap;
    newTcb->id = id;

    gTaskTcbList[id] = newTcb;
    
    return OS_OK;
}

/*==================================================================
* Function	: OS_TASK_SchedulerTask
* Description	: 启动任务调度
* Input Para	: 无
* Output Para	: 无 
* Return Value: 无
==================================================================*/
VOID OS_TASK_SchedulerTask(VOID)
{   
    TASK_StartFirstTask();
    return;
}


/*==================================================================
* Function	: OS_TASK_TaskDelay
* Description	: 用于阻塞任务等待超时
* Input Para	: IN U16 ms  阻塞毫秒数
* Output Para	: 无
* Return Value: 无
==================================================================*/
VOID OS_TASK_TaskDelay(IN U16 ms)
{
    if ((0 == gCurrentTCB->delay) && (0 == gCurrentTCB->delayMax))
    {
        gCurrentTCB->delayMax = ms;
        gCurrentTCB->delay = gTaskSysTickCount;
        gCurrentTCB->state = TASK_SUSPENDED;
        OS_TASK_SWITCH;
    }
}

/*==================================================================
* Function	: OS_TASK_WaitForEvent
* Description	: 用于阻塞任务等待事件
* Input Para	: 无
* Output Para	: 无
* Return Value: 无
==================================================================*/
VOID OS_TASK_WaitForEvent(VOID)
{
    if (NULL == gCurrentTCB->eventBitMap)
    {
        return;
    }
    
    if (0 == *gCurrentTCB->eventBitMap)
    {
        gCurrentTCB->state = TASK_SUSPENDED;
        OS_TASK_SWITCH;
    }
}

