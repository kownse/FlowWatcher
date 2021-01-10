#include "StdAfx.h"
#include "GestureRecognizer.h"

CGestureRecognizer::CGestureRecognizer(void)
{
	m_pGestures[0] = new CLastGesture();
	m_pGestures[1] = new CNextGesture();
	m_pGestures[2] = new CPlainLayoutGes();
	m_pGestures[3] = new CCliqueGes();
}

CGestureRecognizer::~CGestureRecognizer(void)
{
	for(int i=0; i<NUMOFGESTURE; i++)
	{
		delete m_pGestures[i];
	}
}

//识别手势
CGestureRecognizer::eGesType CGestureRecognizer::Recogenize(const CFeature * pfeature)
{
	double max = m_pGestures[0]->Recognition(pfeature);
	//CString msg;
	//	msg.Format("最小化得分  %f", max);
	//	AfxMessageBox(msg);
	double cur;
	int num = 0;

	for(int i=1; i<NUMOFGESTURE; i++)
	{
		cur = m_pGestures[i]->Recognition(pfeature);
		if(max < cur)
		{
			max = cur;
			num = i;
		}
	}

	switch (num)
	{
	case  0:
		{
			return eGesType::LAST;
			break;
		}
	case 1:
		{
			return eGesType::NEXT;
			break;
		}
	case 2:
		{
			return eGesType::PLAIN;
			break;
		}
	case  3:
		{
			return eGesType::CLIQUE;
			break;
		}
	default:
		{
			return eGesType::NONE;
		}
	}
}
