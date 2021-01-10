#include "StdAfx.h"
#include "EmbeddedMySql.h"

//#pragma comment(lib, "D:\\MySQL\\Embedded\\DLL\\release\\libmysqld.lib") 

CEmbeddedMySql::CEmbeddedMySql(void):m_bEmbeddedEnable(FALSE)
{

}

CEmbeddedMySql::~CEmbeddedMySql(void)
{

	if(m_bConnected)
	{
		mysql_close(&m_remoteMysql);
	}
	if(m_bEmbeddedEnable)
	{
		mysql_close(&mysql); 
		//mysql_server_end(); 
	}

}

//生成配置文件
BOOL CEmbeddedMySql::GenerateCnf(char* exactPath)
{
	CFile file;
	CString fileName;
	CString path(exactPath);
	CString dataPath;
	dataPath.Format("%sdata",path);
	bool bDataDirectory = true;

	//创建数据文件夹
	if(::CreateDirectory(dataPath, NULL) == 0)
	{
		//创建失败,判断失败原因是不是文件夹已经存在
		DWORD nErrorNo = GetLastError();
		if(nErrorNo != ERROR_ALREADY_EXISTS)
		{
			bDataDirectory = false;
		}
		else
		{
			bDataDirectory = true;
		}
	}

	fileName.Format("%smy.cnf", path);
	try
	{
		file.Open(fileName,CFile::modeCreate|CFile::modeWrite);
		file.SeekToBegin();
		//file.Write(msg, msg.GetLength());
		//写实际内容
		CString context;
		context.Append("[FlowWatcher]\r\n");
		CString dataPath;
		//判断是否有data文件夹
		if(bDataDirectory)
		{
			dataPath.Format("datadir = %sdata\r\n", path);
		}
		else
		{
			dataPath.Format("datadir = %s\r\n", path);
		}
		context.Append(dataPath);
		CString lagPath;
		lagPath.Format("language = %s\r\n",path);
		context.Append(lagPath);
		context.Append("default-character-set=utf8\r\n");
		context.Append("skip-innodb\r\n\r\n");
		context.Append("[libmysqld_client]\r\n");
		context.Append(lagPath);
		context.Append("default-character-set=utf8\r\n");
		file.Write(context, context.GetLength());

		file.Flush();
		file.Close();
		return TRUE;
	}
	catch(CFileException *e)
	{
		//CString str;
		//str.Format("写入失败的原因是:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
		return FALSE;
	}
}

//初始化嵌入式服务器，并连接之
BOOL CEmbeddedMySql::Connect(void)
{
	//应用程序名
	char *server = "EmbeddedMySql";
	//选项
	char option[MAX_PATH];
	//配置文件路径
	char cnf[MAX_PATH];
	

	//获取绝对路径
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH);
	char szDrive[MAX_PATH]={0};
	char szDir[MAX_PATH]={0};
	char szPath[MAX_PATH]={0};
	_tsplitpath(szFileName,szDrive,szDir,NULL,NULL);
	sprintf(option,"--defaults-file=%s%s%s",szDrive,szDir,"my.cnf");
	sprintf(cnf, "%s%s%s",szDrive,szDir,"my.cnf");
	sprintf(path,"%s%s",szDrive,szDir);

	//static char *server_options[] = { "FlowWatcher", "--defaults-file=D:/my.ini" }; 
	char *server_options[] = {server, option};
	int num_elements = sizeof(server_options)/ sizeof(char *); 
	char *server_groups[] = { "FlowWatcher", "libmysqld_client" }; 

	//判断配置文件是否存在，不存在就生成一个，顺便把路径全部整成却对路径
	CFileFind   find;   
	if(!find.FindFile(cnf))
	{
		//  不存在   
		//生成以绝对路径为基础的配置文件	
		if(GenerateCnf(path) == FALSE)
			return FALSE;
	}

	//确保/data存在
	//创建数据文件夹
	CString dataPath;
	dataPath.Format("%sdata",path);
	if(::CreateDirectory(dataPath, NULL) == 0)
	{
		//创建失败,判断失败原因是不是文件夹已经存在
		DWORD nErrorNo = GetLastError();
		if(nErrorNo != ERROR_ALREADY_EXISTS)
		{
			return FALSE;
		}
	}

	if(0 != mysql_server_init(num_elements, server_options, server_groups))
	{
		//初始化服务器失败
		return FALSE;
	}
	//printf("return %ld\n", ret); 
	if(0 == mysql_init(&mysql))
	{
		//初始化失败
		return FALSE;
	}
	if(0 != mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client"))
	{
		//设置客户端失败
		return FALSE;
	}


	if(0 != mysql_options(&mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL))
	{
		//设置为嵌入式失败
		return FALSE;
	}

	if (!mysql_real_connect(&mysql, NULL,NULL,NULL, NULL, 0,NULL,0))
	{
		//连接失败

		//m_bEmbeddedEnable = FALSE;
		return FALSE;
	}
	else
	{
		//连接服务器成功
		//AfxMessageBox("连接成功");
		//m_bEmbeddedEnable = TRUE;
		//return TRUE;
	}

	//TestEmbeddedMySql();
	if(InitEmbeddedDB() == TRUE)
	{
#ifdef _DEBUG
		////创建调试数据文件夹
		//CString logPath;
		//dataPath.Format("%slog",path);
		//if(::CreateDirectory(dataPath, NULL) == 0)
		//{
		//	//创建失败,判断失败原因是不是文件夹已经存在
		//	DWORD nErrorNo = GetLastError();
		//	if(nErrorNo != ERROR_ALREADY_EXISTS)
		//	{
		//		//return FALSE;
		//		AfxMessageBox("创建日志文件夹失败");
		//	}
		//}
		//if(ExportAllTable() == FALSE)
		//	AfxMessageBox("导出所有数据失败");
		//if(CommitNewRecord() == TRUE)
		//	AfxMessageBox("传输数据成功");
		//else
		//	AfxMessageBox("传输数据失败");
		//if(ShowLocalDBArchitecture() == FALSE)
		//	AfxMessageBox("显示结构失败");
#endif // _DEBUG
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
	//ConnectRemoteServer();
}	

BOOL CEmbeddedMySql::ConnectExistDB()
{
	if(0 == mysql_init(&mysql))
	{
		//初始化失败
		return FALSE;
	}
	if(0 != mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client"))
	{
		//设置客户端失败
		return FALSE;
	}
	if(0 != mysql_options(&mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL))
	{
		//设置为嵌入式失败
		return FALSE;
	}

	if (!mysql_real_connect(&mysql, NULL,NULL,NULL, NULL, 0,NULL,0))
	{
		//连接失败

		//m_bEmbeddedEnable = FALSE;
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

//初始化嵌入式数据库结构
//通过读取本地sql文件和从远程主机下载sql运行两种手段
//确保数据库结构建立成功
BOOL CEmbeddedMySql::InitEmbeddedDB()
{
	//毛球了！直接把SQL写到这。嵌入式mysql头调用source整求不起！我靠！
	static CString initSql[] = {
		"CREATE DATABASE IF NOT EXISTS `local_hook_db` CHARACTER SET 'utf8' COLLATE 'utf8_general_ci';",
		"USE `local_hook_db`;",
		"CREATE TABLE IF NOT EXISTS `rec_com` (`rec_id` int(10) unsigned NOT NULL AUTO_INCREMENT,`com_name` char(20) CHARACTER SET utf8 NOT NULL,`rec_time` date NOT NULL,`version` varchar(20) DEFAULT NULL,`flag` tinyint(4) DEFAULT '0', `remote_rec_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`rec_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;",
		"CREATE TABLE IF NOT EXISTS `rec_act_log` (`act_id` int(10) unsigned NOT NULL AUTO_INCREMENT,`log_text` text,`rec_id` int(11) NOT NULL,`time` datetime NOT NULL,`flag` tinyint(4) DEFAULT '0',`remote_act_time` int(10) unsigned DEFAULT NULL,`remote_rec_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`act_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;",
		"CREATE TABLE IF NOT EXISTS `rec_wnd` (`wnd_id` int(11) NOT NULL AUTO_INCREMENT,`create_time` datetime NOT NULL,`rec_id` int(11) NOT NULL,`init_title` varchar(1000) CHARACTER SET utf8 NOT NULL,`destory_time` datetime DEFAULT NULL,`classname` varchar(500) DEFAULT NULL,`flag` tinyint(4) DEFAULT '0',`remote_wnd_id` int(10) unsigned DEFAULT NULL,`remote_rec_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`wnd_id`),KEY `destory_time` (`destory_time`),KEY `rec_id` (`rec_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=gbk;",
		"CREATE TABLE IF NOT EXISTS `wnd_act_time` (`id` int(10) unsigned NOT NULL AUTO_INCREMENT,`wnd_id` int(10) unsigned NOT NULL,`last_time` int(10) unsigned NOT NULL,`focus` tinyint(1) NOT NULL,`focus_balance` double(15,3) unsigned NOT NULL,`last_time2` int(10) unsigned DEFAULT NULL,`avg_key` double(15,3) unsigned DEFAULT NULL,`avg_mouse` double(15,3) unsigned DEFAULT NULL,`flag` tinyint(4) DEFAULT '0',  `remote_id` int(10) unsigned DEFAULT NULL,`remote_wnd_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,`time` datetime NOT NULL,PRIMARY KEY (`id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;",
		"CREATE TABLE IF NOT EXISTS `wnd_keytype` (`keytype_id` int(11) NOT NULL AUTO_INCREMENT,`wnd_id` int(11) NOT NULL,`keytype_count` int(11) NOT NULL,`time` datetime NOT NULL,`flag` tinyint(4) DEFAULT '0',  `remote_keytype_id` int(10) unsigned DEFAULT NULL,`remote_wnd_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`keytype_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;",
		"CREATE TABLE IF NOT EXISTS `wnd_local` (`local_id` int(11) NOT NULL AUTO_INCREMENT,`wnd_id` int(11) NOT NULL,`local_time` datetime NOT NULL,`x` int(11) NOT NULL,`y` int(11) NOT NULL,`cx` int(11) NOT NULL,`cy` int(11) NOT NULL,`flag` tinyint(4) DEFAULT '0',  `remote_local_id` int(10) unsigned DEFAULT NULL,`remote_wnd_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`local_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 AVG_ROW_LENGTH=84;",
		"CREATE TABLE IF NOT EXISTS `wnd_mouseclick` (`mouse_id` int(11) NOT NULL AUTO_INCREMENT,`wnd_id` int(11) NOT NULL,`mouse_count` int(11) NOT NULL,`time` datetime NOT NULL,`flag` tinyint(4) DEFAULT '0',  `remote_mouse_id` int(10) unsigned DEFAULT NULL,`remote_wnd_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`mouse_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 AVG_ROW_LENGTH=58;",
		"CREATE TABLE IF NOT EXISTS `wnd_title` (`title_id` int(11) NOT NULL AUTO_INCREMENT,`wnd_id` int(10) unsigned NOT NULL,`title` varchar(1000) CHARACTER SET utf8 NOT NULL,`time` datetime NOT NULL,`flag` tinyint(4) DEFAULT '0',  `remote_title_id` int(10) unsigned DEFAULT NULL,`remote_wnd_id` int(10) unsigned DEFAULT NULL,`commit_time` datetime DEFAULT NULL,PRIMARY KEY (`title_id`)) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;",
		"CREATE TABLE IF NOT EXISTS `update_record` (`id` int(10) unsigned NOT NULL AUTO_INCREMENT,`version` int(10) unsigned NOT NULL,`time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',`flag` tinyint(4) DEFAULT '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=utf8;",
		"CREATE TABLE IF NOT EXISTS `wnd_switch` (`id` int(10) unsigned NOT NULL AUTO_INCREMENT,`from_wnd_id` int(10) unsigned NOT NULL,`to_wnd_id` int(10) unsigned NOT NULL,`time` datetime NOT NULL, `flag` tinyint(4) DEFAULT '0', `commit_time` datetime DEFAULT NULL, `remote_id` int(10) unsigned DEFAULT NULL, `rec_id` int(10) unsigned NOT NULL, `remote_rec_id` int(10) unsigned DEFAULT NULL,PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=utf8;"
};
	int i;
	for(i=0; i<12; i++)
	{
		if (mysql_real_query(&mysql, initSql[i], strlen(initSql[i]))!=0)
		{
			//初始化失败了...
			m_bEmbeddedEnable = FALSE;
			break;
		}
	}
	if(i == 12)
	{
		m_bEmbeddedEnable = TRUE;
		return TRUE;
	}

	
	//用远程数据库上的sql初始化
	//先要连接远程服务器
	if(ConnectRemoteServer() == TRUE)
	{
		CString initSQL;
		initSQL.Format("select text from db_version_sql where version=1") ;
		if (mysql_real_query(&m_remoteMysql, initSQL, strlen(initSQL))!=0)
		{
			//AfxMessageBox("添加失败");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
			return FALSE;
		}	

		MYSQL_RES *result;
		MYSQL_ROW    row;
		if (!(result=mysql_use_result(&m_remoteMysql)))
		{
			//AfxMessageBox("数据表错误");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
			return FALSE;
		}
		//找到了记录
		while ((row = mysql_fetch_row(result)))
		{
			//得到一行初始化SQL语句
			//用本地MYSQL连接执行之
			char *sqlStr = row[0];

			if (mysql_real_query(&mysql, sqlStr, strlen(sqlStr))!=0)
			{
				//有一句没成功，都算初始化失败
				m_bEmbeddedEnable = FALSE;
				mysql_free_result(result);
				DisConnect();
				return FALSE;
			}
		}
		//初始化成功
		mysql_free_result(result);
		m_bEmbeddedEnable = TRUE;
		DisConnect();
		return TRUE;
	}
}

//断开嵌入式连接
BOOL CEmbeddedMySql::FreeEmbededConnect(void)
{
	mysql_close(&mysql);
	return TRUE;
}

//连接远程主机
BOOL CEmbeddedMySql::ConnectRemoteServer()
{

	if (0 != mysql_library_init(0, NULL, NULL)) {
		//AfxMessageBox("装载lib失败");
		return FALSE;
	}else{
		//AfxMessageBox("装载lib成功");
	}
	if (NULL == mysql_init(&m_remoteMysql)) {
		//AfxMessageBox("连接初始化失败");
		return FALSE;
	}else{
		//AfxMessageBox("连接初始化成功");
	}
	if (0 != mysql_options(&m_remoteMysql, MYSQL_SET_CHARSET_NAME, "gb2312")) {
		//AfxMessageBox("设置字符集错误");
		return FALSE;
	}else{
	}

	////首先连接服务器
	if (!mysql_real_connect(&m_remoteMysql, "192.168.0.1", "root", "kingofcs", "hook_db",3306, NULL, 0))
	{
		m_bConnected = FALSE;
		m_iFailCount = 0;
		return FALSE;
		//}	
	}
	else
	{
		m_bConnected = TRUE;
		m_iFailCount = 0;
		return TRUE;
	}
}

//取消链接
BOOL CEmbeddedMySql::DisConnect(void)
{
	mysql_close(&m_remoteMysql);
	m_bConnected = FALSE;
	m_iFailCount = 0;
	return TRUE;
}						

//判断是否连接成功
BOOL CEmbeddedMySql::IsConnected()
{
	//由于是嵌入式的，所以返回嵌入式连接
	return m_bEmbeddedEnable;
	//return m_bConnected;
}


//创建新的记录
const unsigned int CEmbeddedMySql::NewRecord(TCHAR *computerName, const CTime& time)
{
	//判断是否连接成功
	if(m_bEmbeddedEnable == FALSE)
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
	//return 0;
}	

//带有版本号的新纪录
const unsigned int CEmbeddedMySql::NewRecord(TCHAR *computerName, const CTime& time, const CString& version)
{
	//判断是否连接成功
	if(m_bEmbeddedEnable == FALSE)
		return 0;

	int rec_id = 0;
	CString date = time.Format("%Y-%m-%d");
	CString ch_query;
	ch_query.Format("select rec_id, version from rec_com where com_name=\'%s\' and rec_time=\'%s\'", computerName, date);

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
		CString oldVersion(row[1]);
		mysql_free_result(result);
		//记录中版本号和现在的版本号不一样，就修改下
		if(oldVersion != version)
		{
			CString updateVersionSQL;
			updateVersionSQL.Format("update rec_com set version=\'%s\', flag=3 where rec_id=\'%d\'", version, rec_id);
			if (mysql_real_query(&mysql, updateVersionSQL, strlen(updateVersionSQL))!=0)
			{
				//AfxMessageBox("添加失败");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}
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
const unsigned int CEmbeddedMySql::NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect)
{
	if(m_bEmbeddedEnable == FALSE)
		return 0;

	int wnd_id;			//窗口标记号

	//以防万一，查询现有wnd_id
	CString create_time = createTime.Format("%Y-%m-%d %H:%M:%S");
	//LPCTSTR title = strTitle;
	CString ch_query;
	ch_query.Format("select wnd_id from rec_wnd where create_time=\'%s\' and rec_id=\'%d\' and init_title=\'%s\'", create_time, rec_id, title);
	//ch_query.Format("select wnd_id from rec_wnd where create_time=\'%s\' and rec_id=\'%d\'", create_time, rec_id);

	if(mysql_real_query(&mysql, ch_query, strlen(ch_query))!=0)
	{
		if(m_bConnected == TRUE) RecordFailed();
		return NULL;
	}

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
//已经不会有wnd_active那个表了
BOOL CEmbeddedMySql::WindowActive(unsigned int wnd_id, bool flag, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//int bit = (int)flag;
	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString activeSQL;
	//activeSQL.Format("insert into wnd_active (wnd_id,flag,time) values (\'%d\',b\'%d\',\'%s\')", wnd_id, bit, timeStr);
	//if (mysql_real_query(&mysql, activeSQL, strlen(activeSQL))!=0)
	//{
	//	//AfxMessageBox("添加窗口活动记录失败:" + activeSQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}

	return TRUE;
}

//添加窗口标题
BOOL CEmbeddedMySql::WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString titleSQL;
	//titleSQL.Format("insert into wnd_title (wnd_id,title,time) values (\'%d\',\'%s\',\'%s\')", wnd_id, title, timeStr);
	//if (mysql_real_query(&mysql, titleSQL, strlen(titleSQL))!=0)
	//{
	//	//AfxMessageBox("添加标题失败:" + titleSQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}
	return TRUE;
}		

//窗口移动
BOOL CEmbeddedMySql::WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect)
{
	if(m_bEmbeddedEnable == FALSE)
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
BOOL CEmbeddedMySql::WindowDestory(unsigned int wnd_id, const CTime& time)
{
	if(m_bEmbeddedEnable == FALSE)
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

//窗口按键消息
BOOL CEmbeddedMySql::NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString keySQL;
	//keySQL.Format("insert into wnd_keytype (wnd_id,keytype_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, keyCount, timeStr);
	//if (mysql_real_query(&mysql, keySQL, strlen(keySQL))!=0)
	//{
	//	//AfxMessageBox("添加按键记录失败:" + keySQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}
	return TRUE;
}

//鼠标点击消息
BOOL CEmbeddedMySql::NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString mouseSQL;
	//mouseSQL.Format("insert into wnd_mouseclick (wnd_id,mouse_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, mouseCount, timeStr);
	//if (mysql_real_query(&mysql, mouseSQL, strlen(mouseSQL))!=0)
	//{
	//	//AfxMessageBox("添加鼠标点击记录失败:" + mouseSQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}
	return TRUE;
}						

//记录焦点持续时间
BOOL CEmbeddedMySql::WindowAttTime(unsigned int wnd_id, int lastTime, bool focus, double attBalance)
{
	if(m_bEmbeddedEnable == FALSE)
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

//记录窗口切换边
BOOL CEmbeddedMySql::LogWindowSwitch(int rec_id,int from_wnd_id, int to_wnd_id, const CTime &time)
{
	if(m_bEmbeddedEnable == FALSE)
		return FALSE;
	if(from_wnd_id <0 || to_wnd_id<0)
		return FALSE;

	//CTime time = CTime::GetCurrentTime();
	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");

	CString switchSQL;
	switchSQL.Format("insert into wnd_switch (from_wnd_id, to_wnd_id, time, rec_id) values (\'%d\',\'%d\',\'%s\',\'%d\')", from_wnd_id, to_wnd_id, timeStr,rec_id);
	if (mysql_real_query(&mysql, switchSQL, strlen(switchSQL))!=0)
	{
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;

}

//记录焦点持续时间和非焦点持续时间
BOOL CEmbeddedMySql::WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, const CTime &time)
{
	if(m_bEmbeddedEnable == FALSE)
		return 0;

	if(wnd_id <= 0 || attBalance == 0)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");

	CString atttimeSQL;
	atttimeSQL.Format("insert into wnd_act_time (wnd_id,last_time,last_time2,focus, focus_balance) values (\'%d\',\'%d\',\'%d\',b\'%d\',\'%f\', \'%s\')", wnd_id, lastTime1, lastTime2, focus,attBalance, timeStr);
	if (mysql_real_query(&mysql, atttimeSQL, strlen(atttimeSQL))!=0)
	{
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//记录焦点持续时间和交互信息
BOOL CEmbeddedMySql::WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, float avgKey, float avgMouse, const CTime &time)
{

	if(m_bEmbeddedEnable == FALSE)
		return 0;

	if(wnd_id <= 0 || attBalance == 0)
		return 0;

	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");

	CString atttimeSQL;
	atttimeSQL.Format("insert into wnd_act_time (wnd_id,last_time,last_time2,focus, focus_balance, avg_key, avg_mouse, time) values (\'%d\',\'%d\',\'%d\',b\'%d\',\'%f\',\'%f\',\'%f\', \'%s\')", wnd_id, lastTime1, lastTime2, focus,attBalance, avgKey, avgMouse, timeStr);
	if (mysql_real_query(&mysql, atttimeSQL, strlen(atttimeSQL))!=0)
	{
		//AfxMessageBox("记录窗口焦点持续时间失败");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

// 记录聚类分析结果
BOOL CEmbeddedMySql::LogCliqueResult(int rec_id, const CTime& time, const CString& logText)
{
	if(m_bEmbeddedEnable == FALSE)
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

//嵌入式记录失败？
BOOL CEmbeddedMySql::RecordFailed()
{
	return FALSE;
}


//记录一定时间内窗口创建数量
//没用，去掉了
BOOL CEmbeddedMySql::RecordWndCreateCount(int rec_id, const CTime& time, int count)
{
	return TRUE;
}

//放弃本地数据库
BOOL CEmbeddedMySql::DropLocalDB()
{
	if(m_bEmbeddedEnable == FALSE)
		return 0;

	CString dropSQL;
	dropSQL.Format("drop database local_hook_db");
	if (mysql_real_query(&mysql, dropSQL, strlen(dropSQL))!=0)
	{
		//AfxMessageBox("添加鼠标点击记录失败:" + mouseSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;

	//CString dataPath;
	//dataPath.Format("%sdata",path);

	////删除文件夹中所有文件
	//{
	//	//WIN32_FIND_DATA ffd;
	//	//LARGE_INTEGER filesize;
	//	//TCHAR szDir[MAX_PATH];
	//	//TCHAR FileName[MAX_PATH];
	//	//size_t length_of_arg;
	//	//HANDLE hFind = INVALID_HANDLE_VALUE;
	//	//DWORD dwError=0;

	//	//// Check that the input path plus 3 is not longer than MAX_PATH.
	//	//// Three characters are for the "\*" plus NULL appended below.
	//	//StringCchLength(sPath, MAX_PATH, &length_of_arg);
	//	//if (length_of_arg > (MAX_PATH - 3))
	//	//{
	//	//	return FALSE;
	//	//}

	//	//// Prepare string for use with FindFile functions. First, copy the
	//	//// string to a buffer, then append '\*' to the directory name.
	//	//StringCchCopy(szDir, MAX_PATH,sPath);
	//	//StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	//	//StringCchCopy(FileName, MAX_PATH,sPath);
	//	//StringCchCat(FileName, MAX_PATH,TEXT("\\"));

	//	//// Find the first file in the directory.
	//	//hFind = FindFirstFile(szDir, &ffd);
	//	//if (INVALID_HANDLE_VALUE == hFind) 
	//	//{
	//	//	return FALSE;
	//	//} 
	//	//// List all the files in the directory with some info about them.
	//	//do
	//	//{
	//	//	_tcscat(FileName,ffd.cFileName);
	//	//	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	//	//	{
	//	//		DeleteDirectory(FileName);
	//	//		RemoveDirectory(FileName);
	//	//		StringCchCopy(FileName, MAX_PATH,sPath);
	//	//		StringCchCat(FileName, MAX_PATH,TEXT("\\"));
	//	//	}
	//	//	else
	//	//	{
	//	//		DeleteFile(FileName);
	//	//		StringCchCopy(FileName, MAX_PATH,sPath);
	//	//		StringCchCat(FileName, MAX_PATH,TEXT("\\"));
	//	//	}
	//	//}
	//	//while (FindNextFile(hFind, &ffd) != 0);
	//	//FindClose(hFind);
	//	//return dwError;
	//}
	////删除data 文件夹
	//if(::RemoveDirectory(dataPath) == 0)
	//{
	//	#ifdef _DEBUG
	//	
	//				DWORD nErrorNo = GetLastError ( ); // 得到错误代码
	//				LPSTR lpBuffer;    
	//				FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER  | 
	//					FORMAT_MESSAGE_IGNORE_INSERTS  | 
	//					FORMAT_MESSAGE_FROM_SYSTEM,
	//					NULL,
	//					nErrorNo, // 此乃错误代码，通常在程序中可由 GetLastError()得之
	//					LANG_NEUTRAL,
	//					(LPTSTR) & lpBuffer,
	//					0 ,
	//					NULL );
	//				//CString strErrorCause=lpBuffer?_T(lpBuffer):_T( " Sorry, cannot find this error info. " );
	//				CString strErrorCause = lpBuffer? CString(lpBuffer) : CString("can not find error info");
	//	//			UpdateData (FALSE);
	//				//  Free the buffer. 
	//				LocalFree (lpBuffer);
	//				AfxMessageBox(strErrorCause);
	//	#endif // _DEBUG

	//				return FALSE;
	//}

	////重新创建数据文件夹
	//if(::CreateDirectory(dataPath, NULL) == 0)
	//{
	//	//创建失败,判断失败原因是不是文件夹已经存在
	//	DWORD nErrorNo = GetLastError();
	//	if(nErrorNo != ERROR_ALREADY_EXISTS)
	//	{
	//		return FALSE;
	//	}
	//	else
	//	{
	//		return TRUE;
	//	}
	//}
	//return FALSE;

}

//传递新生成的记录
//////////////////////////////////////////////////////////////////////////
//该函数级联传输数据，保证表间关系
//该函数应该是以尽力而为为终止，中途出错不能中断运行
//////////////////////////////////////////////////////////////////////////
BOOL CEmbeddedMySql::CommitNewRecord()
{
	//先连接远程服务器
	if(m_bConnected == FALSE)
	{
		if(ConnectRemoteServer() == FALSE)
		{
			//没连接上，就退出吧
			mysql_close(&m_remoteMysql);
			return FALSE;
		}
	}

	CTime time = CTime::GetCurrentTime();
	CString date = time.Format("%Y-%m-%d");
	CString selectSQL;
	//选出那些从来没有提交的记录
	selectSQL.Format("select rec_id,com_name,rec_time,version,flag,remote_rec_id from rec_com where flag!=2 or rec_time=\'%s\'", date);
	//特殊情况，要求重新传递所有没有处理的窗口
	//selectSQL.Format("select rec_id,com_name,rec_time,version,flag,remote_rec_id from rec_com ");

	if(0 != mysql_real_query(&mysql, selectSQL, strlen(selectSQL)))
	{
		//一个记录都没有得到，退出
		return FALSE;		
	}
	MYSQL_RES *rec_result;
	MYSQL_ROW    row;
	if (!(rec_result=mysql_use_result(&mysql)))
	{
		//一个记录都没有得到，退出
		DisConnect();
		return FALSE;
	}
	while(row=mysql_fetch_row(rec_result))
	{
		BOOL bRecInfoSended = TRUE;				//该记录的所有信息都传输成功
		//每个记录每个记录地处理
		CString local_rec_id(row[0]);
		CString com_name(row[1]);
		CString rec_time(row[2]);
		//CString version(row[3]);
		CString version = getNullableValue(row[3]);
		int flag = atoi(row[4]);
		//得到远程id号
		CString remote_rec_id = getNullableValue(row[5]);
		CTime time = CTime::GetCurrentTime();
		CString commit_time = time.Format("%Y-%m-%d %H:%M:%S");

		//如果是个全新的记录，就往服务器写
		if(flag == 0 || remote_rec_id == "NULL")
		{
			//向服务器写信息
			CString inserRec;
			inserRec.Format("insert into rec_com (com_name, rec_time, version, local_rec_id, commit_time, commit_step) \
							values (\'%s\',\'%s\',%s,%s,\'%s\',\'%s\')", com_name, rec_time, version, local_rec_id, commit_time, "uncomplete") ;
			if(0 != mysql_real_query(&m_remoteMysql, inserRec, strlen(inserRec)))
			{
				//DisConnect();
				//return FALSE;		

				LogBugRemote(com_name,inserRec, "query");
				//失败了就继续传输下一个记录
				bRecInfoSended = FALSE;
				continue;
			}

			//更新本地表
			CString getRemoteID;
			//需要本地记录号，提交时间和计算机名才能唯一确定一个远程记录号
			getRemoteID.Format("select rec_id from rec_com where local_rec_id=%s and commit_time=\'%s\' and com_name=\'%s\'", local_rec_id, commit_time, com_name);
			if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
			{
				//DisConnect();
				//return FALSE;		
				//失败了就继续传输下一个记录
				LogBugRemote(com_name, getRemoteID, "query");
				bRecInfoSended = FALSE;
				continue;
			}
			MYSQL_RES *result;
			MYSQL_ROW    row;
			if (!(result=mysql_use_result(&m_remoteMysql)))
			{
				//DisConnect();
				//return FALSE;
				//失败了就继续传输下一个记录
				LogBugRemote(com_name, getRemoteID, "use result");
				bRecInfoSended = FALSE;
				continue;
			}

			if(row=mysql_fetch_row(result))
			{
				CString r_rec_id = CString(row[0]);
				remote_rec_id = r_rec_id;
				//只有新纪录才更新表
				CString update;
				update.Format("update rec_com set remote_rec_id=%s, flag=1, commit_time=\'%s\' \
							  where rec_id=%s", remote_rec_id, commit_time, local_rec_id);
				if(0 != mysql_real_query(&mysql, update, strlen(update)))
				{
					//DisConnect();
					//return FALSE;	

					//失败了就继续传输下一个记录
					LogBugRemote(com_name, update, "query");
					bRecInfoSended = FALSE;
					mysql_free_result(result);
					continue;
				}
			}
			else
			{
				//DisConnect();
				//return FALSE;
				//失败了就继续传输下一个记录
				LogBugRemote(com_name, getRemoteID, "fetch row");
				bRecInfoSended = FALSE;
				mysql_free_result(result);
				continue;
			}
			mysql_free_result(result);
		}
		else if(flag == 3)
		{
			//本地版本更新过，向服务器写新的版本信息
			CString updateVersion;
			updateVersion.Format("update rec_com set version=%s where rec_id=%s", version, remote_rec_id);
			if(0 != mysql_real_query(&m_remoteMysql, updateVersion, strlen(updateVersion)))
			{
				//DisConnect();
				//return FALSE;		
				//版本更新失败，不影响后面的传输
				LogBugRemote(com_name, updateVersion, "query");
				bRecInfoSended = FALSE;
			}
		}
		
		//传输rec_wnd表
		CString selectLocalWnd;
		selectLocalWnd.Format("select wnd_id,create_time,init_title,destory_time,classname,flag,remote_wnd_id from rec_wnd where rec_id=%s", local_rec_id);
		if(0 != mysql_real_query(&mysql, selectLocalWnd, strlen(selectLocalWnd)))
		{
			//DisConnect();
			//return FALSE;		
			
			//一个窗口都没有读出来，直接进入下一个记录
			LogBugRemote(com_name, selectLocalWnd, "query");
			bRecInfoSended = FALSE;
			continue;
		}

		MYSQL_RES *wnd_result;
		MYSQL_ROW    row;
		if (!(wnd_result=mysql_use_result(&mysql)))
		{
			//DisConnect();
			//return FALSE;
			LogBugRemote(com_name, selectLocalWnd, "use result");
			bRecInfoSended = FALSE;
			continue;
		}
		//rec_wnd也是一个级联上级的表，所以也必须等与之相关的所有表都更新后才能更改其标志
		while(row=mysql_fetch_row(wnd_result))
		{
			CString local_wnd_id(row[0]);
			CString create_time(row[1]);
			CString init_title(row[2]);
			//CString destory_time(row[3]);
			CString destory_time = getNullableValue(row[3]);
			//CString classname(row[4]);
			CString classname = getNullableValue(row[4]);
			int flag = atoi(row[5]);
			CString remote_wnd_id = getNullableValue(row[6]);
			
			//只有从来没被处理过的窗口才需要在服务器新插入数据
			if(flag == 0)
			{
				CString inserRecWnd;
				inserRecWnd.Format("insert into rec_wnd (create_time, rec_id, init_title, destory_time, classname, local_wnd_id, local_rec_id, commit_time)\
								   values (\'%s\',%s,\'%s\',%s,%s,%s,%s,\'%s\')", \
								   create_time, remote_rec_id, init_title, destory_time, classname, local_wnd_id, local_rec_id, commit_time) ;
				if(0 != mysql_real_query(&m_remoteMysql, inserRecWnd, strlen(inserRecWnd)))
				{
					//DisConnect();
					//return FALSE;		
					LogBugRemote(com_name, inserRecWnd, "query");
					bRecInfoSended = FALSE;
					continue;
				}

				//更新本地表
				CString getRemoteID;
				//需要本地窗口号,本地记录号,提交时间,初始化标题才能尽量唯一确定一个远程窗口号
				getRemoteID.Format("select wnd_id from rec_wnd where local_wnd_id=%s and local_rec_id=%s and commit_time=\'%s\' and init_title=\'%s\'", \
					local_wnd_id, local_rec_id, commit_time, init_title);
				if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
				{
					//DisConnect();
					//return FALSE;		

					//向服务器记录失败，接着搞下一个
					LogBugRemote(com_name, getRemoteID, "query");
					bRecInfoSended = FALSE;
					continue;
				}
				MYSQL_RES *result;
				MYSQL_ROW    row;
				if (!(result=mysql_use_result(&m_remoteMysql)))
				{
					//DisConnect();
					//return FALSE;

					//记录失败，搞下一个
					LogBugRemote(com_name, getRemoteID, "use result");
					bRecInfoSended = FALSE;
					continue;
				}

				if(row=mysql_fetch_row(result))
				{
					remote_wnd_id = CString(row[0]);
					//只有从来没有进行过更新的窗口才需要更新这些东西
					CString update;
					update.Format("update rec_wnd set flag=1, remote_wnd_id=%s, remote_rec_id=%s, commit_time=\'%s\' \
								  where wnd_id=%s", remote_wnd_id, remote_rec_id, commit_time, local_wnd_id);
					if(0 != mysql_real_query(&mysql, update, strlen(update)))
					{
						//DisConnect();
						//return FALSE;		
						LogBugRemote(com_name, update, "query");
						bRecInfoSended = FALSE;
						mysql_free_result(result);
						continue;
					}
				}
				else
				{
					//DisConnect();
					//return FALSE;
					LogBugRemote(com_name, getRemoteID, "fetch row");
					bRecInfoSended = FALSE;
					mysql_free_result(result);
					continue;
				}
				mysql_free_result(result);
			}


			//////////////////////////////////////////////////////////////////////////
			//下面根据得到的remote_wnd_id更新其余窗口相关信息
			//////////////////////////////////////////////////////////////////////////

			BOOL bAllTablesSended = TRUE;				//所有表都传输成功的标志

			//////////////////////////////////////////////////////////////////////////
			//传输wnd_act_time表
			//////////////////////////////////////////////////////////////////////////
Send_table1:

			CString selectLocalWndActTime;
			selectLocalWndActTime.Format("select id, last_time, focus, focus_balance, last_time2, avg_key, avg_mouse,time from wnd_act_time \
								  where wnd_id=%s and flag=0", local_wnd_id);
			if(0 != mysql_real_query(&mysql, selectLocalWndActTime, strlen(selectLocalWndActTime)))
			{
				//DisConnect();
				//return FALSE;		

				//一个表都没有读出来，接着读后面的表
				//虽然很不想用goto，但是这里用goto是最方便的
				LogBugRemote(com_name, selectLocalWndActTime, "query");
				bAllTablesSended = FALSE;
				goto Send_table2;
			}
			MYSQL_RES *wnd_act_result;
			MYSQL_ROW    row;
			
			if (!(wnd_act_result=mysql_use_result(&mysql)))
			{
				//DisConnect();
				//return FALSE;
				LogBugRemote(com_name, selectLocalWndActTime, "use result");
				bAllTablesSended = FALSE;
				goto Send_table2;
			}
			while(row=mysql_fetch_row(wnd_act_result))
			{
				CString local_id(row[0]);
				CString last_time(row[1]);
				CString focus(row[2]);
				CString focus_balance(row[3]);
				CString last_time2 = getNullableValue(row[4]);
				CString avg_key = getNullableValue(row[5]);
				CString avg_mouse = getNullableValue(row[6]);
				CString time = getNullableValue(row[7]);

				CString inserWndActTime;
				inserWndActTime.Format("insert into wnd_act_time (wnd_id, last_time, focus, focus_balance, last_time2, avg_key, avg_mouse, local_id, local_wnd_id, commit_time,time)\
								values (%s,\'%s\',%s,%s,%s,%s,%s,%s,%s,\'%s\',%s)", \
								remote_wnd_id, last_time, focus, focus_balance, last_time2, avg_key, avg_mouse, local_id, local_wnd_id, commit_time, time) ;
				if(0 != mysql_real_query(&m_remoteMysql, inserWndActTime, strlen(inserWndActTime)))
				{
/*					DisConnect();
					return FALSE;	*/	
					//还有下一个表
					LogBugRemote(com_name, inserWndActTime, "query");
					bAllTablesSended = FALSE;
					continue;
				}

				CString getRemoteID;
				//需要本地id，本地窗口号，提交时间，avg_mouse才能唯一确定一个记录
				getRemoteID.Format("select id from wnd_act_time where wnd_id=%s and local_id=%s and local_wnd_id=%s and commit_time=\'%s\'",\
					remote_wnd_id, local_id, local_wnd_id, commit_time);
				if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
				{
/*					DisConnect();
					return FALSE;	*/	
					LogBugRemote(com_name, getRemoteID, "query");
					bAllTablesSended = FALSE;
					continue;
				}
				MYSQL_RES *result;
				MYSQL_ROW    row;
				if (!(result=mysql_use_result(&m_remoteMysql)))
				{
					//DisConnect();
					//return FALSE;
					LogBugRemote(com_name, getRemoteID, "use result");
					bAllTablesSended = FALSE;
					continue;
				}
				if(row=mysql_fetch_row(result))
				{
					CString remote_id = CString(row[0]);
					CString update;
					update.Format("update wnd_act_time set flag=1, remote_id=%s, remote_wnd_id=%s, commit_time=\'%s\' \
								  where id=%s", remote_id, remote_wnd_id, commit_time, local_id);
					if(0 != mysql_real_query(&mysql, update, strlen(update)))
					{
						//DisConnect();
						//return FALSE;		
						LogBugRemote(com_name, update, "query");
						bAllTablesSended = FALSE;
						mysql_free_result(result);
						continue;
					}
				}
				else
				{
					//DisConnect();
					//return FALSE;
					LogBugRemote(com_name, getRemoteID, "fetch row");
					bAllTablesSended = FALSE;
					mysql_free_result(result);
					continue;
				}
				mysql_free_result(result);
			}//wnd_act_time处理结束
			mysql_free_result(wnd_act_result);

			//////////////////////////////////////////////////////////////////////////
			//传输wnd_local
			//////////////////////////////////////////////////////////////////////////
Send_table2:

			CString selectLocalWndLocal;
			selectLocalWndLocal.Format("select local_id, local_time, x, y, cx, cy from wnd_local\
										 where wnd_id=%s and flag=0", local_wnd_id);
			if(0 != mysql_real_query(&mysql, selectLocalWndLocal, strlen(selectLocalWndLocal)))
			{
/*				DisConnect();
				return FALSE;	*/	
				LogBugRemote(com_name, selectLocalWndLocal, "query");
				bAllTablesSended = FALSE;
				goto Send_table3;
			}
			MYSQL_RES *local_result;
//			MYSQL_ROW    row;
			if (!(local_result=mysql_use_result(&mysql)))
			{
				//DisConnect();
				//return FALSE;
				LogBugRemote(com_name, selectLocalWndLocal, "use result");
				bAllTablesSended = FALSE;
				goto Send_table3;
			}
			while(row=mysql_fetch_row(local_result))
			{
				CString local_local_id(row[0]);
				CString local_time(row[1]);
				CString x(row[2]);
				CString y(row[3]);
				CString cx(row[4]);
				CString cy(row[5]);

				CString inserWndLocal;
				inserWndLocal.Format("insert into wnd_local (wnd_id, local_time, x, y, cx, cy, local_local_id, local_wnd_id, commit_time)\
									   values (%s,\'%s\',%s,%s,%s,%s,%s,%s,\'%s\')", \
									   remote_wnd_id, local_time, x, y, cx, cy, local_local_id, local_wnd_id, commit_time) ;
				if(0 != mysql_real_query(&m_remoteMysql, inserWndLocal, strlen(inserWndLocal)))
				{
/*					DisConnect();
					return FALSE;	*/	
					LogBugRemote(com_name, inserWndLocal, "query");
					bAllTablesSended = FALSE;
					continue;
				}

				CString getRemoteID;
				//需要本地id，本地窗口号，提交时间，local_time才能尽量唯一确定一条记录
				getRemoteID.Format("select local_id from wnd_local where local_local_id=%s and local_wnd_id=%s and commit_time=\'%s\' and local_time=\'%s\'", \
					local_local_id, local_wnd_id, commit_time, local_time);
				if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
				{
/*					DisConnect();
					return FALSE;	*/	
					LogBugRemote(com_name, getRemoteID, "query");
					bAllTablesSended = FALSE;
					continue;
				}
				MYSQL_RES *result;
				MYSQL_ROW    row;
				if (!(result=mysql_use_result(&m_remoteMysql)))
				{
					//DisConnect();
					//return FALSE;
					LogBugRemote(com_name, getRemoteID, "use result");
					bAllTablesSended = FALSE;
					continue;
				}
				if(row=mysql_fetch_row(result))
				{
					CString remote_local_id = CString(row[0]);
					CString update;
					update.Format("update wnd_local set flag=1, remote_local_id=%s, remote_wnd_id=%s, commit_time=\'%s\' \
								  where local_id=%s", remote_local_id, remote_wnd_id, commit_time, local_local_id);
					if(0 != mysql_real_query(&mysql, update, strlen(update)))
					{
/*						DisConnect();
						return FALSE;	*/	
						LogBugRemote(com_name, update, "query");
						mysql_free_result(result);
						bAllTablesSended = FALSE;
						continue;
					}
				}
				else
				{
					//DisConnect();
					//return FALSE;
					LogBugRemote(com_name, getRemoteID, "fetch row");
					mysql_free_result(result);
					bAllTablesSended = FALSE;
					continue;
				}
				mysql_free_result(result);
			}//wnd_local处理结束
			mysql_free_result(local_result);

			//////////////////////////////////////////////////////////////////////////
			// Table3是空的
			//////////////////////////////////////////////////////////////////////////		
Send_table3:
			//如果该记录中还有表没搞完，就说明这个窗口相关的信息没搞完
			if(bAllTablesSended == FALSE)
			{
				//这说明该记录的东西也没传完
				bRecInfoSended = FALSE;
				continue;
			}

			//与这个窗口相关的数据都更新完了，修改该窗口更新标志
			CString update;
			update.Format("update rec_wnd set flag=2 \
						  where wnd_id=%s", local_wnd_id);
			if(0 != mysql_real_query(&mysql, update, strlen(update)))
			{
/*				DisConnect();
				return FALSE;	*/	
				LogBugRemote(com_name, update, "query");
				bRecInfoSended = FALSE;
				continue;
			}
		}
		mysql_free_result(wnd_result);

		//////////////////////////////////////////////////////////////////////////
		//传输窗口切换边
		//这个表中需要远程窗口id，必须在所有窗口都得到远程id后才能插入
		//所以放在窗口相关表之外
		//////////////////////////////////////////////////////////////////////////
		CString selectWndSwitch;
		selectWndSwitch.Format("select id, from_wnd_id, to_wnd_id, time, rec_id from wnd_switch\
							   where rec_id=%s and flag=0", local_rec_id);
		if(0 != mysql_real_query(&mysql, selectWndSwitch, strlen(selectWndSwitch)))
		{
			LogBugRemote(com_name, selectWndSwitch, "query");
			bRecInfoSended = FALSE;
			goto Record_end;
		}
		MYSQL_RES *switch_result;
		//			MYSQL_ROW    row;
		if (!(switch_result=mysql_use_result(&mysql)))
		{
			LogBugRemote(com_name, selectWndSwitch, "query");
			bRecInfoSended = FALSE;
			goto Record_end;
		}
		while(row=mysql_fetch_row(switch_result))
		{
			CString local_id(row[0]);
			CString local_from_wnd_id(row[1]);
			CString local_to_wnd_id(row[2]);
			CString time(row[3]);
			CString local_rec_id(row[4]);
			CString from_wnd_id;
			CString to_wnd_id;

			//查询from_wnd的remote_wnd_id
			CString getFromRemoteID;
			getFromRemoteID.Format("select remote_wnd_id from rec_wnd where wnd_id=%s", local_from_wnd_id);
			if(0 != mysql_real_query(&mysql, getFromRemoteID, strlen(getFromRemoteID)))
			{
				LogBugRemote(com_name, getFromRemoteID, "query");
				bRecInfoSended = FALSE;
				continue;
			}
			MYSQL_RES *from_remote_result;
			//			MYSQL_ROW    row;
			if (!(from_remote_result=mysql_use_result(&mysql)))
			{
				LogBugRemote(com_name, getFromRemoteID, "query");
				bRecInfoSended = FALSE;
				continue;
			}
			if(row=mysql_fetch_row(from_remote_result))
			{
				from_wnd_id = CString(row[0]);
				mysql_free_result(from_remote_result);
			}
			else
			{
				LogBugRemote(com_name, getFromRemoteID, "query");
				bRecInfoSended = FALSE;
				mysql_free_result(from_remote_result);
				continue;
			}

			//查询to_wnd的remote_wnd_id
			CString getToRemoteID;
			getToRemoteID.Format("select remote_wnd_id from rec_wnd where wnd_id=%s", local_to_wnd_id);
			if(0 != mysql_real_query(&mysql, getToRemoteID, strlen(getToRemoteID)))
			{
				LogBugRemote(com_name, getToRemoteID, "query");
				bRecInfoSended = FALSE;
				continue;
			}
			MYSQL_RES *to_remote_result;
			if (!(to_remote_result=mysql_use_result(&mysql)))
			{
				LogBugRemote(com_name, getToRemoteID, "query");
				bRecInfoSended = FALSE;
				continue;
			}
			if(row=mysql_fetch_row(to_remote_result))
			{
				to_wnd_id = CString(row[0]);
				mysql_free_result(to_remote_result);
			}
			else
			{
				LogBugRemote(com_name, getToRemoteID, "query");
				bRecInfoSended = FALSE;
				mysql_free_result(to_remote_result);
				continue;
			}

			CString inserWndSwitch;
			inserWndSwitch.Format("insert into wnd_switch (from_wnd_id, to_wnd_id, time, local_id, local_from_wnd_id, local_to_wnd_id, commit_time, rec_id, local_rec_id)\
								 values (%s,%s,\'%s\',%s,%s,%s,\'%s\',%s,%s)", \
								 from_wnd_id, to_wnd_id, time, local_id, local_from_wnd_id, local_to_wnd_id, commit_time,remote_rec_id, local_rec_id) ;
			if(0 != mysql_real_query(&m_remoteMysql, inserWndSwitch, strlen(inserWndSwitch)))
			{
				/*					DisConnect();
				return FALSE;	*/	
				LogBugRemote(com_name, inserWndSwitch, "query");
				bRecInfoSended = FALSE;
				continue;  
			}

			CString getRemoteID;
			//需要本地id，本地窗口号，提交时间，local_time才能尽量唯一确定一条记录
			getRemoteID.Format("select id from wnd_switch where from_wnd_id=%s and to_wnd_id=%s and time=\'%s\'", \
				from_wnd_id, to_wnd_id, time);
			if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
			{
				/*					DisConnect();
				return FALSE;	*/	
				LogBugRemote(com_name, getRemoteID, "query");
				bRecInfoSended = FALSE;
				continue;
			}
			MYSQL_RES *id_result;
			MYSQL_ROW    row;
			if (!(id_result=mysql_use_result(&m_remoteMysql)))
			{
				//DisConnect();
				//return FALSE;
				LogBugRemote(com_name, getRemoteID, "use result");
				bRecInfoSended = FALSE;
				continue;
			}
			if(row=mysql_fetch_row(id_result))
			{
				CString remote_id = CString(row[0]);
				CString update;
				update.Format("update wnd_switch set flag=1, remote_id=%s, commit_time=\'%s\', remote_rec_id=%s \
							  where id=%s", remote_id, commit_time, remote_rec_id, local_id);
				if(0 != mysql_real_query(&mysql, update, strlen(update)))
				{
					/*						DisConnect();
					return FALSE;	*/	
					LogBugRemote(com_name, update, "query");
					mysql_free_result(id_result);
					bRecInfoSended = FALSE;
					continue;
				}
			}
			else
			{
				//DisConnect();
				//return FALSE;
				LogBugRemote(com_name, getRemoteID, "fetch row");
				mysql_free_result(id_result);
				bRecInfoSended = FALSE;
				continue;
			}
			mysql_free_result(id_result);
		}//wnd_local处理结束
		mysql_free_result(switch_result);

		//////////////////////////////////////////////////////////////////////////
		//一个记录传输完成
		//////////////////////////////////////////////////////////////////////////
Record_end:
		if(bRecInfoSended == TRUE)
		{
			//与该记录相关的窗口都更新完了，现在把当前记录更新完毕的信息写入本地和远程数据库
			CString updateRemote;
			updateRemote.Format("update rec_com set commit_step=\'complete\',commit_time=\'%s\' where rec_id=%s", commit_time,remote_rec_id);
			if(0 != mysql_real_query(&m_remoteMysql, updateRemote, strlen(updateRemote)))
			{
				/*			DisConnect();
				return FALSE;*/		
				LogBugRemote(com_name, updateRemote, "query");
				continue;
			}
		}
		//如果传完了就改变标志，那么如果在实验室传过一次后，标志已经被修改，回到家中的新记录会由于标志和日期问题而无法在第二天传达
		//在此改变逻辑为：除非今天已经过去，或者该记录日期不等于当前日期才修改标

		//获得当前日期
		//CTime time = CTime::GetCurrentTime();
		CString strCurrentDate = time.Format("%Y-%m-%d");
		if(strCurrentDate != rec_time)
		{
			CString updateLocal;
			updateLocal.Format("update rec_com set flag=2 where rec_id=%s", local_rec_id);
			if(0 != mysql_real_query(&mysql, updateLocal, strlen(updateLocal)))
			{
				LogBugRemote(com_name, updateLocal, "query");
				continue;
			}
		}
	}
	mysql_free_result(rec_result);

	DisConnect();
	return TRUE;
}

//向远程服务器存储BUG信息
BOOL CEmbeddedMySql::LogBugRemote(const CString &com_name, const CString &bugSql, const CString &describe)
{
	if(m_bConnected == FALSE)
		return FALSE;

	CString bug_sql(bugSql);
	bug_sql.Replace("\'", "#");

	CTime time = CTime::GetCurrentTime();
	CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	CString sql;
	sql.Format("insert into rec_bug (com_name, bug_sql, commit_time, des) values (\'%s\',\'%s\',\'%s\', \'%s\')", com_name, bug_sql, timeStr, describe);
	if(mysql_real_query(&m_remoteMysql, sql, strlen(sql)) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

//以每个表中的服务器信息为基础传递信息
BOOL CEmbeddedMySql::CommitOldRecord()
{

	return TRUE;
}

//重新传递所有信息，并更新服务器相关信息
BOOL CEmbeddedMySql::ReCommitAllRecord()
{

	return TRUE;
}

//从远程主机获取信息
BOOL CEmbeddedMySql::GetDataFromServer()
{

	return TRUE;
}

CString CEmbeddedMySql::getNullableValue(const char* value)
{
	CString tmp;
	if(value == NULL)
		tmp = CString("NULL");
	else
		tmp.Format("\'" + CString(value) + "\'");
	return tmp;
}

//重建本地数据库
BOOL CEmbeddedMySql::RebuildLocalDB()
{
	return InitEmbeddedDB();
	
}

#ifdef _DEBUG

//////////////////////////////////////////////////////////////////////////
//测试范例
//
//MYSQL_RES *results; 
//MYSQL_ROW record; 
//char *showSql = "show databases";
//if(mysql_real_query(&mysql, showSql, strlen(showSql)))
//{
//	AfxMessageBox("执行SQL失败");
//}

//results = mysql_store_result(&mysql); 

//while((record = mysql_fetch_row(results))) { 
//	CString result;
//	result.Format("%s - %s \n", record[0], record[1]);
//	AfxMessageBox(result); 
//} 

//mysql_free_result(results); 
//mysql_close(&mysql); 
//mysql_server_end(); 
//////////////////////////////////////////////////////////////////////////


//测试嵌入式mysql是否能建立数据库
BOOL CEmbeddedMySql::TestEmbeddedMySql()
{
	CString testSql;
	testSql.Format("create database if not exists test_db");
	if(mysql_real_query(&mysql, testSql, strlen(testSql)))
	{
		AfxMessageBox("创建数据库失败");
	}

	testSql.Format("use test_db");
	if(mysql_real_query(&mysql, testSql, strlen(testSql)))
	{
		AfxMessageBox("使用数据库失败");
	}

	testSql.Format("create table if not exists test_table(test int(0))");
	if(mysql_real_query(&mysql, testSql, strlen(testSql)))
	{
		AfxMessageBox("创建表失败失败");
	}
	return TRUE;
}


BOOL CEmbeddedMySql::ShowLocalDBArchitecture()
{
	CFile file;
	try
	{
		file.Open("log/DB_Arc.txt",CFile::modeCreate|CFile::modeWrite);
		file.SeekToBegin();

		//创建信息
		//显示所有表
		CString ch_query;
		ch_query.Format("show tables");

		if(0 != mysql_real_query(&mysql, ch_query, strlen(ch_query)))
		{
			return FALSE;		
		}

		MYSQL_RES *result;
		MYSQL_ROW    row;

		if (!(result=mysql_use_result(&mysql)))
		{
			return FALSE;
		}
		//找到了记录
		CString msg;
		vector<CString> vecTables;
		while(row=mysql_fetch_row(result))
		{
			CString tmp(row[0]);
			msg.Append(tmp + "\r\n");
			vecTables.push_back(tmp);
			//#if DEBUG
			//CString msg;
			//msg.Format("现有 rec_id = %d",rec_id);
			//AfxMessageBox(msg);
			//#endif
		}
		mysql_free_result(result);
		msg.Append("\r\n\r\n");
		file.Write(msg, msg.GetLength());
		msg.Empty();

		//获取表详细结构
		for(vector<CString>::iterator it = vecTables.begin(); it != vecTables.end(); it++)
		{
			CString desSql;
			desSql.Format("describe %s", *it);
			if(0 != mysql_real_query(&mysql, desSql, strlen(desSql)))
			{
				return FALSE;		
			}

			MYSQL_RES *result;
			MYSQL_ROW    row;
			CString msg;
			msg.Format("%s:\r\n", *it);

			if (!(result=mysql_use_result(&mysql)))
			{
				return FALSE;
			}
			//找到了记录

			while(row=mysql_fetch_row(result))
			{
				for(int i=0; i<5; i++)
				{
					CString tmp(row[i]);
					msg.Append(tmp + "\t");
				}
				msg.Append("\r\n");
			}
			mysql_free_result(result);
			msg.Append("\r\n\r\n");
			file.Write(msg, msg.GetLength());
		}
		//AfxMessageBox(msg);

		//file.Write(msg, msg.GetLength());
		file.Flush();
		file.Close();
		//MessageBox("写入成功！");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("写入失败的原因是:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
	}

	return TRUE;

}

//导出表数据
BOOL CEmbeddedMySql::ExportTable(const CString &table)
{
	CFile file;
	try
	{
		CString sFileName;
		sFileName.Format("%slog/%sData.txt",path,table);
		file.Open(sFileName,CFile::modeCreate|CFile::modeWrite);
		file.SeekToBegin();
		CString selectSQL;
		selectSQL.Format("select * from %s", table);

		if(0 != mysql_real_query(&mysql, selectSQL, strlen(selectSQL)))
		{
			return FALSE;		
		}

		MYSQL_RES *result;
		MYSQL_ROW    row;

		if (!(result=mysql_use_result(&mysql)))
		{
			return FALSE;
		}
		CString msg;
		int num_fields = mysql_num_fields(result);
		//int rows = mysql_num_rows( result );
		//msg.Format("%d rows:\r\n",rows);
		//file.Write(msg, msg.GetLength());
		//msg.Empty();

		while(row=mysql_fetch_row(result))
		{
			for(int i=0; i<num_fields; i++)
			{
				CString tmp(row[i]);
				msg.Append(tmp + "\t");
			}
			msg.Append("\r\n");
			file.Write(msg, msg.GetLength());
			msg.Empty();
			//count++;
		}
		mysql_free_result(result);
		//msg.Format("act row = %d", count);
		//file.Write(msg, msg.GetLength());
		
		file.Flush();
		file.Close();
		//MessageBox("写入成功！");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("写入失败的原因是:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
		return FALSE;
	}
	return TRUE;
}

//导出所有表
BOOL CEmbeddedMySql::ExportAllTable()
{
	CString logPath;
	logPath.Format("%slog",path);
	if(::CreateDirectory(logPath, NULL) == 0)
	{
		//创建失败,判断失败原因是不是文件夹已经存在
		DWORD nErrorNo = GetLastError();
		if(nErrorNo != ERROR_ALREADY_EXISTS)
		{
			//return FALSE;
			AfxMessageBox("创建日志文件夹失败");
		}
	}
	//创建信息
	//显示所有表
	CString ch_query;
	ch_query.Format("show tables");

	if(0 != mysql_real_query(&mysql, ch_query, strlen(ch_query)))
	{
		return FALSE;		
	}

	MYSQL_RES *result;
	MYSQL_ROW    row;

	if (!(result=mysql_use_result(&mysql)))
	{
		return FALSE;
	}
	//找到了记录
	
	//vector<CString> vecTables;
	while(row=mysql_fetch_row(result))
	{
		CString table(row[0]);
		//vecTables.push_back(tmp);
		ExportTable(table);
	}
	return TRUE;

}
#endif // _DEBUG
