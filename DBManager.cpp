#include "StdAfx.h"
#include "DBManager.h"
#include <iostream>

using namespace std;

DBManager::DBManager(void)
{
	//m_iFailCount = 0;
}
//断开连接
DBManager::~DBManager(void)
{
	mysql_close(&mysql);
	m_bConnected = FALSE;
}

// 连接数据库
//首先连接192.168.0.1服务器，失败再连接本机，再失败应该是用嵌入式mysql服务器，但是还没开始整
BOOL DBManager::Connect(void)
{
	if (0 != mysql_library_init(0, NULL, NULL)) {
		//AfxMessageBox("装载lib失败");
		return FALSE;
	}else{
		//AfxMessageBox("装载lib成功");
	}
	if (NULL == mysql_init(&mysql)) {
		//AfxMessageBox("连接初始化失败");
		return FALSE;
	}else{
		//AfxMessageBox("连接初始化成功");
	}
	if (0 != mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gb2312")) {
		//AfxMessageBox("设置字符集错误");
		return FALSE;
	}else{
	}

	////首先连接服务器
	if (!mysql_real_connect(&mysql, "192.168.0.1", "root", "kingofcs", "hook_db",3306, NULL, 0))
	{
		////其次连接本机
		//if (!mysql_real_connect(&mysql, "localhost", "root", "kingofcs", "hook_db",3306, NULL, 0))
		//{
		//	AfxMessageBox("连接数据库失败");
		//	m_bConnected = FALSE;
		//	m_iFailCount = 0;
		//	return FALSE;
		//}
		//else
		//{
			//连接本机成功
			//AfxMessageBox("连接失败");
			m_bConnected = FALSE;
			m_iFailCount = 0;
			return FALSE;
		//}	
	}
	else
	{
		//连接服务器成功
		//AfxMessageBox("连接成功");
		m_bConnected = TRUE;
		m_iFailCount = 0;
		return TRUE;
	}
} 

//取消链接
BOOL DBManager::DisConnect(void)
{
	mysql_close(&mysql);
	m_bConnected = FALSE;
	m_iFailCount = 0;

	return TRUE;
}

//创建新的记录
//先在数据库中查找有没有现成的rec_id，有就用，没有就新建
const unsigned int DBManager::NewRecord(TCHAR *computerName, const CTime& time)
{
	//判断是否连接成功
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
		//AfxMessageBox("查询记录表失败");
		if(m_bConnected == TRUE) RecordFailed();
		return 0;
	}
	//找到了记录
	if(row=mysql_fetch_row(result))
	{
		rec_id = atoi(row[0]);
//#if DEBUG
			//CString msg;
			//msg.Format("现有 rec_id = %d",rec_id);
			//AfxMessageBox(msg);
//#endif
		mysql_free_result(result);
	}
	//没找到记录，创建新纪录
	else
	{
		mysql_free_result(result);
		CString strSQL;
		strSQL.Format("insert into rec_com (com_name, rec_time) values (\'%s\',\'%s\')", computerName, date) ;
		if (mysql_real_query(&mysql, strSQL, strlen(strSQL))!=0)
		{
			//AfxMessageBox("添加失败");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
		}
		else
		{
			//重新获得rec_id
			mysql_real_query(&mysql, ch_query, strlen(ch_query));

			MYSQL_RES *result;
			MYSQL_ROW    row;

			if (!(result=mysql_use_result(&mysql)))
			{
				//AfxMessageBox("数据表错误");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}
			//找到了记录
			if(row=mysql_fetch_row(result))
			{
				rec_id = atoi(row[0]);
//#if DEBUG
					//CString msg;
					//msg.Format("新建 rec_id = %d",rec_id);
					//AfxMessageBox(msg);
//#endif
			}
			else
			{
				//AfxMessageBox("无法创建记录");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}

			mysql_free_result(result);
		}
	}

	//mysql_free_result(result);
	
	return rec_id;
}

//创建新的记录
//先在数据库中查找有没有现成的rec_id，有就用，没有就新建
//带有版本号信息
const unsigned int DBManager::NewRecord(TCHAR *computerName, const CTime& time, const CString &version)
{
	//判断是否连接成功
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
		//AfxMessageBox("查询记录表失败");
		if(m_bConnected == TRUE) RecordFailed();
		return 0;
	}
	//找到了记录
	if(row=mysql_fetch_row(result))
	{
		rec_id = atoi(row[0]);
		mysql_free_result(result);
		//找到了纪录，修改记录中的版本号
		CString updateVersionSQL;
		updateVersionSQL.Format("update rec_com set version=\'%s\' where rec_id=\'%d\'", version, rec_id);
		if (mysql_real_query(&mysql, updateVersionSQL, strlen(updateVersionSQL))!=0)
		{
			//AfxMessageBox("添加失败");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
		}
	}
	//没找到记录，创建新纪录
	else
	{
		mysql_free_result(result);
		CString strSQL;
		strSQL.Format("insert into rec_com (com_name, rec_time, version) values (\'%s\',\'%s\',\'%s\')", computerName, date, version) ;
		if (mysql_real_query(&mysql, strSQL, strlen(strSQL))!=0)
		{
			//AfxMessageBox("添加失败");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
		}
		else
		{
			//重新获得rec_id
			mysql_real_query(&mysql, ch_query, strlen(ch_query));

			MYSQL_RES *result;
			MYSQL_ROW    row;

			if (!(result=mysql_use_result(&mysql)))
			{
				//AfxMessageBox("数据表错误");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}
			//找到了记录
			if(row=mysql_fetch_row(result))
			{
				rec_id = atoi(row[0]);
				//#if DEBUG
				//CString msg;
				//msg.Format("新建 rec_id = %d",rec_id);
				//AfxMessageBox(msg);
				//#endif
			}
			else
			{
				//AfxMessageBox("无法创建记录");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}

			mysql_free_result(result);
		}
	}

	//mysql_free_result(result);

	return rec_id;
}

//新窗口记录
//先通过创建时间和rec_id生成wnd_id，然后再写相应的信息
const unsigned int DBManager::NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect)
{
	if(m_bConnected == FALSE)
		return 0;

	int wnd_id;			//窗口标记号

	//以防万一，查询现有wnd_id
	CString create_time = createTime.Format("%Y-%m-%d %H:%M:%S");
	//LPCTSTR title = strTitle;
	CString ch_query;
	ch_query.Format("select wnd_id from rec_wnd where create_time=\'%s\' and rec_id=\'%d\' and init_title=\'%s\'", create_time, rec_id, title);

	mysql_real_query(&mysql, ch_query, strlen(ch_query));

	MYSQL_RES *result;
	MYSQL_ROW    row;

	if (!(result=mysql_use_result(&mysql)))
	{
		//AfxMessageBox("窗口记录查询失败");
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}
	//找到了记录
	if(row=mysql_fetch_row(result))
	{
		wnd_id = atoi(row[0]);
//#if DEBUG
			//CString msg;
			//msg.Format("现有 wnd_id = %d",rec_id);
			//AfxMessageBox(msg);
//#endif
		mysql_free_result(result);
	}
	//没找到记录，创建新纪录
	else
	{
		mysql_free_result(result);

		CString strSQL;
		strSQL.Format("insert into rec_wnd (create_time, rec_id, init_title, classname) values (\'%s\',\'%d\',\'%s\',\'%s\')", create_time, rec_id, title, className) ;
		if (mysql_real_query(&mysql, strSQL, strlen(strSQL))!=0)
		{
			//AfxMessageBox("添加窗口失败");
			if(m_bConnected == TRUE) RecordFailed();
			return NULL;
		}
		else
		{
			//重新获得rec_id
			mysql_real_query(&mysql, ch_query, strlen(ch_query));

			MYSQL_RES *result;
			MYSQL_ROW    row;

			if (!(result=mysql_use_result(&mysql)))
			{
				//AfxMessageBox("数据表错误");
				if(m_bConnected == TRUE) RecordFailed();
				return NULL;
			}
			//找到了记录
			if(row=mysql_fetch_row(result))
			{
				wnd_id = atoi(row[0]);
//#if DEBUG
					//CString msg;
					//msg.Format("新建 wnd_id = %d",rec_id);
					//AfxMessageBox(msg);
//#endif
				mysql_free_result(result);
			}
			else
			{
				mysql_free_result(result);
				if(m_bConnected == TRUE) RecordFailed();
				//AfxMessageBox("无法创建窗口记录");
				return NULL;
			}
		}
	}

	//得到wnd_id后写相关信息表
	CString localSQL;
	localSQL.Format("insert into wnd_local (wnd_id,local_time,x,y,cx,cy) values (\'%d\',\'%s\',\'%d\',\'%d\',\'%d\',\'%d\')", wnd_id,create_time, rect.left, rect.top, rect.right, rect.bottom);
	if (mysql_real_query(&mysql, localSQL, strlen(localSQL))!=0)
	{
		//AfxMessageBox("添加位置失败:" + localSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}

	CString titleSQL;
	titleSQL.Format("insert into wnd_title (wnd_id,title,time) values (\'%d\',\'%s\',\'%s\')", wnd_id, title, create_time);
	if (mysql_real_query(&mysql, titleSQL, strlen(titleSQL))!=0)
	{
		//AfxMessageBox("初始化标题失败:" + titleSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}
	return wnd_id;
}

//窗口激活信息
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
		//AfxMessageBox("添加窗口活动记录失败:" + activeSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}

	return TRUE;

}

//记录窗口切换边
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
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//窗口增加标题
BOOL DBManager::WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString titleSQL;
	titleSQL.Format("insert into wnd_title (wnd_id,title,time) values (\'%d\',\'%s\',\'%s\')", wnd_id, title, timeStr);
	if (mysql_real_query(&mysql, titleSQL, strlen(titleSQL))!=0)
	{
		//AfxMessageBox("添加标题失败:" + titleSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//记录焦点持续时间
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
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//记录焦点持续时间和非焦点持续时间
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
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//记录焦点持续时间和非焦点持续时间
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
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//窗口移动
BOOL DBManager::WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString localSQL;
	localSQL.Format("insert into wnd_local (wnd_id,local_time,x,y,cx,cy) values (\'%d\',\'%s\',\'%d\',\'%d\',\'%d\',\'%d\')", wnd_id,timeStr, rect.left, rect.top, rect.right, rect.bottom);
	if (mysql_real_query(&mysql, localSQL, strlen(localSQL))!=0)
	{
		//AfxMessageBox("添加位置失败:" + localSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//窗口关闭
BOOL DBManager::WindowDestory(unsigned int wnd_id, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString updateSQL;
	updateSQL.Format("update rec_wnd set destory_time=\'%s\' where wnd_id=\'%d\'", timeStr, wnd_id);
	if (mysql_real_query(&mysql, updateSQL, strlen(updateSQL))!=0)
	{
		//AfxMessageBox("添加窗口关闭时间失败:" + updateSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//新的案件信息
BOOL DBManager::NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString keySQL;
	keySQL.Format("insert into wnd_keytype (wnd_id,keytype_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, keyCount, timeStr);
	if (mysql_real_query(&mysql, keySQL, strlen(keySQL))!=0)
	{
		//AfxMessageBox("添加按键记录失败:" + keySQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//新的鼠标点击信息
BOOL DBManager::NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString mouseSQL;
	mouseSQL.Format("insert into wnd_mouseclick (wnd_id,mouse_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, mouseCount, timeStr);
	if (mysql_real_query(&mysql, mouseSQL, strlen(mouseSQL))!=0)
	{
		//AfxMessageBox("添加鼠标点击记录失败:" + mouseSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//记录窗口创建计数
BOOL DBManager::RecordWndCreateCount(int rec_id, const CTime& time, int count)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString wndCreateCountSQL;
	wndCreateCountSQL.Format("insert into wnd_create_count (rec_id,time,count) values (\'%d\',\'%s\',\'%d\')", rec_id, timeStr,count);
	if (mysql_real_query(&mysql, wndCreateCountSQL, strlen(wndCreateCountSQL))!=0)
	{
		//AfxMessageBox("添加鼠标点击记录失败:" + mouseSQL);
		if(m_bConnected == TRUE)
		{
			RecordFailed();
			//AfxMessageBox("窗口创建计数记录失败");
		}
		return FALSE;
	}
	return TRUE;
}


// 记录聚类分析结果
BOOL DBManager::LogCliqueResult(int rec_id, const CTime& time, const CString& logText)
{
	if(m_bConnected == FALSE)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString logSQL;
	logSQL.Format("insert into rec_act_log (rec_id,time,log_text) values (\'%d\',\'%s\',\'%s\')", rec_id, timeStr, logText);
	if (mysql_real_query(&mysql, logSQL, strlen(logSQL))!=0)
	{
		//AfxMessageBox("添加鼠标点击记录失败:" + mouseSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;
}

//记录失败
BOOL DBManager::RecordFailed()
{
	m_iFailCount++;

	if(m_iFailCount > Fail_Threshold)
	{
		//断开连接
		DisConnect();
		//AfxMessageBox("取消链接");
		return TRUE;
	}
	return FALSE;
}


//判断是否连接成功
BOOL DBManager::IsConnected()
{
	return m_bConnected;
}

