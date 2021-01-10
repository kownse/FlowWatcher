#pragma once
#include "abstractdbmanager.h"

//////////////////////////////////////////////////////////////////////////
//该类是对嵌入式mysql的封装。
//为了达到从主数据库更新自身表结构的目的，该类也包含与主数据库连接
//提取更新SQL语句的功能
//////////////////////////////////////////////////////////////////////////
class CEmbeddedMySql :
	public CAbstractDBManager
{
public:
	CEmbeddedMySql(void);
	~CEmbeddedMySql(void);

public:
	const unsigned int NewRecord(TCHAR *computerName, const CTime& time);																	//创建新的记录
	const unsigned int NewRecord(TCHAR *computerName, const CTime& time, const CString& version);								//带有版本号的新纪录
	//const unsigned int NewVersion(unsigned int rec_id, const CString& version);																	//更新版本号
	const unsigned int NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect);									//新窗口记录
	BOOL WindowActive(unsigned int wnd_id, bool flag, const CTime& time);																//窗口激活信息
	BOOL WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time);															//添加窗口标题
	BOOL WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect);																//窗口移动
	BOOL WindowDestory(unsigned int wnd_id, const CTime& time);																				//窗口关闭
	BOOL NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time);												//窗口按键消息
	BOOL NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time);									//鼠标点击消息
	BOOL WindowAttTime(unsigned int wnd_id, int lastTime, bool focus, double attBalance);								//记录焦点持续时间
	BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, const CTime &time);		//记录焦点持续时间和非焦点持续时间
	BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, float avgKey, float avgMouse, const CTime &time);
	BOOL LogCliqueResult(int rec_id, const CTime& time, const CString& logText);																		// 记录聚类分析结果
	BOOL LogWindowSwitch(int rec_id,int from_wnd_id, int to_wnd_id, const CTime &time);																				//记录切换边历史
	BOOL RecordFailed();																																	//记录失败

	BOOL Connect(void);							//初始化本地嵌入式数据库		
	BOOL ConnectExistDB(void);				//连接已经存在的数据库
	BOOL FreeEmbededConnect(void);	//释放嵌入式连接
	BOOL DisConnect(void);					//关闭远程连接																										//取消链接
	BOOL IsConnected();							//返回本地嵌入式数据库是否开启																																//判断是否连接成功

	BOOL RecordWndCreateCount(int rec_id, const CTime& time, int count);		//这条没用
	BOOL ConnectRemoteServer();																		//连接远处的主服务器
	BOOL DropLocalDB();																					//放起本地数据库
	BOOL RebuildLocalDB();																				//重建本地数据库

	//////////////////////////////////////////////////////////////////////////
	//向服务器传输记录数据
	BOOL CommitNewRecord();																			//传递新生成的记录
	BOOL CommitOldRecord();																			//以每个表中的服务器信息为基础传递信息
	BOOL ReCommitAllRecord();																			//重新传递所有数据，并更新服务器相关信息
	//////////////////////////////////////////////////////////////////////////
	//从服务器获取数据
	BOOL GetDataFromServer();																			//从远程主机获取自己的信息
	//////////////////////////////////////////////////////////////////////////
	BOOL Log(){return CommitNewRecord();}																	//直接是传输
	BOOL ExportAllTable();														//将所有表导出到文件中

private:
	BOOL GenerateCnf(char* exactPath);								//创建配置文件
	BOOL InitEmbeddedDB();													//初始化嵌入式数据库结构
	CString getNullableValue(const char* value);						//得到那种可能为NULL的元素
	BOOL LogBugRemote(const CString &com_name, const CString &bugSql, const CString &describe);													//向远程服务器存储数据库SQL BUG信息

#ifdef _DEBUG
	BOOL TestEmbeddedMySql();											//测试嵌入式mysql是否能建数据库
	BOOL ShowLocalDBArchitecture();										//显示本地数据库结构
	BOOL ExportTable(const CString &table);							//输出表内信息
	
	
#endif // _DEBUG

private:
	MYSQL m_remoteMysql;													//远处MYSQL主机
	BOOL m_bEmbeddedEnable;														//嵌入式数据库运行成功
	char path[MAX_PATH];														//当前工作路径
};
