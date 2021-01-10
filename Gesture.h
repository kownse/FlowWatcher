#pragma once
#include "feature.h"

class CGesture
{
public:
	CGesture(void);
	virtual ~CGesture(void) = 0;
private:
	CGesture(const CGesture &);
	CGesture& operator=(const CGesture &);

public:
	double Recognition(const CFeature *input);												//识别手势

protected:
	double  m_weight[FEATUREDISMENSION + 1];									//只写权值~你吃我嘛！
};

//全部最小化手势
class CLastGesture :
	public CGesture
{
public:
	CLastGesture(void)
	{
		m_weight[0] = -382.94415854997908;
		m_weight[1] = -133.9704197510448;
		m_weight[2] = 8.0094007179894646;
		m_weight[3] = 2324.5596789594611;
		m_weight[4] = 383.85327636261889;
		m_weight[5] = 322.51137530445402;
		m_weight[6] = 15252.401809822377;
		m_weight[7] = 6.5651198560566542;
		m_weight[8] = -7.679682392895594;
		m_weight[9] = 398.67701434149097;
		m_weight[10] = 11.94417993088493;
		m_weight[11] = 13.191299506966581;
		m_weight[12] = -14.410380659271548;
	}
	~CLastGesture(void){}
};
//关闭所有窗口手势
class CNextGesture :
	public CGesture
{
public:
	CNextGesture(void)
	{
		m_weight[0] = -382.69525747490178;
		m_weight[1] = 132.04186683106195;
		m_weight[2] = 21.489791182319969;
		m_weight[3] = 3106.3126255065722;
		m_weight[4] = 378.58038952959629;
		m_weight[5] = 316.38718998273833;
		m_weight[6] = 16377.553371302256;
		m_weight[7] = 0.41022533565984454;
		m_weight[8] = 4.1344703847901165;
		m_weight[9] = 374.62199762967634;
		m_weight[10] = -12.91225522379597;
		m_weight[11] = 5.1723395317383822;
		m_weight[12] = -3.7410449124751999;
	}
	~CNextGesture(void){}
};



//平铺手势
class CPlainLayoutGes :
	public CGesture
{
public:
	CPlainLayoutGes(void)
	{
		m_weight[0] = -382.28787099796807;
		m_weight[1] = -0.82655365912873813;
		m_weight[2] = -54.828987307993465;
		m_weight[3] = 2860.6058649049824;
		m_weight[4] = 380.57918382906763;
		m_weight[5] = 324.4855630641577;
		m_weight[6] = 18650.467980912679;
		m_weight[7] = 22.088358253697493;
		m_weight[8] = -3.935446872661986;
		m_weight[9] = 524.81383478527061;
		m_weight[10] = -17.685443552127346;
		m_weight[11] =7.8110561535182192;
		m_weight[12] = -7.7801921184464851;
	}
	~CPlainLayoutGes(void){}
};

class CCliqueGes :
	public CGesture
{
public:
	CCliqueGes(void)
	{
		m_weight[0] = -384.41591673446175;
		m_weight[1] = 54.89409359912068;
		m_weight[2] = 49.865261449805018;
		m_weight[3] = 2240.9740539921254;
		m_weight[4] = 380.73561010344798;
		m_weight[5] = 325.21402054845737;
		m_weight[6] = 19422.50835181832;
		m_weight[7] = 29.296278560410364;
		m_weight[8] = -15.93217447290214;
		m_weight[9] = 425.59264450471682;
		m_weight[10] = 12.718092099073136;
		m_weight[11] =11.12322020713191;
		m_weight[12] = -11.299518984832847;
	}
	~CCliqueGes(void){}
};
