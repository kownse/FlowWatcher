#pragma once

//���ڴ�����Ļ�����ĸ�����
class CScreenHelper
{
public:
	CScreenHelper(void);
	~CScreenHelper(void);
	// ��HWND�õ����ڽ�ͼ���������ļ�
	static BOOL ScreenshotHWNDFile(HWND hWnd);
	// ��HWND�õ����ڽ�ͼ��������ָ����С��CBitmap
	static BOOL ScreenshotInBitmap(HWND hWnd, CBitmap *dst, const CRect &rect);
	//��HWND�õ����ڽ�ͼ���������ƶ���С��DC
	static BOOL ScreenshotInDC(HWND hWnd, CDC *dstDC, CRect &dstRect);
	// ��HBITMAPд���ļ�
	static BOOL WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC);
};
