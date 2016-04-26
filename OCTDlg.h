#pragma once
#include "resource.h"
#include "IntensityDlg.h"
#include "PhaseDlg.h"
#include "CurveDlg.h"
#include "afxwin.h"
//#include "mgmotorctrl1.h"


class COCTDlg : public CDialog
{
	//DECLARE_DYNCREATE(COCTDlg)
protected:
	CToolBar    m_wndToolBar;
	int m_iWidth,m_iHeight;
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	static BOOL bCurveDlgEn;
	static BOOL bPhaseDlgEn;
	static BOOL bIntensityDlgEn;
	int m_ALineNum, m_IterationNum;
	int m_SliderCamGain, m_SliderIntTime;
	CIntensityDlg * m_pIntensityDlg;
	CPhaseDlg * m_pPhaseDlg;
	CCurveDlg * m_pCurveDlg;
	//CMgmotorctrl1 m_TStage;
	COCTDlg(CWnd* pParent = NULL);	
	virtual ~COCTDlg();
	virtual void OnOK();
	virtual void OnCancel();

	void ReconfigureScan();
	void ValidateConf();
	enum { IDD = IDD_SDOCT_DIALOG };

	//{{AFX_MSG
	afx_msg void IntensityEn();
	afx_msg void CurveEn();
	afx_msg void PhaseEn();
	afx_msg void ResetWndPos();
	afx_msg void OnSaveConf();
	afx_msg void OnLoadConf();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedStartImaqFrames();
	afx_msg void OnBnClickedGetrefImaq();
	afx_msg void OnBnClickedCalibrationImaq();
	afx_msg void OnBnClickedPauseImaq();
	afx_msg void OnBnClickedStartImaq();
	//afx_msg void OnBnClickedEnableStage();
	afx_msg void OnBnClickedScanBackward();
	afx_msg void OnBnClickedResetImaq();
	afx_msg void OnBnClickedFilepathButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedReconstructImage();
	afx_msg void OnBnClickedImaqFramesStop();
	afx_msg void OnBnClickedScanStepx();
	afx_msg void OnBnClickedScanStepy();
};
