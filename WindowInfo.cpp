#include "StdAfx.h"
#include "WindowInfo.h"
#include <math.h>

#define  SHOW FALSE

void WindowInfo::Init()
{
	//m_tDestory = new CTime();
#ifdef DATA_CACHE
	title_list = new list<TitleNode>;
	local_list = new list<LocalNode>;								//λ���б�
	keyType_list = new list<KeyTypeNode>;					//��������б�
	mouseClic_list = new list<MouseClickNode>;			//������б�
//	active_list = new list<ActiveNode>;							//�����б� 
#endif // DATA_CACHE

	m_tLastAttTime = CTime::GetCurrentTime();												
	m_iTotalTime = 0;													
	m_iTotalAttTime = 0;		
	m_dFocusBalance = 0.0;
	m_iLastAttTime = 0;
	m_iLastNoAttTime = 0;

	m_tLastLocalTime = m_tLastAttTime;
	m_iMouseCount = 0;												
	m_iKeyCount = 0;	

	m_iAttRate = 0;
	m_fImportance = 0.0f;

	m_iInDegree = 0;
	m_iOutDegree = 0;
}

//��ʾ���ٶ���
void WindowInfo::Destroy()
{
#ifdef DATA_CACHE
	local_list->clear();
	keyType_list->clear();
	mouseClic_list->clear();
//	active_list->clear();
	title_list->clear();
	delete local_list;
	delete keyType_list;
	delete mouseClic_list;
//	delete active_list;
	delete title_list;
#endif // DATA_CACHE

	//if(m_tDestory != NULL)
	//	delete m_tDestory;
}

WindowInfo::WindowInfo(HWND hWnd, const CString& title, const CTime& tCreate, bool active)
	:m_hWnd(hWnd),m_tCreate(tCreate)
{
	Init();
}

WindowInfo::WindowInfo(void)// : m_bContact(false)
{
	Init();
	//map<CTime, CRect>::
}

//Ԥ���ڶ������ٵ�ʱ��д��־�ļ�
WindowInfo::~WindowInfo(void)
{

}

//���д���ʱ��Ĺ��캯��
WindowInfo::WindowInfo(const ATL::CTime& tCreate)
:m_tCreate(tCreate)
{
	Init();
	this->m_tCreate = tCreate;
}

//���д���ʱ��
WindowInfo::WindowInfo(const ATL::CTime& tCreate, const RECT& lRect)
:m_tCreate(tCreate)
{
	Init();
	//this->m_tCreate = tCreate;
	//this->local_list.push_back(lRect);
}

//Ӧ������ʵ�õĹ��캯��
WindowInfo::WindowInfo(HWND hWnd, const CString& title, const RECT& lRect, bool active)// : m_bContact(false)
:m_tCreate(CTime::GetCurrentTime()),m_hWnd(hWnd)
{
	Init();
	//m_tCreate = CTime::GetCurrentTime();
	//this->m_hWnd = hWnd;
	//this->m_sIniTitle = title;
#ifdef DATA_CACHE
	this->title_list->push_back(TitleNode(m_tCreate, title));
	this->local_list->push_back(LocalNode(m_tCreate,lRect));
//	this->active_list->push_back(ActiveNode(m_tCreate, active));
#endif
	
	//if(SHOW)
	//{
	//	CString msg;
	//	msg.Format("New Window %s %d", title_list->begin()->title, m_tCreate.GetSecond());
	//	AfxMessageBox(msg);
	//}
}

//�������Ĺ��캯��
WindowInfo::WindowInfo(HWND hWnd, const CString& title, const RECT& lRect, bool active, const CString& className)
:m_tCreate(CTime::GetCurrentTime()),m_hWnd(hWnd),m_csclassName(className)
{
	Init();
	//m_tCreate = CTime::GetCurrentTime();
	//this->m_hWnd = hWnd;
	//this->m_sIniTitle = title;
	//this->className = className;

#ifdef DATA_CACHE
	this->title_list->push_back(TitleNode(m_tCreate, title));
	this->local_list->push_back(LocalNode(m_tCreate,lRect));
//	this->active_list->push_back(ActiveNode(m_tCreate, active));
#endif
}

//�Ӿ���õ���ʼ����Ϣ
WindowInfo::WindowInfo(HWND hWnd)
:m_hWnd(hWnd),m_tCreate(CTime::GetCurrentTime())
{
	Init();
	//this->m_hWnd = hWnd;									//���þ��
	//m_tCreate = CTime::GetCurrentTime();				//���ô���ʱ��
	TCHAR title[TITLE_SIZE];
	//�õ�����
	SendMessage(hWnd, WM_GETTEXT, TITLE_SIZE, (LPARAM)title);
	//m_sIniTitle.SetString(title);									//���ñ���
	
	//�õ�����λ��
	RECT rect;
	GetWindowRect(hWnd, &rect);
#ifdef DATA_CACHE
	local_list->push_back(LocalNode(m_tCreate,CRect(rect)));								//����λ��
//	active_list->push_back(ActiveNode(m_tCreate, true));
	title_list->push_back(TitleNode(m_tCreate, CString(title)));
#endif

}

//�µ�λ��
void WindowInfo::NewRect(const RECT& rect)
{
	CTime time = CTime::GetCurrentTime();
	this->local_list->push_back(LocalNode(time,rect));
}

#ifdef DB_LOG
//���������¼�
void WindowInfo::OnKeyType(const CTime& watchTime, CAbstractDBManager *db)
{
	//��ǰ�����¼�����++
	m_iKeyCount++;

	//������̼����б�Ϊ�գ��ͽ����µ��б�
	if(keyType_list->empty())
	{
		keyType_list->push_back(KeyTypeNode(watchTime, 1));
		return;

		//AfxMessageBox("�����µļ���ڵ�");
	}

	//�Ѿ��нڵ����,�õ����һ���ڵ�
	list<KeyTypeNode>::iterator it = --keyType_list->end();
	//��������ڵ�ļ��ʱ����ǵ�ǰʱ�䣬�����Ӽ�����
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("ԭ����ڵ�����");
	}
	//��������ڣ��Ͳ����µĽڵ�
	else
	{
		//д���ݿ�
		db->NewKeyType(wnd_id, (unsigned int)(it->count), it->time);
		//�����µĽ��
		keyType_list->push_back(KeyTypeNode(watchTime, 1));	
		//AfxMessageBox("�����µļ���ڵ�");
	}
}
#else
//���������¼�
void WindowInfo::OnKeyType(const CTime& watchTime)
{
	//��ǰ�����¼�����++
	m_iKeyCount++;

	//������̼����б�Ϊ�գ��ͽ����µ��б�
	if(keyType_list->empty())
	{
		keyType_list->push_back(KeyTypeNode(watchTime, 1));
		return;

		//AfxMessageBox("�����µļ���ڵ�");
	}

	//�Ѿ��нڵ����,�õ����һ���ڵ�
	list<KeyTypeNode>::iterator it = --keyType_list->end();
	//��������ڵ�ļ��ʱ����ǵ�ǰʱ�䣬�����Ӽ�����
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("ԭ����ڵ�����");
	}
	//��������ڣ��Ͳ����µĽڵ�
	else
	{
		//�����µĽ��
		keyType_list->push_back(KeyTypeNode(watchTime, 1));	
		//AfxMessageBox("�����µļ���ڵ�");
	}
}

#endif // DB_LOG

#ifdef DB_LOG
//���������¼�
void WindowInfo::OnMouseClick(const CTime& watchTime, CAbstractDBManager *db)
{
	//��ǰ����¼�����++
	m_iMouseCount++;

	//������̼����б�Ϊ�գ��ͽ����µ��б�
	if(mouseClic_list->empty())
	{
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		return;

		//AfxMessageBox("�����µ����ڵ�");
	}

	//�Ѿ��нڵ����,�õ����һ���ڵ�
	list<MouseClickNode>::iterator it = --mouseClic_list->end();
	//��������ڵ�ļ��ʱ����ǵ�ǰʱ�䣬�����Ӽ�����
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("ԭ���ڵ�����");
	}
	//��������ڣ��Ͳ����µĽڵ�
	else
	{
		//д���ݿ�
		db->NewMouseClick(wnd_id, (unsigned int)(it->count), it->time);
		//�����½ڵ�
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		//AfxMessageBox("�����µ����ڵ�");
	}
}

#else
//���������¼�
void WindowInfo::OnMouseClick(const CTime& watchTime)
{
	//��ǰ����¼�����++
	m_iMouseCount++;

	//������̼����б�Ϊ�գ��ͽ����µ��б�
	if(mouseClic_list->empty())
	{
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		return;

		//AfxMessageBox("�����µ����ڵ�");
	}

	//�Ѿ��нڵ����,�õ����һ���ڵ�
	list<MouseClickNode>::iterator it = --mouseClic_list->end();
	//��������ڵ�ļ��ʱ����ǵ�ǰʱ�䣬�����Ӽ�����
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("ԭ���ڵ�����");
	}
	//��������ڣ��Ͳ����µĽڵ�
	else
	{
		//�����½ڵ�
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		//AfxMessageBox("�����µ����ڵ�");
	}
}
#endif // DB_LOG

#ifdef DB_LOG
//���ڼ���
void WindowInfo::WindowActive(const CTime& time, CAbstractDBManager *db)
{
	//���¿�ʼ��¼��ע�Ȳ���
	NewAtt(time);
	//CTime time = CTime::GetCurrentTime();
//	active_list->push_back(ActiveNode(time, false));

	//��ȡʧȥ�����ʱ��
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iTotalTime += timeCount;
	//����ʧȥ����ʱ��
	m_iLastNoAttTime = timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//�жϱ������ǰ��һ����
	TCHAR buffer[TITLE_SIZE];
	//�õ�����
	SendMessage(m_hWnd, WM_GETTEXT,TITLE_SIZE, (LPARAM)buffer);
	CString title(buffer);
	list<TitleNode>::const_iterator title_it = --title_list->end();
	//�������ȣ�������µĽڵ�
	if(title.Compare(title_it->title) != 0)
	{
		title_list->push_back(TitleNode(time, title));
		//д���ݿ�
		db->WindowAddTitle(wnd_id, title, time);
	}
	//��¼�������ʱ��
	db->WindowAttTime(wnd_id, m_iLastNoAttTime, m_iLastAttTime, false, m_dFocusBalance, time);
}

#else
//���ڼ���,�����ݿ�汾
void WindowInfo::WindowActive(const CTime& time)
{
	//���¿�ʼ��¼��ע�Ȳ���
	NewAtt(time);
	//CTime time = CTime::GetCurrentTime();
	active_list->push_back(ActiveNode(time, false));

	//��ȡʧȥ�����ʱ��
	CTimeSpan timeElipse = m_tLastAttTime - time;
	int timeCount = timeElipse.GetTotalSeconds();
	m_iTotalTime += timeCount;
	//����ʧȥ����ʱ��
	m_iLastNoAttTime = timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//�жϱ������ǰ��һ����
	TCHAR buffer[TITLE_SIZE];
	//�õ�����
	SendMessage(m_hWnd, WM_GETTEXT,TITLE_SIZE, (LPARAM)buffer);
	CString title(buffer);
	list<TitleNode>::const_iterator title_it = --title_list->end();
	//�������ȣ�������µĽڵ�
	if(title.Compare(title_it->title) != 0)
	{
		title_list->push_back(TitleNode(time, title));
	}
}
#endif // DB_LOG

#ifdef DB_LOG
//����ʧȥ����
void WindowInfo::WindowUnActive(const CTime& time, int desktopArea, CAbstractDBManager *db)
{
//	this->active_list->push_back(ActiveNode(time, false));

	//���㽹���ȡʱ��
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//д��¼
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance, time);

	//�Թ�ע�Ƚ���һ������
	//������̺�����¼�����
	m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
//	ASSERT(seconds <= 300);
	//���㴰�ڴ�С�������С����
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//���㴰�ڽ���ʱ��
	m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);

	m_tLastLocalTime = time;
}

void WindowInfo::WindowUnActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db)
{
	//this->active_list->push_back(ActiveNode(time, false));

	//��������
	m_iOutDegree++;

	//������������
	float fAvgKey = 0.0f;
	float fAvgMouse = 0.0f;

	//���㽹���ȡʱ��
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;

	//û��1��ͽ���Ϊһ��
	fAvgKey = static_cast<float>(keyCount) / 2;
	fAvgMouse = static_cast<float>(mouseCount) / 2;
	if(m_iTotalTime != 0)
	{
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
		fAvgKey = static_cast<float>(keyCount) / m_iLastAttTime;
		fAvgMouse = static_cast<float>(mouseCount) / m_iLastAttTime;
	}
	m_tLastAttTime = time;

	//д��¼
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance,fAvgKey,fAvgMouse, time);

	//�Թ�ע�Ƚ���һ������
	//������̺�����¼�����
	//m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	//CTimeSpan elipseTime = time - m_tLastLocalTime;
	//LONGLONG seconds = elipseTime.GetTotalSeconds();
	//ASSERT(seconds <= 300);
	////���㴰�ڴ�С�������С����
	//list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	//int width = rit->rect.right - rit->rect.left;
	//int height = rit->rect.bottom - rit->rect.top;
	//int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	//float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	////���㴰�ڽ���ʱ��
	//m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);

	m_tLastLocalTime = time;
}

void WindowInfo::WindowActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db)
{
	//�������
	m_iInDegree++;
	//ƽ����������
	float fAvgKey = 0.0f;
	float fAvgMouse = 0.0f;

	//��ȡʧȥ�����ʱ��
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iTotalTime += timeCount;
	//����ʧȥ����ʱ��
	m_iLastNoAttTime = timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//û��1��ͽ���Ϊһ��
	fAvgKey = static_cast<float>(keyCount) / 2;
	fAvgMouse = static_cast<float>(mouseCount) / 2;
	if(m_iTotalTime != 0)
	{
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
		fAvgKey = static_cast<float>(keyCount) / m_iLastAttTime;
		fAvgMouse = static_cast<float>(mouseCount) / m_iLastAttTime;
	}
	m_tLastAttTime = time;

	//д��¼
	db->WindowAttTime(wnd_id, m_iLastNoAttTime, m_iLastAttTime, false, m_dFocusBalance,fAvgKey,fAvgMouse, time);
}
#else
//����ʧȥ����
void WindowInfo::WindowUnActive(const CTime& time, int desktopArea)
{
	this->active_list->push_back(ActiveNode(time, false));

	//���㽹���ȡʱ��
	CTimeSpan timeElipse = m_tLastAttTime - time;
	int timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;


	//�Թ�ע�Ƚ���һ������
	//������̺�����¼�����
	m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	ASSERT(seconds <= 300);
	//���㴰�ڴ�С�������С����
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	ASSERT(windowBalance <= 1.05f);
	//���㴰�ڽ���ʱ��
	m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);

	m_tLastLocalTime = time;
}
#endif

//�����ע��
void WindowInfo::CalculateAttRate(const CTime& time,int desktopArea)
{
	//CTime time = CTime::GetCurrentTime();
	//������̺�����¼�����
	m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	if(local_list->size() > 0)
	{
		CTimeSpan elipseTime = time - m_tLastLocalTime;
		LONGLONG seconds = elipseTime.GetTotalSeconds();
		//���㴰�ڴ�С�������С����
		list<LocalNode>::reverse_iterator rit = local_list->rbegin();
		int width = rit->rect.right - rit->rect.left;
		int height = rit->rect.bottom - rit->rect.top;
		int windowArea = width * height;
		//ASSERT(windowArea >= 0);
		float windowBalance = static_cast<float>(windowArea) / desktopArea;
//		ASSERT(seconds <= 300);
		//ASSERT(windowBalance <= 1.05f);
		//���㴰�ڽ���ʱ��
		m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);
		//���ڴ��ڵ������ڿ�����1�����ڣ��ǽ��������ע��Ϊ0�����Ժ���ļ����������Ӱ�죬����Ҫ����
		if(m_iAttRate <= 0)
			m_iAttRate = 1;

		m_tLastLocalTime = time;
	}
}

//������Ҫ��
void WindowInfo::CalculateImportance()
{
	//��ƫ�Ƶ�Sigmoid������������Ҫ��
	m_fImportance = 1.0f / (1.0f + exp(-(static_cast<int>(m_iAttRate) - 2000.0f) / 600.0f));
}

//�õ���ǰλ��
const CRect WindowInfo::GetCurrentLocal() const
{
	list<LocalNode>::const_reverse_iterator it = local_list->rbegin();
	return it->rect;
}

//�����ƶ�
void WindowInfo::OnWindowMove(const CRect& rect, int desktopArea)
{
	CTime time = CTime::GetCurrentTime();
	//����ǰһ������λ�ô�С�Թ�ע�ȵ�Ӱ��
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	//���㴰�ڴ�С�������С����
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//���㴰�ڽ���ʱ��
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//���õ�ǰʱ��Ϊ�µ�λ��ʱ��
	m_tLastLocalTime = time;

	local_list->push_back(LocalNode(time, rect));
}

#ifdef DB_LOG
//��������
void WindowInfo::OnWindowDestory(const CTime& time, CAbstractDBManager *db)
{
	//���㽹���ȡʱ��
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;

	//д��¼
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance, time);
//	active_list->push_back(ActiveNode(time, false));
}

void WindowInfo::OnWindowDestory(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db)
{
	//���㽹���ȡʱ��
	float fAvgKey = 0.0f;
	float fAvgMouse = 0.0f;

	//���㽹���ȡʱ��
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;

	//û��1��ͽ���Ϊһ��
	fAvgKey = static_cast<float>(keyCount) / 2;
	fAvgMouse = static_cast<float>(mouseCount) / 2;
	if(m_iTotalTime != 0)
	{
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
		fAvgKey = static_cast<float>(keyCount) / m_iLastAttTime;
		fAvgMouse = static_cast<float>(mouseCount) / m_iLastAttTime;
	}
	m_tLastAttTime = time;

	//д��¼
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance,fAvgKey,fAvgMouse, time);

}
#else
//��������
void WindowInfo::OnWindowDestory(const CTime& time)
{
	
	active_list->push_back(ActiveNode(time, false));
}
#endif // DB_LOG

//��С��
void WindowInfo::OnMin(int desktopArea)
{
	CTime time = CTime::GetCurrentTime();
	//����ǰһ������λ�ô�С�Թ�ע�ȵ�Ӱ��
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
//	ASSERT(seconds <= 300);
	//���㴰�ڴ�С�������С����
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//���㴰�ڽ���ʱ��
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//���õ�ǰʱ��Ϊ�µ�λ��ʱ��
	m_tLastLocalTime = time;

	//local_list->push_back(LocalNode(time, rect));
	local_list->push_back(LocalNode(CTime::GetCurrentTime(), CRect(0, 0, 0, 0)));
}

//���
void WindowInfo::OnMax(int desktopArea, const RECT& deskArea)
{
	CTime time = CTime::GetCurrentTime();
	//����ǰһ������λ�ô�С�Թ�ע�ȵ�Ӱ��
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	//���㴰�ڴ�С�������С����
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//���㴰�ڽ���ʱ��
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//���õ�ǰʱ��Ϊ�µ�λ��ʱ��
	m_tLastLocalTime = time;

	local_list->push_back(LocalNode(CTime::GetCurrentTime(), static_cast<CRect>(deskArea)));
}

//�ָ���С
void WindowInfo::OnRestore(int desktopArea, HWND hWnd)
{
	CTime time = CTime::GetCurrentTime();
	//����ǰһ������λ�ô�С�Թ�ע�ȵ�Ӱ��
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	//���㴰�ڴ�С�������С����
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//���㴰�ڽ���ʱ��
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//���õ�ǰʱ��Ϊ�µ�λ��ʱ��
	m_tLastLocalTime = time;

	RECT rect;
	::GetWindowRect(hWnd, &rect);	
	local_list->push_back(LocalNode(CTime::GetCurrentTime(), rect));
}

//�����Ƿ���ô��ڷ���������
BOOL WindowInfo::Contacted()
{
	if((!mouseClic_list->empty() || !keyType_list->empty()) && m_csclassName != "Shell_TrayWnd" && title_list->front().title != "")
		return true;
	else
		return false;
}

//�õ���ʼ������
const CString WindowInfo::getInitTitle() const
{
	if(title_list->size() >0)
		return ((TitleNode)*(title_list->begin())).title;
	else
		return CString("N/A");
}

//���»�ȡ���㣬��ʼ�����������ע�ȵı���
void WindowInfo::NewAtt(const CTime &attTime)
{
	m_tLastLocalTime = attTime;
	m_iMouseCount = 0;												
	m_iKeyCount = 0;													
}

//���򻯹�ע��
void WindowInfo::RefreshAttRate()
{
	m_iAttRate = 0;
	m_fImportance = 0.0f;
}

//�������
const size_t WindowInfo::GetInDegree() const
{
	return m_iInDegree;
}

//���س���
const size_t WindowInfo::GetOutDegree() const
{
	return m_iOutDegree;
}

//���ع�ע��
const size_t WindowInfo::GetAttRate() const
{
	return m_iAttRate;
}

//������Ҫ��
const float WindowInfo::GetImportance() const
{
	return m_fImportance;
}

//�ú�����Χһ���޴��CString������ĳЩ����¿��ܵ��³������
//Ԥ�ƽ��ú�����Ϊֱ��д��־�ļ�
const CString WindowInfo::toString()
{
	CString msg;
	msg.Format("Window:\t%s\r\n---------------------------------------------\r\n", title_list->begin()->title);													//����
	msg.Append("Create: " + m_tCreate.Format("%H:%M:%S") + "\r\n");					//����ʱ��
	//if(m_tDestory != NULL)
	//	msg.Append("Destory: " + (*m_tDestory).Format("%H:%M:%S") + "\r\n");				//����ʱ��
	msg.Append("Titles:\r\n");																				//����
	if(!title_list->empty())
	{
		list<TitleNode>::const_iterator title_it = title_list->begin();
		while(title_it != title_list->end())
		{
			CString tMsg = (title_it->time).Format("%H:%M:%S");
			CString title = title_it->title;
			msg.Append(tMsg + "\t" + title +  "\r\n");

			++title_it;
		}
	}
	
	msg.Append("Local and Size:\r\n");																		//λ����Ϣ
//	map<CTime, CRect>::const_iterator local_it = local_list->begin();
	if(!local_list->empty())
	{
		list<LocalNode>::const_iterator local_it = local_list->begin();
		while(local_it != local_list->end())
		{
			CString tStr = local_it->time.Format("%H:%M:%S");
			CRect rect = local_it->rect;
			CString rStr;
			rStr.Format("\t[%d-%d,%d-%d]", rect.left, rect.right, rect.top, rect.bottom);
			if(local_it->type == LocalNode.MOVE)
			{
				CRect rect = local_it->rect;
				CString rStr;
				rStr.Format("\t[%d-%d,%d-%d]", rect.left, rect.right, rect.top, rect.bottom);
				msg.Append(tStr + rStr + "\r\n");
			}
			else if(local_it->type == LocalNode.MAX)
			{
				msg.Append(tStr + "\tMAX\r\n");
			}
			else if(local_it->type == LocalNode.MIN)
			{
				msg.Append(tStr + "\tMIN\r\n");
			}
			else if(local_it->type == LocalNode.RESTORE)
			{
				msg.Append(tStr + "\tRESTORE\r\n");
			}

			++local_it;
		}
	}

	//msg.Append("Active:\r\n");																				//���Ϣ
	//if(!active_list->empty())
	//{
	//	list<ActiveNode>::const_iterator active_it = active_list->begin();
	//	while(active_it != active_list->end())
	//	{
	//		CString tMsg = (active_it->time).Format("%H:%M:%S");
	//		bool flag = active_it->flag;
	//		if(flag)
	//			msg.Append(tMsg + "\t Active\r\n");
	//		else
	//			msg.Append(tMsg + "\t UnActive\r\n");

	//		++active_it;
	//	}
	//}

	msg.Append("KeyTyped:\r\n");																				//���Ϣ
	if(!keyType_list->empty())
	{
		list<KeyTypeNode>::const_iterator key_it = keyType_list->begin();
		while(key_it != keyType_list->end())
		{
			int count = key_it->count;
			CString tMsg = (key_it->time).Format("%H:%M:%S");
			tMsg.Format("%s \t%d", tMsg, count);
			msg.Append(tMsg + "\r\n");

			++key_it;
		}
	}

	msg.Append("MouseClick:\r\n");																				//���Ϣ
	if(!mouseClic_list->empty())
	{
		list<MouseClickNode>::const_iterator mouse_it = mouseClic_list->begin();
		while(mouse_it != mouseClic_list->end())
		{
			int count = mouse_it->count; 
			//CString nullForma ="";
			CString tMsg = (mouse_it->time).Format("%H:%M:%S");
			//if (tMsg!=nullForma)
			//{
				tMsg.Format("%s \t%d", tMsg, count);
				msg.Append(tMsg + "\r\n");
			//}
			
			++mouse_it;
		}
	}

	msg.Append("\r\n");
//	 m_tCreate.
	return msg;
}

//д��־�ļ�
bool WindowInfo::WriteLog(CFile& file)
{
	try
	{
		CString msg;
		msg.Format("Window:\t%s\r\n---------------------------------------------\r\n", title_list->begin()->title);													//����
		msg.Append("Create: ");
		msg += m_tCreate.Format("%Y-%m-%d %H:%M:%S");
		//msg += m_tCreate.Format("%Y:%m:%d");
		msg += "\r\n";					//����ʱ��
		//if(m_tDestory != NULL)
		//	msg.Append("Destory: " + (*m_tDestory).Format("%H:%M:%S") + "\r\n");				//����ʱ��

		msg.Append("Local and Size:\r\n");																		//λ����Ϣ
		//	map<CTime, CRect>::const_iterator local_it = local_list->begin();
		list<LocalNode>::const_iterator local_it = local_list->begin();
		while(local_it != local_list->end())
		{
			CString tStr = local_it->time.Format("%H:%M:%S");
			if(local_it->type == LocalNode.MOVE)
			{
				CRect rect = local_it->rect;
				CString rStr;
				rStr.Format("\t[%d-%d,%d-%d]", rect.left, rect.right, rect.top, rect.bottom);
				//msg.Append(tStr+ rStr + "\r\n"););
				msg += tStr;
				msg += rStr;
				msg += "\r\n";
				
			}
			else if(local_it->type == LocalNode.MAX)
			{
				msg.Append(tStr + "\tMAX\r\n");
			}
			else if(local_it->type == LocalNode.MIN)
			{
				msg.Append(tStr + "\tMIN\r\n");
			}
			else if(local_it->type == LocalNode.RESTORE)
			{
				msg.Append(tStr + "\tRESTORE\r\n");
			}

			++local_it;
		}
		
		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//ˢ�£��е�д��
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//ˢ�£��е�д��
		msg.Empty();									//���

		msg.Append("Titles:\r\n");																				//����
		if(!title_list->empty())
		{
			list<TitleNode>::const_iterator title_it = title_list->begin();
			while(title_it != title_list->end())
			{
				CString tMsg = (title_it->time).Format("%H:%M:%S");
				CString title = title_it->title;
				//msg.Append(tMsg + "\t" + title +  "\r\n");
				msg += tMsg;
				msg += "\t";
				msg += title;
				msg += "\r\n";

				++title_it;
			}
		}

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//ˢ�£��е�д��
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//ˢ�£��е�д��
		msg.Empty();									//���

		//msg.Append("Active:\r\n");																				//���Ϣ
		//list<ActiveNode>::const_iterator active_it = active_list->begin();
		//while(active_it != active_list->end())
		//{
		//	CString tMsg = (active_it->time).Format("%H:%M:%S");
		//	bool flag = active_it->flag;
		//	if(flag)
		//	{
		//		//msg.Append(tMsg + "\t Active\r\n");
		//		msg += tMsg;
		//		msg += "\t Active\r\n";
		//	}
		//	else
		//	{
		//		//msg.Append(tMsg + "\t UnActive\r\n");
		//		msg += tMsg;
		//		msg += "\t UnActive\r\n";
		//	}

		//	++active_it;
		//}

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//ˢ�£��е�д��
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//ˢ�£��е�д��
		msg.Empty();									//���

		msg.Append("KeyTyped:\r\n");																				//���Ϣ
		list<KeyTypeNode>::const_iterator key_it = keyType_list->begin();
		while(key_it != keyType_list->end())
		{
			int count = key_it->count;
			CString tMsg = (key_it->time).Format("%H:%M:%S");
			tMsg.Format("%s \t%d", tMsg, count);
			//msg.Append(tMsg + "\r\n");
			msg += tMsg;
			msg += "\r\n";

			++key_it;
		}

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//ˢ�£��е�д��
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//ˢ�£��е�д��
		msg.Empty();									//���

		msg.Append("MouseClick:\r\n");																				//���Ϣ
		list<MouseClickNode>::const_iterator mouse_it = mouseClic_list->begin();
		while(mouse_it != mouseClic_list->end())
		{
			int count = mouse_it->count;
			CString tMsg = (mouse_it->time).Format("%H:%M:%S");
			tMsg.Format("%s \t%d", tMsg, count);
			//msg.Append(tMsg + "\r\n");
			msg += tMsg;
			msg += "\r\n";

			++mouse_it;
		}

		msg.Append("\r\n");

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//ˢ�£��е�д��
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//ˢ�£��е�д��
		msg.Empty();									//���
		//	 m_tCreate.
		return true;
	}
	catch (CMemoryException* e)
	{
		//AfxMessageBox("�ڴ����");
		return false;
	}
	catch (CFileException* e)
	{
		CString str;
		str.Format("д��ʧ�ܵ�ԭ����:%d",e->m_cause);
		//AfxMessageBox("str");
//		(*file).Abort();
		file.Abort();
		e->Delete();
		return false;
	}
	catch (CException* e)
	{
		//AfxMessageBox("δ֪����");
		return false;
	}
}

// ����IsWindow����hwnd_info�ṹ
BOOL WindowInfo::ExamMap(void)
{
	return 0;
}
