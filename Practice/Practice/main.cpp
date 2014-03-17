//�����߳�ͬ����������
#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //ȫ����Դ
unsigned int __stdcall Fun(void *pPM); //�̺߳���
const int THREAD_NUM = 10; //���̸߳���
//�ź�����ؼ���
HANDLE  g_hThreadSemaphore;
CRITICAL_SECTION g_csThreadCode;
int main()
{
    //��ʼ���ź����͹ؼ��� 
    g_hThreadSemaphore = CreateSemaphore(NULL, 0, 1, NULL);   
    InitializeCriticalSection(&g_csThreadCode);

    g_nNum = 0;
    HANDLE handle[THREAD_NUM];

    int i = 0;
    while (i < THREAD_NUM) 
    {
        handle[i] = (HANDLE)_beginthreadex(NULL, 0, Fun, &i, 0, NULL);
        WaitForSingleObject(g_hThreadSemaphore, INFINITE);
        i++;
    }
    WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);

    CloseHandle(g_hThreadSemaphore);
    DeleteCriticalSection(&g_csThreadCode);
    for (i = 0; i < THREAD_NUM; i++)  
        CloseHandle(handle[i]); 
    return 0;
}

unsigned int __stdcall Fun(void *pPM)
{
    int nThreadNum = *(int*)pPM;
    ReleaseSemaphore(g_hThreadSemaphore, 1, NULL);

    Sleep(50);//some work should to do

    EnterCriticalSection(&g_csThreadCode);
    g_nNum++;
    Sleep(20);//some work should to do
    printf("�̱߳��Ϊ%d  ȫ����ԴֵΪ%d\n", nThreadNum, g_nNum);
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}