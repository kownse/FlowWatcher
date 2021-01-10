#pragma once

#include "stdafx.h"
#include <map>
#include "WindowInfo.h"
#include "DBManager.h"
#include "WndActiveGraph.h"
#include "AbstractDBManager.h"
#include "EmbeddedMySql.h"
#include <vector>

//���ڴ��������ṹ��
typedef struct _wndcreate
{
	CTime time;
	int count;
	bool recorded;
	_wndcreate(CTime t, int c):time(t),count(c),recorded(false)
	{
		//this->time = time;
		//this->count = count;
		//this->recorded = false;
	}
}WndCreateNode;

class WinInfoCtl
{
public:
	explicit WinInfoCtl(void);
	~WinInfoCtl(void);
	
	void NewWindow(HWND hWnd);																		//�´��ڴ���
	void NewWindow(HWND hWnd, const CString& title, const CString& className, const RECT& rect, bool active);				//�´��ڴ���
	void WindowActive(HWND hWnd);																	//���ڼ���
	//void WindowUnActive(HWND hWnd);															//����ʧȥ����
	void WindowMove(HWND hWnd, const CRect& rect);													//�����ƶ�
	void WindowDestory(HWND hWnd);																//��������
	void OnKeyType();																								//�����û���Ϣ
	void OnMouseClick();																						//�����
	void OnMin(HWND hWnd);																				//��С��
	void OnMax(HWND hWnd);																				//���
	void OnRestore(HWND hWnd);																			//�ָ���С

	void Log();																										//����һ���޴��CString��д��־����ʱ��������ڴ�������
	void WriteLog();																								//��д��־�ļ�
	void NewWatchTime();																						//�����µļ��ʱ��
	void LogActiveGraphInFile();																				//���ͼ����־�ļ���д����
	void LogActiveGraphInDB();																				//���Ӷ�ͼ�ļ�д�����ݿ���
	void CalculateImportance();																				//���������Ҫ��

	/////////////////////////////////////////////////////////////////
	//Getters
	size_t GetMaxAtt();																								//�õ�����ע��
	size_t GetTotalAtt();																								//�õ��ܵĹ�ע��
	float GetNormalBase();																							//�õ�������
	const int GetWorkArea();																							//�õ����������
	BOOL GetWindowLocals(vector<sHWNDAtt> &result);																//�õ�����λ��
	BOOL GetHWND(vector<HWND> &result);																//�õ�HWND

	//////////////////////////////////////////////////////////////////////////
	// �������
	void GetBKCliqueWinInfo(vector<vector<const WindowInfo* const>> &result);			//�õ�BK��������WIndowInfo��ʾ
	BOOL getGlobeClique(void);																							// �ø�˹�˺�K-MEANS����ȫ�־���

	////////////////////////////////
	//��������
	///////////////////////////////
	BOOL OnSetWinsLocal(void);																			        //���ô�������
	BOOL OnShowNextClique(void);																			//��ʾ��һ�����ര�ڼ���
	BOOL OnScreenShot(void);																					//��ͼ
	BOOL OnPlainLayout(void	);																					//ƽ������

	//////////////////////////////////////////////////////////////////////////
	// �ȽϺ���
	static bool sHWNDAttCompare(const sHWNDAtt &a, const sHWNDAtt & b);

	//////////////////////////////////////////////////////////////////////////
	// ������
	//////////////////////////////////////////////////////////////////////////
	BOOL ExamMaps();																								//����hwnd_info

#ifdef DB_LOG
	BOOL ConnectDB();																								//�������ݿ�
	BOOL ReConnect();																								//��������
	BOOL DropLocalDB();																							//�����������ݿ�
	BOOL RebuildLocalDB();																						//�ؽ��������ݿ�
	BOOL ExportData();																								//��������
#endif // DB_LOG

private:
	CString GetVersion();																	//��ȡ�汾��Ϣ
	

private:
	map<HWND, WindowInfo> hwnd_info;											//HWND������Ϣ��map,��¼��ǰ����ڵ���Ϣ
	CWndActiveGraph activeGraph;														//�����л�ͼ�ṹ
	list<WndCreateNode> wnd_create_count_list;									//���ڴ��������б�
	list<WindowInfo> history_list;															//��¼�Ѿ��رմ��ڵ���ʷ��Ϣ
	vector<vector<pair<HWND, float>>> m_vGlobelClique;		//ȫ�־���
	HWND m_hCurrentActiveWnd;															//��һ������ڣ����ڱ�Ǵ���ʧȥ����̫
	CTime m_tWatchTime;																		//��ǰ���ʱ��,���ڼ�ش��ڽ��ܵĽ�����ϢƵ�ʼ���
	unsigned int tickCount;																		//������
	int m_iDeskArea;																				//�������
	RECT m_cDeskRect;																			//����RECT
	
	CTime rec_time;																				//��ǰ��¼ʱ��
	long m_lKeyCount;																			//���̼���
	long m_lMouseCount;																		//������

	//////////////////////////////////////////////////////////////////////////
	// ���ڲ���
	int m_iCurClique;																				//��ǰ��ʾ�ľ�����

	//////////////////////////////////////////////////////////////////////////
	//����
private:
	static const int MIN_WINDOW_WIDTH = 500;
	static const int MIN_WINDOW_HEIGHT = 450;

#ifdef DB_LOG
	TCHAR *computer;																			//�������
	unsigned int rec_id;																			//�����ݿ��еļ�¼�� 
	//DBManager	m_remoteMySql;																//mysql���ݿ�����ʵ��
	CEmbeddedMySql m_embMySql;														//Ƕ��ʽmysql
	CAbstractDBManager *m_mysql;
	BOOL m_bDBConnect;																		//���ݿ����ӱ�־
#endif


};
