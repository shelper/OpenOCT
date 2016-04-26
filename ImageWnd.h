#pragma once
#include "OCTDlg.h"
#define  IMAGE_WND_POS_SMALL 100, 2, 513,503
#define  IMAGE_WND_POS_BIG   100, 2, 1025,1002

// CImageWnd

class CImageWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageWnd)

protected:
	CPoint StartPoint, EndPoint;
	double tmpVMinX, tmpVMaxX,tmpVMinY, tmpVMaxY;

public:
	CImageWnd();
	virtual ~CImageWnd();
	//{{AFX_MSG
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


