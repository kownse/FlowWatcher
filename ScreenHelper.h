#pragma once

//用于处理屏幕操作的辅助类
class CScreenHelper
{
public:
	CScreenHelper(void);
	~CScreenHelper(void);
	// 从HWND得到窗口截图并保存在文件
	static BOOL ScreenshotHWNDFile(HWND hWnd);
	// 从HWND得到窗口截图并保存在指定大小的CBitmap
	static BOOL ScreenshotInBitmap(HWND hWnd, CBitmap *dst, const CRect &rect);
	//从HWND得到窗口截图并放缩在制定大小的DC
	static BOOL ScreenshotInDC(HWND hWnd, CDC *dstDC, CRect &dstRect);
	// 将HBITMAP写入文件
	static BOOL WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC);
};
