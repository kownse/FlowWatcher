#include "StdAfx.h"
#include "WndActiveGraph.h"
#include <math.h>
//#include "BronKerbosch.h"

CWndActiveGraph::CWndActiveGraph(void) : Vcnt(0), Ecnt(0), direct(false), lastV(NULL), lastW(NULL)
, m_tLastTime(0)
{
	fixTime = CTime::GetCurrentTime();
	m_tLastTime = CTime::GetCurrentTime();
}

CWndActiveGraph::~CWndActiveGraph(void)
{
}

//插入具有起点与终点边
void CWndActiveGraph::insert(HWND v, HWND w)
{
	CTime currentTime = CTime::GetCurrentTime();
	//两个窗口信息结点的索引
	int indexV = -1, indexW = -1;
	//遍历所有节点，找到对应的索引号
	unsigned int index = 0;
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++, index++)
	{
		//判断结束
		if(indexV != -1 && indexW != -1)
			break;

		//找到v索引号
		if(indexV == -1 && it->hWnd == v)
		{
			indexV = index;
		}
		//找到w索引号
		else if(indexW == -1 && it->hWnd == w)
		{
			indexW = index;
		}
	}

	//判断是否有新节点
	//有就插入新节点
	if(indexV == -1)
	{
		adj.push_back(headnode(v, vector<listnode>(), fixTime));
		indexV = adj.size() - 1;
		Vcnt++;
	}

	if(indexW == -1)
	{
		adj.push_back(headnode(w, vector<listnode>(), fixTime));
		indexW = adj.size() - 1;
		Vcnt++;
	}

	//添加边信息
	//adj[indexV].list.push_back(indexW);
	//adj[indexW].list.push_back(indexV);
	//遍历寻找边
	unsigned int i=0;
	for(; i<adj[indexV].list.size(); i++)
	{
		//找到了那个边，就增加边的值
		if(adj[indexV].list[i].v == indexW)
		{
			adj[indexV].list[i].count++;
			adj[indexV].list[i].time = currentTime;
			break;
		}
	}
	//没有找到那个边，就增加一条边
	if(i >= adj[indexV].list.size())
	{
		adj[indexV].list.push_back(listnode(indexW, 1, currentTime));
		Ecnt++;
	}
	//修改节点活动时间为当前时间
	adj[indexV].activeTime = currentTime;
	adj[indexW].activeTime = currentTime;
}

//插入只有起点
void CWndActiveGraph::insert(HWND v)
{
	CTime currentTime = CTime::GetCurrentTime();
	//两个窗口信息结点的索引
	int indexV = -1;
	//遍历所有节点，找到对应的索引号
	//for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	for(unsigned int i=0; i < adj.size(); i++)
	{
		headnode *it = &adj[i];
		//找到v索引号
		if(indexV == -1 && it->hWnd == v)
		{
			indexV = i;
			adj[indexV].activeTime = currentTime;
			break;
		}
	}

	//判断是否有新节点
	//有就插入新节点
	if(indexV == -1)
	{
		adj.push_back(headnode(v, vector<listnode>(), currentTime));
		indexV = adj.size() - 1;
		Vcnt++;
	}
}

//删除一个节点
std::vector<CWndActiveGraph::headnode>::iterator CWndActiveGraph::remove(HWND v)
{
	//两个窗口信息结点的索引
	int indexV = -1;
	//遍历所有节点，找到对应的索引号
	//for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	vector<headnode>::iterator target = adj.begin();
	for(unsigned int i=0; i < adj.size() && target != adj.end(); i++, target++)
	{
		headnode *it = &adj[i];
		//找到v索引号
		if(indexV == -1 && it->hWnd == v)
		{
			indexV = i;
			break;
		}
	}

	//如果没找到就退出
	if(target == adj.end())
		return adj.end();

	//vector<headnode>::iterator erease =
	//遍历全表，删除所有与这个节点相关的信息
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	{
		for (vector<listnode>::iterator ij = it->list.begin(); ij != it->list.end();)
		{
			//删除与这个节点相关的边
			if(ij->v == indexV)
			{
				//vector<listnode>::iterator tmp = ij;
				//ij++;
				ij = it->list.erase(ij);
				Ecnt--;		

				//if(ij == it->list.end())
				//	break;
			}
			//调整索引号大于该节点索引的边
			//这样有用哦。。。？
			else
			{
				if(ij->v > indexV)
					ij->v--;
				ij++;
			}
		}
	}
	//删除当前节点
	//vector<headnode>::iterator it = adj.;
	Vcnt--;
	return adj.erase(target);
}

//无向邻接矩阵中连接确认
const bool CWndActiveGraph::connected_moral(int v, int w) const
{
	if(v == w)
		return true;

	if(moralized_graph[v][w] == true)
		return true;

	if(moralized_graph[w][v] == true)
		return true;

	return false;
}

//连接确认
const bool CWndActiveGraph::connected(int v, int w) const
{
	//如果是自己，就返回真
	if(v == w)
		return true;

	//从v的角度遍历
	for(vector<listnode>::const_iterator it = adj[v].list.begin(); it != adj[v].list.end(); it++)
	{
		if(it->v == w && it->count >= threshold)
			return true;
	}

	//从w的角度遍历
	for(vector<listnode>::const_iterator it = adj[w].list.begin(); it != adj[w].list.end(); it++)
	{
		if(it->v == v && it->count >= threshold)
			return true;
	}

	//两边都没有，就返回假
	return false;
}

//得到聚类结果
void CWndActiveGraph::GetBKCliqueInt(vector<vector<int>> &result)
{
	//有向图转无向图
	moralize(moralized_graph);
	//BronKerbosch::Procedure(*this, result);
	BronKerbosch(result);
}

//得到以HWND表示的聚类结果
void CWndActiveGraph::GetBKCliqueHWND(vector<vector<const HWND>> &result)
{
	//由于BK直接返回的是以标号表示的结果，所以在这里需要一个零时变量
	vector<vector<int>> tmpResult;

	moralize(moralized_graph);
	BronKerbosch(tmpResult);

	//遍历图以得到打包的HWND
	for(vector<vector<int>>::const_iterator it = tmpResult.begin(); it != tmpResult.end(); it++)
	{
		vector<const HWND> clique;
		for(vector<int>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			clique.push_back(adj[*ij].hWnd);
		}
		result.push_back(clique);
	}
}

//判断是否有必要记录
const bool CWndActiveGraph::hasSomething() const
{
	if(adj.size() > 0)
		return true;
	else
		return false;
}

//将有向图转换为无向图
//无向图以临界表表示
vector<CWndActiveGraph::headnode>& CWndActiveGraph::moralize(vector<CWndActiveGraph::headnode> &result)
{
	result = adj;

	return result;
}

//移除那些过时的节点和边
BOOL CWndActiveGraph::RemoveFadeNodeEdge(map<HWND, WindowInfo> &hwnd_info)
{
	//遍历所有结点，移除那些过时的节点
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	{
one_turn:
		if(it == adj.end())
		{
			break;
		}
		CTime &nodeTime = it->activeTime;
		CTimeSpan timeElipse = fixTime - nodeTime;
		int timeCount = timeElipse.GetTotalSeconds();
		//该节点距离当前观察时间已经有5分钟没有动作了
		//先将该窗口最小化，然后再从切换图中剔除他
		if(timeCount > 300)
		{
			//最小化
			::ShowWindow(it->hWnd, SW_SHOWMINIMIZED);
			//这个移除函数效率比较低，但是节点数量不会太多，
			//现在先将就
			it = remove(it->hWnd);
			goto one_turn;
		}
		
		//在hwnd_info中查找该节点的重要性，如果重要性没有达到阈值，就删除它
		map<HWND, WindowInfo>::const_iterator itl = hwnd_info.find(it->hWnd);
		WindowInfo *tempInfo;
		if(itl != hwnd_info.end())
		{
			tempInfo = &hwnd_info[it->hWnd];
			size_t attRate = tempInfo->GetAttRate();

			if(attRate <= 50)
			{
				it = remove(it->hWnd);
				goto one_turn;
			}
		}
	}
	return TRUE;
}

//有向图转无向图
//无向图以邻接矩阵表示
vector<vector<bool>>& CWndActiveGraph::moralize(vector<vector<bool>> &result)
{
	//清空邻接矩阵
	for(vector<vector<bool>>::iterator it = result.begin(); it != result.end(); it++)
	{
		it->clear();
	}
	result.clear();

	//构建与邻接表等价的有向邻接矩阵
	vector<headnode>::size_type i=0;
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++, i++)
	{
		vector<bool> temp(adj.size(), false);

		//判断当前节点是否与其他节点相连
		bool bConnectOther = false;

		for(vector<listnode>::iterator ij = it->list.begin(); ij != it->list.end(); ij++)
		{
			//删除那些切换次数少于阈值的边
			//这里尚待考虑，也许删除那些重要性低的节点比较划算
			if(ij->count >= threshold)
			{
				temp[ij->v] = true;
				bConnectOther = true;
			}
		}
		//如果没有和其他节点相连，则它至少要和自己是相连的
		if(bConnectOther == false)
		{
			temp[i] = true;
		}

		result.push_back(temp);
	}

	vector<pair<int, int>> newEage;
	//将具有相同子节点的父母节点相连
	for(vector<headnode>::size_type i=0; i<adj.size(); i++)
	{
		for(vector<headnode>::size_type j=0; j<adj.size(); j++)
		{
			if(result[j][i] == true)
			{
				for(vector<headnode>::size_type k=j + 1; k<adj.size(); k++)
				{
					if(result[k][i] == true)
					{
						//result[j][k] = true;
						//这里不能立刻添加边，否则在迭代中会影响后面的处理造成多余的添加
						//在此先记录下来
						newEage.push_back(make_pair(j, k));
					}
				}
			}
		}
	}

	//添加那些新加入的边
	for(vector<pair<int, int>>::iterator it = newEage.begin(); it != newEage.end(); it++)
	{
		int i = it->first;
		int j = it->second;

		result[i][j] = true;
	}

	return result;
}

//将图用日志写出来
const CString CWndActiveGraph::log()
{	
	CString msg;
	//遍历全表
	for(unsigned int i=0; i<adj.size(); i++)
	{
		CString num;
		num.Format("%d", i);
		msg.Append(num);
		for (unsigned int j=0; j<adj[i].list.size(); j++)
		{
			CString v;
			v.Format("%d", adj[i].list[j].v);
			CString count;
			count.Format("%d", adj[i].list[j].count);
			msg.Append("->[" + v + "," + count + "]");
		}
		msg.Append("\r\n");
	}
	msg.Append("\r\n");
	CString num;
	num.Format("%d", Vcnt);
	msg.Append(num + "节点\r\n聚类:\r\n");

	//聚类结果是在这里产生的
	vector<vector<int>> cliqueResult;
	GetBKCliqueInt(cliqueResult);
	CString cliqueStr;
	for(vector<vector<int>>::iterator it1 = cliqueResult.begin(); it1 != cliqueResult.end(); it1++)
	{
		for (vector<int>::iterator it2 = it1->begin(); it2 != it1->end(); it2++)
		{
			CString cliNum;
			cliNum.Format("%d", *it2);
			cliqueStr.Append(cliNum + " ");
		}
		cliqueStr.Append("\r\n");
	}

	cliqueStr.Append("--------------------------------------\r\n");

	msg.Append(cliqueStr);

	return msg;
}

//得到所有的HWND
const vector<HWND> CWndActiveGraph::getHWNDs() const
{
	vector<HWND> hWndList;
	for(vector<headnode>::const_iterator it = adj.begin(); it != adj.end(); it++)
	{
		hWndList.push_back(it->hWnd);
	}

	return hWndList;
}

//新的检测时间
void CWndActiveGraph::newFixTime()
{
	fixTime = CTime::GetCurrentTime();
}

//刷新监控时间和当前活动窗口的活动时间
void CWndActiveGraph::newFixTime(HWND hWnd)
{
	fixTime = CTime::GetCurrentTime();

	//找到当前活动节点并刷新其活动时间
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	{
		if(it->hWnd == hWnd)
		{
			it->activeTime = fixTime;
			break;
		}
	}
}

//Bron-Kerbosch聚类函数
vector<vector<int>> CWndActiveGraph::BronKerbosch(vector<vector<int>> &result)
{
	int *ALL = new int[Vcnt];
	int *compsub = new int[Vcnt];

	int a=Vcnt, b=Vcnt;

	//初始化索引列表
	for(c=0; c<Vcnt; c++)
	{
		ALL[c] = c;
	}
	c = 0;
	extend2(ALL, compsub, 0, Vcnt, result);

	delete [] ALL;
	delete [] compsub;

	return result;
}

//Bron-Kerbosch扩展函数
void CWndActiveGraph::extend2(int *old, int *compsub, int ne, int ce, vector<vector<int>> &result)
{
	int *newArray = new int[Vcnt];
	int nod = 0;														//
	int fixp = 0;														//选定的not节点
	int newne = 0,													//新的ne 
		newce = 0,													//新的ce
		i = 0, 
		j = 0, 
		count = 0, 
		pos = 0,
		p,
		s = 0,															//选出的最优候选节点
		sel,
		minnod = 0;												//最小连接计数

	minnod = ce;
	nod = 0;

	//寻找连接数最小的节点
	//遍历所有节点
	for(i = 0; i<ce && minnod != 0; i++)
	{
		p = old[i];
		count = 0;

		//遍历所有candidate
		for(j =ne; j<ce && count < minnod; j++)
		{
			//如果p和这个j节点没有连接
			//if(!connected[p][old[j]])
			if(!connected_moral(p,old[j]))
			{
				count++;
				pos = j;
			}
		}

		//测试新的最小连接节点
		if(count < minnod)
		{
			//选择当前节点为fixp并记录连接数
			fixp = p;
			minnod = count;
			//如果当前连接最少的节点是在not集合中，则s记录最后一个不与i相交的位置
			if(i < ne)
			{
				s = pos;
			}
			//否则s记录i的位置
			else
			{
				s = i;
				nod = 1;
			}
		}
	}

	//回溯处理所有没有连接到s的节点
	for(nod = minnod + nod; nod >=1; nod--)
	{
		//交换位置
		p = old[s];
		old[s] = old[ne];
		sel = old[ne] = p;			//sel是选中的下一个candidate

		//产生新的not集合
		newne = 0;
		for(i = 0; i<ne; i++)
		{
			//if(connected[sel][old[i]])
			if(connected_moral(sel, old[i]))
			{	
				newArray[newne] = old[i];
				newne++;
			}
		}

		//产生新的candidate集合
		newce = newne;

		for(i = ne + 1; i<ce; i++)
		{
			//if(connected[sel][old[i]])
			if(connected_moral(sel, old[i]))
			{

				newArray[newce] = old[i];
				newce++;
			}
		}

		//将当前节点加入到新的compsub
		compsub[c] = sel;
		c++;

		//not和candidate都为空
		//cout << "newce = " << newce << endl;
		if(newce == 0)
		{
			vector<int> row;
			int loc;
			for(loc = 0; loc < c; loc++)
				row.push_back(compsub[loc]);
			result.push_back(row);
		}
		else
		{
			if(newne < newce)
				extend2(newArray, compsub, newne, newce, result);
		}

		//从compsub中移除
		c--;
		//加入到not中
		ne++;
		if(nod > 1)
		{
			//选择一个没有和fixp相连的候选节点
			for(s = ne; s < ce; s++)
			{
				//if(!connected[fixp][old[s]])
				if(!connected_moral(fixp, old[s]))
				{
					break;
				}
			}
		}
	}

	delete [] newArray;
}

// //延迟插入
void CWndActiveGraph::insertDelay(HWND v, HWND w)
{
	//前一次切换的HWND

	//前一次插入时间
	//当前时间
	CTime currentTime = CTime::GetCurrentTime();
	CTimeSpan escapeTime = currentTime - m_tLastTime;

	//若焦点持续时间大于了3秒,就将句柄真正插入图中
	if(escapeTime.GetTotalSeconds() >= thresholdDelay)
	{
		//前一个插入也是双向边
		if(lastV != NULL && lastW != NULL)
		{
			insert(lastV, lastW);
		}
		//前一个插入仅仅是结点
		else if(lastV != NULL && lastW == NULL)
		{
			insert(lastV);
		}
		else if(lastV == NULL && lastW == NULL)
		{
			//没有前一次插入
		}
		else
		{
			AfxMessageBox("窗口句柄为空");
		}
	}
	else
	{
		//否则就不插入
	}

	//替换变量
	m_tLastTime = currentTime;
	lastV = v;
	lastW = w;
}

//延迟插入
void CWndActiveGraph::insertDelay(HWND v)
{
	//前一次切换的HWND
	//当前时间
	CTime currentTime = CTime::GetCurrentTime();
	CTimeSpan escapeTime = currentTime - m_tLastTime;

	//若焦点持续时间大于了3秒,就将句柄真正插入图中
	if(escapeTime.GetTotalSeconds() >= thresholdDelay)
	{
		if(lastV != NULL && lastW != NULL)
		{
			insert(lastV, lastW);
		}
		else if(lastV != NULL && lastW == NULL)
		{
			insert(lastV);
		}
		else if(lastV == NULL && lastW == NULL)
		{
			//没有前一次插入
		}
		else
		{
			AfxMessageBox("窗口句柄错误");
		}
	}
	else
	{
		//否则就不插入
	}

	//替换变量
	m_tLastTime = currentTime;
	lastV = v;
	lastW = NULL;
}

// 将聚类结果存储到历史记录中
bool CWndActiveGraph::ClusterInHistory(map<HWND, WindowInfo> &hwnd_info)
{
	//得到本次聚类的HWND
	vector<vector<const HWND>> resHWND;
	GetBKCliqueHWND(resHWND);

	//得到正交底
	float base = 0.0f;
	for(vector<vector<const HWND>>::iterator it = resHWND.begin(); it != resHWND.end(); it++)
	{
			for(vector<const HWND>::iterator ij = it->begin(); ij != it->end();)
			{
				//找到那个句柄
				map<HWND, WindowInfo>::iterator ik = hwnd_info.find(*ij);

				//ASSERT(ik != hwnd_info.end());
				//找到句柄
				if(ik != hwnd_info.end())
				{
					float tmp = static_cast<float>(ik->second.GetAttRate());
					base += pow(tmp,2);
					ij++;
				}
				else
				{
					//如果hwnd_info里没有找到那个句柄，说明这个句柄已经被洗白了，应该删除聚类结果中的这个东西
					ij = it->erase(ij);
				}

			}
	}
	if(base <= 0)
	{
		base = 1;
	}
	base = sqrt(base);

	//本次历史存储
	vector<vector<pair<HWND, float>>> currentHistory;

	for(vector<vector<const HWND>>::const_iterator it = resHWND.begin(); it != resHWND.end(); it++)
	{
		vector<pair<HWND, float>> oneCluster;
		for(vector<const HWND>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			//找到那个句柄
			map<HWND, WindowInfo>::iterator ik = hwnd_info.find(*ij);
			//找到句柄
			if(ik != hwnd_info.end())
			{
				oneCluster.push_back(make_pair(static_cast<HWND>(*ij), ik->second.GetAttRate() / base));
			}
		}
		currentHistory.push_back(oneCluster);
	}
	////存入历史记录中
	m_vClusterHistory.push_back(currentHistory);

	return false;
}

//得到聚类历史记录
const vector<vector<vector<pair<HWND, float>>>>& CWndActiveGraph::GetClusterHistory()
{
	return m_vClusterHistory;
}

// 从历史记录中删除HWND记录
bool CWndActiveGraph::RemoveFromClusterHistory(HWND target)
{
	for(vector<vector<vector<pair<HWND, float>>>>::iterator it = m_vClusterHistory.begin(); it != m_vClusterHistory.end();)
	{
		for(vector<vector<pair<HWND, float>>>::iterator ij = it->begin(); ij != it->end();)
		{
			for(vector<pair<HWND, float>>::iterator ik = ij->begin(); ik != ij->end();)
			{
				if(ik->first == target)
				{
					ik = ij->erase(ik);
				}
				else
				{
					ik++;
				}
			}
			//判断这行删除完没
			if(ij->size() <= 0)
			{
				ij = it->erase(ij);
			}
			else
			{
				ij++;
			}
		}

		//判断这个集合删除完没
		if(it->size() <= 0)
		{
			it = m_vClusterHistory.erase(it);
		}
		else
		{
			it++;
		}
	}
	return true;
}
