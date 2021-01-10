#include "StdAfx.h"
#include "ScreenHelper.h"

CScreenHelper::CScreenHelper(void)
{
}

CScreenHelper::~CScreenHelper(void)
{
}

BOOL CScreenHelper::ScreenshotInDC(HWND hWnd, CDC *dstDC, CRect &dstRect)
{
	if(hWnd == NULL)
	{
		AfxMessageBox("没有焦点窗口");
		return FALSE;
	}

	if(!(::IsWindow(hWnd)))
	{
		AfxMessageBox("焦点窗口无效");
		return FALSE;
	}

	//获取目标窗口的CDC
	CDC dc;
	HDC hdc = ::GetWindowDC(hWnd);
	dc.Attach(hdc);
	//建立内存CDC
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	//得到目标窗口的大小
	CRect rect(CPoint(0,0), CPoint(0,0));
	CBitmap bitmap;
	::GetWindowRect(hWnd, &rect);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//最终是绘制在内存里的CDC
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	//dstDC->SelectObject(&bitmap);
	//::PrintWindow(hWnd, dstDC->m_hDC, 0);
	::PrintWindow(hWnd, memDC.m_hDC, 0);
	//pOldBitmap = memDC.SelectObject(pOldBitmap);
	//dstDC->SelectObject(&bitmap);

	dstDC->StretchBlt(dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), 
		&memDC, 0, 0, rect.Width(), 
		rect.Height(), SRCCOPY);

	::DeleteObject(bitmap);
	::DeleteObject(hdc);
	::DeleteObject(dc);
	::DeleteObject(memDC);

	return TRUE;
}

// 从HWND得到窗口截图并保存在指定大小的CBitmap
BOOL CScreenHelper::ScreenshotInBitmap(HWND hWnd, CBitmap *dst, const CRect &rect)
{
	if(hWnd == NULL)
	{
		AfxMessageBox("没有焦点窗口");
		return FALSE;
	}

	if(!(::IsWindow(hWnd)))
	{
		AfxMessageBox("焦点窗口无效");
		return FALSE;
	}

	//获取目标窗口的CDC
	CDC dc;
	HDC hdc = ::GetWindowDC(hWnd);
	dc.Attach(hdc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	//得到目标窗口的大小
	//CRect rect(CPoint(0,0), CPoint(0,0));
	//CBitmap bitmap;
	//::GetWindowRect(hWnd, &rect);
	dst->CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//最终是绘制在内存里的CDC
	memDC.SelectObject(dst);
	::PrintWindow(hWnd, memDC.m_hDC, 0);
	

	//::DeleteObject(src);
	::DeleteObject(hdc);
	::DeleteObject(dc);
	::DeleteObject(memDC);
	//::DeleteObject(memDC);
	//::DeleteObject(dstDC);

	return TRUE;

}

// 从HWND得到窗口截图并保存在文件
BOOL CScreenHelper::ScreenshotHWNDFile(HWND hWnd)
{
	if(hWnd == NULL)
	{
		AfxMessageBox("没有焦点窗口");
		return FALSE;
	}

	if(!(::IsWindow(hWnd)))
	{
		AfxMessageBox("焦点窗口无效");
		return FALSE;
	}

	//得到标题
	TCHAR title[TITLE_SIZE];
	::SendMessage(hWnd, WM_GETTEXT, TITLE_SIZE, (LPARAM)title);
	
	//获取目标窗口的CDC
	CDC dc;
	HDC hdc = ::GetWindowDC(hWnd);
	dc.Attach(hdc);
	//建立内存CDC
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	//得到目标窗口的大小
	CRect rect(CPoint(0,0), CPoint(0,0));
	CBitmap bitmap;
	::GetWindowRect(hWnd, &rect);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//最终是绘制在内存里的CDC
	memDC.SelectObject(&bitmap);
	::PrintWindow(hWnd, memDC.m_hDC, 0);
	//memDC.BitBlt(0, 0, rect.Width(), rect.Height(), dc, SRCCOPY);
	//memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

	CString fileName;
	CTime date = CTime::GetCurrentTime();
	CString dateMsg = date.Format("%H_%M_%S");
	fileName.Format("%s_%s.bmp", dateMsg, title);

	WriteBMPFile(bitmap, fileName.GetBuffer(), hdc);

	::DeleteObject(bitmap);
	::DeleteObject(hdc);
	::DeleteObject(dc);
	::DeleteObject(memDC);
	//::DeleteDC(hBufDC);
	//::ReleaseDC(hWnd, hDC);
	//::DeleteObject(hBitmap);
	//SelectObject(hDCMem, hOld);
	//::DeleteObject(hDCMem);
	return TRUE;
}

// 将HBITMAP写入文件
BOOL CScreenHelper::WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC)
{
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD cClrBits; 
	HANDLE hf; // file handle 
	BITMAPFILEHEADER hdr; // bitmap file-header 
	PBITMAPINFOHEADER pbih; // bitmap info-header 
	LPBYTE lpBits; // memory pointer 
	DWORD dwTotal; // total count of bytes 
	DWORD cb; // incremental count of bytes 
	BYTE *hp; // byte pointer 
	DWORD dwTmp; 

	// create the bitmapinfo header information

	if (!::GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp)){
		AfxMessageBox("Could not retrieve bitmap info");
		return FALSE;
	}

	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 
	// Allocate memory for the BITMAPINFO structure.
	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits)); 
	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 

	// Initialize the fields in the BITMAPINFO structure. 

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 

	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits; 
	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 

	// now open file and save the data
	pbih = (PBITMAPINFOHEADER) pbmi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) {
		AfxMessageBox("writeBMP::Could not allocate memory");
		return FALSE;
	}

	//HDC   hDC   =   GetDC(NULL);   
	//if   (hDC   ==   NULL)   return   FALSE;   

	// Retrieve the color table (RGBQUAD array) and the bits 
	if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi, 
		DIB_RGB_COLORS)) {
			AfxMessageBox("writeBMP::GetDIB error");
			return FALSE;
	}

	//ReleaseDC(NULL, hDC);

	// Create the .BMP file. 
	hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
	if (hf == INVALID_HANDLE_VALUE){
		AfxMessageBox("Could not create file for writing");
		return FALSE;
	}
	hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 

	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD); 

	// Copy the BITMAPFILEHEADER into the .BMP file. 
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
		(LPDWORD) &dwTmp, NULL)) {
			AfxMessageBox("Could not write in to file");
			return FALSE;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL))){
			AfxMessageBox("Could not write in to file");
			return FALSE;
	}


	// Copy the array of color indices into the .BMP file. 
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)){
		AfxMessageBox("Could not write in to file");
		return FALSE;
	}

	// Close the .BMP file. 
	if (!CloseHandle(hf)){
		AfxMessageBox("Could not close file");
		return FALSE;
	}

	// Free memory. 
	GlobalFree((HGLOBAL)lpBits);

	return TRUE;
}
