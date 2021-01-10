#include "StdAfx.h"
#include "Gesture.h"

CGesture::CGesture(void)
{
	//AfxMessageBox("CGesture���캯��");
}

CGesture::~CGesture(void)
{
}

//ʶ������
double CGesture::Recognition(const CFeature *input)
{
	double value = m_weight[0];

	for (int i = 1; i < FEATUREDISMENSION; i++)
	{
		value += m_weight[i] * input->m_featureVector[i - 1];
	}

	return value;
}
