#pragma once
#include "resource.h"

#define CURVE_DLG_POS 350,0, 1135,282
#define ID_CURVE_IMAGE 0x9999

class CCurveDlg : public CDialog
{
	DECLARE_DYNCREATE(CCurveDlg)

protected:
	CWnd * m_pCurveImgWnd;
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);	

public:
	virtual void OnCancel();
	enum { IDD = IDD_CURVE_DLG };
	CCurveDlg();           
	virtual ~CCurveDlg();
	virtual BOOL OnInitDialog();

	//{{AFX_MSG
	afx_msg void OnBnClickedStartCurve();
	afx_msg void OnBnClickedPauseCurve();
	afx_msg void OnBnClickedSaveCurve();
	afx_msg void OnBnClickedLoadCurve();
	afx_msg void OnBnClickedRawSpectrum();
	afx_msg void OnBnClickedAxialIntensity();
	afx_msg void OnBnClickedAxialPhase();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
	afx_msg void OnBnClickedCaliedSpectrum();
	afx_msg void OnBnClickedPower();
};


