#pragma once
#include "WindowInfo.h"
#include <utility>
//#include <list>
#include <vector>
#include <list>



//ר�����ڴ��ڽ����л���ͼ�ṹ
class CWndActiveGraph
{
	////�ڽӱ�ڵ�
	struct listnode 
	{
		int v;					//ͷ�������
		int count;			//�л�����
		CTime time;		//��ǰ������ʱ��
		explicit listnode(int x, int c, CTime actTime):v(x),count(c),time(actTime)
		{
			//v = x;
			//count = c;
		}
	};

	//����ڵ�
	struct headnode 
	{
		CTime activeTime;																			//��ǰ�ʱ��
		HWND hWnd;
		vector<listnode> list;																	//��������
		explicit headnode(HWND hTmp, const vector<listnode>& tmp)
			:hWnd(hTmp),list(tmp)
		{
			//hWnd = hTmp;
			//list = tmp;
			//next = tmp;
		}

		explicit headnode(HWND hTmp, const vector<listnode>& tmp, const CTime& time)
			:hWnd(hTmp),list(tmp),activeTime(time)
		{
			//hWnd = hTmp;
			//list = tmp;
			//activeTime = time;
		}
	};

public:
	explicit CWndActiveGraph(void);
	~CWndActiveGraph(void);
	int V() const {return Vcnt;}																	//���ض�����
	int E() const {return Ecnt;}																	//���ر���
	bool directed() const {return direct;}													//�����Ƿ�������ͼ
	void insert(HWND v, HWND w);															//�������������յ��
	void insert(HWND v);																			//����ֻ�����
	// //�ӳٲ���
	void insertDelay(HWND v, HWND w);
	void insertDelay(HWND v);

	std::vector<CWndActiveGraph::headnode>::iterator remove(HWND v);																		//ɾ��һ���ڵ�
	const CString log();																						//���ڽ�ͼ��ͼ�ķ�ʽ����
	const vector<HWND> getHWNDs() const;															//�õ��ڵ������е�HWND
	const bool connected(int v, int w) const;														//���ڽӱ�������ȷ��
	const bool connected_moral(int v, int w) const;											//�������ڽӾ���������ȷ��
	const int getV() const{return Vcnt;};																		//���ؽڵ���
	const bool hasSomething() const;																		//�Ƿ�������Ҫ��¼
	void newFixTime();																			//�µļ��ʱ��
	void newFixTime(HWND hWnd);														//ˢ�¼��ʱ��͵�ǰ����ڵĻʱ��
	BOOL RemoveFadeNodeEdge(map<HWND, WindowInfo> &hwnd_info);																//�Ƴ���Щ��ʱ�Ľڵ�ͱ�

	//�õ�������
	void GetBKCliqueInt(vector<vector<int>> &result);													//�õ���������ͼ�ı�ű�ʾ
	void GetBKCliqueHWND(vector<vector<const HWND>> &result);							//�õ��������Դ��ھ����ʾ
	bool ClusterInHistory(map<HWND, WindowInfo> &hwnd_info);								// ���������洢����ʷ��¼��
	const vector<vector<vector<pair<HWND, float>>>>& GetClusterHistory();						//�õ�������ʷ��¼ 

private:
	//Bron-Kerbosch��������///////////////////////////////////////////////
	vector<vector<int>> BronKerbosch(vector<vector<int>> &result);		//Bron-Kerbosch����
	void extend2(int *old, int *compsub,int ne, int ce, vector<vector<int>> &result);									//Bron-Kerbosch��չ����
	int c;																										//�������

	vector<headnode>& moralize(vector<headnode> &result);				//����ͼת����ͼ.����ͼ���ڽӱ��ʾ
	vector<vector<bool>>& moralize(vector<vector<bool>> &result);		//����ͼת����ͼ.����ͼ��vector�ڽӾ����ʾ
	//////////////////////////////////////////////////////////////////////////

	int Vcnt;																							//�������
	int Ecnt;																							//�߼���
	bool direct;																						//�Ƿ�������ͼ

	//����ڽӱ�����ݽṹ���кܴ�Ŀɱ��ԣ���ʱ��list��
	vector<headnode> adj;																	//ͷ����б�
	vector<vector<bool>> moralized_graph;												//ת���������ͼ
	CTime fixTime;																						//��ǰ���ʱ��

	static const int  threshold = 2;																	//������ֵ
	static const int thresholdDelay = 3;															//��������ʱ�����ֵ
	

private:
	// ǰһ����㴰�ھ��
	HWND lastV;
	// ǰһ���յ���
	HWND lastW;
	// ǰһ�β���ʱ��
	CTime m_tLastTime;
	//������ʷ
	vector<vector<vector<pair<HWND, float>>>> m_vClusterHistory;

public:
	// ����ʷ��¼��ɾ��HWND��¼
	bool RemoveFromClusterHistory(HWND target);
};
