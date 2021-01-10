#include "StdAfx.h"
#include "AnimatinoHelper.h"

HANDLE AnimatinoHelper::g_hDoneEvent;
HANDLE AnimatinoHelper::g_hMutext = CreateMutex(NULL, FALSE,	NULL);
int AnimatinoHelper::g_iFrameNum;																			//��������֡��
int AnimatinoHelper::g_icurFrame;																				//��ǰ֡��
std::vector<AnimatinoHelper::sFrameMove> AnimatinoHelper::g_vFrameStep;			//ÿ֡�ƶ��Ĳ���
std::vector<HWND> AnimatinoHelper::g_vHWNDs;							//���ھ��Щ

// �����ƶ��켣��ʱ��
BOOL AnimatinoHelper::AddMove(HWND hWnd, const CRect &srcRect, const CRect &dstRect, int time)
{
	//��������֡��
	g_iFrameNum = time / TIMEPERFRAME;			//40����1֡
	//����ÿ֡�ƶ���
	int moveX = (dstRect.left - srcRect.left) / g_iFrameNum;
	int moveCX = (dstRect.right - srcRect.right) / g_iFrameNum;
	int moveY = (dstRect.top - srcRect.top) / g_iFrameNum;
	int moveCY = (dstRect.bottom - srcRect.bottom) / g_iFrameNum;

	g_vFrameStep.push_back(sFrameMove(hWnd, srcRect, CRect(moveX, moveY, moveCX, moveCY), dstRect));
	
	return TRUE;
}

// ��Ӵ��ھ��
BOOL AnimatinoHelper::AddHWND(HWND hWnd)
{
	g_vHWNDs.push_back(hWnd);
	return TRUE;
}

// ��ʼ���ܶ�������
void AnimatinoHelper::Begin(void)
{
	DWORD dw = WaitForSingleObject(g_hMutext, INFINITE);
	if(dw == WAIT_OBJECT_0)
	{
		g_vFrameStep.clear();
		g_vHWNDs.clear();
		g_iFrameNum = 0;
		g_icurFrame = 0;
	}
	else
	{
		AfxMessageBox("�ȴ�����������");
	}

	return;
	
}

// �������ܶ�������
void AnimatinoHelper::End(void)
{
	ReleaseMutex(g_hMutext);

	return;
}

// ��������
BOOL AnimatinoHelper::MoveAnimation(void)
{
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;

	g_hDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == g_hDoneEvent)
	{
		AfxMessageBox("�����¼�ʧ�� (%d)\n", GetLastError());
		return FALSE;
	}

	hTimerQueue = CreateTimerQueue();
	if (NULL == hTimerQueue)
	{
		AfxMessageBox("����TimerQueueʧ�� (%d)\n", GetLastError());
		return FALSE;
	}


	if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, 
		(WAITORTIMERCALLBACK)DrawFrame,	NULL , TIMEPERFRAME, TIMEPERFRAME, 0))
	{
		AfxMessageBox("����TimerQueueTimer failed (%d)\n", GetLastError());
		return FALSE;
	}

	if (WaitForSingleObject(g_hDoneEvent, INFINITE) != WAIT_OBJECT_0)
	{
		AfxMessageBox("WaitForSingleObject ʧ�� (%d)\n", GetLastError());
	}

	CloseHandle(g_hDoneEvent);

	if (!DeleteTimerQueue(hTimerQueue))
	{
		AfxMessageBox("DeleteTimerQueueʧ�� (%d)\n", GetLastError());
	}

	return TRUE;
}

//����һ֡
VOID CALLBACK AnimatinoHelper::DrawFrame(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	++g_icurFrame;

	for(std::vector<sFrameMove>::iterator it = g_vFrameStep.begin(); it != g_vFrameStep.end(); it++)
	{
		//��������һ֡��ֱ�ӵ�λ
		if(g_icurFrame >= g_iFrameNum)
		{	
			::SetWindowPos(it->hWnd, HWND_BOTTOM, it->dstRect.left, it->dstRect.top, it->dstRect.Width(), it->dstRect.Height(),
				SWP_ASYNCWINDOWPOS | SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_SHOWWINDOW);

			continue;
		}

		//�������һ֡��һ��һ����
		CRect curRect;
		curRect.left = it->curRect.left + it->stepRect.left;
		curRect.right = it->curRect.right + it->stepRect.right;
		curRect.top = it->curRect.top + it->stepRect.top;
		curRect.bottom = it->curRect.bottom + it->stepRect.bottom;

		::SetWindowPos(it->hWnd, HWND_TOPMOST, curRect.left, curRect.top, curRect.Width(), curRect.Height(),
			 SWP_ASYNCWINDOWPOS | SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOCOPYBITS);

		it->curRect = curRect;
		it->curStep++;
	}

	//���һ֡�����˾ͽ���
	if(g_icurFrame >= g_iFrameNum + 1)
	{
		SetEvent(g_hDoneEvent);
	}
}

//������С��
BOOL AnimatinoHelper::AllMiniMize(void)
{
	//������Դ
	int num = g_vHWNDs.size();
	HANDLE *threads = new HANDLE[num];
	sHWNDOperation *sOperation = new sHWNDOperation[num];

	//����
	int i=0;
	for(std::vector<HWND>::const_iterator it = g_vHWNDs.begin(); it != g_vHWNDs.end(); it++, i++)
	{
		sOperation[i].hWnd = *it;
		sOperation[i].operation = SW_MINIMIZE;
		threads[i] = ::CreateThread(NULL, 0, &AnimatinoHelper::MiniMize, &sOperation[i], 0, NULL);
	}

//	if (WaitForMultipleObjects(num, threads, INFINITE) != WAIT_OBJECT_0)
	if (WaitForMultipleObjects(num, threads, TRUE, INFINITE) != WAIT_OBJECT_0)
	{
		AfxMessageBox("WaitForSingleObject ʧ�� (%d)\n", GetLastError());
	}

	//������Դ
	delete [] sOperation;
	for(int i=0; i<num; i++)
	{
		CloseHandle(threads[i]);
	}
	delete [] threads;

	return TRUE;
}

//��С������
DWORD WINAPI AnimatinoHelper::MiniMize(LPVOID param)
{
	sHWNDOperation *operation = static_cast<sHWNDOperation *>(param);
	::ShowWindow(operation->hWnd, SW_MINIMIZE);
	return 0;
}


// ����setTimerQueueTimer
void AnimatinoHelper::TestTimerQueue(void)
{
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;
	int arg = 123;

	g_hDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == g_hDoneEvent)
	{
		AfxMessageBox("CreateEvent failed (%d)\n", GetLastError());
		return;
	}

	hTimerQueue = CreateTimerQueue();
	if (NULL == hTimerQueue)
	{
		AfxMessageBox("CreateTimerQueue failed (%d)\n", GetLastError());
		return;
	}


	if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, 
		(WAITORTIMERCALLBACK)TimerRoutine, &arg , 5000, 0, 0))
	{
		AfxMessageBox("CreateTimerQueueTimer failed (%d)\n", GetLastError());
		return;
	}


	AfxMessageBox("5���Ӻ󴥷���ʱ���¼�\n");


	if (WaitForSingleObject(g_hDoneEvent, INFINITE) != WAIT_OBJECT_0)
		AfxMessageBox("WaitForSingleObject failed (%d)\n", GetLastError());

	CloseHandle(g_hDoneEvent);

	if (!DeleteTimerQueue(hTimerQueue))
		AfxMessageBox("DeleteTimerQueue failed (%d)\n", GetLastError());

	AfxMessageBox("Timer���Գɹ�");

}

VOID CALLBACK AnimatinoHelper::TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (lpParam == NULL)
	{
		AfxMessageBox("TimerRoutine lpParam is NULL\n");
	}
	else
	{

		CString msg;
		msg.Format("Timer routine called. Parameter is %d.\n", *(int*)lpParam);
		MessageBox(NULL, msg, "result", MB_OK);
	}

	SetEvent(g_hDoneEvent);
}


