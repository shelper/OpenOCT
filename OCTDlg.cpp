#include "stdafx.h"
#include "OCT.h"
#include "OCTDlg.h"
#include "CurveDlg.h"
#include "PhaseDlg.h"
#include "IntensityDlg.h"
#include <tiffio.h>
#include "ImgAcq.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

#include "Defines.h"
#include "ExternVariables.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL COCTDlg::bCurveDlgEn=FALSE;
BOOL COCTDlg::bIntensityDlgEn=FALSE;
BOOL COCTDlg::bPhaseDlgEn=FALSE;

COCTDlg::COCTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COCTDlg::IDD, pParent)
{
	m_pPhaseDlg=NULL;
	m_pCurveDlg=NULL;
	m_pIntensityDlg=NULL;
	m_ALineNum=IMAGEHEIGHT;
	m_IterationNum=1;
}

COCTDlg::~COCTDlg()
{	
	delete m_pCurveDlg;
	delete m_pIntensityDlg;	
	delete m_pPhaseDlg;
}
void COCTDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Slider(pDX, IDC_CAM_GAIN_SLIDER, m_SliderCamGain);
	DDX_Text(pDX, IDC_CAM_GAIN, CamGain);
	DDX_Slider(pDX, IDC_CAM_IT_SLIDER, m_SliderIntTime);
	DDX_Text(pDX, IDC_CAM_IT, IntTime);
	DDX_Text(pDX, IDC_SCAN_VMINX, ScanVMinX);
	DDX_Text(pDX, IDC_SCAN_VMAXX, ScanVMaxX);
	DDX_Text(pDX, IDC_SCAN_VMINY, ScanVMinY);
	DDX_Text(pDX, IDC_SCAN_VMAXY, ScanVMaxY);
	DDX_Text(pDX, IDC_A_LINE_NUM, m_ALineNum);
	DDX_Text(pDX, IDC_ITERATION_NUM, m_IterationNum);
	DDX_Text(pDX, IDC_SCAN_FREQ, ScanFreq);
	DDX_Text(pDX, IDC_FRAME_SHIFT, FrameThickness);
	DDX_Text(pDX, IDC_FRAME_NUM, FrameNum);
	DDX_Text(pDX, IDC_AVG_NUM, AvgNum);
	DDX_Text(pDX, IDC_FILE_PATH, DefaultFilePath);
	//DDX_Text(pDX, IDC_MOTOR_STEPSIZE, MotorStepsize);
	DDX_Text(pDX, IDC_DISPERSION, Dispersion);

	DDX_Check(pDX, IDC_SCAN_STEPX, bStepX);
	DDX_Check(pDX, IDC_SCAN_STEPY, bStepY);
	//DDX_Check(pDX, IDC_ENABLE_STAGE, bEnableStage);
	DDX_Check(pDX, IDC_SCAN_BACKWARD, bScanBackward);
	DDX_Check(pDX, IDC_RECONSTRUCT_IMAGE, bReconstructImage);

	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_MGMOTORCTRL1, m_TStage);
}


BEGIN_MESSAGE_MAP(COCTDlg, CDialog)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_CURVE, CurveEn)
	ON_COMMAND(ID_INTENSITY, IntensityEn)
	ON_COMMAND(ID_PHASE, PhaseEn)
	ON_COMMAND(ID_RESET_WND_POS,ResetWndPos)
	ON_COMMAND(ID_SAVE_CONF, OnSaveConf)
	ON_COMMAND(ID_LOAD_CONF, OnLoadConf)
	ON_BN_CLICKED(IDC_IMAQ_FRAMES_START, OnBnClickedStartImaqFrames)
	ON_BN_CLICKED(IDC_GETREF_IMAQ, OnBnClickedGetrefImaq)
	ON_BN_CLICKED(IDC_CALIBRATION_IMAQ, OnBnClickedCalibrationImaq)
	ON_BN_CLICKED(IDC_PAUSE_IMAQ, OnBnClickedPauseImaq)
	ON_BN_CLICKED(IDC_START_IMAQ, OnBnClickedStartImaq)
//	ON_BN_CLICKED(IDC_ENABLE_STAGE, OnBnClickedEnableStage)
	ON_BN_CLICKED(IDC_SCAN_BACKWARD, OnBnClickedScanBackward)
	ON_BN_CLICKED(IDC_RESET_IMAQ, OnBnClickedResetImaq)
	ON_BN_CLICKED(IDC_FILEPATH_BUTTON, OnBnClickedFilepathButton)
	ON_BN_CLICKED(IDC_RECONSTRUCT_IMAGE, OnBnClickedReconstructImage)
	ON_BN_CLICKED(IDC_IMAQ_FRAMES_STOP, OnBnClickedImaqFramesStop)
	ON_BN_CLICKED(IDC_SCAN_STEPX, OnBnClickedScanStepx)
	ON_BN_CLICKED(IDC_SCAN_STEPY, OnBnClickedScanStepy)
END_MESSAGE_MAP()


BOOL COCTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//create shortcut toolbar for the dialog
	//step1.load toolbar to m_wndToolBar
	if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_TOOLBAR))	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//step2. get the client rect of the dialog
	CRect rcClientStart;
	CRect rcClientNow;
	GetClientRect(rcClientStart);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);

	CPoint ptOffset(rcClientNow.left - rcClientStart.left,
		rcClientNow.top - rcClientStart.top);

	CRect  rcChild;
	CWnd* pwndChild = GetWindow(GW_CHILD);
	while (pwndChild)	{
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild, FALSE);
		pwndChild = pwndChild->GetNextWindow();
	}

	//step3.caculate the space for placing the toolbar
	CRect rcWindow;
	GetWindowRect(rcWindow);
	rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
	m_iWidth=rcWindow.Width();
	rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();
	m_iHeight=rcWindow.Height();

	//reset the size of the window
	::MoveWindow(this->m_hWnd,0,0,m_iWidth,m_iHeight,FALSE);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	
	CString strPathName=".\\DOCT.cfg";
	ifstream fin(strPathName);		
	fin>>CamGain>>IntTime>>ScanFreq>>m_ALineNum>>m_IterationNum
		>>ScanVMinX>>ScanVMaxX>>ScanVMinY>>ScanVMaxY>>bStepX>>bStepY
		>>FrameNum>>FrameThickness>>Dispersion>>AvgNum>>bScanBackward
		>>bReconstructImage;
	fin.close();
	RawDataSize=ALineNum* IterationNum * SPECTRUMWIDTH ;
	FrameSize=ALineNum*IterationNum*FFTNUM/2;

	m_SliderCamGain = CamGain/CAMGAINFACTOR;
	m_SliderIntTime = IntTime/INTTIMEFACTOR;
	UpdateData(FALSE);

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	return TRUE; 
}


void COCTDlg::OnOK()
{
	//if Imaq is running, reconfigure the system immediately
	if (bImaq)  OnBnClickedStartImaq();
	else	{
		UpdateData(TRUE);
		ValidateConf();
		UpdateData(FALSE);
	}

}
void COCTDlg::OnCancel()
{
	theApp.TerminateDisplayPhaseThread();
	theApp.TerminateDisplayIntensityThread();
	theApp.TerminateSDOCTThreads();
	theApp.TerminateDrawCurveThread();
	theApp.FreeArrays();
	CDialog::OnCancel();
}

void COCTDlg::IntensityEn()
{
	//enable Intensity Dialog
	if(!bIntensityDlgEn)	{
		if (m_pIntensityDlg == NULL)
			m_pIntensityDlg = new CIntensityDlg;

		if(!(m_pIntensityDlg->Create(IDD_INTENSITY_DLG,this)))   
			AfxMessageBox("Error creating Dialog");

		m_pIntensityDlg->ShowWindow(SW_SHOW);
		bIntensityDlgEn=TRUE;
	}

	else 	{	
		//put Intensity window into top
		m_pIntensityDlg->SetForegroundWindow();

		if (bZoom)
			::SetWindowPos(m_pIntensityDlg->GetSafeHwnd(),HWND_TOP,INTENSITY_DLG_POS_BIG,NULL);
		else 
			::SetWindowPos(m_pIntensityDlg->GetSafeHwnd(),HWND_TOP,INTENSITY_DLG_POS_SMALL,NULL);
	}
}

//these are similar to the IntensityEn() function.
void COCTDlg::PhaseEn()
{
	if(!bPhaseDlgEn)	{
		if(m_pPhaseDlg == NULL)
			m_pPhaseDlg = new CPhaseDlg;

		if(!(m_pPhaseDlg->Create(IDD_PHASE_DLG,this)))   
			AfxMessageBox("Error creating Dialog");

		m_pPhaseDlg->ShowWindow(SW_SHOW);
		bPhaseDlgEn=TRUE;
	}
	else 	{
		m_pPhaseDlg->SetForegroundWindow();
		if (bZoom)
			::SetWindowPos(m_pPhaseDlg->GetSafeHwnd(),HWND_TOP,PHASE_DLG_POS_BIG,NULL);
		else 
			::SetWindowPos(m_pPhaseDlg->GetSafeHwnd(),HWND_TOP,PHASE_DLG_POS_SMALL,NULL);
	}


}

void COCTDlg::CurveEn()
{
	if (! bCurveDlgEn)		
	{
		if(m_pCurveDlg == NULL)
			m_pCurveDlg = new CCurveDlg;

		if(!(m_pCurveDlg->Create(IDD_CURVE_DLG,this)))   
			AfxMessageBox("Error creating Dialog");
			
		m_pCurveDlg->ShowWindow(SW_SHOW);
		 bCurveDlgEn=TRUE;
	}

	else 
	{
		m_pCurveDlg->SetForegroundWindow();
		::SetWindowPos(m_pCurveDlg->GetSafeHwnd(),HWND_TOP,	CURVE_DLG_POS,NULL);
	}
}

void COCTDlg::ResetWndPos()
{
	CRect rcWindow;
	GetWindowRect(rcWindow);
	if (rcWindow.left!=0 || rcWindow.top!=0)
		::SetWindowPos(this->GetSafeHwnd(),HWND_TOP,0, 0,m_iWidth, m_iHeight,0);

	CurveEn();
	IntensityEn();
	PhaseEn();
}

//Save configurations for current acquisition.
void COCTDlg::OnSaveConf()
{
	char szFilter[] = " Config file (*.cfg)|*.cfg||";
	CFileDialog fileDlg(FALSE, "cfg", "", OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , szFilter, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK)
	{
		CString strPathName = fileDlg.GetPathName();
		ofstream fout(strPathName);	
		fout<<CamGain<<'\n'<<IntTime<<'\n'<<ScanFreq<<'\n'<<m_ALineNum<<'\n'<<m_IterationNum
			<<'\n'<<ScanVMinX<<'\n'<<ScanVMaxX<<'\n'<<ScanVMinY<<'\n'<<ScanVMaxY<<'\n'<<bStepX<<'\n'<<bStepY
			<<'\n'<<FrameNum<<'\n'<<FrameThickness<<'\n'<<Dispersion<<'\n'<<AvgNum<<'\n'<<bScanBackward
			<<'\n'<<bReconstructImage;
		
		fout.close();
	}
}

//Load configurations for current acquisition.
void COCTDlg::OnLoadConf()
{
	char szFilter[] = " Config file (*.cfg)|*.cfg||";
	CFileDialog fileDlg(TRUE, "cfg", "", OFN_EXPLORER | OFN_HIDEREADONLY , szFilter, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK) {
		CString strPathName = fileDlg.GetPathName();
		ifstream fin(strPathName);		
		fin>>CamGain>>IntTime>>ScanFreq>>m_ALineNum>>m_IterationNum
			>>ScanVMinX>>ScanVMaxX>>ScanVMinY>>ScanVMaxY>>bStepX>>bStepY
			>>FrameNum>>FrameThickness>>Dispersion>>AvgNum>>bScanBackward
			>>bReconstructImage;
		fin.close();
		RawDataSize=ALineNum* IterationNum * SPECTRUMWIDTH ;
		FrameSize=ALineNum*IterationNum*FFTNUM/2;
	}
	m_SliderCamGain = CamGain/CAMGAINFACTOR;
	m_SliderIntTime = IntTime/INTTIMEFACTOR;
	UpdateData(FALSE);
}

void COCTDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(TRUE);
	CamGain = m_SliderCamGain * CAMGAINFACTOR;	
	IntTime = m_SliderIntTime * INTTIMEFACTOR+0.5;
	ValidateConf();
	UpdateData(FALSE);

	//bResetCam = TRUE;
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COCTDlg::ReconfigureScan()
{	
	BOOL bImaqWasRunning=bImaq;
	BOOL bCaliWasRunning=bCali;
	BOOL bFFTWasRunning=bFFT;
	BOOL bDrawCurveWasRunning=bDrawCurve;
	BOOL bDisplayIntensityWasRunning=bDisplayIntensity;
	BOOL bDisplayPhaseWasRunning=bDisplayPhase;

	OnBnClickedPauseImaq();
	theApp.TerminateDisplayPhaseThread();
	theApp.TerminateDisplayIntensityThread();
	theApp.TerminateSDOCTThreads();	
	theApp.TerminateDrawCurveThread();

	//validate the configuration parameters.
	ValidateConf();
	UpdateData(FALSE);

	IntensityGain=IntensityGain*IterationNum/m_IterationNum;
	IterationNum=m_IterationNum;
	ALineNum=m_ALineNum;
	RawDataSize=ALineNum* IterationNum * SPECTRUMWIDTH ;
	FrameSize=ALineNum*IterationNum*FFTNUM/2;
	LeftLateralIndex = (IMAGEHEIGHT-ALineNum)/2;
	RightLateralIndex = LeftLateralIndex + ALineNum-1;
	theApp.InitiateArrays();

	//if(bImaqWasRunning && bIntensityDlgEn)
	if(bImaqWasRunning )
		theApp.StartImaqThread();
	if(bCaliWasRunning)
		theApp.StartCaliThread();
	if(bFFTWasRunning)
		theApp.StartFFTThread();
	if(bDrawCurveWasRunning)
		theApp.StartDrawCurveThread();
	if(bDisplayIntensityWasRunning)
		theApp.StartDisplayIntensityThread();
	if(bDisplayPhaseWasRunning)
		theApp.StartDisplayPhaseThread();
}

void COCTDlg::OnBnClickedStartImaqFrames()
{
	OnBnClickedPauseImaq();
	UpdateData(TRUE);
	ValidateConf();
	UpdateData(FALSE);

	if (DefaultFilePath.IsEmpty() || 0==FrameNum) return;
	
	//if (bEnableStage)
	//	m_TStage.SetJogStepSize(0,abs(MotorStepsize)/1000.0f);
	bImqFrames=TRUE;
	if (bReconstructImage) 
		theApp.StartSDOCTThreads();
	else
		theApp.StartImaqThread();

	GetDlgItem(IDC_IMAQ_FRAMES_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_IMAQ_FRAMES_START)->ShowWindow(FALSE);
	GetDlgItem(IDC_IMAQ_FRAMES_STOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_IMAQ_FRAMES_STOP)->EnableWindow(TRUE);
}

void COCTDlg::OnBnClickedGetrefImaq()
{
	bGetRef=TRUE;
}
void COCTDlg::OnBnClickedCalibrationImaq()
{
	theApp.GenCaliCoeff(FALSE);
}
void COCTDlg::OnBnClickedPauseImaq()
{
	//if (bReconstructImage) 
	//	theApp.PauseSDOCTThreads();
	//else
	//	theApp.PauseImaqThread();

	theApp.PauseImaqThread();

	UpdateData(FALSE);
	GetDlgItem(IDC_PAUSE_IMAQ)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSE_IMAQ)->ShowWindow(FALSE);
	GetDlgItem(IDC_START_IMAQ)->ShowWindow(TRUE);
	GetDlgItem(IDC_START_IMAQ)->EnableWindow(TRUE);
	//Disable get reference when ImaqThread is stopped
	GetDlgItem(IDC_GETREF_IMAQ)->EnableWindow(FALSE);

}
//

void COCTDlg::OnBnClickedStartImaq()
{
	UpdateData(TRUE);
	ValidateConf();
	UpdateData(FALSE);

	if (ALineNum!= m_ALineNum || IterationNum!=m_IterationNum)
		ReconfigureScan();
	
	//if (bEnableStage)
	//	m_TStage.SetJogStepSize(0,abs(MotorStepsize)/10000.0f);

	if (bReconstructImage)
		theApp.StartSDOCTThreads();
	else
		theApp.StartImaqThread();

	GetDlgItem(IDC_START_IMAQ)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_IMAQ)->ShowWindow(FALSE);
	GetDlgItem(IDC_PAUSE_IMAQ)->ShowWindow(TRUE);
	GetDlgItem(IDC_PAUSE_IMAQ)->EnableWindow(TRUE);
	//enable get reference when ImaqThread is running
	GetDlgItem(IDC_GETREF_IMAQ)->EnableWindow(TRUE);
}



//void COCTDlg::OnBnClickedEnableStage()
//{
//	UpdateData(TRUE);	
//	ValidateConf();
//	UpdateData(FALSE);
//	if (bEnableStage){
//		BOOL bImaqWasRunning=bImaq;
//		theApp.PauseImaqThread();
//		//m_TStage.StartCtrl();
//		//m_TStage.SetJogStepSize(0,abs(MotorStepsize)/10000.0f);
//		if (bImaqWasRunning) 
//			theApp.StartImaqThread();
//	}
//
//	else
//		m_TStage.StopCtrl();
//}

void COCTDlg::OnBnClickedScanBackward()
{
	UpdateData(TRUE);	
	ValidateConf();
	UpdateData(FALSE);

}

//reset acquisition parameters to default value.
void COCTDlg::OnBnClickedResetImaq()
{
	UpDepthIndex = 0;
	DownDepthIndex = IMAGEWIDTH-1;

	CamGain=DEFAULTCAMGAIN;
	IntTime=DEFAULTINTTIME;
	m_SliderCamGain = (CamGain) /CAMGAINFACTOR;
	m_SliderIntTime = IntTime/INTTIMEFACTOR;

	m_ALineNum=IMAGEHEIGHT;
	m_IterationNum=1;
	ScanVMaxX=0.5;
	ScanVMinX=-0.5;
	ScanVMaxY=0;
	ScanVMinY=0;

	ScanFreq=25000;
	bStepX=FALSE;
	bStepY=FALSE;
	//bEnableStage=FALSE;
	bReconstructImage=TRUE;
	bScanBackward=FALSE;
	FrameNum=0;
	//MotorStepsize=0;
	UpdateData(FALSE);

	if (ALineNum!= m_ALineNum || IterationNum!=m_IterationNum)
		ReconfigureScan();
}

//set the default file path for saving and loading data
void COCTDlg::OnBnClickedFilepathButton()
{
	CFileDialog fileDlg(FALSE, NULL, NULL, OFN_EXPLORER , NULL, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK)
		DefaultFilePath = fileDlg.GetPathName();// TODO: Add your control notification handler code here

	UpdateData(FALSE);
}

//for pixel gain Calibration
//void COCTDlg::OnBnClickedOddpixGainCali()
//{
//	char szFilter[] = " Camera Odd Pixel Gain Cali file (*.Cam)|*.Cam||";
//	CFileDialog fileDlg(TRUE, "Cam", "", OFN_EXPLORER | OFN_HIDEREADONLY , szFilter, NULL);
//	int nResponse = fileDlg.DoModal();
//	if(nResponse == IDOK) {
//		CString strPathName = fileDlg.GetPathName();
//		ifstream fin(strPathName);		
//		for (int i = 0; i<SPECTRUMWIDTH/2; i++){
//			fin >> OddPixGain[i*2];
//		}
//		fin.close();//strPathName
//	}
//}


void COCTDlg::OnBnClickedReconstructImage()
{
	UpdateData(TRUE);	
	ValidateConf();
	UpdateData(FALSE);
	if (bReconstructImage) {
		theApp.StartCaliThread();
		theApp.StartFFTThread();
	}
	else	{
		theApp.PauseCaliThread();
		theApp.PauseFFTThread();
	}
// TODO: Add your control notification handler code here
}

void COCTDlg::OnBnClickedImaqFramesStop()
{
	bImqFrames=FALSE;

	GetDlgItem(IDC_IMAQ_FRAMES_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_IMAQ_FRAMES_STOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_IMAQ_FRAMES_START)->ShowWindow(TRUE);
	GetDlgItem(IDC_IMAQ_FRAMES_START)->EnableWindow(TRUE);
	// TODO: Add your control notification handler code here}
}
void COCTDlg::OnBnClickedScanStepx()
{
	UpdateData(TRUE);	
	ValidateConf();
	UpdateData(FALSE);
}

void COCTDlg::OnBnClickedScanStepy()
{
	UpdateData(TRUE);	
	ValidateConf();
	UpdateData(FALSE);
}
