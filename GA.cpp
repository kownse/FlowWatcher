#include "StdAfx.h"
#include "GA.h"
#include <algorithm>
#include <math.h>

const double CGA::CrossProb = 0.4;
const double CGA::MutateProb = 0.9;
const double CGA::w1 = 0.4;
const double CGA::w2 = 0.4;
const double CGA::w3 = 0.2;
const double CGA::w4 = 0.4;

CGA::CGA(void)
{
	elitist = vector<MyIndiv>(NElitists);		//ÿ���ľ�Ӣ����

	//�ӿ���̨������ʼ�������������
	SYSTEMTIME st;
	GetLocalTime(&st);
	//Rng::seed((long)st.wMilliseconds );
	srand((long)st.wMilliseconds);
}

CGA::CGA(CRect ClientRect)
{
	//�ӿ���̨������ʼ�������������
	SYSTEMTIME st;
	GetLocalTime(&st);
	//Rng::seed((long)st.wMilliseconds );
	srand((long)st.wMilliseconds);

	m_clientRect = ClientRect;
	elitist = vector<MyIndiv>(NElitists);		//ÿ���ľ�Ӣ����

}

CGA::~CGA(void)
{
}

//��Ҫ�ԱȽ�
bool CGA::ImportanceCompare(MyRect& a, MyRect& b)
{
	return a.m_dImportance > b.m_dImportance ? true : false;
}

/************************************************************************/
/* �����Ժ����������󲢵ȣ�                                             */
/************************************************************************/

//���������
unsigned CGA::rectsUnionArea(const MyIndiv& rectangle)
{
	ASSERT(!rectangle.empty());

	//��ȡX������
	vector<int> RectX;
	for(vector<int>::size_type i=0;i<rectangle.size();i++){
		RectX.push_back(rectangle[i].left);
		RectX.push_back(rectangle[i].right);
	}

	//�������
	vector<int>::size_type l=RectX.size();int menu;
	while(l){
		for(vector<int>::size_type j=0;j<l-1;j++){
			if(RectX[j]>RectX[j+1]){
				menu=RectX[j];
				RectX[j]=RectX[j+1];
				RectX[j+1]=menu;
			}
		}
		l-=1;
	}

	int x1,x2,sum=0;
	for(vector<int>::size_type m=0;m<RectX.size()-1;m++){
		x1=RectX[m];
		x2=RectX[m+1];
		if(x1==x2) continue;

		//�ҳ���x1,x2�йصľ��ε�Y
		vector<int> mm;
		for(vector<int>::size_type n=0;n<rectangle.size();n++){
			if(rectangle[n].left<=x1 && rectangle[n].right>=x2){
				mm.push_back(rectangle[n].top);
				mm.push_back(rectangle[n].bottom);
			}
		}

		if(mm.size()==0) continue;//�¼��ϵġ�ȷʵ������ʱ��û����ص�Y�Ŀ��ܡ�

		//��¼�ϱߺ��±�
		vector<int> shang,xia;
		for(vector<int>::size_type q=0;q<mm.size()-1;q+=2){
			int e=0,r=0;
			for(vector<int>::size_type g=0;g<shang.size();g++){
				if(mm[q]==shang[g]) {e=1; break;}
			}
			for(vector<int>::size_type h=0;h<xia.size();h++){
				if(mm[q+1]==xia[h]) {r=1; break;}
			}
			if(e==0) shang.push_back(mm[q]);
			if(r==0) xia.push_back(mm[q+1]);
		}

		//����
		vector<int>::size_type l2=mm.size();int menu2;
		while(l2){
			for(vector<int>::size_type j2=0;j2<l2-1;j2++){
				if(mm[j2]>mm[j2+1]){
					menu2=mm[j2];
					mm[j2]=mm[j2+1];
					mm[j2+1]=menu2;
				}
			}
			l2-=1;
		}

		//�����ն�
		int up=0,ss=0;
		for(vector<int>::size_type p=0;p<mm.size();p++){
			int e2=0;
			for(vector<int>::size_type f=0;f<shang.size();f++){
				if(mm[p]==shang[f]){
					e2=1;
					break;
				}
			}
			if(e2==1) up+=1;else up-=1;

			if(up==0 && p<(mm.size()-1)){
				ss=ss+((x2-x1) * (mm[p+1]-mm[p]));
			}
		}

		sum=sum+(x2-x1) * (mm[mm.size()-1]-mm[0])-ss;
	}

	//cout<<"���ε������"<<sum<<endl;
	return sum;
}

//����������ж�
bool CGA::IsEqual(const double& dA, const double& dB, const double& dE)
{
	return (dA == dB || fabs(dA - dB) < dE) ? true : false;
}

//������Ӧ��ֵ�Ƚ�
bool CGA::FitnessCompare(MyIndiv& a, MyIndiv& b)
{
	return a.m_dFitness > b.m_dFitness ? true : false;
}

//Ŀ�꺯��1-3
double CGA::objective1(const MyIndiv& rectangle, const CRect& screenWorkRect)
{
	ASSERT(!rectangle.empty() && !screenWorkRect.IsRectEmpty());
	//ASSERT(rectsUnionArea(rectangle) <= screenWorkRect.Width() * screenWorkRect.Height());
	//ԭʼ����
	double percent = (double)rectsUnionArea(rectangle) / (screenWorkRect.Width() * screenWorkRect.Height());

	if (percent < 0.9)
		percent *= sqrt(percent) * percent;

	return percent;
}

double CGA::objective2(const MyIndiv& rectangle, const CRect& screenWorkRect)
{
	ASSERT(!rectangle.empty() && !screenWorkRect.IsRectEmpty());
	
	double fit = 0.;
	for (MyIndiv::size_type i = 0; i < rectangle.size(); i++)
	{
		double percent = 0.;

		MyIndiv tempIndiv;
		CRect tempRect;
		for (MyIndiv::size_type j = 0; j < rectangle.size(); j++)
		{
			if (j != i)
			{
				if (tempRect.IntersectRect(rectangle[i], rectangle[j]))
				{
					tempIndiv.push_back(tempRect);
				}
			}
		}
		percent = tempIndiv.empty() ? 0. : (double)rectsUnionArea(tempIndiv) / 
			(rectangle[i].Width() * rectangle[i].Height());
		
		if (percent > 0.2)
			percent *= (sqrt(1. + percent) * (1. + percent));
		/*else
			percent *= sqrt(1. + percent);*/
		//percent *= (1. + 3 * percent);

		fit += (1 - percent) /** rectangle[i].m_dImportance*/;
	}

	return fit / rectangle.size();

}

double CGA::objective3(const MyIndiv& originRect, const MyIndiv& rectangle)
{
	ASSERT(!originRect.empty() && !rectangle.empty() && originRect.size() == rectangle.size());

	double fit = 0.;
	for (MyIndiv::size_type i = 0; i < rectangle.size(); i++)
	{
		double percent = 0.;

		CRect tempRect;
		if (tempRect.IntersectRect(originRect[i], rectangle[i]))
		{
			percent = (double)tempRect.Width() * tempRect.Height() / 
				(originRect[i].Width() * originRect[i].Height());
		}
		//percent *= sqrt(1. + percent);

		fit += percent * originRect[i].m_dImportance;
	}

	return fit / originRect.size();

}

// double CGA::objective4(const MyIndiv& originRect, const MyIndiv& rectangle)
// {
// 	ASSERT(!originRect.empty() && !rectangle.empty() && originRect.size() == rectangle.size());
// 
// 	double fit = 0.;
// 	for (MyIndiv::size_type i = 0; i < rectangle.size(); i++)
// 	{
// 		CRect tempRect;
// 		if (tempRect.IntersectRect(originRect[i], rectangle[i]))
// 		{
// 			fit += (tempRect.Width() * tempRect.Height()) / 
// 				(originRect[i].Width() * originRect[i].Height());
// 		}
// 	}
// 	return fit / rectangle.size();
// }

double CGA::objective4(const MyRect& coreRect, const CRect& focusRect)//��ΪԼ��������
{
	ASSERT(!focusRect.IsRectEmpty());

	CRect rt;
	if (rt.IntersectRect((CRect)coreRect, focusRect))
	{
		//ASSERT((double)(rt.Width() * rt.Height()) < (coreRect.Width() * coreRect.Height()));
		return (double)(rt.Width() * rt.Height()) / (coreRect.Width() * coreRect.Height());
	}
	else return 0.;
}

//Լ�����Ժ���1-3

//ȷ��ÿ�����ڵĴ�С��ռ�õ���Ļ�����
void CGA::SetRectArea(const MyIndiv& indiv, const CRect& screenWorkRect)
{
	ASSERT(!indiv.empty() && !screenWorkRect.IsRectEmpty());

	if (!Area.empty())
	{
		Area.clear();
	}

	double impSum = 0.;
	for (MyIndiv::size_type i = 0; i < indiv.size(); i++)
	{
		impSum += indiv[i].m_dImportance;
	}

	unsigned screenArea = screenWorkRect.Width() * screenWorkRect.Height();

	for (MyIndiv::size_type j = 0; j < indiv.size(); j++)
	{
		Area.push_back(screenArea * (indiv[j].m_dImportance / impSum));
	}
}

void CGA::constraint1(MyRect& coreRect, const MyRect& focusRect, const double& dRate)
{
	ASSERT(!coreRect.IsRectEmpty() && !focusRect.IsRectEmpty() && dRate != 0.);

	CRect rt;
	if (rt.IntersectRect((CRect)coreRect, focusRect))
	{
		//ASSERT((double)(rt.Width() * rt.Height()) < (coreRect.Width() * coreRect.Height()));
		double tempRate = (rt.Width() * rt.Height()) / (coreRect.Width() * coreRect.Height());
		if (tempRate < dRate)
		{
			coreRect.left = focusRect.left;
			coreRect.top = focusRect.top;
			coreRect.right = focusRect.right;
			coreRect.bottom = focusRect.bottom;
		}
	}	
	else
	{
		coreRect.left = focusRect.left;
		coreRect.top = focusRect.top;
		coreRect.right = focusRect.right;
		coreRect.bottom = focusRect.bottom;
	}
}

void CGA::constraint2(MyIndiv& rectangle, const CRect& screenWorkRect)
{
	ASSERT(!rectangle.empty());
}

void CGA::constraint3(MyIndiv& rectangle)
{
	ASSERT(!rectangle.empty());
}

//NEW!!������С�������
double CGA::AreaLimit(const MyIndiv& indiv, const CRect& screenWorkRect)
{
	ASSERT(!indiv.empty() && !screenWorkRect.IsRectEmpty());

	unsigned expectedArea = screenWorkRect.Width() * screenWorkRect.Height() / 10;

	if (0 == indiv.size())
	{
		return 0.;
	}
	else if (indiv.size() > 1)
	{
		unsigned passedCheckCount = indiv.size();
		unsigned unreasonableCount = 0;

		for (MyIndiv::size_type i = 0; i < indiv.size(); i++)
		{
			unsigned rectArea = indiv[i].Width() * indiv[i].Height();
			if (rectArea < expectedArea)
			{
				passedCheckCount--;
			}

			for (MyIndiv::size_type j = i + 1; j < indiv.size(); j++)
			{
				//���
				unsigned ai = indiv[i].Width() * indiv[i].Height();
				unsigned aj = indiv[j].Width() * indiv[j].Height();

				if ((ai > aj && indiv[i].m_dImportance < indiv[j].m_dImportance) ||
					(ai < aj && indiv[i].m_dImportance > indiv[j].m_dImportance))
				{
					unreasonableCount++;
				}
			}
		}

		return (double)passedCheckCount / indiv.size() * 
			(1. - unreasonableCount / (indiv.size() * (indiv.size() - 1) / 2.)); 
	}
	else return 1.;

}

//�Ŵ�����1-3
void CGA::tournamentSelect(const vector<MyIndiv>& pop, vector<MyIndiv>& matingPool, const unsigned& poolSize, const unsigned& dimension)//ѡ������
{
	ASSERT(!pop.empty() && dimension > 0 && poolSize > 0);

	for (unsigned t = 0; t < poolSize; t++)
	{
		vector<MyIndiv> tempPop;
		for (unsigned i = 0; i < dimension; i++)
		{
			//unsigned index = Rng::discrete(0, pop.size() - 1);
			unsigned index = rand() % pop.size();
			tempPop.push_back(pop[index]);
		}
		//�ҳ�С������Ӧ��ֵ��ߵĸ�����뽻���
		//���̶�ѡ��һ������
		//sort(tempPop.begin(), tempPop.end(), FitnessCompare);

		double fitnessSum = 0.;
		for (vector<MyIndiv>::size_type j = 0; j < tempPop.size(); j++)
		{
			fitnessSum += tempPop[j].m_dFitness;
		}

		vector<double> probSum(tempPop.size());
		double tempFitness = 0.;
		for (vector<MyIndiv>::size_type m = 0; m < tempPop.size(); m++)
		{
			tempFitness += tempPop[m].m_dFitness;	
			probSum[m] = tempFitness / fitnessSum;
		}

		//double p = Rng::uni(0., 1.);
		double p = rand() / (double)(RAND_MAX + 1);
		for (vector<double>::size_type n = 0; n < probSum.size(); n++)
		{
			if (probSum[n] >= p)
			{
				matingPool.push_back(tempPop[n]);
				break;
			}
		}
	}
}

void CGA::crossover(const vector<MyIndiv>& pop, vector<MyIndiv>& resPop, const double& dPc)//��������
{
	ASSERT(!pop.empty() && dPc != 0);

	for (vector<MyIndiv>::size_type i = 0; i < pop.size() / 2; i++)
	{
		//unsigned dadIndex = Rng::discrete(0, pop.size() - 1);
		//unsigned momIndex = Rng::discrete(0, pop.size() - 1);
		unsigned dadIndex = rand() % pop.size();
		unsigned momIndex = rand() % pop.size();

		MyIndiv dad = pop[dadIndex];
		MyIndiv mom = pop[momIndex];

		//if (Rng::coinToss(dPc))
		if((rand() / (double)(RAND_MAX + 1)) < dPc && dad.size() != 1)
		{
			MyIndiv temp;
			//unsigned crossPoint = Rng::discrete(1, dad.size() - 2);
			int crossPoint = rand() % (dad.size() - 1) + 1;
			temp.insert(temp.begin(), dad.begin() + crossPoint, dad.end());
			dad.erase(dad.begin() + crossPoint, dad.end());
			dad.insert(dad.begin() + crossPoint, mom.begin() + crossPoint, mom.end());
			mom.erase(mom.begin() + crossPoint, mom.end());
			mom.insert(mom.begin() + crossPoint, temp.begin(), temp.end());

			resPop.push_back(dad);
			resPop.push_back(mom);
		}
		else
		{
			resPop.push_back(dad);
			resPop.push_back(mom);
		}
	}
}

void CGA::mutate(vector<MyIndiv>& pop, const CRect& screenWorkRect, const double& dPm)//��������
{
	ASSERT(!pop.empty());

	//ȷ��X��Y������췶Χ
	int mutateScopeRadiusX = screenWorkRect.Width();
	int mutateScopeRadiusY = screenWorkRect.Height();

	int Xrag = mutateScopeRadiusX / 10;
	int Yrag = mutateScopeRadiusY / 10;

	for (vector<MyIndiv>::size_type i = 0; i < pop.size(); i++)
	{
		//if (Rng::coinToss(dPm))
		if((rand() / (double)(RAND_MAX + 1)) < dPm)
		{
			//unsigned mutatePoint = Rng::discrete(0, pop[i].size() - 1);
			unsigned mutatePoint = rand() % pop[i].size();

			//����ʱ���ݴ��ڵ�ǰ�������������췶Χ������
			//�ȱ�����촰�ڵ����
			unsigned area = pop[i][mutatePoint].Width() * pop[i][mutatePoint].Height();
			//����ϵ�����ֱ�Ϊ��������ͷ��������ϵ��
			double ep = 0., em = 0.;
			//��������ϵ��
			for (MyIndiv::size_type j = 0; j < pop[i].size(); j++)
			{
				if (j != mutatePoint)
				{
					unsigned tempArea = pop[i][j].Width() * pop[i][j].Height();
					if (pop[i][mutatePoint].m_dImportance < pop[i][j].m_dImportance && 
						area > tempArea)
					{
						//��������Χ�����������С������
						em += (area - tempArea) / (double)area * 
							(pop[i][j].m_dImportance - pop[i][mutatePoint].m_dImportance);
					}
					else if (pop[i][mutatePoint].m_dImportance > pop[i][j].m_dImportance && 
						area < tempArea)
					{
						//��������Χ������������������
						ep += (tempArea - area) / (double)area * 
							(pop[i][mutatePoint].m_dImportance - pop[i][j].m_dImportance);
					}
				}
			}

			em /= pop[i].size() - 1/*mutatePoints.size()*/;
			ep /= pop[i].size() - 1/*mutatePoints.size()*/;

			if(em > ep)
			{
				pop[i][mutatePoint].left += rand() % Xrag * (1. + em - ep);
				pop[i][mutatePoint].top += rand() % Yrag * (1. + em - ep);
			}
			else
			{
				pop[i][mutatePoint].left -= rand() % Xrag * (1. + em - ep);
				pop[i][mutatePoint].top -= rand() % Yrag * (1. + em - ep);
			}

			if(em >= ep)
			{
				pop[i][mutatePoint].right -= rand() % Xrag * (1. + ep - em);
				pop[i][mutatePoint].bottom -= rand() % Yrag * (1. + ep - em);
			}
			else
			{
				pop[i][mutatePoint].right += rand() % Xrag * (1. + ep - em);
				pop[i][mutatePoint].bottom += rand() % Yrag * (1. + ep - em);
			}

			//pop[i][mutatePoint].left += Rng::discrete(-Xrag, Xrag);
			//pop[i][mutatePoint].top += Rng::discrete(-Yrag, Yrag);
			//pop[i][mutatePoint].right += Rng::discrete(-Xrag, Xrag);
			//pop[i][mutatePoint].bottom += Rng::discrete(-Yrag, Yrag);

			//pop[i][mutatePoint].left += rand() % (Xrag * 2 + 1) - Xrag;
			//pop[i][mutatePoint].top += rand() % (Yrag * 2 + 1) - Yrag;
			//pop[i][mutatePoint].right += rand() % (Xrag * 2 + 1) - Xrag;
			//pop[i][mutatePoint].bottom +=rand() % (Yrag * 2 + 1) - Yrag;

			pop[i][mutatePoint].left = (pop[i][mutatePoint].left < 0)? 0: pop[i][mutatePoint].left;
			pop[i][mutatePoint].left = (pop[i][mutatePoint].left > screenWorkRect.Width())? screenWorkRect.Width(): pop[i][mutatePoint].left;
			pop[i][mutatePoint].right = (pop[i][mutatePoint].right < 0)? 0: pop[i][mutatePoint].right;
			pop[i][mutatePoint].right = (pop[i][mutatePoint].right > screenWorkRect.Width())? screenWorkRect.Width(): pop[i][mutatePoint].right;
			pop[i][mutatePoint].top = (pop[i][mutatePoint].top < 0)? 0: pop[i][mutatePoint].top;
			pop[i][mutatePoint].top = (pop[i][mutatePoint].top > screenWorkRect.Height())? screenWorkRect.Height(): pop[i][mutatePoint].top;
			pop[i][mutatePoint].bottom = (pop[i][mutatePoint].bottom < 0)? 0: pop[i][mutatePoint].bottom;
			pop[i][mutatePoint].bottom = (pop[i][mutatePoint].bottom > screenWorkRect.Height())? screenWorkRect.Height(): pop[i][mutatePoint].bottom;

			//������ɺ��������
			pop[i][mutatePoint].NormalizeRect();
		}
	}
}

MyIndiv CGA::mutateNewIndiv(const MyIndiv& rectangle, const CRect& screenWorkRect, const double& dPm)
{
	MyIndiv resIndiv(rectangle);

	//ȷ��X��Y������췶Χ
	int mutateScopeRadiusX = screenWorkRect.Width();
	int mutateScopeRadiusY = screenWorkRect.Height();

	int Xrag = mutateScopeRadiusX / 10;
	int Yrag = mutateScopeRadiusY / 10;

	for (MyIndiv::size_type i = 0; i < resIndiv.size(); i++)
	{
		//if (Rng::coinToss(dPm))
		if((rand() / (double)(RAND_MAX + 1)) < dPm)
		{
			//resIndiv[i].left += Rng::discrete(-Xrag, Xrag);
			//resIndiv[i].top += Rng::discrete(-Yrag, Yrag);
			//resIndiv[i].right += Rng::discrete(-Xrag, Xrag);
			//resIndiv[i].bottom += Rng::discrete(-Yrag, Yrag);

			resIndiv[i].left += rand() % (Xrag * 2 + 1) - Xrag;
			resIndiv[i].top += rand() % (Yrag * 2 + 1) - Yrag;
			resIndiv[i].right += rand() % (Xrag * 2 + 1) - Xrag;
			resIndiv[i].bottom +=rand() % (Yrag * 2 + 1) - Yrag;

			resIndiv[i].left = (resIndiv[i].left < 0)? 0: resIndiv[i].left;
			resIndiv[i].left = (resIndiv[i].left > screenWorkRect.Width())? screenWorkRect.Width(): resIndiv[i].left;
			resIndiv[i].right = (resIndiv[i].right < 0)? 0: resIndiv[i].right;
			resIndiv[i].right = (resIndiv[i].right > screenWorkRect.Width())? screenWorkRect.Width(): resIndiv[i].right;
			resIndiv[i].top = (resIndiv[i].top < 0)? 0: resIndiv[i].top;
			resIndiv[i].top = (resIndiv[i].top > screenWorkRect.Height())? screenWorkRect.Height(): resIndiv[i].top;
			resIndiv[i].bottom = (resIndiv[i].bottom < 0)? 0: resIndiv[i].bottom;
			resIndiv[i].bottom = (resIndiv[i].bottom > screenWorkRect.Height())? screenWorkRect.Height(): resIndiv[i].bottom;


			//������ɺ��������
			resIndiv[i].NormalizeRect();
		}

	}
	return resIndiv;
}

//��ʼ����ʼ��Ⱥ��Լ�����Դ�����
void CGA::initializePop(const MyIndiv& rectangle, vector<MyIndiv>& pop, const unsigned& popSize, const CRect& screenWorkRect)//���ݳ�ʼ�����ʼ����ʼ��Ⱥ
{
	ASSERT(!rectangle.empty());

	if (!pop.empty())
		pop.clear();

	for (vector<MyIndiv>::size_type i = 0; i < popSize; i++)
	{
		//��һ��������Ǿ�Ӣ����
		if (!i)
		{
			pop.push_back(rectangle);
			continue;
		}
		//����Ķ��Ǳ��������
		pop.push_back(mutateNewIndiv(rectangle, screenWorkRect, 1.0));
	}
}

// �������������
BOOL CGA::RandomRect()
{
	//const int numOfRects = 2;
	//AllRects = MyIndiv(numOfRects);

	//for (MyIndiv::size_type i = 0; i < numOfRects; i++)
	//{		
	//	while(AllRects[i].Width() < m_clientRect.Width()/4 || AllRects[i].Width() > m_clientRect.Width()*2/3
	//		|| AllRects[i].Height() < m_clientRect.Height()/4 || AllRects[i].Height() > m_clientRect.Height()*2/3)
	//	{
	//		//AllRects[i].left = Rng::discrete(0, m_clientRect.Width() - 1);
	//		//AllRects[i].top = Rng::discrete(0, m_clientRect.Height() - 1);
	//		//AllRects[i].right = Rng::discrete(0, m_clientRect.Width() - 1);
	//		//AllRects[i].bottom = Rng::discrete(0, m_clientRect.Height() - 1);

	//		AllRects[i].left = rand() % m_clientRect.Width();
	//		AllRects[i].top = rand() %  m_clientRect.Height();
	//		AllRects[i].right = rand() %  m_clientRect.Width();
	//		AllRects[i].bottom = rand() %  m_clientRect.Height();
	//		AllRects[i].NormalizeRect();

	//	}


	//	//if (Rng::coinToss(0.5))
	//	if((rand() / (double)(RAND_MAX + 1)) < 0.5)
	//	{
	//		AllRects[i].m_bRelationFlag = true;
	//		//AllRects[i].m_dImportance = Rng::uni(0.5, 1.);	
	//		AllRects[i].m_dImportance = rand() / (double)(RAND_MAX * 2 + 1) + 0.5;
	//		relatedRects.push_back(AllRects[i]);
	//	}
	//	else
	//	{
	//		AllRects[i].m_bRelationFlag = false;
	//		AllRects[i].m_dImportance = rand() / (double)(RAND_MAX * 2 + 1) - 0.5;
	//		nonRelatedRects.push_back(AllRects[i]);
	//	}
	//}

	////�Ծ��μ��ϰ���Ҫ�Խ������������ľ��μ��Ͼ��ǳ�ʼ����
	//sort(AllRects.begin(), AllRects.end(), &CGA::ImportanceCompare);

	//coreRect = AllRects[0];

	return TRUE;
}

//���ÿͻ�����С
void CGA::SetClientRect(const CRect & rect)
{
	m_clientRect = rect;
}

//���õ��Ĵ�����Ϣת��Ϊ�Ŵ��㷨���õĸ�ʽ
BOOL CGA::TransformData(vector<pair<sHWNDAtt,CBitmap *>> &source)
{
	AllRects.clear();
	AllRects = MyIndiv(source.size());
	for (MyIndiv::size_type i = 0; i < source.size(); i++)
	{
		AllRects[i].left = source[i].first.rect.left;
		AllRects[i].top = source[i].first.rect.top;
		AllRects[i].right =source[i].first.rect.right;
		AllRects[i].bottom = source[i].first.rect.bottom;
		AllRects[i].NormalizeRect();
		AllRects[i].m_dImportance = source[i].first.importance;

		AllRects[i].hWnd = source[i].first.hwnd;
		AllRects[i].bitmap = source[i].second;
		
	}

	//�Ծ��μ��ϰ���Ҫ�Խ������������ľ��μ��Ͼ��ǳ�ʼ����
	sort(AllRects.begin(), AllRects.end(), &CGA::ImportanceCompare);
	
	if(AllRects.size() > 0)
		return TRUE;
	else
		return FALSE;
}

//�õ����о���
const MyIndiv & CGA::GetAllRects()
{
	return AllRects;
}

//�õ����ľ���
const MyRect & CGA::GetCoreRect()
{
	return coreRect;

}

//�õ���һ��
const vector<MyIndiv> & CGA::GetOffsprings()
{
	return offsprings;
}

//�õ���Ӣ
const vector<MyIndiv> & CGA::GetElitist()
{
	return elitist;
}

//�õ���Ӧ��
const double CGA::GetBestFitness()
{
	return elitist[0].m_dFitness;
}

//�õ���õ�һ������
const MyIndiv & CGA::GetBestElitist()
{
	return elitist[0];
}

// ׼������
BOOL CGA::PrePare(void)
{
	//��2�������ɳ�ʼ��Ⱥ
	initializePop(AllRects, parent, PopSize, m_clientRect);

	//��3�������������Ӧ��
	m_focusRect = MyRect(m_clientRect.Width() / 5, m_clientRect.Height() / 5,
		m_clientRect.Width() * 3 / 5, m_clientRect.Height() * 3 / 5);//Ĭ�Ͻ����������

//	double w1 = 0.3, w2 = 0.1, w3 = 0.2, w4 = 0.4;	//��Ӧ������Ŀ�꺯����Ȩֵ
	for (vector<MyIndiv>::size_type j = 0; j < parent.size(); j++)
	{
// 		parent[j].m_dFitness = w1 * objective1(parent[j], m_clientRect) /*+ w2 * objective2(parent[j])*/ +
// 			w3 * objective3(coreRect, m_focusRect) + w4 * objective4(AllRects, parent[j]);

		double f0 = AreaLimit(parent[j], m_clientRect);
		double f1 = objective1(parent[j], m_clientRect);
		double f2 = objective2(parent[j], m_clientRect);
		double f3 = objective3(AllRects, parent[j]);

		parent[j].m_dFitness = f0 * (f1 * w1 + f2 * w2 + f3 * w3);
		//parent[j].m_dFitness = AreaLimit(parent[j], m_clientRect) *
		//(w1 * objective1(parent[j], m_clientRect) + 
		//w2 * objective2(parent[j], m_clientRect) + 
		//w3 * objective3(AllRects, parent[j]));
	}

	//����
	sort(parent.begin(), parent.end(), &CGA::FitnessCompare);
	for (vector<MyIndiv>::size_type r = 0; r < NElitists; r++)
	{
		elitist[r] = parent[r];
	}

	return TRUE;
}

// һ�μ���
BOOL CGA::OneTurn(void)
{
	//ѡ��
	tournamentSelect(parent, matingPool, poolSize, Dimension);

	//�õ�����أ��Խ�����и�����н������
	crossover(matingPool, offsprings, CrossProb);
	//�Ӵ���Ⱥ�뽻����еĸ�����ϳ��µ��Ӵ���Ⱥ
	for (vector<MyIndiv>::size_type k = 0; k < matingPool.size(); k++)
	{
		offsprings.push_back(matingPool[k]);
	}

	//��Ӣ����ֱ�Ӽ����Ӵ�
	for (vector<MyIndiv>::size_type s = 0; s < NElitists; s++)
	{
		offsprings.push_back(elitist[s]);
	}

	sort(offsprings.begin(), offsprings.end(), &CGA::FitnessCompare);
	//ɾȥNElitist����Ӧ��ֵ�͵ľ�Ӣ���壬ֻ����NElitist����Ӣ
	for (vector<MyIndiv>::size_type v = 0; v < NElitists; v++)
	{
		offsprings.pop_back();
	}

	//���Ӵ���Ⱥ�еĸ�����б���
	mutate(offsprings, m_clientRect, MutateProb);
	parent.assign(offsprings.begin(), offsprings.end());
	offsprings.clear();
	matingPool.clear();

	//�����Ӵ���Ⱥ�и������Ӧ��ֵ
	for (vector<MyIndiv>::size_type m = 0; m < parent.size(); m++)
	{
// 		parent[m].m_dFitness = w1 * objective1(parent[m], m_clientRect) /*+ w2 * objective2(parent[m])*/ +
// 			w3 * objective3(coreRect, m_focusRect) + w4 * objective4(AllRects, parent[m]);

		parent[m].m_dFitness = AreaLimit(parent[m], m_clientRect) * (w1 * objective1(parent[m], m_clientRect) + 
			w2 * objective2(parent[m], m_clientRect) +
			/*w3 * objective4(parent[m][0], m_focusRect) +*/ w3 * objective3(AllRects, parent[m]));
	}

	//����ǰ��Ⱥ�еľ�Ӣ���屣��������Ҫ�Ȳ����Ƿ�����о�Ӣ�������
	sort(parent.begin(), parent.end(), &CGA::FitnessCompare);
	for (vector<MyIndiv>::size_type q = 0; q <NElitists; q++)
	{
		elitist.push_back(parent[q]);
	}
	sort(elitist.begin(), elitist.end(), &CGA::FitnessCompare);
	//ɾȥNElitist����Ӧ��ֵ�͵ľ�Ӣ���壬ֻ����NElitist����Ӣ
	for (vector<MyIndiv>::size_type u = 0; u < NElitists; u++)
	{
		elitist.pop_back();
	}
	return 0;
}
