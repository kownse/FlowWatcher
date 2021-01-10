#pragma once
#include "abstractdbmanager.h"

//////////////////////////////////////////////////////////////////////////
//�����Ƕ�Ƕ��ʽmysql�ķ�װ��
//Ϊ�˴ﵽ�������ݿ���������ṹ��Ŀ�ģ�����Ҳ�����������ݿ�����
//��ȡ����SQL���Ĺ���
//////////////////////////////////////////////////////////////////////////
class CEmbeddedMySql :
	public CAbstractDBManager
{
public:
	CEmbeddedMySql(void);
	~CEmbeddedMySql(void);

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
	BOOL LogWindowSwitch(int rec_id,int from_wnd_id, int to_wnd_id, const CTime &time);																				//��¼�л�����ʷ
	BOOL RecordFailed();																																	//��¼ʧ��

	BOOL Connect(void);							//��ʼ������Ƕ��ʽ���ݿ�		
	BOOL ConnectExistDB(void);				//�����Ѿ����ڵ����ݿ�
	BOOL FreeEmbededConnect(void);	//�ͷ�Ƕ��ʽ����
	BOOL DisConnect(void);					//�ر�Զ������																										//ȡ������
	BOOL IsConnected();							//���ر���Ƕ��ʽ���ݿ��Ƿ���																																//�ж��Ƿ����ӳɹ�

	BOOL RecordWndCreateCount(int rec_id, const CTime& time, int count);		//����û��
	BOOL ConnectRemoteServer();																		//����Զ������������
	BOOL DropLocalDB();																					//���𱾵����ݿ�
	BOOL RebuildLocalDB();																				//�ؽ��������ݿ�

	//////////////////////////////////////////////////////////////////////////
	//������������¼����
	BOOL CommitNewRecord();																			//���������ɵļ�¼
	BOOL CommitOldRecord();																			//��ÿ�����еķ�������ϢΪ����������Ϣ
	BOOL ReCommitAllRecord();																			//���´����������ݣ������·����������Ϣ
	//////////////////////////////////////////////////////////////////////////
	//�ӷ�������ȡ����
	BOOL GetDataFromServer();																			//��Զ��������ȡ�Լ�����Ϣ
	//////////////////////////////////////////////////////////////////////////
	BOOL Log(){return CommitNewRecord();}																	//ֱ���Ǵ���
	BOOL ExportAllTable();														//�����б������ļ���

private:
	BOOL GenerateCnf(char* exactPath);								//���������ļ�
	BOOL InitEmbeddedDB();													//��ʼ��Ƕ��ʽ���ݿ�ṹ
	CString getNullableValue(const char* value);						//�õ����ֿ���ΪNULL��Ԫ��
	BOOL LogBugRemote(const CString &com_name, const CString &bugSql, const CString &describe);													//��Զ�̷������洢���ݿ�SQL BUG��Ϣ

#ifdef _DEBUG
	BOOL TestEmbeddedMySql();											//����Ƕ��ʽmysql�Ƿ��ܽ����ݿ�
	BOOL ShowLocalDBArchitecture();										//��ʾ�������ݿ�ṹ
	BOOL ExportTable(const CString &table);							//���������Ϣ
	
	
#endif // _DEBUG

private:
	MYSQL m_remoteMysql;													//Զ��MYSQL����
	BOOL m_bEmbeddedEnable;														//Ƕ��ʽ���ݿ����гɹ�
	char path[MAX_PATH];														//��ǰ����·��
};
