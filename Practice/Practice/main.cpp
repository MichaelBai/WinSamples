//经典线程同步互斥问题
#include <stdio.h>
#include <process.h>
#include <windows.h>

const int PROD_NUM = 10;
const int BUFFER_SIZE = 4;
HANDLE  g_semaphoreBufferFull, g_semaphoreBufferEmpty;
CRITICAL_SECTION g_cs;
int g_buffer[BUFFER_SIZE];
int g_prodIndex = 0, g_cnsmIndex = 0;

unsigned int __stdcall productor(void *pPM)
{
    for (int i = 1; i <= PROD_NUM; i++)
    {
        WaitForSingleObject(g_semaphoreBufferEmpty, INFINITE);

        EnterCriticalSection(&g_cs);
        g_buffer[g_prodIndex] = i;
        printf("生产者向缓冲区写入%d\n", i);
        g_prodIndex = (g_prodIndex + 1) % BUFFER_SIZE;
        LeaveCriticalSection(&g_cs);

        ReleaseSemaphore(g_semaphoreBufferFull, 1, NULL);
    }
    return 0;
}

unsigned int __stdcall consumer(void *pPM)
{
    while (true)
    {
        WaitForSingleObject(g_semaphoreBufferFull, INFINITE);

        EnterCriticalSection(&g_cs);
        printf("  消费者%d读出%d\n", GetCurrentThreadId(), g_buffer[g_cnsmIndex]);
        if (g_buffer[g_cnsmIndex] == PROD_NUM)
        {
            // make another consumer thread quit
            ReleaseSemaphore(g_semaphoreBufferFull, 1, NULL);
            LeaveCriticalSection(&g_cs);
            break;
        }
        g_cnsmIndex = (g_cnsmIndex + 1) % BUFFER_SIZE;
        LeaveCriticalSection(&g_cs);

        ReleaseSemaphore(g_semaphoreBufferEmpty, 1, NULL);
    }
    return 0;
}

int main()
{
    //初始化信号量和关键段 
    g_semaphoreBufferFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
    g_semaphoreBufferEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    InitializeCriticalSection(&g_cs);

    const int THREAD_NUM = 3;
    HANDLE handle[THREAD_NUM];

    handle[0] = (HANDLE)_beginthreadex(NULL, 0, productor, NULL, 0, NULL);
    handle[1] = (HANDLE)_beginthreadex(NULL, 0, consumer, NULL, 0, NULL);
    handle[2] = (HANDLE)_beginthreadex(NULL, 0, consumer, NULL, 0, NULL);
    WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);

    CloseHandle(g_semaphoreBufferEmpty);
    CloseHandle(g_semaphoreBufferFull);
    DeleteCriticalSection(&g_cs);
    for (int i = 0; i < THREAD_NUM; i++)  
        CloseHandle(handle[i]); 
    return 0;
}
