#pragma once
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "afxmt.h"

class COCTApp : public CWinApp
{
public:
	COCTApp();	
	virtual ~COCTApp();
	void StartSDOCTThreads();
	void PauseSDOCTThreads();
	void GenCaliCoeff(BOOL bAutoCali);
	void InitiateArrays();
	void FreeArrays();
	void CallocArrays();
	void StartImaqThread();
	void StartCaliThread();
	void StartFFTThread();	
	void StartDisplayIntensityThread();
	void StartDisplayPhaseThread();
	void StartDrawCurveThread();
	void PauseImaqThread();
	void PauseCaliThread();
	void PauseFFTThread();
	void PauseDisplayIntensityThread();
	void PauseDisplayPhaseThread();
	void PauseDrawCurveThread();
	void TerminateImaqThread();
	void TerminateCaliThread();
	void TerminateFFTThread();					
	void TerminateSDOCTThreads();
	void TerminateDisplayIntensityThread();
	void TerminateDisplayPhaseThread();
	void TerminateDrawCurveThread();
	virtual BOOL InitInstance();

	//{{AFX_MSG
	afx_msg void OnAppAbout();
	afx_msg void OnErrorDaq();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	HANDLE m_hImaqThread;
	HANDLE m_hFFTThread;
	HANDLE m_hCaliThread;	
	HANDLE m_hDrawCurveThread;
	HANDLE m_hDisplayIntensityThread;
	HANDLE m_hDisplayPhaseThread;
};

extern COCTApp theApp;
