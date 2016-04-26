#pragma once
#include "resource.h"
#include "afxcmn.h"
#define  INTENSITY_DLG_POS_SMALL 350,283, 623,538
#define  INTENSITY_DLG_POS_BIG 350,283, 1135,1031

#define ID_INTENSITY_IMAGE_WINDOW 0x9998

class CIntensityDlg : public CDialog
{
	DECLARE_DYNCREATE(CIntensityDlg)
public:

protected:
	CWnd * m_pIntensityImgWnd;
	int m_iIntensityGain;
	virtual void OnOK();
	

public:
	CSpinButtonCtrl m_FileSpin;
	virtual void OnCancel();
	enum { IDD = IDD_INTENSITY_DLG};
	CIntensityDlg();     
	virtual ~CIntensityDlg();
	virtual BOOL OnInitDialog();
	void ResizeWindow();
	void LoadRawImage();
	void LoadTiffImage();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG
	afx_msg void OnBnClickedStartIntensity();
	afx_msg void OnBnClickedPauseIntensity();
	afx_msg void OnBnClickedSaveIntensity();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckBox();
	afx_msg void OnBnClickedZoom();
	afx_msg void OnBnClickedLoadIntensity();
	afx_msg void OnBnClickedHotColor();
	afx_msg void OnDeltaposFileSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedReconIntensity();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedReplay();
	afx_msg void OnBnClickedSnap();
};


