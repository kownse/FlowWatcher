#include "StdAfx.h"
#include "AnimatinoHelper.h"

HANDLE AnimatinoHelper::g_hDoneEvent;
HANDLE AnimatinoHelper::g_hMutext = CreateMutex(NULL, FALSE,	NULL);
int AnimatinoHelper::g_iFrameNum;																			//动画所需帧数
int AnimatinoHelper::g_icurFrame;																				//当前帧数
std::vector<AnimatinoHelper::sFrameMove> AnimatinoHelper::g_vFrameStep;			//每帧移动的步伐
std::vector<HWND> AnimatinoHelper::g_vHWNDs;							//窗口句柄些

// 设置移动轨迹和时间
BOOL AnimatinoHelper::AddMove(HWND hWnd, const CRect &srcRect, const CRect &dstRect, int time)
{
	//计算所需帧数
	g_iFrameNum = time / TIMEPERFRAME;			//40毫秒1帧
	//计算每帧移动量
	int moveX = (dstRect.left - srcRect.left) / g_iFrameNum;
	int moveCX = (dstRect.right - srcRect.right) / g_iFrameNum;
	int moveY = (dstRect.top - srcRect.top) / g_iFrameNum;
	int moveCY = (dstRect.bottom - srcRect.bottom) / g_iFrameNum;

	g_vFrameStep.push_back(sFrameMove(hWnd, srcRect, CRect(moveX, moveY, moveCX, moveCY), dstRect));
	
	return TRUE;
}

// 添加窗口句柄
BOOL AnimatinoHelper::AddHWND(HWND hWnd)
{
	g_vHWNDs.push_back(hWnd);
	return TRUE;
}

// 开始接受动画参数
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
		AfxMessageBox("等待互斥量错误");
	}

	return;
	
}

// 结束接受动画参数
void AnimatinoHelper::End(void)
{
	ReleaseMutex(g_hMutext);

	return;
}

// 动画绘制
BOOL AnimatinoHelper::MoveAnimation(void)
{
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;

	g_hDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == g_hDoneEvent)
	{
		AfxMessageBox("创建事件失败 (%d)\n", GetLastError());
		return FALSE;
	}

	hTimerQueue = CreateTimerQueue();
	if (NULL == hTimerQueue)
	{
		AfxMessageBox("创建TimerQueue失败 (%d)\n", GetLastError());
		return FALSE;
	}


	if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, 
		(WAITORTIMERCALLBACK)DrawFrame,	NULL , TIMEPERFRAME, TIMEPERFRAME, 0))
	{
		AfxMessageBox("创建TimerQueueTimer failed (%d)\n", GetLastError());
		return FALSE;
	}

	if (WaitForSingleObject(g_hDoneEvent, INFINITE) != WAIT_OBJECT_0)
	{
		AfxMessageBox("WaitForSingleObject 失败 (%d)\n", GetLastError());
	}

	CloseHandle(g_hDoneEvent);

	if (!DeleteTimerQueue(hTimerQueue))
	{
		AfxMessageBox("DeleteTimerQueue失败 (%d)\n", GetLastError());
	}

	return TRUE;
}

//绘制一帧
VOID CALLBACK AnimatinoHelper::DrawFrame(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	++g_icurFrame;

	for(std::vector<sFrameMove>::iterator it = g_vFrameStep.begin(); it != g_vFrameStep.end(); it++)
	{
		//如果是最后一帧就直接到位
		if(g_icurFrame >= g_iFrameNum)
		{	
			::SetWindowPos(it->hWnd, HWND_BOTTOM, it->dstRect.left, it->dstRect.top, it->dstRect.Width(), it->dstRect.Height(),
				SWP_ASYNCWINDOWPOS | SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_SHOWWINDOW);

			continue;
		}

		//不是最后一帧就一步一步走
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

	//最后一帧画完了就结束
	if(g_icurFrame >= g_iFrameNum + 1)
	{
		SetEvent(g_hDoneEvent);
	}
}

//集体最小化
BOOL AnimatinoHelper::AllMiniMize(void)
{
	//创建资源
	int num = g_vHWNDs.size();
	HANDLE *threads = new HANDLE[num];
	sHWNDOperation *sOperation = new sHWNDOperation[num];

	//开搞
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
		AfxMessageBox("WaitForSingleObject 失败 (%d)\n", GetLastError());
	}

	//清理资源
	delete [] sOperation;
	for(int i=0; i<num; i++)
	{
		CloseHandle(threads[i]);
	}
	delete [] threads;

	return TRUE;
}

//最小化流程
DWORD WINAPI AnimatinoHelper::MiniMize(LPVOID param)
{
	sHWNDOperation *operation = static_cast<sHWNDOperation *>(param);
	::ShowWindow(operation->hWnd, SW_MINIMIZE);
	return 0;
}


// 测试setTimerQueueTimer
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


	AfxMessageBox("5秒钟后触发计时器事件\n");


	if (WaitForSingleObject(g_hDoneEvent, INFINITE) != WAIT_OBJECT_0)
		AfxMessageBox("WaitForSingleObject failed (%d)\n", GetLastError());

	CloseHandle(g_hDoneEvent);

	if (!DeleteTimerQueue(hTimerQueue))
		AfxMessageBox("DeleteTimerQueue failed (%d)\n", GetLastError());

	AfxMessageBox("Timer测试成功");

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


