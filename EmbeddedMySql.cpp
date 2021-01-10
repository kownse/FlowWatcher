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

//���������ļ�
BOOL CEmbeddedMySql::GenerateCnf(char* exactPath)
{
	CFile file;
	CString fileName;
	CString path(exactPath);
	CString dataPath;
	dataPath.Format("%sdata",path);
	bool bDataDirectory = true;

	//���������ļ���
	if(::CreateDirectory(dataPath, NULL) == 0)
	{
		//����ʧ��,�ж�ʧ��ԭ���ǲ����ļ����Ѿ�����
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
		//дʵ������
		CString context;
		context.Append("[FlowWatcher]\r\n");
		CString dataPath;
		//�ж��Ƿ���data�ļ���
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
		//str.Format("д��ʧ�ܵ�ԭ����:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
		return FALSE;
	}
}

//��ʼ��Ƕ��ʽ��������������֮
BOOL CEmbeddedMySql::Connect(void)
{
	//Ӧ�ó�����
	char *server = "EmbeddedMySql";
	//ѡ��
	char option[MAX_PATH];
	//�����ļ�·��
	char cnf[MAX_PATH];
	

	//��ȡ����·��
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

	//�ж������ļ��Ƿ���ڣ������ھ�����һ����˳���·��ȫ������ȴ��·��
	CFileFind   find;   
	if(!find.FindFile(cnf))
	{
		//  ������   
		//�����Ծ���·��Ϊ�����������ļ�	
		if(GenerateCnf(path) == FALSE)
			return FALSE;
	}

	//ȷ��/data����
	//���������ļ���
	CString dataPath;
	dataPath.Format("%sdata",path);
	if(::CreateDirectory(dataPath, NULL) == 0)
	{
		//����ʧ��,�ж�ʧ��ԭ���ǲ����ļ����Ѿ�����
		DWORD nErrorNo = GetLastError();
		if(nErrorNo != ERROR_ALREADY_EXISTS)
		{
			return FALSE;
		}
	}

	if(0 != mysql_server_init(num_elements, server_options, server_groups))
	{
		//��ʼ��������ʧ��
		return FALSE;
	}
	//printf("return %ld\n", ret); 
	if(0 == mysql_init(&mysql))
	{
		//��ʼ��ʧ��
		return FALSE;
	}
	if(0 != mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client"))
	{
		//���ÿͻ���ʧ��
		return FALSE;
	}


	if(0 != mysql_options(&mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL))
	{
		//����ΪǶ��ʽʧ��
		return FALSE;
	}

	if (!mysql_real_connect(&mysql, NULL,NULL,NULL, NULL, 0,NULL,0))
	{
		//����ʧ��

		//m_bEmbeddedEnable = FALSE;
		return FALSE;
	}
	else
	{
		//���ӷ������ɹ�
		//AfxMessageBox("���ӳɹ�");
		//m_bEmbeddedEnable = TRUE;
		//return TRUE;
	}

	//TestEmbeddedMySql();
	if(InitEmbeddedDB() == TRUE)
	{
#ifdef _DEBUG
		////�������������ļ���
		//CString logPath;
		//dataPath.Format("%slog",path);
		//if(::CreateDirectory(dataPath, NULL) == 0)
		//{
		//	//����ʧ��,�ж�ʧ��ԭ���ǲ����ļ����Ѿ�����
		//	DWORD nErrorNo = GetLastError();
		//	if(nErrorNo != ERROR_ALREADY_EXISTS)
		//	{
		//		//return FALSE;
		//		AfxMessageBox("������־�ļ���ʧ��");
		//	}
		//}
		//if(ExportAllTable() == FALSE)
		//	AfxMessageBox("������������ʧ��");
		//if(CommitNewRecord() == TRUE)
		//	AfxMessageBox("�������ݳɹ�");
		//else
		//	AfxMessageBox("��������ʧ��");
		//if(ShowLocalDBArchitecture() == FALSE)
		//	AfxMessageBox("��ʾ�ṹʧ��");
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
		//��ʼ��ʧ��
		return FALSE;
	}
	if(0 != mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client"))
	{
		//���ÿͻ���ʧ��
		return FALSE;
	}
	if(0 != mysql_options(&mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL))
	{
		//����ΪǶ��ʽʧ��
		return FALSE;
	}

	if (!mysql_real_connect(&mysql, NULL,NULL,NULL, NULL, 0,NULL,0))
	{
		//����ʧ��

		//m_bEmbeddedEnable = FALSE;
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

//��ʼ��Ƕ��ʽ���ݿ�ṹ
//ͨ����ȡ����sql�ļ��ʹ�Զ����������sql���������ֶ�
//ȷ�����ݿ�ṹ�����ɹ�
BOOL CEmbeddedMySql::InitEmbeddedDB()
{
	//ë���ˣ�ֱ�Ӱ�SQLд���⡣Ƕ��ʽmysqlͷ����source�������ҿ���
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
			//��ʼ��ʧ����...
			m_bEmbeddedEnable = FALSE;
			break;
		}
	}
	if(i == 12)
	{
		m_bEmbeddedEnable = TRUE;
		return TRUE;
	}

	
	//��Զ�����ݿ��ϵ�sql��ʼ��
	//��Ҫ����Զ�̷�����
	if(ConnectRemoteServer() == TRUE)
	{
		CString initSQL;
		initSQL.Format("select text from db_version_sql where version=1") ;
		if (mysql_real_query(&m_remoteMysql, initSQL, strlen(initSQL))!=0)
		{
			//AfxMessageBox("���ʧ��");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
			return FALSE;
		}	

		MYSQL_RES *result;
		MYSQL_ROW    row;
		if (!(result=mysql_use_result(&m_remoteMysql)))
		{
			//AfxMessageBox("���ݱ����");
			//return 0;
			if(m_bConnected == TRUE) RecordFailed();
			return FALSE;
		}
		//�ҵ��˼�¼
		while ((row = mysql_fetch_row(result)))
		{
			//�õ�һ�г�ʼ��SQL���
			//�ñ���MYSQL����ִ��֮
			char *sqlStr = row[0];

			if (mysql_real_query(&mysql, sqlStr, strlen(sqlStr))!=0)
			{
				//��һ��û�ɹ��������ʼ��ʧ��
				m_bEmbeddedEnable = FALSE;
				mysql_free_result(result);
				DisConnect();
				return FALSE;
			}
		}
		//��ʼ���ɹ�
		mysql_free_result(result);
		m_bEmbeddedEnable = TRUE;
		DisConnect();
		return TRUE;
	}
}

//�Ͽ�Ƕ��ʽ����
BOOL CEmbeddedMySql::FreeEmbededConnect(void)
{
	mysql_close(&mysql);
	return TRUE;
}

//����Զ������
BOOL CEmbeddedMySql::ConnectRemoteServer()
{

	if (0 != mysql_library_init(0, NULL, NULL)) {
		//AfxMessageBox("װ��libʧ��");
		return FALSE;
	}else{
		//AfxMessageBox("װ��lib�ɹ�");
	}
	if (NULL == mysql_init(&m_remoteMysql)) {
		//AfxMessageBox("���ӳ�ʼ��ʧ��");
		return FALSE;
	}else{
		//AfxMessageBox("���ӳ�ʼ���ɹ�");
	}
	if (0 != mysql_options(&m_remoteMysql, MYSQL_SET_CHARSET_NAME, "gb2312")) {
		//AfxMessageBox("�����ַ�������");
		return FALSE;
	}else{
	}

	////�������ӷ�����
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

//ȡ������
BOOL CEmbeddedMySql::DisConnect(void)
{
	mysql_close(&m_remoteMysql);
	m_bConnected = FALSE;
	m_iFailCount = 0;
	return TRUE;
}						

//�ж��Ƿ����ӳɹ�
BOOL CEmbeddedMySql::IsConnected()
{
	//������Ƕ��ʽ�ģ����Է���Ƕ��ʽ����
	return m_bEmbeddedEnable;
	//return m_bConnected;
}


//�����µļ�¼
const unsigned int CEmbeddedMySql::NewRecord(TCHAR *computerName, const CTime& time)
{
	//�ж��Ƿ����ӳɹ�
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
	//return 0;
}	

//���а汾�ŵ��¼�¼
const unsigned int CEmbeddedMySql::NewRecord(TCHAR *computerName, const CTime& time, const CString& version)
{
	//�ж��Ƿ����ӳɹ�
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
		//AfxMessageBox("��ѯ��¼��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();
		return 0;
	}
	//�ҵ��˼�¼
	if(row=mysql_fetch_row(result))
	{
		rec_id = atoi(row[0]);
		CString oldVersion(row[1]);
		mysql_free_result(result);
		//��¼�а汾�ź����ڵİ汾�Ų�һ�������޸���
		if(oldVersion != version)
		{
			CString updateVersionSQL;
			updateVersionSQL.Format("update rec_com set version=\'%s\', flag=3 where rec_id=\'%d\'", version, rec_id);
			if (mysql_real_query(&mysql, updateVersionSQL, strlen(updateVersionSQL))!=0)
			{
				//AfxMessageBox("���ʧ��");
				//return 0;
				if(m_bConnected == TRUE) RecordFailed();
			}
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
const unsigned int CEmbeddedMySql::NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect)
{
	if(m_bEmbeddedEnable == FALSE)
		return 0;

	int wnd_id;			//���ڱ�Ǻ�

	//�Է���һ����ѯ����wnd_id
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
//�Ѿ�������wnd_active�Ǹ�����
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
	//	//AfxMessageBox("��Ӵ��ڻ��¼ʧ��:" + activeSQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}

	return TRUE;
}

//��Ӵ��ڱ���
BOOL CEmbeddedMySql::WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString titleSQL;
	//titleSQL.Format("insert into wnd_title (wnd_id,title,time) values (\'%d\',\'%s\',\'%s\')", wnd_id, title, timeStr);
	//if (mysql_real_query(&mysql, titleSQL, strlen(titleSQL))!=0)
	//{
	//	//AfxMessageBox("��ӱ���ʧ��:" + titleSQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}
	return TRUE;
}		

//�����ƶ�
BOOL CEmbeddedMySql::WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect)
{
	if(m_bEmbeddedEnable == FALSE)
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
BOOL CEmbeddedMySql::WindowDestory(unsigned int wnd_id, const CTime& time)
{
	if(m_bEmbeddedEnable == FALSE)
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

//���ڰ�����Ϣ
BOOL CEmbeddedMySql::NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString keySQL;
	//keySQL.Format("insert into wnd_keytype (wnd_id,keytype_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, keyCount, timeStr);
	//if (mysql_real_query(&mysql, keySQL, strlen(keySQL))!=0)
	//{
	//	//AfxMessageBox("��Ӱ�����¼ʧ��:" + keySQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}
	return TRUE;
}

//�������Ϣ
BOOL CEmbeddedMySql::NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time)
{
	//if(m_bEmbeddedEnable == FALSE)
	//	return 0;

	//CString timeStr = time.Format("%Y-%m-%d %H:%M:%S");
	//CString mouseSQL;
	//mouseSQL.Format("insert into wnd_mouseclick (wnd_id,mouse_count,time) values (\'%d\',\'%d\',\'%s\')", wnd_id, mouseCount, timeStr);
	//if (mysql_real_query(&mysql, mouseSQL, strlen(mouseSQL))!=0)
	//{
	//	//AfxMessageBox("����������¼ʧ��:" + mouseSQL);
	//	if(m_bConnected == TRUE) RecordFailed();
	//	return FALSE;
	//}
	return TRUE;
}						

//��¼�������ʱ��
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
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}			

//��¼�����л���
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
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;

}

//��¼�������ʱ��ͷǽ������ʱ��
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
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

//��¼�������ʱ��ͽ�����Ϣ
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
		//AfxMessageBox("��¼���ڽ������ʱ��ʧ��");
		if(m_bConnected == TRUE) RecordFailed();  
		return FALSE;
	}
	return TRUE;
}

// ��¼����������
BOOL CEmbeddedMySql::LogCliqueResult(int rec_id, const CTime& time, const CString& logText)
{
	if(m_bEmbeddedEnable == FALSE)
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

//Ƕ��ʽ��¼ʧ�ܣ�
BOOL CEmbeddedMySql::RecordFailed()
{
	return FALSE;
}


//��¼һ��ʱ���ڴ��ڴ�������
//û�ã�ȥ����
BOOL CEmbeddedMySql::RecordWndCreateCount(int rec_id, const CTime& time, int count)
{
	return TRUE;
}

//�����������ݿ�
BOOL CEmbeddedMySql::DropLocalDB()
{
	if(m_bEmbeddedEnable == FALSE)
		return 0;

	CString dropSQL;
	dropSQL.Format("drop database local_hook_db");
	if (mysql_real_query(&mysql, dropSQL, strlen(dropSQL))!=0)
	{
		//AfxMessageBox("����������¼ʧ��:" + mouseSQL);
		if(m_bConnected == TRUE) RecordFailed();
		return FALSE;
	}
	return TRUE;

	//CString dataPath;
	//dataPath.Format("%sdata",path);

	////ɾ���ļ����������ļ�
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
	////ɾ��data �ļ���
	//if(::RemoveDirectory(dataPath) == 0)
	//{
	//	#ifdef _DEBUG
	//	
	//				DWORD nErrorNo = GetLastError ( ); // �õ��������
	//				LPSTR lpBuffer;    
	//				FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER  | 
	//					FORMAT_MESSAGE_IGNORE_INSERTS  | 
	//					FORMAT_MESSAGE_FROM_SYSTEM,
	//					NULL,
	//					nErrorNo, // ���˴�����룬ͨ���ڳ����п��� GetLastError()��֮
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

	////���´��������ļ���
	//if(::CreateDirectory(dataPath, NULL) == 0)
	//{
	//	//����ʧ��,�ж�ʧ��ԭ���ǲ����ļ����Ѿ�����
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

//���������ɵļ�¼
//////////////////////////////////////////////////////////////////////////
//�ú��������������ݣ���֤����ϵ
//�ú���Ӧ�����Ծ�����ΪΪ��ֹ����;�������ж�����
//////////////////////////////////////////////////////////////////////////
BOOL CEmbeddedMySql::CommitNewRecord()
{
	//������Զ�̷�����
	if(m_bConnected == FALSE)
	{
		if(ConnectRemoteServer() == FALSE)
		{
			//û�����ϣ����˳���
			mysql_close(&m_remoteMysql);
			return FALSE;
		}
	}

	CTime time = CTime::GetCurrentTime();
	CString date = time.Format("%Y-%m-%d");
	CString selectSQL;
	//ѡ����Щ����û���ύ�ļ�¼
	selectSQL.Format("select rec_id,com_name,rec_time,version,flag,remote_rec_id from rec_com where flag!=2 or rec_time=\'%s\'", date);
	//���������Ҫ�����´�������û�д���Ĵ���
	//selectSQL.Format("select rec_id,com_name,rec_time,version,flag,remote_rec_id from rec_com ");

	if(0 != mysql_real_query(&mysql, selectSQL, strlen(selectSQL)))
	{
		//һ����¼��û�еõ����˳�
		return FALSE;		
	}
	MYSQL_RES *rec_result;
	MYSQL_ROW    row;
	if (!(rec_result=mysql_use_result(&mysql)))
	{
		//һ����¼��û�еõ����˳�
		DisConnect();
		return FALSE;
	}
	while(row=mysql_fetch_row(rec_result))
	{
		BOOL bRecInfoSended = TRUE;				//�ü�¼��������Ϣ������ɹ�
		//ÿ����¼ÿ����¼�ش���
		CString local_rec_id(row[0]);
		CString com_name(row[1]);
		CString rec_time(row[2]);
		//CString version(row[3]);
		CString version = getNullableValue(row[3]);
		int flag = atoi(row[4]);
		//�õ�Զ��id��
		CString remote_rec_id = getNullableValue(row[5]);
		CTime time = CTime::GetCurrentTime();
		CString commit_time = time.Format("%Y-%m-%d %H:%M:%S");

		//����Ǹ�ȫ�µļ�¼������������д
		if(flag == 0 || remote_rec_id == "NULL")
		{
			//�������д��Ϣ
			CString inserRec;
			inserRec.Format("insert into rec_com (com_name, rec_time, version, local_rec_id, commit_time, commit_step) \
							values (\'%s\',\'%s\',%s,%s,\'%s\',\'%s\')", com_name, rec_time, version, local_rec_id, commit_time, "uncomplete") ;
			if(0 != mysql_real_query(&m_remoteMysql, inserRec, strlen(inserRec)))
			{
				//DisConnect();
				//return FALSE;		

				LogBugRemote(com_name,inserRec, "query");
				//ʧ���˾ͼ���������һ����¼
				bRecInfoSended = FALSE;
				continue;
			}

			//���±��ر�
			CString getRemoteID;
			//��Ҫ���ؼ�¼�ţ��ύʱ��ͼ����������Ψһȷ��һ��Զ�̼�¼��
			getRemoteID.Format("select rec_id from rec_com where local_rec_id=%s and commit_time=\'%s\' and com_name=\'%s\'", local_rec_id, commit_time, com_name);
			if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
			{
				//DisConnect();
				//return FALSE;		
				//ʧ���˾ͼ���������һ����¼
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
				//ʧ���˾ͼ���������һ����¼
				LogBugRemote(com_name, getRemoteID, "use result");
				bRecInfoSended = FALSE;
				continue;
			}

			if(row=mysql_fetch_row(result))
			{
				CString r_rec_id = CString(row[0]);
				remote_rec_id = r_rec_id;
				//ֻ���¼�¼�Ÿ��±�
				CString update;
				update.Format("update rec_com set remote_rec_id=%s, flag=1, commit_time=\'%s\' \
							  where rec_id=%s", remote_rec_id, commit_time, local_rec_id);
				if(0 != mysql_real_query(&mysql, update, strlen(update)))
				{
					//DisConnect();
					//return FALSE;	

					//ʧ���˾ͼ���������һ����¼
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
				//ʧ���˾ͼ���������һ����¼
				LogBugRemote(com_name, getRemoteID, "fetch row");
				bRecInfoSended = FALSE;
				mysql_free_result(result);
				continue;
			}
			mysql_free_result(result);
		}
		else if(flag == 3)
		{
			//���ذ汾���¹����������д�µİ汾��Ϣ
			CString updateVersion;
			updateVersion.Format("update rec_com set version=%s where rec_id=%s", version, remote_rec_id);
			if(0 != mysql_real_query(&m_remoteMysql, updateVersion, strlen(updateVersion)))
			{
				//DisConnect();
				//return FALSE;		
				//�汾����ʧ�ܣ���Ӱ�����Ĵ���
				LogBugRemote(com_name, updateVersion, "query");
				bRecInfoSended = FALSE;
			}
		}
		
		//����rec_wnd��
		CString selectLocalWnd;
		selectLocalWnd.Format("select wnd_id,create_time,init_title,destory_time,classname,flag,remote_wnd_id from rec_wnd where rec_id=%s", local_rec_id);
		if(0 != mysql_real_query(&mysql, selectLocalWnd, strlen(selectLocalWnd)))
		{
			//DisConnect();
			//return FALSE;		
			
			//һ�����ڶ�û�ж�������ֱ�ӽ�����һ����¼
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
		//rec_wndҲ��һ�������ϼ��ı�����Ҳ�������֮��ص����б����º���ܸ������־
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
			
			//ֻ�д���û��������Ĵ��ڲ���Ҫ�ڷ������²�������
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

				//���±��ر�
				CString getRemoteID;
				//��Ҫ���ش��ں�,���ؼ�¼��,�ύʱ��,��ʼ��������ܾ���Ψһȷ��һ��Զ�̴��ں�
				getRemoteID.Format("select wnd_id from rec_wnd where local_wnd_id=%s and local_rec_id=%s and commit_time=\'%s\' and init_title=\'%s\'", \
					local_wnd_id, local_rec_id, commit_time, init_title);
				if(0 != mysql_real_query(&m_remoteMysql, getRemoteID, strlen(getRemoteID)))
				{
					//DisConnect();
					//return FALSE;		

					//���������¼ʧ�ܣ����Ÿ���һ��
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

					//��¼ʧ�ܣ�����һ��
					LogBugRemote(com_name, getRemoteID, "use result");
					bRecInfoSended = FALSE;
					continue;
				}

				if(row=mysql_fetch_row(result))
				{
					remote_wnd_id = CString(row[0]);
					//ֻ�д���û�н��й����µĴ��ڲ���Ҫ������Щ����
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
			//������ݵõ���remote_wnd_id�������ര�������Ϣ
			//////////////////////////////////////////////////////////////////////////

			BOOL bAllTablesSended = TRUE;				//���б�����ɹ��ı�־

			//////////////////////////////////////////////////////////////////////////
			//����wnd_act_time��
			//////////////////////////////////////////////////////////////////////////
Send_table1:

			CString selectLocalWndActTime;
			selectLocalWndActTime.Format("select id, last_time, focus, focus_balance, last_time2, avg_key, avg_mouse,time from wnd_act_time \
								  where wnd_id=%s and flag=0", local_wnd_id);
			if(0 != mysql_real_query(&mysql, selectLocalWndActTime, strlen(selectLocalWndActTime)))
			{
				//DisConnect();
				//return FALSE;		

				//һ����û�ж����������Ŷ�����ı�
				//��Ȼ�ܲ�����goto������������goto������
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
					//������һ����
					LogBugRemote(com_name, inserWndActTime, "query");
					bAllTablesSended = FALSE;
					continue;
				}

				CString getRemoteID;
				//��Ҫ����id�����ش��ںţ��ύʱ�䣬avg_mouse����Ψһȷ��һ����¼
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
			}//wnd_act_time�������
			mysql_free_result(wnd_act_result);

			//////////////////////////////////////////////////////////////////////////
			//����wnd_local
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
				//��Ҫ����id�����ش��ںţ��ύʱ�䣬local_time���ܾ���Ψһȷ��һ����¼
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
			}//wnd_local�������
			mysql_free_result(local_result);

			//////////////////////////////////////////////////////////////////////////
			// Table3�ǿյ�
			//////////////////////////////////////////////////////////////////////////		
Send_table3:
			//����ü�¼�л��б�û���꣬��˵�����������ص���Ϣû����
			if(bAllTablesSended == FALSE)
			{
				//��˵���ü�¼�Ķ���Ҳû����
				bRecInfoSended = FALSE;
				continue;
			}

			//�����������ص����ݶ��������ˣ��޸ĸô��ڸ��±�־
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
		//���䴰���л���
		//���������ҪԶ�̴���id�����������д��ڶ��õ�Զ��id����ܲ���
		//���Է��ڴ�����ر�֮��
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

			//��ѯfrom_wnd��remote_wnd_id
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

			//��ѯto_wnd��remote_wnd_id
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
			//��Ҫ����id�����ش��ںţ��ύʱ�䣬local_time���ܾ���Ψһȷ��һ����¼
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
		}//wnd_local�������
		mysql_free_result(switch_result);

		//////////////////////////////////////////////////////////////////////////
		//һ����¼�������
		//////////////////////////////////////////////////////////////////////////
Record_end:
		if(bRecInfoSended == TRUE)
		{
			//��ü�¼��صĴ��ڶ��������ˣ����ڰѵ�ǰ��¼������ϵ���Ϣд�뱾�غ�Զ�����ݿ�
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
		//��������˾͸ı��־����ô�����ʵ���Ҵ���һ�κ󣬱�־�Ѿ����޸ģ��ص����е��¼�¼�����ڱ�־������������޷��ڵڶ��촫��
		//�ڴ˸ı��߼�Ϊ�����ǽ����Ѿ���ȥ�����߸ü�¼���ڲ����ڵ�ǰ���ڲ��޸ı�

		//��õ�ǰ����
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

//��Զ�̷������洢BUG��Ϣ
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

//��ÿ�����еķ�������ϢΪ����������Ϣ
BOOL CEmbeddedMySql::CommitOldRecord()
{

	return TRUE;
}

//���´���������Ϣ�������·����������Ϣ
BOOL CEmbeddedMySql::ReCommitAllRecord()
{

	return TRUE;
}

//��Զ��������ȡ��Ϣ
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

//�ؽ��������ݿ�
BOOL CEmbeddedMySql::RebuildLocalDB()
{
	return InitEmbeddedDB();
	
}

#ifdef _DEBUG

//////////////////////////////////////////////////////////////////////////
//���Է���
//
//MYSQL_RES *results; 
//MYSQL_ROW record; 
//char *showSql = "show databases";
//if(mysql_real_query(&mysql, showSql, strlen(showSql)))
//{
//	AfxMessageBox("ִ��SQLʧ��");
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


//����Ƕ��ʽmysql�Ƿ��ܽ������ݿ�
BOOL CEmbeddedMySql::TestEmbeddedMySql()
{
	CString testSql;
	testSql.Format("create database if not exists test_db");
	if(mysql_real_query(&mysql, testSql, strlen(testSql)))
	{
		AfxMessageBox("�������ݿ�ʧ��");
	}

	testSql.Format("use test_db");
	if(mysql_real_query(&mysql, testSql, strlen(testSql)))
	{
		AfxMessageBox("ʹ�����ݿ�ʧ��");
	}

	testSql.Format("create table if not exists test_table(test int(0))");
	if(mysql_real_query(&mysql, testSql, strlen(testSql)))
	{
		AfxMessageBox("������ʧ��ʧ��");
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

		//������Ϣ
		//��ʾ���б�
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
		//�ҵ��˼�¼
		CString msg;
		vector<CString> vecTables;
		while(row=mysql_fetch_row(result))
		{
			CString tmp(row[0]);
			msg.Append(tmp + "\r\n");
			vecTables.push_back(tmp);
			//#if DEBUG
			//CString msg;
			//msg.Format("���� rec_id = %d",rec_id);
			//AfxMessageBox(msg);
			//#endif
		}
		mysql_free_result(result);
		msg.Append("\r\n\r\n");
		file.Write(msg, msg.GetLength());
		msg.Empty();

		//��ȡ����ϸ�ṹ
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
			//�ҵ��˼�¼

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

	return TRUE;

}

//����������
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
		//MessageBox("д��ɹ���");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("д��ʧ�ܵ�ԭ����:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
		return FALSE;
	}
	return TRUE;
}

//�������б�
BOOL CEmbeddedMySql::ExportAllTable()
{
	CString logPath;
	logPath.Format("%slog",path);
	if(::CreateDirectory(logPath, NULL) == 0)
	{
		//����ʧ��,�ж�ʧ��ԭ���ǲ����ļ����Ѿ�����
		DWORD nErrorNo = GetLastError();
		if(nErrorNo != ERROR_ALREADY_EXISTS)
		{
			//return FALSE;
			AfxMessageBox("������־�ļ���ʧ��");
		}
	}
	//������Ϣ
	//��ʾ���б�
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
	//�ҵ��˼�¼
	
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
