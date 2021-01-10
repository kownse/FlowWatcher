// ChildView.h : CChildView ��Ľӿ�
//


#pragma once

#include "GA.h"

// CChildView ����

class CChildView : public CWnd
{
// ����
public:
	CChildView();

// ����
public:

// ����
public:

// ��д
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnEraseBkgnd(CDC* pDC);

// ʵ��
public:
	virtual ~CChildView();

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetCurWinLocals();
	afx_msg void OnPaintDesktop();

private:
	BOOL CleanDeskInfo();																//����m_vWinLocals;

private:
	vector<pair<sHWNDAtt,CBitmap *>> m_vWinLocals;				//�������洰����Ϣ�ͽ�ͼ

private:
	enum eShowState {CUR, RESULT, NONE};
	eShowState m_eShowState;														//��ʾģʽ
	CGA m_cga;																				//�Ŵ��㷨�����
	BOOL m_bPrepared;																	//׼�����
	int m_iGen;																				//�Ŵ�����
public:
	afx_msg void OnPrePareGA();
	afx_msg void OnOneTurn();
	afx_msg void OnSetDesktopWithResult();
	afx_msg void On100Gen();
};

