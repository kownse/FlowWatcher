// ChildView.cpp : CChildView ���ʵ��
//

#include "stdafx.h"
#include "FlowWatcher.h"
#include "ChildView.h"
#include "ScreenHelper.h"
#include "AnimatinoHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_eShowState = eShowState::NONE;
}

CChildView::~CChildView()
{
	CleanDeskInfo();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_32788, &CChildView::OnGetCurWinLocals)
	ON_COMMAND(ID_32789, &CChildView::OnPaintDesktop)
	ON_COMMAND(ID_32790, &CChildView::OnPrePareGA)
	ON_COMMAND(ID_32791, &CChildView::OnOneTurn)
	ON_COMMAND(ID_ONE_TURN, &CChildView::OnOneTurn)
	ON_COMMAND(ID_32794, &CChildView::OnSetDesktopWithResult)
	ON_COMMAND(ID_32795, &CChildView::On100Gen)
END_MESSAGE_MAP()



// CChildView ��Ϣ�������

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{

	//����brushΪϣ���ı�����ɫ
	CBrush backBrush(RGB(255, 128, 128));

	//����ɵ�brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);

	//����Ҫ������
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;

}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������


	
	// TODO: �ڴ˴������Ϣ����������

	//////////////////////////////////////////////////////////////////////////
	//�õ���������
	CRect clientRect;
	::GetClientRect(m_hWnd, &clientRect);
	//��ȡ��Ļ�����������
	CRect screenRect;
	SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &screenRect,   0);
	double winDivH = (double)clientRect.right / screenRect.right;
	double winDivV = (double)clientRect.bottom / screenRect.bottom;

	if(m_eShowState == eShowState::CUR)
	{
		//////////////////////////////////////////////////////////////////////////
		//����ʵʱ����
		for(vector<pair<sHWNDAtt, CBitmap*>>::const_iterator it = m_vWinLocals.begin(); it != m_vWinLocals.end(); it++)
		{
			//�õ�Ŀ����δ�С
			CRect curRect;
			::GetWindowRect(it->first.hwnd, &curRect);

			CRect tmpR;
			tmpR.left = curRect.left * winDivH;
			tmpR.right = curRect.right * winDivH;
			tmpR.top = curRect.top * winDivV;
			tmpR.bottom =curRect.bottom * winDivV;
			dc.Rectangle(tmpR);

			CDC memDC;
			memDC.CreateCompatibleDC(NULL);
			memDC.SelectObject(it->second);

			dc.StretchBlt(tmpR.left, tmpR.top, tmpR.Width(), tmpR.Height(), 
				&memDC, 0, 0,it->first.rect.Width(), 
				it->first.rect.Height(), SRCCOPY);

			::DeleteObject(memDC);
		}
	}
	else if(m_eShowState == eShowState::RESULT)
	{
		//ֻ������õ���һ��
		MyIndiv elitist = m_cga.GetBestElitist();

		for(MyIndiv::const_iterator it = elitist.begin(); it != elitist.end(); it++)
		{
			//�õ�Ŀ����δ�С
			//������ط������С����ָ����ͼ����Ĵ�С
			CRect curRect;
			::GetWindowRect(it->hWnd, &curRect);

			CRect tmpR;
			tmpR.left = it->left * winDivH;
			tmpR.right = it->right * winDivH;
			tmpR.top = it->top * winDivV;
			tmpR.bottom =it->bottom * winDivV;
			dc.Rectangle(tmpR);

			CDC memDC;
			memDC.CreateCompatibleDC(NULL);
			memDC.SelectObject(it->bitmap);

			dc.StretchBlt(tmpR.left, tmpR.top, tmpR.Width(), tmpR.Height(), 
				&memDC, 0, 0,curRect.Width(), 
				curRect.Height(), SRCCOPY);

			::DeleteObject(memDC);
			
		}


	}
	else if(m_eShowState == eShowState::NONE)
	{

	}
	

	::DeleteObject(dc);
	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
}

//�õ���ǰ���洰��λ��
void CChildView::OnGetCurWinLocals()
{
	// TODO: �ڴ���������������
	//vector<sHWNDAtt> result;
	//theApp.GetScreenWinLocals(result);
}

void CChildView::OnPaintDesktop()
{
	//// TODO: �ڴ���������������
	//m_vWinLocals.clear();
	//for(vector<pair<sHWNDAtt, CBitmap *>>::iterator it = m_vWinLocals.begin(); it != m_vWinLocals.end(); it++)
	//{
	//	DeleteObject(*(it->second));
	//	delete it->second;
	//	m_vWinLocals.erase(it);
	//}
	//m_vWinLocals.clear();
	CleanDeskInfo();

	//�õ�HWND����Ҫ��
	vector<sHWNDAtt> result;
	theApp.GetScreenWinLocals(result);

	for(vector<sHWNDAtt>::iterator it = result.begin(); it != result.end(); it++)
	{
		CBitmap *bitmap = new CBitmap;
		CScreenHelper::ScreenshotInBitmap(it->hwnd, bitmap, it->rect);

		m_vWinLocals.push_back(make_pair(*it, bitmap));
	}

	m_eShowState = eShowState::CUR;
	m_bPrepared = FALSE;

	Invalidate(TRUE);
}

BOOL  CChildView::CleanDeskInfo()
{
	for(vector<pair<sHWNDAtt, CBitmap *>>::iterator it = m_vWinLocals.begin(); it != m_vWinLocals.end();)
	{
		DeleteObject(*(it->second));
		delete it->second;
		it = m_vWinLocals.erase(it);
	}
	m_vWinLocals.clear();

	return TRUE;
}

//Ϊ�Ŵ��㷨��׼��
void CChildView::OnPrePareGA()
{
	// TODO: �ڴ���������������
	if(m_cga.TransformData(m_vWinLocals) == FALSE)
	{
		AfxMessageBox("����ת��ʧ��");
		return;
	}
	else
	{
		m_bPrepared = FALSE;
		m_iGen = 0;
		//��ȡ��Ļ�����������
		CRect screenRect;
		SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &screenRect,   0);
		m_cga.SetClientRect(screenRect);
	}

	if(m_cga.PrePare() == FALSE)
	{
		AfxMessageBox("�Ŵ��㷨׼��ʧ��");
		return;
	}
	else
	{
		m_bPrepared = TRUE;
		m_eShowState = eShowState::RESULT;
	}

}

void CChildView::OnOneTurn()
{
	// TODO: �ڴ���������������
	if(m_bPrepared == FALSE)
	{
		OnPrePareGA();
	}

	m_cga.OneTurn();
	m_iGen = m_iGen+1;
	m_eShowState = eShowState::RESULT;
	this->Invalidate(TRUE);
}

//���Ŵ��㷨�����������
void CChildView::OnSetDesktopWithResult()
{
	// TODO: �ڴ���������������
	MyIndiv result = m_cga.GetBestElitist();

	AnimatinoHelper::Begin();
	for(MyIndiv::const_iterator it = result.begin(); it != result.end(); it++)
	{
		//::SetWindowPos(it->hWnd, HWND_BOTTOM, it->left, it->top, it->Width(), it->Height(), SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
		CRect srcRect;
		::GetWindowRect(it->hWnd, &srcRect);
		CRect dstRect(it->left, it->top, it->right, it->bottom);
		AnimatinoHelper::AddMove(it->hWnd, srcRect, dstRect);
	}
	AnimatinoHelper::MoveAnimation();
	AnimatinoHelper::End();

	this->Invalidate(TRUE);
}

void CChildView::On100Gen()
{
	// TODO: �ڴ���������������
	// TODO: �ڴ���������������
	if(m_bPrepared == FALSE)
	{
		OnPrePareGA();
	}
	while (m_iGen++ < 500)
	{
		m_cga.OneTurn();
		m_iGen = m_iGen+1;
	}
	m_eShowState = eShowState::RESULT;
	this->Invalidate(TRUE);
	
}
