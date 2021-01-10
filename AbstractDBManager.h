#pragma once

class CAbstractDBManager
{
public:
	CAbstractDBManager(void);
	virtual ~CAbstractDBManager(void);

protected:
	MYSQL mysql;																																				//mysql连接
	int m_iFailCount;																																			//失败计数
	BOOL m_bConnected;																																	//链接标志
public:
	virtual const unsigned int NewRecord(TCHAR *computerName, const CTime& time) = 0;																	//创建新的记录
	virtual const unsigned int NewRecord(TCHAR *computerName, const CTime& time, const CString& version) = 0;								//带有版本号的新纪录
	//virtual const unsigned int NewVersion(unsigned int rec_id, const CString& version) = 0;																	//更新版本号
	virtual const unsigned int NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect) = 0;									//新窗口记录
	virtual BOOL WindowActive(unsigned int wnd_id, bool flag, const CTime& time) = 0;																//窗口激活信息
	virtual BOOL WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time) = 0;															//添加窗口标题
	virtual BOOL WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect) = 0;																//窗口移动
	virtual BOOL WindowDestory(unsigned int wnd_id, const CTime& time) = 0;																				//窗口关闭
	virtual BOOL NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time) = 0;												//窗口按键消息
	virtual BOOL NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time) = 0;									//鼠标点击消息
	virtual BOOL WindowAttTime(unsigned int wnd_id, int lastTime, bool focus, double attBalance) = 0;								//记录焦点持续时间
	virtual BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance,const CTime &time) = 0;		//记录焦点持续时间和非焦点持续时间
	virtual BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, float avgKey, float avgMouse, const CTime &time) = 0;
	virtual BOOL LogCliqueResult(int rec_id, const CTime& time, const CString& logText) = 0;																		// 记录聚类分析结果
	virtual BOOL LogWindowSwitch(int rec_id, int from_wnd_id, int to_wnd_id, const CTime &time) = 0;	
	virtual BOOL Log() = 0;							//记录																													
	virtual BOOL RecordFailed() = 0;																																	//记录失败

	virtual BOOL Connect(void) = 0;																																		// 连接数据库
	virtual BOOL DisConnect(void) = 0;																																//取消链接
	virtual BOOL IsConnected() = 0;	 																																	//判断是否连接成功

	virtual BOOL RecordWndCreateCount(int rec_id, const CTime& time, int count) = 0;
};
