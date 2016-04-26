#include "stdafx.h"
#include "OCT.h"
#include "PhaseDlg.h"
#include "OCTDlg.h"
#include "Defines.h"
#include "ImageWnd.h"
#include "ExternVariables.h"
#include <iostream>
#include <fstream> 
#include <tiffio.h>
extern UINT DisplayPhaseThreadProc(LPVOID);

IMPLEMENT_DYNAMIC(CPhaseDlg, CDialog)

CPhaseDlg::CPhaseDlg()
{
	m_pPhaseImgWnd = NULL;
	bDisplayPhase = FALSE;
}

CPhaseDlg::~CPhaseDlg()
{
	//OnCancel();
	delete m_pPhaseImgWnd;
}


BEGIN_MESSAGE_MAP(CPhaseDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_START_PHASE, OnBnClickedStartPhase)
	ON_BN_CLICKED(IDC_PAUSE_PHASE, OnBnClickedPausePhase)
	ON_BN_CLICKED(IDC_SAVE_PHASE, OnBnClickedSavePhase)
END_MESSAGE_MAP()

void CPhaseDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_PHASE, phase);
	DDX_Slider(pDX, IDC_PHASE_THRESH,PhaseThresh);
	CDialog::DoDataExchange(pDX);
}

void CPhaseDlg::OnCancel() 
{
	COCTDlg::bPhaseDlgEn = FALSE;
	theApp.TerminateDisplayPhaseThread();
	delete m_pPhaseImgWnd;
	m_pPhaseImgWnd=NULL;
	DestroyWindow();
}


void CPhaseDlg::OnOK() 
{
	UpdateData();
}

BOOL CPhaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (bZoom)
		SetWindowPos(&wndTop,PHASE_DLG_POS_BIG,NULL);
	else
		SetWindowPos(&wndTop,PHASE_DLG_POS_SMALL,NULL);

	RECT rect;
	rect.left=100;
	rect.right=613+512*bZoom;
	rect.top = 2;
	rect.bottom=505+500*bZoom;
	m_pPhaseImgWnd = new CImageWnd;
	m_pPhaseImgWnd->Create(NULL,"Phase Image",WS_BORDER | WS_VISIBLE | CS_HREDRAW | CS_VREDRAW,
		rect,this,ID_PHASE_IMAGE_WINDOW,NULL) ;

	return TRUE;
}


void CPhaseDlg::OnBnClickedStartPhase()
{
	theApp.StartDisplayPhaseThread();
	GetDlgItem(IDC_START_PHASE)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_PHASE)->ShowWindow(FALSE);
	GetDlgItem(IDC_PAUSE_PHASE)->ShowWindow(TRUE);
	GetDlgItem(IDC_PAUSE_PHASE)->EnableWindow(TRUE);
}

void CPhaseDlg::OnBnClickedPausePhase()
{
	theApp.PauseDisplayPhaseThread();
	GetDlgItem(IDC_PAUSE_PHASE)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSE_PHASE)->ShowWindow(FALSE);
	GetDlgItem(IDC_START_PHASE)->ShowWindow(TRUE);
	GetDlgItem(IDC_START_PHASE)->EnableWindow(TRUE);
}

void CPhaseDlg::OnBnClickedSavePhase()
{
	//save the status of threads for resuming.
	BOOL bDisplayPhaseWasRunning=bDisplayPhase;
	OnBnClickedPausePhase();
	TIFF *image;

	char szFilter[] = " TIFF Image (*.TIF)|*.TIF||";
	CFileDialog fileDlg(FALSE, "TIF.RAW", NULL, OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , szFilter, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK)
	{
		//save Raw Data or reconstructed Image data Depend on the File extension name.	
		CString strPathName = fileDlg.GetPathName();
		image = TIFFOpen(strPathName, "w");
		TIFFSetField(image, TIFFTAG_IMAGEWIDTH, IMAGEWIDTH );
		TIFFSetField(image, TIFFTAG_IMAGELENGTH, IMAGEHEIGHT);
		TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, IMAGEHEIGHT);
		TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(image, TIFFTAG_COMPRESSION, 1);
		TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
		TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFWriteEncodedStrip(image, 0, PhaseImage, IMAGESIZE);
		TIFFClose(image);
	}	

	if (bDisplayPhaseWasRunning)	
		OnBnClickedStartPhase();

}

void CPhaseDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(TRUE);
	if (!bDisplayPhase)
		theApp.StartDisplayPhaseThread();
	SetEvent(PhaseBuffNew);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPhaseDlg::ResizeWindow()
{
	if (bZoom) {
		SetWindowPos(&wndTop,PHASE_DLG_POS_BIG,NULL);
		m_pPhaseImgWnd->SetWindowPos(&wndTop,IMAGE_WND_POS_BIG,NULL);
	}
	else {
		SetWindowPos(&wndTop,PHASE_DLG_POS_SMALL,NULL);
		m_pPhaseImgWnd->SetWindowPos(&wndTop,IMAGE_WND_POS_SMALL,NULL);
	}
}

