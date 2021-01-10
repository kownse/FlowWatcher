#include "StdAfx.h"
#include "DBManager.h"
#include <iostream>

using namespace std;

DBManager::DBManager(void)
{
	//m_iFailCount = 0;
}
//�Ͽ�����
DBManager::~DBManager(void)
{
	mysql_close(&mysql);
	m_bConnected = FALSE;
}

// �������ݿ�
//��������192.168.0.1��������ʧ�������ӱ�������ʧ��Ӧ������Ƕ��ʽmysql�����������ǻ�û��ʼ��
BOOL DBManager::Connect(void)
{
	if (0 != mysql_library_init(0, NULL, NULL)) {
		//AfxMessageBox("װ��libʧ��");
		return FALSE;
	}else{
		//AfxMessageBox("װ��lib�ɹ�");
	}
	if (NULL == mysql_init(&mysql)) {
		//AfxMessageBox("���ӳ�ʼ��ʧ��");
		return FALSE;
	}else{
		//AfxMessageBox("���ӳ�ʼ���ɹ�");
	}
	if (0 != mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gb2312")) {
		//AfxMessageBox("�����ַ�������");
		return FALSE;
	}else{
	}

	////�������ӷ�����
	if (!mysql_real_connect(&mysql, "192.168.0.1", "root", "kingofcs", "hook_db",3306, NULL, 0))
	{
		////������ӱ���
		//if (!mysql_real_connect(&mysql, "localhost", "root", "kingofcs", "hook_db",3306, NULL, 0))
		//{
		//	AfxMessageBox("�������ݿ�ʧ��");
		//	m_bConnected = FALSE;
		//	m_iFailCount = 0;
		//	return FALSE;
		//}
		//else
		//{
			//���ӱ����ɹ�
			//AfxMessageBox("����ʧ��");
			m_bConnected = FALSE;
			m_iFailCount = 0;
			return FALSE;
		//}	
	}
	else
	{
		//���ӷ������ɹ�
		//AfxMessageBox("���ӳɹ�");
		m_bConnected = TRUE;
		m_iFailCount = 0;
		return TRUE;
	}
} 

//ȡ������
BOOL DBManager::DisConnect(void)
{
	mysql_close(&mysql);
	m_bConnected = FALSE;
	m_iFailCount = 0;

	return TRUE;
}

//�����µļ�¼
//�������ݿ��в�����û���ֳɵ�rec_id���о��ã�û�о��½�
const unsigned int DBManager::NewRecord(TCHAR *computerName, const CTime& time)
{
	//�ж��Ƿ����ӳɹ�
	if(m_bConnected == FALSE)
		return 0;

	int rec_id = 0;
	CString date = time.Format("%Y-%m-%d");
	CString ch_query;
	ch_query.Format("select rec_id from rec_com where com_name=\'%s\' and rec_time=\'%s\'", computerName, date);
	
	mysql_real_query(&mysql, ch_query, strlen(ch_query));

	MYSQL_RES *result;
	MYSQL_ROW    row;

	if (!(result=mysql_use_result(&mysql)))
	{
		//AfxMessageBox("��ѯ��¼��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();
		return 0;
	}
	//�ҵ��˼�¼
	if(row=mysql_fetch_row(result))
	{
		rec_id = atoi(row[0]);
//#if DEBUG
			//CString msg;
			//msg.Format("���� rec_id = %d",rec_id);
			//AfxMessageBox(msg);
//#endif
		mysql_free_result(result);
	}
	//û�ҵ���¼�������¼�¼
	else
	{
		mysql_free_result(result);
		CString strSQL;
		strSQL.Format("insert into rec_com (com_name, rec_time) values (\'%s\',\'%s\')", computerName, date) ;
		if (mysql_real_query(&mysql, strSQL, strlen(strSQL))!=0)
		{
			//AfxMessageBox("���ʧ��");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
		}
		else
		{
			//���»��rec_id
			mysql_real_query(&mysql, ch_query, strlen(ch_query));

			MYSQL_RES *result;
			MYSQL_ROW    row;

			if (!(result=mysql_use_result(&mysql)))
			{
				//AfxMessageBox("���ݱ����");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}
			//�ҵ��˼�¼
			if(row=mysql_fetch_row(result))
			{
				rec_id = atoi(row[0]);
//#if DEBUG
					//CString msg;
					//msg.Format("�½� rec_id = %d",rec_id);
					//AfxMessageBox(msg);
//#endif
			}
			else
			{
				//AfxMessageBox("�޷�������¼");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}

			mysql_free_result(result);
		}
	}

	//mysql_free_result(result);
	
	return rec_id;
}

//�����µļ�¼
//�������ݿ��в�����û���ֳɵ�rec_id���о��ã�û�о��½�
//���а汾����Ϣ
const unsigned int DBManager::NewRecord(TCHAR *computerName, const CTime& time, const CString &version)
{
	//�ж��Ƿ����ӳɹ�
	if(m_bConnected == FALSE)
		return 0;

	int rec_id = 0;
	CString date = time.Format("%Y-%m-%d");
	CString ch_query;
	ch_query.Format("select rec_id from rec_com where com_name=\'%s\' and rec_time=\'%s\'", computerName, date);

	mysql_real_query(&mysql, ch_query, strlen(ch_query));

	MYSQL_RES *result;
	MYSQL_ROW    row;

	if (!(result=mysql_use_result(&mysql)))
	{
		//AfxMessageBox("��ѯ��¼��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();
		return 0;
	}
	//�ҵ��˼�¼
	if(row=mysql_fetch_row(result))
	{
		rec_id = atoi(row[0]);
		mysql_free_result(result);
		//�ҵ��˼�¼���޸ļ�¼�еİ汾��
		CString updateVersionSQL;
		updateVersionSQL.Format("update rec_com set version=\'%s\' where rec_id=\'%d\'", version, rec_id);
		if (mysql_real_query(&mysql, updateVersionSQL, strlen(updateVersionSQL))!=0)
		{
			//AfxMessageBox("���ʧ��");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
		}
	}
	//û�ҵ���¼�������¼�¼
	else
	{
		mysql_free_result(result);
		CString strSQL;
		strSQL.Format("insert into rec_com (com_name, rec_time, version) values (\'%s\',\'%s\',\'%s\')", computerName, date, version) ;
		if (mysql_real_query(&mysql, strSQL, strlen(strSQL))!=0)
		{
			//AfxMessageBox("���ʧ��");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
		}
		else
		{
			//���»��rec_id
			mysql_real_query(&mysql, ch_query, strlen(ch_query));

			MYSQL_RES *result;
			MYSQL_ROW    row;

			if (!(result=mysql_use_result(&mysql)))
			{
				//AfxMessageBox("���ݱ����");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}
			//�ҵ��˼�¼
			if(row=mysql_fetch_row(result))
			{
				rec_id = atoi(row[0]);
				//#if DEBUG
				//CString msg;
				//msg.Format("�½� rec_id = %d",rec_id);
				//AfxMessageBox(msg);
				//#endif
			}
			else
			{
				//AfxMessageBox("�޷�������¼");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}

			mysql_free_result(result);
		}
	}

	//mysql_free_result(result);

	return rec_id;
}

//�´��ڼ�¼
//��ͨ������ʱ���rec_id����wnd_id��Ȼ����д��Ӧ����Ϣ
const unsigned int DBManager::NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect)
{
	if(m_bConnected == FALSE)
		return 0;

	int wnd_id;			//���ڱ�Ǻ�

	//�Է���һ����ѯ����wnd_id
	CString create_time = createTime.Format("%Y-%m-%d %H:%M:%S");
	//LPCTSTR title = strTitle;
	CString ch_query;
	ch_query.Format("select wnd_id from rec_wnd where create_time=\'%s\' and rec_id=\'%d\' and init_title=\'%s\'", create_time, rec_id, title);

	mysql_real_query(&mysql, ch_query, strlen(ch_query));

	MYSQL_RES *result;
	MYSQL_ROW    row;

	if (!(result=mysql_use_result(&mysql)))
	{
		//AfxMessageBox("���ڼ�¼��ѯʧ��");
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}
	//�ҵ��˼�¼
	if(row=mysql_fetch_row(result))
	{
		wnd_id = atoi(row[0]);
//#if DEBUG
			//CString msg;
			//msg.Format("���� wnd_id = %d",rec_id);
			//AfxMessageBox(msg);
//#endif
		mysql_free_result(result);
	}
	//û�ҵ���¼�������¼�¼
	else
	{
		mysql_free_result(result);

		CString strSQL;
		strSQL.Format("insert into rec_wnd (create_time, rec_id, init_title, classname) values (\'%s\',\'%d\',\'%s\',\'%s\')", create_time, rec_id, title, className) ;
		if (mysql_real_query(&mysql, strSQL, strlen(strSQL))!=0)
		{
			//AfxMessageBox("��Ӵ���ʧ��");
			if(m_bConnected == TRUE) RecordFailed();
			return NULL;
		}
		else
		{
			//���»��rec_id
			mysql_real_query(&mysql, ch_query, strlen(ch_query));

			MYSQL_RES *result;
			MYSQL_ROW    row;

			if (!(result=mysql_use_result(&mysql)))
			{
				//AfxMessageBox("���ݱ����");
				if(m_bConnected == TRUE) RecordFailed();
				return NULL;
			}
			//�ҵ��˼�¼
			if(row=mysql_fetch_row(result))
			{
				wnd_id = atoi(row[0]);
//#if DEBUG
					//CString msg;
					//msg.Format("�½� wnd_id = %d",rec_id);
					//AfxMessageBox(msg);
//#endif
				mysql_free_result(result);
			}
			else
			{
				mysql_free_result(result);
				if(m_bConnected == TRUE) RecordFailed();
				//AfxMessageBox("�޷��������ڼ�¼");
				return NULL;
			}
		}
	}

	//�õ�wnd_id��д�����Ϣ��
	CString localSQL;
	localSQL.Format("insert into wnd_local (wnd_id,local_time,x,y,cx,cy) values (\'%d\',\'%s\',\'%d\',\'%d\',\'%d\',\'%d\')", wnd_id,create_time, rect.left, rect.top, rect.right, rect.bottom);
	if (mysql_real_query(&mysql, localSQL, strlen(localSQL))!=0)
	{
		//AfxMessageBox("���λ��ʧ��:" + localSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}

	CString titleSQL;
	titleSQL.Format("insert into wnd_title (wnd_id,title,time) values (\'%d\',\'%s\',\'%s\')", wnd_id, title, create_time);
	if (mysql_real_query(&mysql, titleSQL, strlen(titleSQL))!=0)
	{
		//AfxMessageBox("��ʼ������ʧ��:" + titleSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}
	return wnd_id;
}

//���ڼ�����Ϣ
BOOL DBManager::WindowActive(unsigned int wnd_id, bool flag, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	int bit = (int)flag;
	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString activeSQL;
	activeSQL.Format("insert into wnd_active (wnd_id,flag,time) values (\'%d\',b\'%d\',\'%s\')", wnd_id, bit, timeStr);
	if (mysql_real_query(&mysql, activeSQL, strlen(activeSQL))!=0)
	{
		//AfxMessageBox("��Ӵ��ڻ��¼ʧ��:" + activeSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}

	return TRUE;

}

//��¼�����л���
BOOL DBManager::LogWindowSwitch(int rec_id, int from_wnd_id, int to_wnd_id, const CTime &time)
{
	if(m_bConnected == FALSE)
		return FALSE;
	if(from_wnd_id <0 || to_wnd_id<0)
		return FALSE;

	//CTime time = CTime::GetCurrentTime();
	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");

	CString switchSQL;
	switchSQL.Format("insert into wnd_switch (from_wnd_id, to_wnd_id, time,rec_id) values (\'%d\',\'%d\',\'%s\',\'%d\')", from_wnd_id, to_wnd_id, time,rec_id);
	if (mysql_real_query(&mysql, switchSQL, strlen(switchSQL))!=0)
	{
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//�������ӱ���
BOOL DBManager::WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString titleSQL;
	titleSQL.Format("insert into wnd_title (wnd_id,title,time) values (\'%d\',\'%s\',\'%s\')", wnd_id, title, timeStr);
	if (mysql_real_query(&mysql, titleSQL, strlen(titleSQL))!=0)
	{
		//AfxMessageBox("��ӱ���ʧ��:" + titleSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//��¼�������ʱ��
BOOL DBManager::WindowAttTime(unsigned int wnd_id, int lastTime, bool focus, double attBalance)
{
	if(m_bConnected == FALSE)
		return 0;

	if(wnd_id <= 0 || attBalance == 0)
		return 0;

	CString atttimeSQL;
	atttimeSQL.Format("insert into wnd_act_time (wnd_id,last_time,focus, focus_balance) values (\'%d\',\'%d\',b\'%d\',\'%f\')", wnd_id, lastTime,focus,attBalance);
	if (mysql_real_query(&mysql, atttimeSQL, strlen(atttimeSQL))!=0)
	{
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//��¼�������ʱ��ͷǽ������ʱ��
BOOL DBManager::WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, const CTime &time)
{
	if(m_bConnected == FALSE)
		return 0;

	if(wnd_id <= 0 || attBalance == 0)
		return 0;

	CString atttimeSQL;
	atttimeSQL.Format("insert into wnd_act_time (wnd_id,last_time,last_time2,focus, focus_balance) values (\'%d\',\'%d\',\'%d\',b\'%d\',\'%f\')", wnd_id, lastTime1, lastTime2, focus,attBalance);
	if (mysql_real_query(&mysql, atttimeSQL, strlen(atttimeSQL))!=0)
	{
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//��¼�������ʱ��ͷǽ������ʱ��
BOOL DBManager::WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, float avgKey, float avgMouse, const CTime &time)
{
	if(m_bConnected == FALSE)
		return 0;

	if(wnd_id <= 0 || attBalance == 0)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString atttimeSQL;
	atttimeSQL.Format("insert into wnd_act_time (wnd_id,last_time,last_time2,focus, focus_balance, avg_key, avg_mouse) values (\'%d\',\'%d\',\'%d\',b\'%d\',\'%f\',\'%f\',\'%f\',\'%s\')", wnd_id, lastTime1, lastTime2, focus,attBalance, avgKey, avgMouse, timeStr);
	if (mysql_real_query(&mysql, atttimeSQL, strlen(atttimeSQL))!=0)
	{
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//�����ƶ�
BOOL DBManager::WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString localSQL;
	localSQL.Format("insert into wnd_local (wnd_id,local_time,x,y,cx,cy) values (\'%d\',\'%s\',\'%d\',\'%d\',\'%d\',\'%d\')", wnd_id,timeStr, rect.left, rect.top, rect.right, rect.bottom);
	if (mysql_real_query(&mysql, localSQL, strlen(localSQL))!=0)
	{
		//AfxMessageBox("���λ��ʧ��:" + localSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//���ڹر�
BOOL DBManager::WindowDestory(unsigned int wnd_id, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString updateSQL;
	updateSQL.Format("update rec_wnd set destory_time=\'%s\' where wnd_id=\'%d\'", timeStr, wnd_id);
	if (mysql_real_query(&mysql, updateSQL, strlen(updateSQL))!=0)
	{
		//AfxMessageBox("��Ӵ��ڹر�ʱ��ʧ��:" + updateSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//�µİ�����Ϣ
BOOL DBManager::NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString keySQL;
	keySQL.Format("insert into wnd_keytype (wnd_id,keytype_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, keyCount, timeStr);
	if (mysql_real_query(&mysql, keySQL, strlen(keySQL))!=0)
	{
		//AfxMessageBox("��Ӱ�����¼ʧ��:" + keySQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//�µ��������Ϣ
BOOL DBManager::NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString mouseSQL;
	mouseSQL.Format("insert into wnd_mouseclick (wnd_id,mouse_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, mouseCount, timeStr);
	if (mysql_real_query(&mysql, mouseSQL, strlen(mouseSQL))!=0)
	{
		//AfxMessageBox("����������¼ʧ��:" + mouseSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//��¼���ڴ�������
BOOL DBManager::RecordWndCreateCount(int rec_id, const CTime& time, int count)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString wndCreateCountSQL;
	wndCreateCountSQL.Format("insert into wnd_create_count (rec_id,time,count) values (\'%d\',\'%s\',\'%d\')", rec_id, timeStr,count);
	if (mysql_real_query(&mysql, wndCreateCountSQL, strlen(wndCreateCountSQL))!=0)
	{
		//AfxMessageBox("����������¼ʧ��:" + mouseSQL);
		if(m_bConnected == TRUE)
		{
			RecordFailed();
			//AfxMessageBox("���ڴ���������¼ʧ��");
		}
		return FALSE;
	}
	return TRUE;
}


// ��¼����������
BOOL DBManager::LogCliqueResult(int rec_id, const CTime& time, const CString& logText)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString logSQL;
	logSQL.Format("insert into rec_act_log (rec_id,time,log_text) values (\'%d\',\'%s\',\'%s\')", rec_id, timeStr, logText);
	if (mysql_real_query(&mysql, logSQL, strlen(logSQL))!=0)
	{
		//AfxMessageBox("����������¼ʧ��:" + mouseSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//��¼ʧ��
BOOL DBManager::RecordFailed()
{
	m_iFailCount++;

	if(m_iFailCount > Fail_Threshold)
	{
		//�Ͽ�����
		DisConnect();
		//AfxMessageBox("ȡ������");
		return TRUE;
	}
	return FALSE;
}


//�ж��Ƿ����ӳɹ�
BOOL DBManager::IsConnected()
{
	return m_bConnected;
}

