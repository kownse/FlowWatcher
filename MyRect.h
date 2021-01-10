#ifndef MYRECT_H_H
#define MYRECT_H_H



class MyRect: public CRect
{
public:
	MyRect():m_dImportance(0.), m_bRelationFlag(false) {}
	MyRect(const CRect& rt):m_dImportance(0.), m_bRelationFlag(false) {
		(CRect)*this = (CRect)rt;
	}
	MyRect(unsigned l, unsigned t, unsigned r, unsigned b):m_dImportance(0.), m_bRelationFlag(false) {
		this->left = l;
		this->top = t;
		this->right = r;
		this->bottom = b;
	}
	MyRect& operator =(const MyRect& r) {
		(CRect)*this = (CRect)r;
		this->left = r.left;
		this->right = r.right;
		this->top = r.top;
		this->bottom = r.bottom;
		this->m_dImportance = r.m_dImportance;
		this->m_bRelationFlag = r.m_bRelationFlag;
		return *this;
	}
	bool operator !=(const MyRect& r) {
		if ((CRect)*this != (CRect)r || this->m_dImportance != r.m_dImportance ||
			this->m_bRelationFlag != r.m_bRelationFlag)
			return true;
		else return false;
	}
public:
	double m_dImportance;
	bool m_bRelationFlag;
	HWND hWnd;
	CBitmap *bitmap;
protected:
private:
};

#endif