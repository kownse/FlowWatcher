#pragma once
#include "WindowInfo.h"
#include <utility>
//#include <list>
#include <vector>
#include <list>



//专门用于窗口焦点切换的图结构
class CWndActiveGraph
{
	////邻接表节点
	struct listnode 
	{
		int v;					//头结点索引
		int count;			//切换计数
		CTime time;		//当前边最近活动时间
		explicit listnode(int x, int c, CTime actTime):v(x),count(c),time(actTime)
		{
			//v = x;
			//count = c;
		}
	};

	//链表节点
	struct headnode 
	{
		CTime activeTime;																			//当前活动时间
		HWND hWnd;
		vector<listnode> list;																	//连接索引
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
	int V() const {return Vcnt;}																	//返回顶点数
	int E() const {return Ecnt;}																	//返回边数
	bool directed() const {return direct;}													//返回是否是有向图
	void insert(HWND v, HWND w);															//插入具有起点与终点边
	void insert(HWND v);																			//插入只有起点
	// //延迟插入
	void insertDelay(HWND v, HWND w);
	void insertDelay(HWND v);

	std::vector<CWndActiveGraph::headnode>::iterator remove(HWND v);																		//删除一个节点
	const CString log();																						//将邻接图以图的方式生成
	const vector<HWND> getHWNDs() const;															//得到节点中所有的HWND
	const bool connected(int v, int w) const;														//在邻接表中连接确认
	const bool connected_moral(int v, int w) const;											//在无向邻接矩阵中连接确认
	const int getV() const{return Vcnt;};																		//返回节点数
	const bool hasSomething() const;																		//是否有内容要记录
	void newFixTime();																			//新的检测时间
	void newFixTime(HWND hWnd);														//刷新监控时间和当前活动窗口的活动时间
	BOOL RemoveFadeNodeEdge(map<HWND, WindowInfo> &hwnd_info);																//移除那些过时的节点和边

	//得到聚类结果
	void GetBKCliqueInt(vector<vector<int>> &result);													//得到聚类结果以图的标号表示
	void GetBKCliqueHWND(vector<vector<const HWND>> &result);							//得到聚类结果以窗口句柄表示
	bool ClusterInHistory(map<HWND, WindowInfo> &hwnd_info);								// 将聚类结果存储到历史记录中
	const vector<vector<vector<pair<HWND, float>>>>& GetClusterHistory();						//得到聚类历史记录 

private:
	//Bron-Kerbosch聚类内容///////////////////////////////////////////////
	vector<vector<int>> BronKerbosch(vector<vector<int>> &result);		//Bron-Kerbosch聚类
	void extend2(int *old, int *compsub,int ne, int ce, vector<vector<int>> &result);									//Bron-Kerbosch扩展函数
	int c;																										//聚类阶数

	vector<headnode>& moralize(vector<headnode> &result);				//有向图转无向图.无向图以邻接表表示
	vector<vector<bool>>& moralize(vector<vector<bool>> &result);		//有向图转无向图.无向图以vector邻接矩阵表示
	//////////////////////////////////////////////////////////////////////////

	int Vcnt;																							//顶点计数
	int Ecnt;																							//边计数
	bool direct;																						//是否是有向图

	//这个邻接表的数据结构具有很大的可变性，暂时用list吧
	vector<headnode> adj;																	//头结点列表
	vector<vector<bool>> moralized_graph;												//转换后的无向图
	CTime fixTime;																						//当前检测时间

	static const int  threshold = 2;																	//连接阈值
	static const int thresholdDelay = 3;															//持续焦点时间的阈值
	

private:
	// 前一个起点窗口句柄
	HWND lastV;
	// 前一个终点句柄
	HWND lastW;
	// 前一次插入时间
	CTime m_tLastTime;
	//聚类历史
	vector<vector<vector<pair<HWND, float>>>> m_vClusterHistory;

public:
	// 从历史记录中删除HWND记录
	bool RemoveFromClusterHistory(HWND target);
};
