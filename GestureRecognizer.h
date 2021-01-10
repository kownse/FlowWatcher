#pragma once
#include "Gesture.h"
#include "Feature.h"

#define NUMOFGESTURE  4

class CGestureRecognizer
{
public:
	CGestureRecognizer(void);
	~CGestureRecognizer(void);

public:
	enum eGesType{LAST, NEXT, PLAIN, CLIQUE, NONE};
	CGestureRecognizer::eGesType Recogenize(const CFeature *);								//识别手势

private:
	CGesture* m_pGestures[NUMOFGESTURE];
	//CLastGesture					m_gLast;
	//CNextGesture					m_gNext;
	//CPlainLayoutGes				m_gPlain;
	//CCliqueGes						m_gClique;
};




