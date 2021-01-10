#pragma once
#include "stdafx.h"
#include "MyRect.h"
#include "MyIndiv.h"

class CGA
{
public:
	CGA(void);
	CGA(CRect ClientRect);
	~CGA(void);
private:
	MyIndiv relatedRects;					//��ؾ��μ���
	MyIndiv nonRelatedRects;				//����ؾ��μ���

	static const int PopSize = 100;							//��Ⱥ��С
	static const int Iterations = 300;						//��������
	static const int Dimension = 3;					//������ѡ��ľ�����ģ
	static const int  NElitists = 2;							//��Ӣ���屣������
	static const int Omega = 5;							//�������ţ�
	static const double   CrossProb;						//�������
	static const double   MutateProb;					//�������
	static const int poolSize = PopSize / 2;						//����ش�С
	static const double w1;
	static const double w2;
	static const double w3;
	static const double w4;	//��Ӧ������Ŀ�꺯����Ȩֵ

	CRect m_clientRect;									//�ͻ�����С
	MyRect m_focusRect;
	MyIndiv AllRects;										//���в����ľ���
	MyRect coreRect;										//����Ȩ������

	//MyIndiv OriginIndiv/*, IndivForEvolve*/;					//���в����ľ���
	vector<unsigned> Area;						//������Ӧ���еĴ�С

	vector<MyIndiv> parent;	//��ʼ��Ⱥ������Ⱥ��
	vector<MyIndiv> offsprings;	//����Ⱥ
	vector<MyIndiv> matingPool;//�����
	vector<MyIndiv> elitist;//ÿ���ľ�Ӣ����

private:
	static bool ImportanceCompare(MyRect& a, MyRect& b);							//�Ƚ���Ҫ��
	static bool FitnessCompare(MyIndiv& a, MyIndiv& b);										//������Ӧ��ֵ�Ƚ�
	unsigned rectsUnionArea(const MyIndiv& rectangle);								//���������
	bool IsEqual(const double& dA, const double& dB, const double& dE);		//����������ж�


	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* Ŀ�꺯��ǰ������                                                   */
	/************************************************************************/

	double objective1(const MyIndiv& rectangle, const CRect& screenWorkRect);	//��Ļ�ռ��˷���
	double objective2(const MyIndiv& rectangle, const CRect& screenWorkRect);	//�������ռ���д������������
	double objective4(const MyRect& coreRect, const CRect& focusRect);	//Ȩ�������뽹������Ľ���ռȨ�������������
	double objective3(const MyIndiv& originRect, const MyIndiv& rectangle);//���������ͼ


	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* Լ���ж�����ǰ������                                                 */
	/************************************************************************/

	void SetRectArea(const MyIndiv& indiv, const CRect& screenWorkRect);//ȷ��ÿ�����ڵĴ�С��ռ�õ���Ļ�����
	void constraint1(MyRect& coreRect, const MyRect& focusRect, const double& dRate);//Ȩ�������뽹����������
	void constraint2(MyIndiv& rectangle, const CRect& screenWorkRect);//Ȩ�����ڲ��ܱ�����
	void constraint3(MyIndiv& rectangle);//��Ե������ش���λ��

	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* ������С���������Ӧ��ֵ�ͷ�����                                                                     */
	/************************************************************************/

	double AreaLimit(const MyIndiv& indiv, const CRect& screenWorkRect);

	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* �Ŵ����Ӻ���ǰ������                                                 */
	/************************************************************************/

	void tournamentSelect(const vector<MyIndiv>& pop, vector<MyIndiv>& matingPool, const unsigned& poolSize, const unsigned& dimension);//ѡ������
	void crossover(const vector<MyIndiv>& pop, vector<MyIndiv>& resPop, const double& dPc);//��������
	void mutate(vector<MyIndiv>& pop, const CRect& screenWorkRect, const double& dPm);	//��������
	MyIndiv mutateNewIndiv(const MyIndiv& rectangle, const CRect& screenWorkRect, const double& dPm = 1.);	//��һ��ԭʼ�����������һ���¸���

	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* ������ɸ��彫��Ⱥ��������������Լ����                               */
	/************************************************************************/

	void initializePop(const MyIndiv& rectangle, vector<MyIndiv>& pop, const unsigned& popSize, const CRect& screenWorkRect);//���ݳ�ʼ�����ʼ����ʼ��Ⱥ

	//////////////////////////////////////////////////////////////////////////

public:
	//���ÿͻ�������
	void SetClientRect(const CRect & rect);

	//ת������
	BOOL TransformData(vector<pair<sHWNDAtt,CBitmap *>> &source);
	// �������������
	BOOL RandomRect();
	// ׼������
	BOOL PrePare(void);
	// һ�μ���
	BOOL OneTurn(void);

	//////////////////////////////////////////////////////////////////////////
	//Getter����Щ
	const MyIndiv & GetAllRects();
	const MyRect & GetCoreRect();
	const vector<MyIndiv> & GetOffsprings();
	const vector<MyIndiv> & GetElitist();
	const double GetBestFitness();
	const MyIndiv & GetBestElitist();
};

