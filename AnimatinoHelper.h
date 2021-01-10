#pragma once
#include <vector>

class AnimatinoHelper
{
public:
	//AnimatinoHelper(void);
	//~AnimatinoHelper(void);

	// 设置移动轨迹和时间
	static BOOL AddMove(HWND hWnd, const CRect &srcRect, const CRect &dstRect, int time = 300);
	// 设置最小化列表
	static BOOL AddHWND(HWND hWnd);
	// 开始接受动画参数
	static void Begin(void);
	// 结束接受动画参数
	static void End(void);
	// 动画绘制
	static BOOL MoveAnimation(void);
	//最小化
	static BOOL AllMiniMize(void);

private:
	//每帧绘制函数
	static VOID CALLBACK DrawFrame(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	//最小化流程
	static DWORD WINAPI MiniMize(LPVOID param);


	// 测试setTimerQueueTimer
	static void TestTimerQueue(void);
	//用于测试的回调函数
	static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	

private:
	//动画结构体
	typedef struct _FrameMove  
	{
		HWND hWnd;
		CRect   curRect;
		CRect	stepRect;
		CRect dstRect;
		int curStep;

		//构造函数
		_FrameMove(HWND hwnd, const CRect &src, const CRect& step, const CRect & dst): \
			hWnd(hwnd), curRect(src), stepRect(step), dstRect(dst), curStep(0)
		{
		}
	}sFrameMove;

	typedef struct _HWNDOperation
	{
		HWND hWnd;
		int operation;

		////构造函数
		//_HWNDOperation(HWND hwnd, int op):\
		//	hWnd(hwnd), operation(op)
		//{
		//}

	}sHWNDOperation;

private:
	static HANDLE g_hDoneEvent;										//结束控制
	static HANDLE g_hMutext;											//互斥量
	static int g_iFrameNum;												//动画所需帧数
	static int g_icurFrame;													//当前帧数
	static std::vector<sFrameMove> g_vFrameStep;			//每帧移动的步伐
	static std::vector<HWND> g_vHWNDs;							//窗口句柄些

	//常量
	static const int TIMEPERFRAME = 40;									//每帧40毫秒，1秒25帧
};
