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

//�������������յ��
void CWndActiveGraph::insert(HWND v, HWND w)
{
	CTime currentTime = CTime::GetCurrentTime();
	//����������Ϣ��������
	int indexV = -1, indexW = -1;
	//�������нڵ㣬�ҵ���Ӧ��������
	unsigned int index = 0;
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++, index++)
	{
		//�жϽ���
		if(indexV != -1 && indexW != -1)
			break;

		//�ҵ�v������
		if(indexV == -1 && it->hWnd == v)
		{
			indexV = index;
		}
		//�ҵ�w������
		else if(indexW == -1 && it->hWnd == w)
		{
			indexW = index;
		}
	}

	//�ж��Ƿ����½ڵ�
	//�оͲ����½ڵ�
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

	//��ӱ���Ϣ
	//adj[indexV].list.push_back(indexW);
	//adj[indexW].list.push_back(indexV);
	//����Ѱ�ұ�
	unsigned int i=0;
	for(; i<adj[indexV].list.size(); i++)
	{
		//�ҵ����Ǹ��ߣ������ӱߵ�ֵ
		if(adj[indexV].list[i].v == indexW)
		{
			adj[indexV].list[i].count++;
			adj[indexV].list[i].time = currentTime;
			break;
		}
	}
	//û���ҵ��Ǹ��ߣ�������һ����
	if(i >= adj[indexV].list.size())
	{
		adj[indexV].list.push_back(listnode(indexW, 1, currentTime));
		Ecnt++;
	}
	//�޸Ľڵ�ʱ��Ϊ��ǰʱ��
	adj[indexV].activeTime = currentTime;
	adj[indexW].activeTime = currentTime;
}

//����ֻ�����
void CWndActiveGraph::insert(HWND v)
{
	CTime currentTime = CTime::GetCurrentTime();
	//����������Ϣ��������
	int indexV = -1;
	//�������нڵ㣬�ҵ���Ӧ��������
	//for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	for(unsigned int i=0; i < adj.size(); i++)
	{
		headnode *it = &adj[i];
		//�ҵ�v������
		if(indexV == -1 && it->hWnd == v)
		{
			indexV = i;
			adj[indexV].activeTime = currentTime;
			break;
		}
	}

	//�ж��Ƿ����½ڵ�
	//�оͲ����½ڵ�
	if(indexV == -1)
	{
		adj.push_back(headnode(v, vector<listnode>(), currentTime));
		indexV = adj.size() - 1;
		Vcnt++;
	}
}

//ɾ��һ���ڵ�
std::vector<CWndActiveGraph::headnode>::iterator CWndActiveGraph::remove(HWND v)
{
	//����������Ϣ��������
	int indexV = -1;
	//�������нڵ㣬�ҵ���Ӧ��������
	//for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	vector<headnode>::iterator target = adj.begin();
	for(unsigned int i=0; i < adj.size() && target != adj.end(); i++, target++)
	{
		headnode *it = &adj[i];
		//�ҵ�v������
		if(indexV == -1 && it->hWnd == v)
		{
			indexV = i;
			break;
		}
	}

	//���û�ҵ����˳�
	if(target == adj.end())
		return adj.end();

	//vector<headnode>::iterator erease =
	//����ȫ��ɾ������������ڵ���ص���Ϣ
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	{
		for (vector<listnode>::iterator ij = it->list.begin(); ij != it->list.end();)
		{
			//ɾ��������ڵ���صı�
			if(ij->v == indexV)
			{
				//vector<listnode>::iterator tmp = ij;
				//ij++;
				ij = it->list.erase(ij);
				Ecnt--;		

				//if(ij == it->list.end())
				//	break;
			}
			//���������Ŵ��ڸýڵ������ı�
			//��������Ŷ��������
			else
			{
				if(ij->v > indexV)
					ij->v--;
				ij++;
			}
		}
	}
	//ɾ����ǰ�ڵ�
	//vector<headnode>::iterator it = adj.;
	Vcnt--;
	return adj.erase(target);
}

//�����ڽӾ���������ȷ��
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

//����ȷ��
const bool CWndActiveGraph::connected(int v, int w) const
{
	//������Լ����ͷ�����
	if(v == w)
		return true;

	//��v�ĽǶȱ���
	for(vector<listnode>::const_iterator it = adj[v].list.begin(); it != adj[v].list.end(); it++)
	{
		if(it->v == w && it->count >= threshold)
			return true;
	}

	//��w�ĽǶȱ���
	for(vector<listnode>::const_iterator it = adj[w].list.begin(); it != adj[w].list.end(); it++)
	{
		if(it->v == v && it->count >= threshold)
			return true;
	}

	//���߶�û�У��ͷ��ؼ�
	return false;
}

//�õ�������
void CWndActiveGraph::GetBKCliqueInt(vector<vector<int>> &result)
{
	//����ͼת����ͼ
	moralize(moralized_graph);
	//BronKerbosch::Procedure(*this, result);
	BronKerbosch(result);
}

//�õ���HWND��ʾ�ľ�����
void CWndActiveGraph::GetBKCliqueHWND(vector<vector<const HWND>> &result)
{
	//����BKֱ�ӷ��ص����Ա�ű�ʾ�Ľ����������������Ҫһ����ʱ����
	vector<vector<int>> tmpResult;

	moralize(moralized_graph);
	BronKerbosch(tmpResult);

	//����ͼ�Եõ������HWND
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

//�ж��Ƿ��б�Ҫ��¼
const bool CWndActiveGraph::hasSomething() const
{
	if(adj.size() > 0)
		return true;
	else
		return false;
}

//������ͼת��Ϊ����ͼ
//����ͼ���ٽ���ʾ
vector<CWndActiveGraph::headnode>& CWndActiveGraph::moralize(vector<CWndActiveGraph::headnode> &result)
{
	result = adj;

	return result;
}

//�Ƴ���Щ��ʱ�Ľڵ�ͱ�
BOOL CWndActiveGraph::RemoveFadeNodeEdge(map<HWND, WindowInfo> &hwnd_info)
{
	//�������н�㣬�Ƴ���Щ��ʱ�Ľڵ�
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
		//�ýڵ���뵱ǰ�۲�ʱ���Ѿ���5����û�ж�����
		//�Ƚ��ô�����С����Ȼ���ٴ��л�ͼ���޳���
		if(timeCount > 300)
		{
			//��С��
			::ShowWindow(it->hWnd, SW_SHOWMINIMIZED);
			//����Ƴ�����Ч�ʱȽϵͣ����ǽڵ���������̫�࣬
			//�����Ƚ���
			it = remove(it->hWnd);
			goto one_turn;
		}
		
		//��hwnd_info�в��Ҹýڵ����Ҫ�ԣ������Ҫ��û�дﵽ��ֵ����ɾ����
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

//����ͼת����ͼ
//����ͼ���ڽӾ����ʾ
vector<vector<bool>>& CWndActiveGraph::moralize(vector<vector<bool>> &result)
{
	//����ڽӾ���
	for(vector<vector<bool>>::iterator it = result.begin(); it != result.end(); it++)
	{
		it->clear();
	}
	result.clear();

	//�������ڽӱ�ȼ۵������ڽӾ���
	vector<headnode>::size_type i=0;
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++, i++)
	{
		vector<bool> temp(adj.size(), false);

		//�жϵ�ǰ�ڵ��Ƿ��������ڵ�����
		bool bConnectOther = false;

		for(vector<listnode>::iterator ij = it->list.begin(); ij != it->list.end(); ij++)
		{
			//ɾ����Щ�л�����������ֵ�ı�
			//�����д����ǣ�Ҳ��ɾ����Щ��Ҫ�Ե͵Ľڵ�Ƚϻ���
			if(ij->count >= threshold)
			{
				temp[ij->v] = true;
				bConnectOther = true;
			}
		}
		//���û�к������ڵ���������������Ҫ���Լ���������
		if(bConnectOther == false)
		{
			temp[i] = true;
		}

		result.push_back(temp);
	}

	vector<pair<int, int>> newEage;
	//��������ͬ�ӽڵ�ĸ�ĸ�ڵ�����
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
						//���ﲻ��������ӱߣ������ڵ����л�Ӱ�����Ĵ�����ɶ�������
						//�ڴ��ȼ�¼����
						newEage.push_back(make_pair(j, k));
					}
				}
			}
		}
	}

	//�����Щ�¼���ı�
	for(vector<pair<int, int>>::iterator it = newEage.begin(); it != newEage.end(); it++)
	{
		int i = it->first;
		int j = it->second;

		result[i][j] = true;
	}

	return result;
}

//��ͼ����־д����
const CString CWndActiveGraph::log()
{	
	CString msg;
	//����ȫ��
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
	msg.Append(num + "�ڵ�\r\n����:\r\n");

	//�������������������
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

//�õ����е�HWND
const vector<HWND> CWndActiveGraph::getHWNDs() const
{
	vector<HWND> hWndList;
	for(vector<headnode>::const_iterator it = adj.begin(); it != adj.end(); it++)
	{
		hWndList.push_back(it->hWnd);
	}

	return hWndList;
}

//�µļ��ʱ��
void CWndActiveGraph::newFixTime()
{
	fixTime = CTime::GetCurrentTime();
}

//ˢ�¼��ʱ��͵�ǰ����ڵĻʱ��
void CWndActiveGraph::newFixTime(HWND hWnd)
{
	fixTime = CTime::GetCurrentTime();

	//�ҵ���ǰ��ڵ㲢ˢ����ʱ��
	for(vector<headnode>::iterator it = adj.begin(); it != adj.end(); it++)
	{
		if(it->hWnd == hWnd)
		{
			it->activeTime = fixTime;
			break;
		}
	}
}

//Bron-Kerbosch���ຯ��
vector<vector<int>> CWndActiveGraph::BronKerbosch(vector<vector<int>> &result)
{
	int *ALL = new int[Vcnt];
	int *compsub = new int[Vcnt];

	int a=Vcnt, b=Vcnt;

	//��ʼ�������б�
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

//Bron-Kerbosch��չ����
void CWndActiveGraph::extend2(int *old, int *compsub, int ne, int ce, vector<vector<int>> &result)
{
	int *newArray = new int[Vcnt];
	int nod = 0;														//
	int fixp = 0;														//ѡ����not�ڵ�
	int newne = 0,													//�µ�ne 
		newce = 0,													//�µ�ce
		i = 0, 
		j = 0, 
		count = 0, 
		pos = 0,
		p,
		s = 0,															//ѡ�������ź�ѡ�ڵ�
		sel,
		minnod = 0;												//��С���Ӽ���

	minnod = ce;
	nod = 0;

	//Ѱ����������С�Ľڵ�
	//�������нڵ�
	for(i = 0; i<ce && minnod != 0; i++)
	{
		p = old[i];
		count = 0;

		//��������candidate
		for(j =ne; j<ce && count < minnod; j++)
		{
			//���p�����j�ڵ�û������
			//if(!connected[p][old[j]])
			if(!connected_moral(p,old[j]))
			{
				count++;
				pos = j;
			}
		}

		//�����µ���С���ӽڵ�
		if(count < minnod)
		{
			//ѡ��ǰ�ڵ�Ϊfixp����¼������
			fixp = p;
			minnod = count;
			//�����ǰ�������ٵĽڵ�����not�����У���s��¼���һ������i�ཻ��λ��
			if(i < ne)
			{
				s = pos;
			}
			//����s��¼i��λ��
			else
			{
				s = i;
				nod = 1;
			}
		}
	}

	//���ݴ�������û�����ӵ�s�Ľڵ�
	for(nod = minnod + nod; nod >=1; nod--)
	{
		//����λ��
		p = old[s];
		old[s] = old[ne];
		sel = old[ne] = p;			//sel��ѡ�е���һ��candidate

		//�����µ�not����
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

		//�����µ�candidate����
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

		//����ǰ�ڵ���뵽�µ�compsub
		compsub[c] = sel;
		c++;

		//not��candidate��Ϊ��
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

		//��compsub���Ƴ�
		c--;
		//���뵽not��
		ne++;
		if(nod > 1)
		{
			//ѡ��һ��û�к�fixp�����ĺ�ѡ�ڵ�
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

// //�ӳٲ���
void CWndActiveGraph::insertDelay(HWND v, HWND w)
{
	//ǰһ���л���HWND

	//ǰһ�β���ʱ��
	//��ǰʱ��
	CTime currentTime = CTime::GetCurrentTime();
	CTimeSpan escapeTime = currentTime - m_tLastTime;

	//���������ʱ�������3��,�ͽ������������ͼ��
	if(escapeTime.GetTotalSeconds() >= thresholdDelay)
	{
		//ǰһ������Ҳ��˫���
		if(lastV != NULL && lastW != NULL)
		{
			insert(lastV, lastW);
		}
		//ǰһ����������ǽ��
		else if(lastV != NULL && lastW == NULL)
		{
			insert(lastV);
		}
		else if(lastV == NULL && lastW == NULL)
		{
			//û��ǰһ�β���
		}
		else
		{
			AfxMessageBox("���ھ��Ϊ��");
		}
	}
	else
	{
		//����Ͳ�����
	}

	//�滻����
	m_tLastTime = currentTime;
	lastV = v;
	lastW = w;
}

//�ӳٲ���
void CWndActiveGraph::insertDelay(HWND v)
{
	//ǰһ���л���HWND
	//��ǰʱ��
	CTime currentTime = CTime::GetCurrentTime();
	CTimeSpan escapeTime = currentTime - m_tLastTime;

	//���������ʱ�������3��,�ͽ������������ͼ��
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
			//û��ǰһ�β���
		}
		else
		{
			AfxMessageBox("���ھ������");
		}
	}
	else
	{
		//����Ͳ�����
	}

	//�滻����
	m_tLastTime = currentTime;
	lastV = v;
	lastW = NULL;
}

// ���������洢����ʷ��¼��
bool CWndActiveGraph::ClusterInHistory(map<HWND, WindowInfo> &hwnd_info)
{
	//�õ����ξ����HWND
	vector<vector<const HWND>> resHWND;
	GetBKCliqueHWND(resHWND);

	//�õ�������
	float base = 0.0f;
	for(vector<vector<const HWND>>::iterator it = resHWND.begin(); it != resHWND.end(); it++)
	{
			for(vector<const HWND>::iterator ij = it->begin(); ij != it->end();)
			{
				//�ҵ��Ǹ����
				map<HWND, WindowInfo>::iterator ik = hwnd_info.find(*ij);

				//ASSERT(ik != hwnd_info.end());
				//�ҵ����
				if(ik != hwnd_info.end())
				{
					float tmp = static_cast<float>(ik->second.GetAttRate());
					base += pow(tmp,2);
					ij++;
				}
				else
				{
					//���hwnd_info��û���ҵ��Ǹ������˵���������Ѿ���ϴ���ˣ�Ӧ��ɾ���������е��������
					ij = it->erase(ij);
				}

			}
	}
	if(base <= 0)
	{
		base = 1;
	}
	base = sqrt(base);

	//������ʷ�洢
	vector<vector<pair<HWND, float>>> currentHistory;

	for(vector<vector<const HWND>>::const_iterator it = resHWND.begin(); it != resHWND.end(); it++)
	{
		vector<pair<HWND, float>> oneCluster;
		for(vector<const HWND>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			//�ҵ��Ǹ����
			map<HWND, WindowInfo>::iterator ik = hwnd_info.find(*ij);
			//�ҵ����
			if(ik != hwnd_info.end())
			{
				oneCluster.push_back(make_pair(static_cast<HWND>(*ij), ik->second.GetAttRate() / base));
			}
		}
		currentHistory.push_back(oneCluster);
	}
	////������ʷ��¼��
	m_vClusterHistory.push_back(currentHistory);

	return false;
}

//�õ�������ʷ��¼
const vector<vector<vector<pair<HWND, float>>>>& CWndActiveGraph::GetClusterHistory()
{
	return m_vClusterHistory;
}

// ����ʷ��¼��ɾ��HWND��¼
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
			//�ж�����ɾ����û
			if(ij->size() <= 0)
			{
				ij = it->erase(ij);
			}
			else
			{
				ij++;
			}
		}

		//�ж��������ɾ����û
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
