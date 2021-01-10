#pragma once
#include "mysql.h"
#include "AbstractDBManager.h"
//数据库控制类
class DBManager : public CAbstractDBManager
{
public:
	explicit DBManager(void);
	~DBManager(void);
	
private:
//	MYSQL mysql;																																				//mysql连接
//	int m_iFailCount;																																			//失败计数
//	BOOL m_bConnected;																																	//链接标志
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
	BOOL LogWindowSwitch(int rec_id, int from_wnd_id, int to_wnd_id, const CTime &time);	
	BOOL RecordFailed();																																	//记录失败

	BOOL Connect(void);																																		// 连接数据库
	BOOL DisConnect(void);																																//取消链接
	BOOL IsConnected();																																		//判断是否连接成功
	
	BOOL RecordWndCreateCount(int rec_id, const CTime& time, int count);
	BOOL Log(){ return TRUE;}
};
