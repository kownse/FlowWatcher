#pragma once
#include "mysql.h"
#include "AbstractDBManager.h"
//���ݿ������
class DBManager : public CAbstractDBManager
{
public:
	explicit DBManager(void);
	~DBManager(void);
	
private:
//	MYSQL mysql;																																				//mysql����
//	int m_iFailCount;																																			//ʧ�ܼ���
//	BOOL m_bConnected;																																	//���ӱ�־
public:
	const unsigned int NewRecord(TCHAR *computerName, const CTime& time);																	//�����µļ�¼
	const unsigned int NewRecord(TCHAR *computerName, const CTime& time, const CString& version);								//���а汾�ŵ��¼�¼
	//const unsigned int NewVersion(unsigned int rec_id, const CString& version);																	//���°汾��
	const unsigned int NewWnd(const CTime& createTime, unsigned int rec_id, const CString& title, const CString& className, const RECT& rect);									//�´��ڼ�¼
	BOOL WindowActive(unsigned int wnd_id, bool flag, const CTime& time);																//���ڼ�����Ϣ
	BOOL WindowAddTitle(unsigned int wnd_id, const CString& title, const CTime& time);															//��Ӵ��ڱ���
	BOOL WindowMove(unsigned int wnd_id, const CTime& time, const RECT& rect);																//�����ƶ�
	BOOL WindowDestory(unsigned int wnd_id, const CTime& time);																				//���ڹر�
	BOOL NewKeyType(unsigned int wnd_id, unsigned int keyCount, const CTime& time);												//���ڰ�����Ϣ
	BOOL NewMouseClick(unsigned int wnd_id, unsigned int mouseCount, const CTime& time);									//�������Ϣ
	BOOL WindowAttTime(unsigned int wnd_id, int lastTime, bool focus, double attBalance);								//��¼�������ʱ��
	BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, const CTime &time);		//��¼�������ʱ��ͷǽ������ʱ��
	BOOL WindowAttTime(unsigned int wnd_id, int lastTime1, int lastTime2, bool focus, double attBalance, float avgKey, float avgMouse, const CTime &time);
	BOOL LogCliqueResult(int rec_id, const CTime& time, const CString& logText);																		// ��¼����������
	BOOL LogWindowSwitch(int rec_id, int from_wnd_id, int to_wnd_id, const CTime &time);	
	BOOL RecordFailed();																																	//��¼ʧ��

	BOOL Connect(void);																																		// �������ݿ�
	BOOL DisConnect(void);																																//ȡ������
	BOOL IsConnected();																																		//�ж��Ƿ����ӳɹ�
	
	BOOL RecordWndCreateCount(int rec_id, const CTime& time, int count);
	BOOL Log(){ return TRUE;}
};
