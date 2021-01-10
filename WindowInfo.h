#pragma once
#include "stdafx.h"
#include <list>
#include <ctime>
#include <map>
#include "DBManager.h"
//#include "CMD_Globle.h"

using namespace std;

//��ṹ��
typedef struct _activenode
{
	CTime time;
	bool flag;
	_activenode(const CTime& t, bool f) : time(t), flag(f)
	{
		//this->time = time; 
		//this->flag = flag;
	}
}ActiveNode;
//ActiveNode
//class ActiveNode
//{
//public:
//	ActiveNode(CTime time, bool flag)
//	{
//		this->time = time;
//		this->flag = flag;
//	}
//	CTime time;
//	bool flag;
//};

//λ�ýṹ��
typedef struct _localnode
{
	enum LOCALTYPE {MOVE, MAX, MIN, RESTORE};
	CTime time;
	CRect rect;
	LOCALTYPE type;
	_localnode(const CTime& t, const CRect& r): time(t), rect(r), type(MOVE)
	{
		//this->time = time;
		//this->rect = rect;
		//type = MOVE;
	}

	_localnode(const CTime& t, LOCALTYPE l):time(t),type(l)
	{
		//this->time = time;
		//this->type = type;
	}
}LocalNode;
//LocalNode::_localnode(ATL::CTime time, CRect rect)
//{
//	this->time = time;
//	this->rect = rect;
//}


//��������ṹ��
typedef struct _keytypenode
{
	CTime time;
	int count;
	_keytypenode(const CTime& t, int c):time(t),count(c)
	{
		//this->time = time;
		//this->count = count;
	}
}KeyTypeNode;
//KeyTypeNode::_keytypenode(ATL::CTime time, int count)
//{
//	this->time = time;
//	this->count = count;
//}

//������ṹ��
typedef struct _mouseclicknode
{
	CTime time;
	int count;
	_mouseclicknode(const CTime& t, int c):time(t),count(c)
	{
		//this->time = time;
		//this->count = count;
	}
}MouseClickNode;
//MouseClickNode::_mouseclicknode(CTime time, int count)
//{
//	this->time = time;
//	this->count = count;
//}

//����ṹ��
typedef struct _titlenode
{
	CTime time;
	CString title;
	_titlenode(const CTime& t, const CString& ti):time(t),title(ti)
	{
		//this->time = time;
		//this->title = title;
	}
}TitleNode;

//��¼�������޷���HWND�õ��������Ϣ
class WindowInfo
{

public:
	WindowInfo(void);
	~WindowInfo(void);

	explicit WindowInfo(const CTime& tCreate);															//������ʱ��Ĺ��캯��
	explicit WindowInfo(const CTime& tCreate, const RECT& lRect);										//��ʱ�����״����Ĺ��캯��
	explicit WindowInfo(HWND hWnd);															//�Ӿ���õ���ʼ����ϢӦ����������
	explicit WindowInfo(HWND hWnd, const CString& title,const RECT& lRect, bool active);				//���о�����Ϣ�Ĺ��캯��
	explicit WindowInfo(HWND hWnd, const CString& title, const CTime& tCreate, bool active);		//���ڴ���
	explicit WindowInfo(HWND hWnd, const CString& title, const RECT& lRect, bool active, const CString& className);
	void NewRect(const RECT& rect);																//�µ�λ�úʹ�С
	void OnDestory(const CTime& tDestory);													//��������
	void OnWindowMove(const CRect& rect, int deskArea);												//�����ƶ�
	void OnMin(int desktopArea);																				//��С��
	void OnMax(int desktopArea, const RECT& deskArea);																				//���
	void OnRestore(int desktopArea, HWND hWnd);																			//�ָ���С
	BOOL Contacted();																								//�Ƿ���ô��ڷ���������
	const CString getInitTitle() const;																								//�õ���ʼ������
	const size_t GetInDegree() const;																					//�õ����
	const size_t GetOutDegree() const;																					//�õ�����
	const size_t GetAttRate() const;																						//���ع�ע��

	void NewAtt(const CTime &attTime);																				//���»�ȡ����
	void RefreshAttRate();																							//����
	const float GetImportance() const;																					//������Ҫ��
	void CalculateAttRate(const CTime& time,int desktopArea);												//�����ע��
	void CalculateImportance();																					//������Ҫ��
	const CRect GetCurrentLocal() const;																						//�õ���ǰλ��

#ifdef DB_LOG
	void OnKeyType(const CTime& watchTime, CAbstractDBManager *db);												//���̼����¼�
	void OnMouseClick(const CTime& watchTime, CAbstractDBManager *db);											//������¼�
	void WindowActive(const CTime& time, CAbstractDBManager *db);														//���ڼ���
	void WindowActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db);
	void WindowUnActive(const CTime& time, int desktopArea, CAbstractDBManager *db);						//����ʧȥ����
	void WindowUnActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db);				//����ʧȥ�����¼��ڼ���̺����ƽ������
	void OnWindowDestory(const CTime& time, CAbstractDBManager *db);																//��������
	void OnWindowDestory(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db);
#else
	void WindowActive(const CTime& time);																				//���ڼ�������ݿ�汾
	void OnKeyType(const CTime& watchTime);																			//���̼����¼�,�����ݿ�汾
	void OnMouseClick(const CTime& watchTime);																		//������¼�,�����ݿ�汾
	void WindowUnActive(const CTime& time, int desktopArea);																//����ʧȥ����
	void OnWindowDestory(const CTime& time);																//��������
#endif

	const CString toString();																			//��ӡ��Ϣ
	bool WriteLog(CFile &file);																//д��־�ļ�
	void Destroy();																				//��������

private:
	void Init();																						//��ʼ�����ֱ���


private:
	HWND m_hWnd;														//���ھ��
	//CString	m_sIniTitle;												//��ʼ��������
	CTime m_tCreate;														//����ʱ��
	//CTime *m_tDestory;												//����ʱ�� �ܶ���
	CString m_csclassName;													//����

	//���ڴ��л������Ϣ////////////////////////////////////////////////////
#ifdef DATA_CACHE
	list<TitleNode> *title_list;											//�����б�
	list<LocalNode> *local_list;										//λ���б�
	list<KeyTypeNode> *keyType_list;								//��������б�
	list<MouseClickNode> *mouseClic_list;						//������б�
	//list<ActiveNode> *active_list;									//�����б�
#endif // DATA_CACHE
	//////////////////////////////////////////////////////////////////////////

	//�����ע�ȵ�����//////////////////////////////////////////////////////
	size_t m_iAttRate;														//���ڹ�ע�ȣ�ÿ���ӽ���һ������
	size_t m_iMouseCount;												//��ʱ����¼�����
	size_t m_iKeyCount;													//��ʱ�����¼�����
	CTime m_tLastLocalTime;											//ǰһ�������ƶ�ʱ�� 
	float m_fImportance;												//������Ҫ�ԣ���Χ[0,1]
	float m_fImpDecSpeed;												//��Ҫ��˥���ٶ�
	//////////////////////////////////////////////////////////////////////////

	//����ʱ��ͳ�����//////////////////////////////////////////////////////
	CTime m_tLastAttTime;												//ǰһ����ȡ�����ʱ��
	size_t m_iTotalTime;													//������ʱ��
	size_t m_iTotalAttTime;												//�ܽ���ʱ��
	double m_dFocusBalance;											//����ʱ���
	int m_iLastAttTime;													//��һ������ʱ��
	int m_iLastNoAttTime;												//��һ��ʧȥ����ʱ��
	//////////////////////////////////////////////////////////////////////////

	//�����ͳ��/////////////////////////////////////////////////////////////
	size_t m_iInDegree;													//���
	size_t m_iOutDegree;												//����
	//////////////////////////////////////////////////////////////////////////

public:
	unsigned int wnd_id;													//���ݿ��еĴ���ID
	// ����IsWindow����hwnd_info�ṹ
	BOOL ExamMap(void);
};


