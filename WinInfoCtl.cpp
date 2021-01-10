#include "StdAfx.h"
#include "WinInfoCtl.h"
#include "figtree.h"
#include "ScreenHelper.h"
#include "AnimatinoHelper.h"
#include <math.h>
#include <algorithm>

//CEmbeddedMySql  WinInfoCtl::m_embMySql = CEmbeddedMySql();												//Ƕ��ʽmysql
//CAbstractDBManager *WinInfoCtl::m_mysql = &WinInfoCtl::m_embMySql;

WinInfoCtl::WinInfoCtl(void)
:m_hCurrentActiveWnd(NULL),m_tWatchTime(CTime::GetCurrentTime())
{
	//m_hCurrentActiveWnd = NULL;
	//m_tWatchTime = CTime::GetCurrentTime();

	//�õ���¼����ʱ��
	rec_time = m_tWatchTime;
	//����������Ϊ0
	tickCount = 0;
	//���ڴ���������ʼ
	wnd_create_count_list.push_back(WndCreateNode(rec_time, 0));

	//��ȡ�������
	RECT rWorkArea;
	BOOL bResult = SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rWorkArea, 0);
	int width = rWorkArea.right - rWorkArea.left;
	int height = rWorkArea.bottom - rWorkArea.top;
	m_iDeskArea = width * height;
	m_cDeskRect = rWorkArea;

	m_lKeyCount = 0;
	m_lMouseCount = 0;

	//m_mysql = &m_remoteMySql;
	m_mysql = &m_embMySql;

	//��ǰ��ʾ�ľ��ര�ڼ��ϱ��Ϊ0
	m_iCurClique = 0;


#ifdef DB_LOG
	//�������ݿ�
	ConnectDB();
#endif
}

#ifdef DB_LOG
//�������ݿ�
BOOL WinInfoCtl::ConnectDB()
{
	//�õ��������
	TCHAR buffer[256] = TEXT("");
	int cnf = 0;
	DWORD dwSize = sizeof(buffer);
	//����ȡ����ȡ�����м������ʾ��
	for (cnf = 0; cnf < ComputerNameMax; cnf++)
	{
		if (!GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
		{
			continue;
			//_tprintf(TEXT("GetComputerNameEx failed (%d)\n"), GetLastError());
			//return;
		}
		else
		{
			computer = buffer;
			//ZeroMemory(buffer, dwSize);
			break;
		}		
	}

	if(!m_mysql->Connect())
	//if(!m_embMySql.Connect())
	{
		m_bDBConnect = FALSE;
		return FALSE;
		//AfxMessageBox("���ݿ�����ʧ��");
		//theApp.LostDBConnect();
	}
	else
	{
		m_bDBConnect = TRUE;
		CString sVersion = GetVersion();
		if(sVersion != "NONE")
		{
			rec_id = m_mysql->NewRecord(computer, rec_time, sVersion);
		}
		else
		{
			rec_id = m_mysql->NewRecord(computer, rec_time);
		}	
		return TRUE;
	}
}
#endif

WinInfoCtl::~WinInfoCtl(void)
{
	//��������ʾ����WindowInfo�ṹ
	map<HWND, WindowInfo>::const_iterator map_it = hwnd_info.begin();
	while(map_it != hwnd_info.end())
	{
		((WindowInfo)map_it->second).Destroy();
		++map_it;
	}
	hwnd_info.clear();
	history_list.clear();
}

//������һ���´���
void WinInfoCtl::NewWindow(HWND hWnd)
{							//���þ��
	CTime createTime = CTime::GetCurrentTime();				//���ô���ʱ��
	TCHAR titlebuffer[TITLE_SIZE];
	//�õ�����
	SendMessage(hWnd, WM_GETTEXT, TITLE_SIZE, (LPARAM)titlebuffer);
	CString title(titlebuffer);
	
	//�õ�����λ��
	RECT rect;
	GetWindowRect(hWnd, &rect);
	//��������һ�����������´���

	//�õ�����
	TCHAR classNameBuffer[TITLE_SIZE] = "";
	
	if(GetClassName(hWnd, classNameBuffer, TITLE_SIZE)==0)
		NewWindow(hWnd, title, "N/A", rect, true);
	else
		NewWindow(hWnd, title, classNameBuffer, rect, true);
	//CString className(classNameBuffer);
	
}

//������һ���´���
void WinInfoCtl::NewWindow(HWND hWnd, const CString& title, const CString& className, const RECT& rect, bool active)
{
	//����map�в����Ǹ����ھ��
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//�ҵ����Ǹ����,���˳������贴��
	if(it != hwnd_info.end())
	{
		return;
		//(*wif).WindowActive();
	}
	//û�ҵ�,Ӧ��Ϊ�ô��ڴ���ʵ��
	else
	{
		//��ȡ��ǰʱ��
		CTime creat_time = CTime::GetCurrentTime();
		//��ǰʱ��δ��ڼ���++
		//list<WndCreateNode>::reverse_iterator rit = wnd_create_count_list.rbegin();
		////��������ڵ�ļ��ʱ����ǵ�ǰʱ�䣬�����Ӽ�����
		//if(rit != wnd_create_count_list.rend())
		//{
		//	if( rit->time == m_tWatchTime)
		//	{
		//		(rit->count)++;
		//		(rit->recorded) = false;
		//		//AfxMessageBox("ԭ����ڵ�����");
		//	}
		//	else
		//	{
		//		wnd_create_count_list.push_back(WndCreateNode(creat_time, 1));
		//	}
		//}

		WindowInfo newInfo(hWnd, title, rect, active, className);
		
		//�����ݿ���Ӽ�¼
#ifdef DB_LOG
		newInfo.wnd_id = m_mysql->NewWnd(creat_time, rec_id, title, className, rect);
		//newInfo.WindowActive(creat_time, m_mysql);
#endif // DB_LOG
		//if(tempInfo.wnd_id == 0)
		//{
		//	m_mysql->DisConnect();
		//	theApp.LostDBConnect();
		//}
		hwnd_info.insert(make_pair(hWnd, newInfo));

		//�ҵ�ʧȥ����̬�Ĵ��ڣ���Ӽ�¼��Ϣ
		//�ж���һ������ڲ�Ϊ��
		if(m_hCurrentActiveWnd != NULL)
		{

			//���ݴ���
			map<HWND, WindowInfo>::iterator itl = hwnd_info.find(m_hCurrentActiveWnd);
			WindowInfo *tempInfo;
			if(itl != hwnd_info.end())
			{
				tempInfo = &hwnd_info[m_hCurrentActiveWnd];

#ifdef DB_LOG
				tempInfo->WindowUnActive(creat_time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
				m_lKeyCount = 0;
				m_lMouseCount = 0;
				//m_mysql->WindowActive(tempInfo->wnd_id, false, creat_time);
#else
				tempInfo->WindowUnActive(creat_time, m_iDeskArea);
#endif // DB_LOG

				//�ж��ڸô������Ƿ��������߼��̵Ľ������н�������ӱ���Ϣ
				//��������Ŀ����Ϊ�˱������Щ���Ŵ��ڽ����ڵ�
				if(tempInfo->Contacted())
				{
					//activeGraph.insertDelay(m_hCurrentActiveWnd, hWnd);
					activeGraph.insert(m_hCurrentActiveWnd, hWnd);
					m_mysql->LogWindowSwitch(rec_id, tempInfo->wnd_id,newInfo.wnd_id, creat_time);
				}

				//��¼��ǰ����Ϊ�����
				m_hCurrentActiveWnd = hWnd;
			}
		}
		else
		{
			//û��ǰһ����ڵ�,�����л�ͼ�в��뵱ǰ����ڵ�
			m_lKeyCount = 0;
			m_lMouseCount = 0;
			//activeGraph.insertDelay(hWnd);
			activeGraph.insert(hWnd);
		}
		//��¼��ǰ����Ϊ�����
		m_hCurrentActiveWnd = hWnd;
	}
}

//���ڱ�����
void WinInfoCtl::WindowActive(HWND hWnd)
{
	CTime time = CTime::GetCurrentTime();
	//��ǰ����ڲ�����һ��
	if(hWnd != m_hCurrentActiveWnd)
	{
		//WindowInfo *wif;
		//�ҵ�Ҫ����Ĵ��ڣ���Ӽ�¼��Ϣ
		map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
		WindowInfo* targetInfo;
		//�ҵ����Ǹ����
		if(it != hwnd_info.end())
		//if(targetInfo.getInitTitle() != "")
		{
			targetInfo = &hwnd_info[hWnd];
			//targetInfo = (WindowInfo)it->second;
			//((WindowInfo)it->second).WindowActive(time, &m_mysql);

#ifdef DB_LOG
			//targetInfo->WindowActive(time, m_mysql);
			targetInfo->WindowActive(time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
			m_lKeyCount = 0;
			m_lMouseCount = 0;
			//д���ݿ�
			//m_mysql->WindowActive(targetInfo->wnd_id, true, time);
#else
			targetInfo->WindowActive(time);
#endif
		}
		else
		{
			//û�ҵ�ʵ����˵�������д��˳�
			return;
		}
		//�ҵ�ʧȥ����̬�Ĵ��ڣ���Ӽ�¼��Ϣ
		//�ж���һ������ڲ�Ϊ��
		if(m_hCurrentActiveWnd != NULL)
		{
			
			//���ݴ���
			map<HWND, WindowInfo>::iterator itl = hwnd_info.find(m_hCurrentActiveWnd);
			if(itl != hwnd_info.end())
			{
			//ֻ���������������޸ĵ�����
			WindowInfo *tempInfo = &hwnd_info[m_hCurrentActiveWnd];
			//if(tempInfo->getInitTitle() != "N/A")
			//{
#ifdef DB_LOG
				//tempInfo->WindowUnActive(time, m_iDeskArea, m_mysql);
				//m_mysql->WindowActive(tempInfo->wnd_id, false, time);
				tempInfo->WindowUnActive(time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
				m_lKeyCount = 0;
				m_lMouseCount = 0;
#else
				tempInfo->WindowUnActive(time, m_iDeskArea);
#endif // DB_LOG

				//�ж��ڸô������Ƿ��������߼��̵Ľ������н�������ӱ���Ϣ
				//��������Ŀ����Ϊ�˱������Щ���Ŵ��ڽ����ڵ�
				if(tempInfo->Contacted())
				{
					//activeGraph.insertDelay(m_hCurrentActiveWnd, hWnd);
					activeGraph.insert(m_hCurrentActiveWnd, hWnd);
					m_mysql->LogWindowSwitch(rec_id, tempInfo->wnd_id,targetInfo->wnd_id, time);
				}

				//��¼��ǰ����Ϊ�����
				m_hCurrentActiveWnd = hWnd;
			}
			else
			{
				//AfxMessageBox("��һ��������Ѿ�ϴ����");
				//NewWindow(hWnd);
			}
			//CString msg
		}
		else
		{
			//��¼��ǰ����Ϊ�����
			m_hCurrentActiveWnd = hWnd;
			//дһ���ڵ�
			if(targetInfo->Contacted())
				//activeGraph.insertDelay(hWnd);
				activeGraph.insert(hWnd);
		}
	}
}

//�����ƶ�
void WinInfoCtl::WindowMove(HWND hWnd, const CRect& rect)
{
	//�ҵ��ƶ��Ĵ��ڣ���Ӽ�¼��Ϣ
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//�ҵ����Ǹ����
	if(it != hwnd_info.end())
	{
		WindowInfo *tempInfo = &hwnd_info[hWnd];
		//WindowInfo tempInfo = ((WindowInfo)it->second);
		tempInfo->OnWindowMove(rect, m_iDeskArea);

#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)rect);
#endif // DB_LOG
		//(*wif).WindowActive();
	}
	////û�ҵ�,Ӧ��Ϊ�ô��ڴ���ʵ��
	//else
	//{
	//	NewWindow(hWnd);
	//}
}

//����ϴ��
void WinInfoCtl::WindowDestory(HWND hWnd)
{
	CTime time = CTime::GetCurrentTime();
	//�����ǰ����ʱ����ڣ�����������ڱ���
	if(hWnd == m_hCurrentActiveWnd)
		m_hCurrentActiveWnd = NULL;
	//�ҵ��رյĴ��ڣ���Ӽ�¼��Ϣ
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//�ҵ����Ǹ����
	if(it != hwnd_info.end())
	{
		WindowInfo *tempInfo = &hwnd_info[hWnd];

		
		//ɾ��ͼ�ڵ�
		if(it->second.Contacted())
		{
			//�����Ƿ��нӴ�����Ҫ����ʷ��ɾ���������
			activeGraph.RemoveFromClusterHistory(hWnd);
			activeGraph.remove(hWnd);
		}

		//WindowInfo tempInfo = (WindowInfo)it->second;

#ifdef DB_LOG
		//tempInfo->OnWindowDestory(time, m_mysql);
		tempInfo->OnWindowDestory(time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
		m_lKeyCount = 0;
		m_lMouseCount = 0;
		m_mysql->WindowDestory(tempInfo->wnd_id, time);
#else
		tempInfo.OnWindowDestory(time);
#endif

		//(*wif).WindowActive();
		hwnd_info.erase(it);
	}
	

	//���ô�����Ϣ������ʷ��Ϣ�б�			��Լ�ڴ棬��������ǰ�Ĵ�����
	//history_list.push_back(it->second);
	//(it->second).Destroy();
	//���ô����Ƴ����¼
	
	
}

//�����û�
void WinInfoCtl::OnKeyType()
{
	if(m_hCurrentActiveWnd == NULL)
		return;

	m_lKeyCount++;
	//AfxMessageBox("Key Typed");
	//�ҵ��رյĴ��ڣ���Ӽ�¼��Ϣ
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(m_hCurrentActiveWnd);
	//�ҵ����Ǹ����
	if(it != hwnd_info.end())
	{
		WindowInfo *temp = &hwnd_info[m_hCurrentActiveWnd];
#ifdef DB_LOG
		//((WindowInfo)it->second).OnKeyType(m_tWatchTime, &m_mysql);
		temp->OnKeyType(m_tWatchTime, m_mysql);
#else
		//((WindowInfo)it->second).OnKeyType(m_tWatchTime);
		temp->OnKeyType(m_tWatchTime);
#endif // DB_LOG
	}
}   

//�����
void WinInfoCtl::OnMouseClick()
{
	if(m_hCurrentActiveWnd == NULL)
		return;

	m_lMouseCount++;
	//�ҵ��Ǹ����
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(m_hCurrentActiveWnd);
	//�ҵ����
	if(it != hwnd_info.end())
	{
		WindowInfo *temp = &hwnd_info[m_hCurrentActiveWnd];
#ifdef DB_LOG
//		((WindowInfo)it->second).OnMouseClick(m_tWatchTime, &m_mysql);
		temp->OnMouseClick(m_tWatchTime, m_mysql);
#else
//		((WindowInfo)it->second).OnMouseClick(m_tWatchTime);
		temp->OnMouseClick(m_tWatchTime);
#endif // DB_LOG
	}
}

//�����С��
void WinInfoCtl::OnMin(HWND hWnd)
{
	//�ҵ��Ǹ����
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	WindowInfo *tempInfo;
	//�ҵ����
	if(it != hwnd_info.end())
	{
		//WindowInfo tempInfo = (WindowInfo)it->second;
		tempInfo = &hwnd_info[hWnd];
		tempInfo->OnMin(m_iDeskArea);
#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)CRect(-1,-1,-1,-1));
#endif // DB_LOG
	}
}

//�����С��
void WinInfoCtl::OnMax(HWND hWnd)
{
	//�ҵ��Ǹ����
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//�ҵ����
	if(it != hwnd_info.end())
	{
		//WindowInfo tempInfo = (WindowInfo)it->second;
		WindowInfo *tempInfo = &hwnd_info[hWnd];
		tempInfo->OnMax(m_iDeskArea, m_cDeskRect);
		//((WindowInfo)it->second).OnMax(m_iDeskArea, m_cDeskRect);
#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)CRect(-2,-2,-2,-2));
#endif // DB_LOG
	}
}

//�ָ���С
void WinInfoCtl::OnRestore(HWND hWnd)
{
	//�ҵ��Ǹ����
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//�ҵ����
	if(it != hwnd_info.end())
	{
		WindowInfo *tempInfo = &hwnd_info[hWnd];
		tempInfo->OnRestore(m_iDeskArea,hWnd);
		//WindowInfo tempInfo = (WindowInfo)it->second;
		//((WindowInfo)it->second).OnRestore(m_iDeskArea,hWnd);
#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)CRect(-3,-3,-3,-3));
#endif // DB_LOG  
	}
}

//�����µļ��ʱ��
//�ú�����Ҫ��MFC���ô���
void WinInfoCtl::NewWatchTime()
{
	//����hwnd_info
	ExamMaps();
	m_tWatchTime = CTime::GetCurrentTime();
	
	//ֻ�е�ǰ����ڲŽ��й�ע�ȼ���
	//��������ֻ����ע������
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		if(it->first == m_hCurrentActiveWnd)
		{
			it->second.CalculateAttRate(m_tWatchTime, m_iDeskArea);
			//it->second.NewAtt(m_tWatchTime);
		}
		it->second.NewAtt(m_tWatchTime);
	}

	//���ûͼ�۲�ʱ��
	if(tickCount >= 5)
	{
		tickCount = 0;
		//ˢ�¼��ʱ�䣬ͬʱ��ˢ�µ�ǰ����ڵĻʱ��
		activeGraph.newFixTime(m_hCurrentActiveWnd);
		//�Ƴ���Щ����5�������ϵĽڵ�
		activeGraph.RemoveFadeNodeEdge(hwnd_info);
		activeGraph.ClusterInHistory(hwnd_info);
		//LogActiveGraphInFile();
		//�������WindowInfo�Ĺ�ע��
		for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
		{
			it->second.RefreshAttRate();
		}
	}
	else
	{
		tickCount++;
	}
#ifdef DB_LOG
	//�������ݿ�����
	//if(m_mysql->IsConnected() == FALSE)
	//{
	//	//m_mysql->Connect();
	//	ConnectDB();
	//}
	//////////////////////////////////////////////////////////////////////////
	//��д���ڴ��������ˣ�û����˼
	//û�ж��ߣ���д�봰�ڴ�������
	//else
	//{
	//	list<WndCreateNode>::reverse_iterator rit = wnd_create_count_list.rbegin();
	//	if(rit != wnd_create_count_list.rend())
	//	{
	//		if(rit->recorded == false)
	//		{
	//			m_mysql->RecordWndCreateCount(rec_id, rit->time, rit->count);
	//			rit->recorded = true;
	//		}
	//	}	
	//}
#endif // DB_LOG
	//����µĴ��ڴ��������ڵ�
	//wnd_create_count_list.push_back(WndCreateNode(m_tWatchTime, 0));
	
//	AfxMessageBox("new watch Time");
}

//�õ����������
const int WinInfoCtl::GetWorkArea()
{
	return m_iDeskArea;
}

//�õ�������ʾ�Ĵ��ڵ�λ��
BOOL WinInfoCtl::GetWindowLocals(vector<sHWNDAtt> &result)																
{
	//����ṹ����֤���صĶ�����ȷ��
	ExamMaps();
	//������Ҫ��
	int sumAtt = 0;
	for(map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		if(!::IsIconic(it->first))
		{
			sumAtt += it->second.GetAttRate();
		}
	}

	if(sumAtt <= 0)
		sumAtt = 1;

	//vector<sWinLocal> tmpV;
	for(map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		if(!::IsIconic(it->first))
		{
			sHWNDAtt tmpS;
			tmpS.hwnd = it->first;
			//tmpS.rect = it->second.GetCurrentLocal();
			::GetWindowRect(it->first, &tmpS.rect);
			tmpS.attRate = it->second.GetAttRate();
			tmpS.importance = (tmpS.attRate > 0) ? tmpS.attRate / (float)sumAtt : 0.5f;
			//tmpV.push_back(tmpS);
			result.push_back(tmpS);
		}
	}

	sort(result.begin(), result.end(), &WinInfoCtl::sHWNDAttCompare);

	return TRUE;
}

//�õ�HWND
BOOL WinInfoCtl::GetHWND(vector<HWND> &result)
{
	//vector<sWinLocal> tmpV;
	for(map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		result.push_back(it->first);
	}
	return TRUE;
}

//������ô���λ��
BOOL  WinInfoCtl::OnSetWinsLocal(void)
{
	//getGlobeClique();
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		//���û����С�����͸ı���λ��
		//if(IsIconic(it->first) == FALSE)
		//{
		::ShowWindow(it->first, SW_RESTORE);
			int x = rand() % 1000;
			int y = rand() % 700;
			int width = rand() % 400 + 300;
			int height = rand() % 400 + 300;
			CRect rect(x, y, x+width, y+height);
			::SetWindowPos(it->first, HWND_TOP, x, y, width, height, SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
		//}
	}
	return TRUE;
}

//��ʾ��һ�����ര�ڼ���
BOOL WinInfoCtl::OnShowNextClique(void)
{
	//ȷ��������һ������
	if(m_vGlobelClique.size() <= 0)
	{
		return FALSE;
	}

	m_iCurClique = ++m_iCurClique % m_vGlobelClique.size();
	for(map<HWND,WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		//::ShowWindow(it->first, SW_SHOWMINIMIZED);
		bool bFound = FALSE;
		for(vector<pair<HWND, float>>::const_iterator ij = m_vGlobelClique[m_iCurClique].begin(); ij != m_vGlobelClique[m_iCurClique].end(); ij++)
		{
			if(::IsWindow(ij->first) == TRUE)
			{
				//�ô����ڸþ����г����ˣ��Ͱ���ԭ��Сλ����ʾ
				if(it->first == ij->first)
				{
					::ShowWindow(it->first, SW_RESTORE);
					bFound = true;
					break;
				}
			}
		}
		//���û���ҵ�����С��
		if(bFound == FALSE)
		{
			::ShowWindow(it->first, SW_SHOWMINIMIZED);
		}
	}
	return TRUE;
}

//д��¼��־
//����һ���޴��CString��д��־
void WinInfoCtl::Log()
{
	//AfxMessageBox("д��־");
	CString msg;
	msg.Append("***********Active Window Infomation************\r\n");
	map<HWND, WindowInfo>::const_iterator map_it = hwnd_info.begin();
	while(map_it != hwnd_info.end())
	{
		msg.Append(((WindowInfo)map_it->second).toString());

		++map_it;
	}

	msg.Append("\r\n\r\n**************History Window Infomation****************\r\n");
	list<WindowInfo>::const_iterator list_it = history_list.begin();
	while (list_it != history_list.end())
	{
		msg.Append(((WindowInfo)*list_it).toString());
		++list_it;
	}

	//AfxMessageBox("�������");

	CFile file;
	CString FileName="WindowInfo.log";
	CTime date = CTime::GetCurrentTime();
	CString dateMsg = date.Format("%y_%m_%d");
	FileName.Format("%s_%s", dateMsg, FileName);
	try
	{
		file.Open(FileName,CFile::modeCreate|CFile::modeWrite);
		file.SeekToBegin();
		file.Write(msg, msg.GetLength());
		file.Flush();
		file.Close();
		//MessageBox("д��ɹ���");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("д��ʧ�ܵ�ԭ����:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
	}
	//AfxMessageBox("��־д��");

}

//��д��־�ļ�
void WinInfoCtl::WriteLog()
{
	if(m_mysql->Log() == FALSE)
	{
		//AfxMessageBox("������Ϣʧ��");
	}
	//m_mysql->Log();
	//CFile file;
	//CString FileName;
	//CTime date = CTime::GetCurrentTime();
	//CString dateMsg = date.Format("%y_%m_%d");
	//FileName.Format("%s_windowlog.log", dateMsg, FileName);
	//try
	//{
	//	file.Open(FileName,CFile::modeCreate|CFile::modeWrite);
	//	file.SeekToBegin();

	//	//������д��־�Ĵ���
	//	{
	//		CString msg;
	//		msg.Append("***********Active Window Infomation************\r\n");
	//		file.Write(msg, msg.GetLength());
	//		msg.Empty();
	//		map<HWND, WindowInfo>::const_iterator map_it = hwnd_info.begin();
	//		while(map_it != hwnd_info.end())
	//		{
	//			((WindowInfo)map_it->second).WriteLog(file);

	//			++map_it;
	//		}

	//		msg.Append("\r\n\r\n**************History Window Infomation****************\r\n");
	//		file.Write(msg, msg.GetLength());
	//		msg.Empty();
	//		list<WindowInfo>::const_iterator list_it = history_list.begin();
	//		while (list_it != history_list.end())
	//		{
	//			((WindowInfo)*list_it).WriteLog(file);
	//			++list_it;
	//		}
	//	}

	//	file.Flush();
	//	file.Close();
	//	//MessageBox("д��ɹ���");
	//}
	//catch(CFileException *e)
	//{
	//	CString str;
	//	str.Format("д��ʧ�ܵ�ԭ����:%d",e->m_cause);
	//	//AfxMessageBox("str");
	//	file.Abort();
	//	e->Delete();
	//	return;
	//}
	//AfxMessageBox("��־д��");
}

#ifdef DB_LOG
//�����������ݿ�
BOOL WinInfoCtl::ReConnect()
{
	//�ȶϿ�����
	m_mysql->DisConnect();

	//����
	if(!m_bDBConnect)
	{
		if(m_mysql->Connect())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

//�����������ݿ�
BOOL WinInfoCtl::DropLocalDB()
{
	return  m_embMySql.DropLocalDB();
}

//�ؽ��������ݿ�
BOOL WinInfoCtl::RebuildLocalDB()
{
	if(m_embMySql.RebuildLocalDB() == TRUE)
	{
		//����ע���¼
		TCHAR buffer[256] = TEXT("");
		int cnf = 0;
		DWORD dwSize = sizeof(buffer);
		//����ȡ����ȡ�����м������ʾ��
		for (cnf = 0; cnf < ComputerNameMax; cnf++)
		{
			if (!GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
			{
				continue;
				//_tprintf(TEXT("GetComputerNameEx failed (%d)\n"), GetLastError());
				//return;
			}
			else
			{
				computer = buffer;
				//ZeroMemory(buffer, dwSize);
				break;
			}		
		}

		CString sVersion = GetVersion();
		if(sVersion != "NONE")
		{
			rec_id = m_mysql->NewRecord(computer, rec_time, sVersion);
		}
		else
		{
			rec_id = m_mysql->NewRecord(computer, rec_time);
		}	
		return TRUE;
	}
	return FALSE;
}

//��������
BOOL WinInfoCtl::ExportData()
{
//	return m_embMySql.ExportAllTable();
	return TRUE;
}
#endif // DB_LOG

//��ȡ����汾
CString WinInfoCtl::GetVersion()
{
	TCHAR szVersionBuffer[8192] = _T("");   
	DWORD dwVerSize;   
	DWORD dwHandle;
	CString sLocalVersion;

	dwVerSize = GetFileVersionInfoSize("FlowWatcher.exe", &dwHandle);   
	if (dwVerSize == 0)   
	{
		//AfxMessageBox(remoteFileName + "�����ڣ���Ҫ����");
		return "NONE";   
	}

	if (GetFileVersionInfo("FlowWatcher.exe", 0, dwVerSize, szVersionBuffer))   
	{   
		VS_FIXEDFILEINFO * pInfo;   
		unsigned int nInfoLen;   

		if (VerQueryValue(szVersionBuffer, _T("\\"), (void**)&pInfo, &nInfoLen))
		{
			sLocalVersion.Format(_T("%d.%d.%d.%d"),   
				HIWORD(pInfo->dwFileVersionMS), LOWORD(pInfo->dwFileVersionMS),   
				HIWORD(pInfo->dwFileVersionLS), LOWORD(pInfo->dwFileVersionLS));  
			//AfxMessageBox(sLocalVersion);
			return sLocalVersion;
		}   
		else
		{
			return "NONE";
		}
	}
	else
	{
		return "NONE";
	}
	return "NONE";
}

//���������Ҫ��
void WinInfoCtl::CalculateImportance()
{
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		it->second.CalculateImportance();
	}
}

//�õ�����ע��
size_t WinInfoCtl::GetMaxAtt()
{
	size_t max = 0;
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		size_t tmp = it->second.GetAttRate();
		if(tmp >= max)
			max = tmp;
	}
	return max;
}

//��ע������
size_t WinInfoCtl::GetTotalAtt()
{
	size_t total = 0;
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		size_t tmp = it->second.GetAttRate();
		total += tmp;
	}
	//ȷ��total�Ǵ���0�ģ����û�д���0��˵����BUG
	ASSERT(total > 0);
	return total;
}

//�õ�������
float WinInfoCtl::GetNormalBase()
{
	float base = 0.0f;
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		float tmp = static_cast<float>(it->second.GetAttRate());
		base += pow(tmp,2);
	}
	//ȷ��total�Ǵ���0�ģ����û�д���0��˵����BUG
	//ò���ǳ���û����ϵͳ��Ļ�������������BUG
	ASSERT(base > 0);
	return sqrt(base);

}

void WinInfoCtl::GetBKCliqueWinInfo(vector<vector<const WindowInfo* const>> &result)
{
	//�õ������Ĵ��ھ��
	vector<vector<const HWND>> resHWND;
	activeGraph.GetBKCliqueHWND(resHWND);

	//ͨ��������ҵõ�WindowInfo����
	for(vector<vector<const HWND>>::const_iterator it = resHWND.begin(); it != resHWND.end(); it++)
	{
		vector<const WindowInfo* const> clique;
		for(vector<const HWND>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			clique.push_back(static_cast<const WindowInfo* const>(&hwnd_info[*ij]));
		}
		result.push_back(clique);
	}
}

//���ͼ��¼�����ݿ���
void WinInfoCtl::LogActiveGraphInDB()
{
	//û�����ݾ��˳�
	if(activeGraph.hasSomething() == false)
		return;

	//���������Ҫ��
	CalculateImportance();
	
	CTime time = CTime::GetCurrentTime();
	CString strT = time.Format("%Y-%m-%d %H:%M:%S\\n");
	//�õ�ͼ�ṹ�ַ���
	CString gMsg;
	gMsg.Append(strT);
	gMsg.Append(activeGraph.log());

	//��ѯ��Ҫ�Եȼ�¼
	CString dataStr;
	vector<HWND> hWndList = activeGraph.getHWNDs();
	//д�ڵ����
	int i = 0;
	for(vector<HWND>::iterator it = hWndList.begin(); it != hWndList.end(); it++, i++)
	{
		map<HWND, WindowInfo>::iterator itm = hwnd_info.find(*it);
		if(itm != hwnd_info.end())
		{
			//������Ҫ��
			//itm->second.CalculateImportance();
			CString msg;
			msg.Format("%d\t%d\t%.2f\t%s\r", i, static_cast<int>(itm->second.GetAttRate()), itm->second.GetImportance(),itm->second.getInitTitle());
			dataStr.Append(msg);
		}
	}
	//���ռ�¼�ĵ�
	CString logStr = gMsg + dataStr;
	//����¼д��DB
	//if(m_mysql->LogCliqueResult(rec_id, time, logStr) == FALSE)
	//{
	//}
		//AfxMessageBox("��־д�����ݿ�ʧ��");
}

//���ͼ��¼����־�ļ���
void WinInfoCtl::LogActiveGraphInFile()
{
	//û�����ݾ��˳�
	if(activeGraph.hasSomething() == false)
		return;

	//���������Ҫ��
	//CalculateImportance();


	CFile file;
	CString FileName;
	CTime date = CTime::GetCurrentTime();
	CString dateMsg = date.Format("%y_%m_%d");
	FileName.Format("%s_activelog.log", dateMsg, FileName);
	try
	{
		file.Open(FileName,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
		//file.SeekToBegin();
		file.SeekToEnd();

		//������д��־�Ĵ���
		{
			CString msg;
			msg.Append("\r\n**Active Window Information**");
			CTime time = CTime::GetCurrentTime();
			CString strT = time.Format("%Y-%m-%d %H:%M:%S****\r\n");
			msg.Append(strT);
			file.Write(msg, msg.GetLength());
			msg.Empty();
			
			const vector<vector<vector<pair<HWND, float>>>> clusterHistory = activeGraph.GetClusterHistory();

			for(vector<vector<vector<pair<HWND, float>>>>::const_iterator it = clusterHistory.begin(); it != clusterHistory.end(); it++)
			{
				//һ������
				for(vector<vector<pair<HWND, float>>>::const_iterator ij = it->begin(); ij != it->end(); ij++)
				{
					//һ��
					for(map<HWND, WindowInfo>::const_iterator il = hwnd_info.begin(); il != hwnd_info.end(); il++)
					{
						bool found = false;
						CString wnd;
						for(vector<pair<HWND, float>>::const_iterator ik = ij->begin(); ik != ij->end(); ik++)
						{		
							//ÿ�����ڶ�Ӧ����һ��ֵ
							if(ik->first == il->first)
							{
								wnd.Format("%.3f, ",static_cast<float>(ik->second));
								msg.Append(wnd);
								found = true;
								break;
							}
						}
						//���û��������ڵľ�����Ϣ����д��0
						if(found == false)
						{
							wnd.Format("%.3f, ", 0.0f);
							msg.Append(wnd);
						}
					}
					msg.Append("\r\n");
				}
				msg.Append("\r\n");
			}
			file.Write(msg, msg.GetLength());
		}

		file.Flush();
		file.Close();
		//MessageBox("д��ɹ���");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("д��ʧ�ܵ�ԭ����:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
		return;
	}
}

// �ø�˹�˺�K-Center����ȫ�־���
BOOL WinInfoCtl::getGlobeClique(void)
{
	const vector<vector<vector<pair<HWND, float>>>> clusterHistory = activeGraph.GetClusterHistory();

	int cliqueNum = 0;
	for(vector<vector<vector<pair<HWND, float>>>>::const_iterator it = clusterHistory.begin(); it != clusterHistory.end(); it++)
	{
		cliqueNum += it->size();
	}

	//����Ҫ��figtree���ݣ�����ʵ�����õ���һά����
	int maxNum = cliqueNum * hwnd_info.size();
	double *history = new double[maxNum];
	//���������index�����һά�����е�����
	int index=0;
	for(vector<vector<vector<pair<HWND, float>>>>::const_iterator it = clusterHistory.begin(); it != clusterHistory.end(); it++)
	{
		//��ÿ�����ϴ���һ��������	

		//��ǰ�׶�ֻ���Ǵ���ÿ�������еĵ�һ������
		//vector<vector<pair<HWND, float>>>::const_iterator ij = it->begin();
		for(vector<vector<pair<HWND, float>>>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			//array[i] = new double[hwnd_info.size()];

			//һ��
			//int j=0;
			for(map<HWND, WindowInfo>::const_iterator il = hwnd_info.begin(); il != hwnd_info.end(); il++)
			{
				bool found = false;
				CString wnd;
				for(vector<pair<HWND, float>>::const_iterator ik = ij->begin(); ik != ij->end(); ik++)
				{		
					//ÿ�����ڶ�Ӧ����һ��ֵ
					if(ik->first == il->first)
					{
						history[index++] = static_cast<float>(ik->second);
						
						found = true;
						break;
					}
				}
				//���û��������ڵľ�����Ϣ����д��0
				if(found == false)
				{
					history[index++] = 0.0f;
				}
			}
		}
	}

	//CString msg;
	//for(int i=0; i<index; i++)
	//{
	//	CString tmp;
	//	tmp.Format("%.3f\t", history[i]);
	//	msg.Append(tmp);

	//	if(i % hwnd_info.size() == 0)
	//		msg.Append("\r\n");
	//}
	//AfxMessageBox(msg);

	int d = hwnd_info.size();		//��Ҫ�����ά��
	int N = cliqueNum;				//��������
	int M = 10;
	double h = .9;						//������
	double epsilon =  0.9;			//����ƫ����

	double y[] = {
		1.01f, 1.01f, 1.02f, 1.03f, 1.02f, 1.44f, 0.98f,
		1.07f, 1.01f, 1.02f, 1.03f, 1.02f, 1.44f, 0.98f};
	double q[] = {0.2280, 0.4496, 0.1722, 0.9688, 0.3557, 0.0490, 0.7553, 0.8948, 0.2861, 0.2512};
	int W = 1;

	double * g_ifgt_nu = new double[W*M];
	memset( g_ifgt_nu    , 0, sizeof(double)*W*M );

	//��������index����Ҫ�Ե���ϼ���
	std::vector<std::vector<std::pair<int, double>>*> *result = new std::vector<std::vector<std::pair<int, double>>*>;
	std::vector<int> *count = new std::vector<int>;
	figtreeKCenter(d, N, M, W, history, h, q, y, epsilon, g_ifgt_nu, result, count, FIGTREE_EVAL_IFGT_TREE, FIGTREE_PARAM_NON_UNIFORM, 1 );
	//NND����������鲻Ҫ��~
	delete [] history;

	//�����ǰ�ľ���
	m_vGlobelClique.clear();
	//������ת��ΪHWND�������m_vGlobelClique��
	//vector<int>::iterator cit = count->begin();
	for(std::vector<std::vector<std::pair<int, double>>*>::const_iterator it = result->begin(); it != result->end(); it++)
	{
		vector<pair<HWND, float>> oneClique;
		//ֻ�����Щ����Ҫ�Դ���ĳ��ֵ�ľ���
		float sumAtt = 0.0f;
		for(std::vector<std::pair<int, double>>::const_iterator ij = (*it)->begin(); ij != (*it)->end(); ij++)
		{
			int i = 0;
			for(map<HWND, WindowInfo>::const_iterator il = hwnd_info.begin(); il != hwnd_info.end(); il++,i++)
			{
				if (i != ij->first)
				{
					continue;
				}
				else
				{
					oneClique.push_back(make_pair(il->first, ij->second));
					sumAtt += ij->second;
				}
			}
		}
		if(sumAtt >= 0.3f)
		{
			m_vGlobelClique.push_back(oneClique);
		}
		else
		{
			//ɾ������
//			count->erase(cit);
		}
	}

	//���ɾ�����Ϣ
	CString totalMsg;
	int indexClique = 0;
	for(vector<vector<pair<HWND, float>>>::const_iterator it = m_vGlobelClique.begin(); it != m_vGlobelClique.end(); it++,indexClique++)
	{
		CString cliqueMsg;
		for(vector<pair<HWND, float>>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			TCHAR titlebuffer[TITLE_SIZE];
			//�õ�����
			SendMessage(ij->first, WM_GETTEXT, TITLE_SIZE, (LPARAM)titlebuffer);
			CString title(titlebuffer);
			cliqueMsg.Append(title + "\r\n");
		}
		CString countStr;
		countStr.Format("%d", count->at(indexClique));
		totalMsg.Append(cliqueMsg + "\r\n" + countStr + "\r\n");
	}
	AfxMessageBox(totalMsg);

	//�����ʷ����
	count->clear();
	for(std::vector<std::vector<std::pair<int, double>>*>::iterator it = result->begin(); it != result->end(); it++)
	{
		(*it)->clear();
	}
	result->clear();

	return 0;
}

// ��ͼ
BOOL WinInfoCtl::OnScreenShot(void)
{
	for (map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		CScreenHelper::ScreenshotHWNDFile(it->first);
	}

	return 0;
}

//ƽ������
BOOL WinInfoCtl::OnPlainLayout(void )
{
	vector<sHWNDAtt> hwnds;
	GetWindowLocals(hwnds);

	int iWinNum = hwnds.size();

	if(iWinNum > 0)
	{
		CRect screenRect;
		SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &screenRect,   0);
		double balance = screenRect.Width() / (double)screenRect.Height();
		int base = (int)sqrt((double)iWinNum);
		int column;
		int row;

		if((base * base) < iWinNum)
		{
			if(balance > 1)
			{
				column = (iWinNum == 1) ? 1 : base + 1;
				row = ceil(iWinNum / (double) column);
			}
			else
			{
				row = base + 1;
				column = ceil(iWinNum / (double) row);
			}
		}
		else
		{
			column = row = base;
		}

		int width = max( screenRect.Width()  / column, MIN_WINDOW_WIDTH);
		int height = max( screenRect.Height() / row, MIN_WINDOW_HEIGHT);
		int stepX;
		int stepY;

		if((screenRect.Width()  / column) < MIN_WINDOW_WIDTH)
		{
			stepX = (column > ceil(balance)) ? (screenRect.Width() - MIN_WINDOW_WIDTH) / (column - 1) : (screenRect.Width() / column );
		}
		else
		{
			stepX = width;
		}
		
		if((screenRect.Height() / row) < MIN_WINDOW_HEIGHT)
		{
			stepY = (row > ceil((double)(1 / balance))) ? (screenRect.bottom - MIN_WINDOW_HEIGHT) / (row - 1) : (screenRect.Height()  / row);
		}
		else
		{
			stepY = height;
		}
		

		int index = iWinNum - 1;
		AnimatinoHelper::Begin();
		for(vector<sHWNDAtt>::const_iterator it = hwnds.begin(); it != hwnds.end(); it++, index--)
		{
			//////////////////////////////////////////////////////////////////////////
			// QQ2010 ��С���ڴ�С ��538 ��500

			int x = stepX * (index % column);
			int y = stepY * (index / column);
			// 
			::ShowWindow(it->hwnd, SW_RESTORE);
			// 			::SetWindowPos(it->hwnd, HWND_TOPMOST, x, y, width, height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW);
			CRect srcRect;
			::GetWindowRect(it->hwnd, &srcRect);
			AnimatinoHelper::AddMove(it->hwnd, srcRect, CRect(x, y, x + width, y + height));
		}
		AnimatinoHelper::MoveAnimation();
		AnimatinoHelper::End();
	}

	return TRUE;
}

//����hwnd_info
BOOL WinInfoCtl::ExamMaps()
{
	vector<HWND> vDeadList;				//����ɾ������Ч���

	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		//�������������Ǵ��ڣ���˵�����Ѿ���ϴ����
		if(!::IsWindow(it->first))
		{
			vDeadList.push_back(it->first);
		}
	}

	for(vector<HWND>::iterator it = vDeadList.begin(); it != vDeadList.end(); it++)
	{
		WindowDestory(*it);
	}
	return TRUE;
}

//sHWNDAtt�Ƚ�
bool WinInfoCtl::sHWNDAttCompare(const sHWNDAtt &a, const sHWNDAtt & b)
{
	return a.attRate > b.attRate ? true : false;
}
