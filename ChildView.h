// ChildView.h : CChildView 类的接口
//


#pragma once

#include "GA.h"

// CChildView 窗口

class CChildView : public CWnd
{
// 构造
public:
	CChildView();

// 属性
public:

// 操作
public:

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnEraseBkgnd(CDC* pDC);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetCurWinLocals();
	afx_msg void OnPaintDesktop();

private:
	BOOL CleanDeskInfo();																//清理m_vWinLocals;

private:
	vector<pair<sHWNDAtt,CBitmap *>> m_vWinLocals;				//保存桌面窗口信息和截图

private:
	enum eShowState {CUR, RESULT, NONE};
	eShowState m_eShowState;														//显示模式
	CGA m_cga;																				//遗传算法类对象
	BOOL m_bPrepared;																	//准备情况
	int m_iGen;																				//遗传代数
public:
	afx_msg void OnPrePareGA();
	afx_msg void OnOneTurn();
	afx_msg void OnSetDesktopWithResult();
	afx_msg void On100Gen();
};

