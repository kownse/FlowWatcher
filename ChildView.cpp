// ChildView.cpp : CChildView 类的实现
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



// CChildView 消息处理程序

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

	//设置brush为希望的背景颜色
	CBrush backBrush(RGB(255, 128, 128));

	//保存旧的brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);

	//画需要的区域
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;

}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文


	
	// TODO: 在此处添加消息处理程序代码

	//////////////////////////////////////////////////////////////////////////
	//得到放缩比例
	CRect clientRect;
	::GetClientRect(m_hWnd, &clientRect);
	//获取屏幕工作区域矩形
	CRect screenRect;
	SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &screenRect,   0);
	double winDivH = (double)clientRect.right / screenRect.right;
	double winDivV = (double)clientRect.bottom / screenRect.bottom;

	if(m_eShowState == eShowState::CUR)
	{
		//////////////////////////////////////////////////////////////////////////
		//绘制实时桌面
		for(vector<pair<sHWNDAtt, CBitmap*>>::const_iterator it = m_vWinLocals.begin(); it != m_vWinLocals.end(); it++)
		{
			//得到目标矩形大小
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
		//只绘制最好的那一个
		MyIndiv elitist = m_cga.GetBestElitist();

		for(MyIndiv::const_iterator it = elitist.begin(); it != elitist.end(); it++)
		{
			//得到目标矩形大小
			//在这个地方这个大小用于指定截图纹理的大小
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
	// 不要为绘制消息而调用 CWnd::OnPaint()
}

//得到当前桌面窗口位置
void CChildView::OnGetCurWinLocals()
{
	// TODO: 在此添加命令处理程序代码
	//vector<sHWNDAtt> result;
	//theApp.GetScreenWinLocals(result);
}

void CChildView::OnPaintDesktop()
{
	//// TODO: 在此添加命令处理程序代码
	//m_vWinLocals.clear();
	//for(vector<pair<sHWNDAtt, CBitmap *>>::iterator it = m_vWinLocals.begin(); it != m_vWinLocals.end(); it++)
	//{
	//	DeleteObject(*(it->second));
	//	delete it->second;
	//	m_vWinLocals.erase(it);
	//}
	//m_vWinLocals.clear();
	CleanDeskInfo();

	//得到HWND和重要性
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

//为遗传算法做准备
void CChildView::OnPrePareGA()
{
	// TODO: 在此添加命令处理程序代码
	if(m_cga.TransformData(m_vWinLocals) == FALSE)
	{
		AfxMessageBox("数据转换失败");
		return;
	}
	else
	{
		m_bPrepared = FALSE;
		m_iGen = 0;
		//获取屏幕工作区域矩形
		CRect screenRect;
		SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &screenRect,   0);
		m_cga.SetClientRect(screenRect);
	}

	if(m_cga.PrePare() == FALSE)
	{
		AfxMessageBox("遗传算法准备失败");
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
	// TODO: 在此添加命令处理程序代码
	if(m_bPrepared == FALSE)
	{
		OnPrePareGA();
	}

	m_cga.OneTurn();
	m_iGen = m_iGen+1;
	m_eShowState = eShowState::RESULT;
	this->Invalidate(TRUE);
}

//以遗传算法结果设置桌面
void CChildView::OnSetDesktopWithResult()
{
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
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
