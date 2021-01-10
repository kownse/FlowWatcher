#pragma once
#include <vector>

class AnimatinoHelper
{
public:
	//AnimatinoHelper(void);
	//~AnimatinoHelper(void);

	// �����ƶ��켣��ʱ��
	static BOOL AddMove(HWND hWnd, const CRect &srcRect, const CRect &dstRect, int time = 300);
	// ������С���б�
	static BOOL AddHWND(HWND hWnd);
	// ��ʼ���ܶ�������
	static void Begin(void);
	// �������ܶ�������
	static void End(void);
	// ��������
	static BOOL MoveAnimation(void);
	//��С��
	static BOOL AllMiniMize(void);

private:
	//ÿ֡���ƺ���
	static VOID CALLBACK DrawFrame(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	//��С������
	static DWORD WINAPI MiniMize(LPVOID param);


	// ����setTimerQueueTimer
	static void TestTimerQueue(void);
	//���ڲ��ԵĻص�����
	static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	

private:
	//�����ṹ��
	typedef struct _FrameMove  
	{
		HWND hWnd;
		CRect   curRect;
		CRect	stepRect;
		CRect dstRect;
		int curStep;

		//���캯��
		_FrameMove(HWND hwnd, const CRect &src, const CRect& step, const CRect & dst): \
			hWnd(hwnd), curRect(src), stepRect(step), dstRect(dst), curStep(0)
		{
		}
	}sFrameMove;

	typedef struct _HWNDOperation
	{
		HWND hWnd;
		int operation;

		////���캯��
		//_HWNDOperation(HWND hwnd, int op):\
		//	hWnd(hwnd), operation(op)
		//{
		//}

	}sHWNDOperation;

private:
	static HANDLE g_hDoneEvent;										//��������
	static HANDLE g_hMutext;											//������
	static int g_iFrameNum;												//��������֡��
	static int g_icurFrame;													//��ǰ֡��
	static std::vector<sFrameMove> g_vFrameStep;			//ÿ֡�ƶ��Ĳ���
	static std::vector<HWND> g_vHWNDs;							//���ھ��Щ

	//����
	static const int TIMEPERFRAME = 40;									//ÿ֡40���룬1��25֡
};
