#pragma once

class CAbstractDBManager
{
public:
	CAbstractDBManager(void);
	virtual ~CAbstractDBManager(void);

protected:
	MYSQL mysql;																																				//mysql����
	int m_iFailCount;																																			//ʧ�ܼ���
	BOOL m_bConnected;																																	//���ӱ�־
public:
	virtual const unsigned int NewRecord(TCHAR *computerName, const CTime& time) = 0;																	//�����µļ�¼
	virtual const unsigned int NewRecord(TCHAR *computerName, const CTime& time, const CString& version) = 0;								//���а汾�ŵ��¼�¼
	//virtual const unsigned int NewVersion(unsigned int rec_id, const CString& version) = 0;																	//���°汾��
	virtual const unsigned int NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect) = 0;									//�´��ڼ�¼
	virtual BOOL WindowActive(unsigned int wnd_id, bool flag, const CTime& time) = 0;																//���ڼ�����Ϣ
	virtual BOOL WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time) = 0;															//��Ӵ��ڱ���
	virtual BOOL WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect) = 0;																//�����ƶ�
	virtual BOOL WindowDestory(unsigned int wnd_id, const CTime& time) = 0;																				//���ڹر�
	virtual BOOL NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time) = 0;												//���ڰ�����Ϣ
	virtual BOOL NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time) = 0;									//�������Ϣ
	virtual BOOL WindowAttTime(unsigned int wnd_id, int lastTime, bool focus, double attBalance) = 0;								//��¼�������ʱ��
	virtual BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance,const CTime &time) = 0;		//��¼�������ʱ��ͷǽ������ʱ��
	virtual BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, float avgKey, float avgMouse, const CTime &time) = 0;
	virtual BOOL LogCliqueResult(int rec_id, const CTime& time, const CString& logText) = 0;																		// ��¼����������
	virtual BOOL LogWindowSwitch(int rec_id, int from_wnd_id, int to_wnd_id, const CTime &time) = 0;	
	virtual BOOL Log() = 0;							//��¼																													
	virtual BOOL RecordFailed() = 0;																																	//��¼ʧ��

	virtual BOOL Connect(void) = 0;																																		// �������ݿ�
	virtual BOOL DisConnect(void) = 0;																																//ȡ������
	virtual BOOL IsConnected() = 0;	 																																	//�ж��Ƿ����ӳɹ�

	virtual BOOL RecordWndCreateCount(int rec_id, const CTime& time, int count) = 0;
};
