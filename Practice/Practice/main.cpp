//�����߳�ͬ����������
#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //ȫ����Դ
unsigned int __stdcall Fun(void *pPM); //�̺߳���
const int THREAD_NUM = 10; //���̸߳���
//�¼���ؼ���
HANDLE  g_hThreadEvent;
CRITICAL_SECTION g_csThreadCode;
int main()
{
    //��ʼ���¼��͹ؼ��� �Զ���λ,��ʼ�޴����������¼�  
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
    g_nNum++;  //����ȫ����Դ
    Sleep(20);//some work should to do
    printf("�̱߳��Ϊ%d  ȫ����ԴֵΪ%d\n", nThreadNum, g_nNum);
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}