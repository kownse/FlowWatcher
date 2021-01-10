#include "StdAfx.h"
#include "AbstractDBManager.h"

CAbstractDBManager::CAbstractDBManager(void):m_iFailCount(0),m_bConnected(FALSE)
{
}

CAbstractDBManager::~CAbstractDBManager(void)
{
}
