#pragma once
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
//����ദ�����е�hook��Ϣ
//////////////////////////////////////////////////////////////////////////
class CIAAnalyser
{
public:
	CIAAnalyser(void);
	CIAAnalyser(HWND hSelf);
	~CIAAnalyser(void);

	//////////////////////////////////////////////////////////////////////////
	//����Ϊ���������Ϣ�ĺ���
	LRESULT OnSubWindowCreate(WPARAM wParam, LPARAM lParam);			//�ӵ�CBT���ӽػ�Ĵ��ڴ�����Ϣ
	LRESULT OnSubWindowActive(WPARAM wParam, LPARAM lParam);			//�ӵ����ڻ�ȡ������Ϣ
	LRESULT OnSubWindowMove(WPARAM wParam, LPARAM lParam);			//�ӵ������ƶ���Ϣ
	LRESULT OnSubWindowDestory(WPARAM wParam, LPARAM lParam);			//�ӵ�����������Ϣ
	LRESULT OnSubKeyType(WPARAM wParam, LPARAM lParam);						//�ӵ�������Ϣ
	LRESULT OnSubMouseClick(WPARAM wParam, LPARAM lParam);				//�ӵ��������Ϣ
	LRESULT OnSubMinMax(WPARAM wParam, LPARAM lParam);						//�ӵ������С����Ϣ
	LRESULT OnSubSetText(WPARAM wParam, LPARAM lParam);					//�ӵ������ı���Ϣ
	
	///////////////////////////////////////////////////////////////////////////
	//���ݿ����
	LRESULT DropLocalDB();																	//��ձ������ݿ�
	LRESULT RebuildLocalDB();																	//�ؽ��������ݿ�
	LRESULT ExportData();																			//��������
	LRESULT NewWatchTime();																//�µĹ۲�ʱ��

	//////////////////////////////////////////////////////////////////////////
	//����������ɶ������߳�
	LRESULT WinsLocal();					//�����������洰��
	LRESULT GlobleClique();						//ȫ�־���
	LRESULT ShowNextClique();						//ȫ�־���
	LRESULT ScreenShot();						//���Խ�ͼ

	//////////////////////////////////////////////////////////////////////////
	//���ƺ���
	static BOOL StopWork();																						//ֹͣ�̹߳���
	BOOL CommitRecord();																				//�������������Ϣ
	static DWORD WINAPI CommitInNewThread(LPVOID param);																	//�����߳����������������Ϣ
	static DWORD WINAPI ThreadTranslateMessage(LPVOID param);					//������������hook��Ϣ�����ô������ĵط���Ӧ����ΪThreadFunc����֮

	///////////////////////////////////////////////////////////////////////////
	//Getters
	BOOL  GetWindowLocals(vector<sHWNDAtt> &result);									//�õ�HWND������¼��λ����Ϣ
	BOOL GetHWND(vector<HWND> &result);													//�õ����м�⵽��HWND

private:
	WinInfoCtl					*m_cWinInfoCtl;					//������Ϣ������
	CGestureRecognizer		*m_cGesRecognizer;					//����ʶ����

//�߳̿���
private:
	static BOOL m_bWorkFlag;			//��ֵΪ�棬�̼߳�������
	static BOOL m_bStopFlag;				//��ֵΪ�棬�߳�ֹͣ�ɹ�
	HWND m_hSelf;							//FlowWatcher�Լ��ľ��
	list<CPoint> m_lLocus;				//���켣
public:
	// ƽ���������д���
	BOOL OnPlainLayout(void);
	// ��ʼ�������켣
	void StartTraceMouse(int x, int y);
	// ��ֹ�������켣
	void EndTraceMouse(int x, int y);
	// ���ڸ������켣
	void InTraceMouse(int x, int y);
};
