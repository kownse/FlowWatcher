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
	MyIndiv relatedRects;					//相关矩形集合
	MyIndiv nonRelatedRects;				//不相关矩形集合

	static const int PopSize = 100;							//种群大小
	static const int Iterations = 300;						//进化代数
	static const int Dimension = 3;					//锦标赛选择的竞赛规模
	static const int  NElitists = 2;							//精英个体保留个数
	static const int Omega = 5;							//线性缩放？
	static const double   CrossProb;						//交叉概率
	static const double   MutateProb;					//变异概率
	static const int poolSize = PopSize / 2;						//交配池大小
	static const double w1;
	static const double w2;
	static const double w3;
	static const double w4;	//对应于三个目标函数的权值

	CRect m_clientRect;									//客户区大小
	MyRect m_focusRect;
	MyIndiv AllRects;										//所有产生的矩形
	MyRect coreRect;										//保存权威矩形

	//MyIndiv OriginIndiv/*, IndivForEvolve*/;					//所有产生的矩形
	vector<unsigned> Area;						//各窗口应该有的大小

	vector<MyIndiv> parent;	//初始种群（父种群）
	vector<MyIndiv> offsprings;	//子种群
	vector<MyIndiv> matingPool;//交配池
	vector<MyIndiv> elitist;//每代的精英个体

private:
	static bool ImportanceCompare(MyRect& a, MyRect& b);							//比较重要性
	static bool FitnessCompare(MyIndiv& a, MyIndiv& b);										//个体适应度值比较
	unsigned rectsUnionArea(const MyIndiv& rectangle);								//矩形求并面积
	bool IsEqual(const double& dA, const double& dB, const double& dE);		//浮点数相等判断


	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* 目标函数前向声明                                                   */
	/************************************************************************/

	double objective1(const MyIndiv& rectangle, const CRect& screenWorkRect);	//屏幕空间浪费率
	double objective2(const MyIndiv& rectangle, const CRect& screenWorkRect);	//覆盖面积占所有窗口总面积比例
	double objective4(const MyRect& coreRect, const CRect& focusRect);	//权威窗口与焦点区域的交集占权威窗口面积比例
	double objective3(const MyIndiv& originRect, const MyIndiv& rectangle);//保存心象地图


	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* 约束判定函数前向声明                                                 */
	/************************************************************************/

	void SetRectArea(const MyIndiv& indiv, const CRect& screenWorkRect);//确定每个窗口的大小（占用的屏幕面积）
	void constraint1(MyRect& coreRect, const MyRect& focusRect, const double& dRate);//权威窗口与焦点区域契合
	void constraint2(MyIndiv& rectangle, const CRect& screenWorkRect);//权威窗口不能被覆盖
	void constraint3(MyIndiv& rectangle);//边缘化非相关窗口位置

	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* 窗口最小面积限制适应度值惩罚函数                                                                     */
	/************************************************************************/

	double AreaLimit(const MyIndiv& indiv, const CRect& screenWorkRect);

	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* 遗传算子函数前身声明                                                 */
	/************************************************************************/

	void tournamentSelect(const vector<MyIndiv>& pop, vector<MyIndiv>& matingPool, const unsigned& poolSize, const unsigned& dimension);//选择算子
	void crossover(const vector<MyIndiv>& pop, vector<MyIndiv>& resPop, const double& dPc);//交叉算子
	void mutate(vector<MyIndiv>& pop, const CRect& screenWorkRect, const double& dPm);	//变异算子
	MyIndiv mutateNewIndiv(const MyIndiv& rectangle, const CRect& screenWorkRect, const double& dPm = 1.);	//由一个原始个体变异生成一个新个体

	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/* 随机生成个体将种群补满（必须满足约束）                               */
	/************************************************************************/

	void initializePop(const MyIndiv& rectangle, vector<MyIndiv>& pop, const unsigned& popSize, const CRect& screenWorkRect);//根据初始个体初始化初始种群

	//////////////////////////////////////////////////////////////////////////

public:
	//设置客户端区域
	void SetClientRect(const CRect & rect);

	//转换数据
	BOOL TransformData(vector<pair<sHWNDAtt,CBitmap *>> &source);
	// 产生随机三角形
	BOOL RandomRect();
	// 准备计算
	BOOL PrePare(void);
	// 一次计算
	BOOL OneTurn(void);

	//////////////////////////////////////////////////////////////////////////
	//Getter函数些
	const MyIndiv & GetAllRects();
	const MyRect & GetCoreRect();
	const vector<MyIndiv> & GetOffsprings();
	const vector<MyIndiv> & GetElitist();
	const double GetBestFitness();
	const MyIndiv & GetBestElitist();
};

