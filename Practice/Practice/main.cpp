//经典线程同步互斥问题
#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //全局资源
unsigned int __stdcall Fun(void *pPM); //线程函数
const int THREAD_NUM = 10; //子线程个数
//事件与关键段
HANDLE  g_hThreadEvent;
CRITICAL_SECTION g_csThreadCode;
int main()
{
    //初始化事件和关键段 自动置位,初始无触发的匿名事件  
    g_hThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);   
    InitializeCriticalSection(&g_csThreadCode);

    g_nNum = 0;
    HANDLE handle[THREAD_NUM];

    int i = 0;
    while (i < THREAD_NUM) 
    {
        handle[i] = (HANDLE)_beginthreadex(NULL, 0, Fun, &i, 0, NULL);
        WaitForSingleObject(g_hThreadEvent, INFINITE);
        i++;
    }
    WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);

    CloseHandle(g_hThreadEvent);
    DeleteCriticalSection(&g_csThreadCode);
    return 0;
}

unsigned int __stdcall Fun(void *pPM)
{
    int nThreadNum = *(int*)pPM;
    SetEvent(g_hThreadEvent);

    Sleep(50);//some work should to do

    EnterCriticalSection(&g_csThreadCode);
    g_nNum++;  //处理全局资源
    Sleep(20);//some work should to do
    printf("线程编号为%d  全局资源值为%d\n", nThreadNum, g_nNum);
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}