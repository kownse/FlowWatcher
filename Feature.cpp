#include "StdAfx.h"
#include "Feature.h"

CFeature::CFeature(void)
{
}

CFeature::~CFeature(void)
{
}

CFeature::CFeature(std::list<CPoint> *plist)
{
	CPoint preVertex = NULL;
	CPoint curVertex = NULL;
	CPoint nextVertex = NULL;
	CPoint headVertex = *plist->begin();
	CPoint tailVertex = *plist->rbegin();
//	POSITION curPosition = plist->GetHeadPosition();
	std::list<CPoint>::iterator it = plist->begin();
	double xmin, xmax, ymin, ymax;
	double xdisfirst, xdissecond, ydisfirst, ydissecond, angle;
	double area;

	m_featureVector[DISHTOT] = verToVerDistance(&headVertex, &tailVertex);  //始末两点的距离
	m_featureVector[COSHTOT] = (tailVertex.x - headVertex.x) / m_featureVector[DISHTOT]; 
	m_featureVector[SINHTOT] = (tailVertex.y - headVertex.x) / m_featureVector[DISHTOT];
	//preVertex = plist->GetNext(curPosition);
	preVertex = *it++;
	xmin = preVertex.x; //求外包围盒对角线
	xmax = preVertex.x;
	ymin = preVertex.y;
	ymax = preVertex.y;
	//curVertex = plist->GetNext(curPosition);
	curVertex = *it++;
	if(curVertex.x > xmax)
	{
		xmax = curVertex.x;
	}
	if (curVertex.x < xmin)
	{
		xmin = curVertex.x;
	}
	if (curVertex.y > ymax)
	{
		ymax = curVertex.y;
	}
	if (curVertex.y < ymin)
	{
		ymin = curVertex.y;
	}
	m_featureVector[SUMOFLENGTH] = verToVerDistance(&preVertex, &curVertex);
	xdisfirst = curVertex.x - preVertex.x;
	ydisfirst = curVertex.y - preVertex.y;

	//nextVertex = plist->GetNext(curPosition);
	nextVertex = *it++;
	if (nextVertex.x > xmax)
	{
		xmax = nextVertex.x;
	}
	if (nextVertex.x < xmin)
	{
		xmin = nextVertex.x;
	}
	if (nextVertex.y > ymax)
	{
		ymax = nextVertex.y;
	}
	if (nextVertex.y < ymin)
	{
		ymin = nextVertex.y;
	}
	xdissecond = nextVertex.x - curVertex.x;
	ydissecond = nextVertex.y - curVertex.y;
	m_featureVector[SUMOFLENGTH] += verToVerDistance(&curVertex, &nextVertex);
	angle = atan((xdissecond * ydisfirst - xdisfirst * ydissecond) / (xdissecond * xdisfirst + ydissecond * ydisfirst));
	m_featureVector[SUMOFANGLE] = angle;
	m_featureVector[SUMOFABSOFANGLE] = abs(angle);
	m_featureVector[SUMOFSQUAREDOFANGLE] = angle * angle;
	m_featureVector[INITCOS] = (nextVertex.x - preVertex.x) / verToVerDistance(&preVertex, &nextVertex);
	m_featureVector[INITSIN] = (nextVertex.y - preVertex.y) / verToVerDistance(&preVertex, &nextVertex);

	preVertex = curVertex;
	curVertex = nextVertex;
	//while(curPosition != NULL)
	while(it != plist->end())
	{
		//nextVertex = plist->GetNext(curPosition);
		nextVertex = *it++;
		if (nextVertex.x > xmax)
		{
			xmax = nextVertex.x;
		}
		if (nextVertex.x < xmin)
		{
			xmin = nextVertex.x;
		}
		if(nextVertex.y > ymax)
		{
			ymax = nextVertex.y;
		}
		if (nextVertex.y < ymin)
		{
			ymin = nextVertex.y;
		}

		xdisfirst = xdissecond;
		ydisfirst = ydissecond;
		xdissecond = nextVertex.x - curVertex.x;
		ydissecond = nextVertex.y - curVertex.y;
		m_featureVector[SUMOFLENGTH] += verToVerDistance(&curVertex, &nextVertex);
		angle = atan((xdissecond * ydisfirst - xdisfirst * ydissecond) / (xdissecond * xdisfirst + ydissecond * ydisfirst));
		m_featureVector[SUMOFANGLE] += angle;
		m_featureVector[SUMOFABSOFANGLE] += abs(angle);
		m_featureVector[SUMOFSQUAREDOFANGLE] += angle * angle;
	}

	m_featureVector[DIAGOFLENGTH] = sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2));
	m_featureVector[DIAGCOS] = (xmax - xmin) / m_featureVector[DIAGOFLENGTH];
	m_featureVector[DIAGSIN] = (ymax - ymin) / m_featureVector[DIAGOFLENGTH];
	/*featureVector[DURATION] = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;*/
	area = (xmax - xmin) * (ymax - ymin);
	m_featureVector[DIAGOFLENGTH] /= area;
	m_featureVector[SUMOFLENGTH] /= area;
	m_featureVector[DISHTOT] /= area;

}
