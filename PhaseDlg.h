#pragma once
#include "resource.h"

#define  PHASE_DLG_POS_SMALL 973,283, 623,538
#define  PHASE_DLG_POS_BIG 350,283, 1135,1031
#define  ID_PHASE_IMAGE_WINDOW 0x9996

class CPhaseDlg : public CDialog
{
	DECLARE_DYNCREATE(CPhaseDlg)
protected:
	CWnd * m_pPhaseImgWnd;
	virtual void OnOK();

public:
	virtual void OnCancel();
	enum { IDD = IDD_PHASE_DLG };
	CPhaseDlg();
	virtual ~CPhaseDlg();
	void ResizeWindow();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//{{AFX_MSG
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedStartPhase();
	afx_msg void OnBnClickedPausePhase();
	afx_msg void OnBnClickedSavePhase();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


