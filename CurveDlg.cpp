#include "stdafx.h"
#include <iostream>
#include <fstream> 
#include "OCT.h"
#include "CurveDlg.h"
#include "OCTDlg.h"
#include "defines.h"
#include "ExternVariables.h"

using namespace std;

IMPLEMENT_DYNAMIC(CCurveDlg, CDialog)

//declaration of the thread for this dialog
extern UINT DrawCurveThreadProc(LPVOID hWnd);

CCurveDlg::CCurveDlg()
{
	m_pCurveImgWnd = NULL;
	bDrawCurve = FALSE;
}

CCurveDlg::~CCurveDlg()
{
	//OnCancel();
	delete m_pCurveImgWnd;
}


BEGIN_MESSAGE_MAP(CCurveDlg, CDialog)
	ON_BN_CLICKED(IDC_START_CURVE, OnBnClickedStartCurve)
	ON_BN_CLICKED(IDC_PAUSE_CURVE, OnBnClickedPauseCurve)
	ON_BN_CLICKED(IDC_SAVE_CURVE, OnBnClickedSaveCurve)
	ON_BN_CLICKED(IDC_LOAD_CURVE, OnBnClickedLoadCurve)
	ON_BN_CLICKED(IDC_AXIAL_INTENSITY, OnBnClickedAxialIntensity)
	ON_BN_CLICKED(IDC_AXIAL_PHASE, OnBnClickedAxialPhase)
	ON_BN_CLICKED(IDC_RAW_SPECTRUM, OnBnClickedRawSpectrum)
	ON_BN_CLICKED(IDC_CALIED_SPECTRUM, OnBnClickedCaliedSpectrum)
	ON_BN_CLICKED(IDC_POWER, OnBnClickedPower)
END_MESSAGE_MAP()

void CCurveDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Check(pDX, IDC_AXIAL_PHASE, bAxialPhase);
	DDX_Check(pDX, IDC_AXIAL_INTENSITY, bAxialIntensity);
	DDX_Check(pDX, IDC_RAW_SPECTRUM, bRawSpectrum);
	DDX_Check(pDX, IDC_CALIED_SPECTRUM, bCaliedSpectrum);
	DDX_Check(pDX, IDC_POWER, bPower);
	CDialog::DoDataExchange(pDX);
}

BOOL CCurveDlg::OnInitDialog()
{	
	//initialize the data
	bRawSpectrum = TRUE;
	bAxialIntensity = FALSE;
	bAxialPhase = FALSE;
	bCaliedSpectrum=FALSE;
	bPower=FALSE;
	CDialog::OnInitDialog();

	//set the poistion and size of the dialog
	SetWindowPos(&wndTop,CURVE_DLG_POS,NULL);
	RECT rect;
	GetClientRect(&rect);
	rect.left=100;
	rect.right=1125;
	m_pCurveImgWnd = new CWnd;
	m_pCurveImgWnd->Create(NULL,"Curve Image", WS_BORDER | WS_VISIBLE | CS_HREDRAW | CS_VREDRAW,rect,
					this,ID_CURVE_IMAGE,NULL) ;

	return TRUE;
}

void CCurveDlg::OnCancel() 
{	
	COCTDlg::bCurveDlgEn = FALSE;
	theApp.TerminateDrawCurveThread();
	delete m_pCurveImgWnd;
	m_pCurveImgWnd=NULL;
	DestroyWindow();
}


void CCurveDlg::OnOK() 
{
	UpdateData(TRUE);
}

void CCurveDlg::OnBnClickedRawSpectrum()
{
	UpdateData(TRUE);
}

void CCurveDlg::OnBnClickedAxialIntensity()
{
	UpdateData(TRUE);
}

void CCurveDlg::OnBnClickedAxialPhase()
{
	UpdateData(TRUE);
}

void CCurveDlg::OnBnClickedCaliedSpectrum()
{
	UpdateData(TRUE);
}

void CCurveDlg::OnBnClickedStartCurve()
{
	theApp.StartDrawCurveThread();	
	GetDlgItem(IDC_START_CURVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_CURVE)->ShowWindow(FALSE);
	GetDlgItem(IDC_PAUSE_CURVE)->ShowWindow(TRUE);
	GetDlgItem(IDC_PAUSE_CURVE)->EnableWindow(TRUE);
}

void CCurveDlg::OnBnClickedPauseCurve()
{
	theApp.PauseDrawCurveThread();
	GetDlgItem(IDC_PAUSE_CURVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSE_CURVE)->ShowWindow(FALSE);
	GetDlgItem(IDC_START_CURVE)->ShowWindow(TRUE);
	GetDlgItem(IDC_START_CURVE)->EnableWindow(TRUE);
}




void CCurveDlg::OnBnClickedSaveCurve()
{
	//resume the DrawCurve thread if it was running before.
	BOOL bDrawCurveWasRunning=bDrawCurve;
	OnBnClickedPauseCurve();

	char szFilter[] = " Raw Spectrum Curves (*.RSC)|*.RSC||";
	CFileDialog fileDlg(FALSE, "RSC", "", OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , szFilter, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK)
	{
		CString strPathName = fileDlg.GetPathName();
		ofstream fout(strPathName);		

		for (int i = 0; i< SPECTRUMWIDTH; i++){
			fout<< (*(RawSpectrumCurveData+i) - *( AveragedReference+i))<<" ";

		}
		fout.close();
	}

	if (bDrawCurveWasRunning)
		OnBnClickedStartCurve();
}

void CCurveDlg::OnBnClickedLoadCurve()
{
	pMainDlg->OnBnClickedPauseImaq();
	OnBnClickedPauseCurve();
	char szFilter[] = " Raw Spectrum Curves (*.RSC)|*.RSC||";
	CFileDialog fileDlg(TRUE, "RSC", "", OFN_EXPLORER | OFN_HIDEREADONLY , szFilter, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK)
	{
		CString strPathName = fileDlg.GetPathName();
		ifstream fin(strPathName);		

		for (int i = 0; i< SPECTRUMWIDTH; i++){
			fin>> *(RawSpectrumCurveData+i);

		}
		fin.close();//strPathName
	}
	OnBnClickedStartCurve();
}



void CCurveDlg::OnBnClickedPower()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
}
