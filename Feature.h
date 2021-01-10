#pragma once

#include <list>
#include <math.h>
#define FEATUREDISMENSION				12
#define INITCOS									0  
#define INITSIN									1  
#define DIAGOFLENGTH						2 
#define DIAGCOS									3  
#define DIAGSIN									4  
#define DISHTOT									5  
#define COSHTOT									6 
#define SINHTOT									7  
#define SUMOFLENGTH						8 
#define SUMOFANGLE							9 
#define SUMOFABSOFANGLE				10 
#define SUMOFSQUAREDOFANGLE		11

class CFeature
{
	friend class CGesture;
public:
	CFeature(void);
	~CFeature(void);
	CFeature(std::list<CPoint> *plist);

private:

	inline double verToVerDistance(const CPoint *start, const CPoint *end)//Á½µãµÄ¾àÀë
	{
		return   sqrtl((end->x - start->x) * (end->x - start->x) 
			+ (end->y - start->y) * (end->y - start->y));
	}

private:
	double m_featureVector[FEATUREDISMENSION];
};
