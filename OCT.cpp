#include "stdafx.h"
#include "OCT.h"
#include "OCTDlg.h"
#include "CurveDlg.h"
#include "PhaseDlg.h"
#include "ImgAcq.h"
#include "IntensityDlg.h"
#include "fftw3.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include "Defines.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//the pointer to the main dialog for SDOCT 
COCTDlg * pMainDlg = NULL;

//configurations for camera 
int		CamGain=DEFAULTCAMGAIN;
int		IntTime=DEFAULTINTTIME;
int		ScanFreq=100000;

//configuration for scanner
double	ScanVMaxX=0.5;
double	ScanVMinX=-0.5;
double	ScanVMaxY=0;
double	ScanVMinY=0;

//Acquisition parameters
int		IterationNum=1;
int		ALineNum=IMAGEHEIGHT;
int		RawDataSize=ALineNum*IterationNum*SPECTRUMWIDTH;
int		FrameSize=ALineNum*IterationNum*FFTNUM/2;
BOOL	bScanDirection=FORWARD;
BOOL	bScanBackward=FALSE;

//configurations for continuous and 3D imaging
int		FrameThickness=0;
int		FrameNum=0;
//int		MotorStepsize=0;
int		AvgNum=1;
float	Dispersion=0;
BOOL	bReconstructImage=TRUE;
BOOL	bStepX=FALSE;
BOOL	bStepY=FALSE;
//BOOL	bEnableStage=FALSE;
BOOL	bImqFrames=FALSE;
BOOL	bGetRef=FALSE;
CString DefaultFilePath = NULL;

//calibration co-efficiencies;
UINT16  LeftPointIndex[SPECTRUMWIDTH];
UINT16  RightPointIndex[SPECTRUMWIDTH];
float	LeftPointCoeff[SPECTRUMWIDTH];
float	RightPointCoeff[SPECTRUMWIDTH];
//float	OddPixGain[SPECTRUMWIDTH];
float	BarthWindow[SPECTRUMWIDTH];

//curve Drawing arrays
UINT16	RawSpectrumCurveData[SPECTRUMWIDTH]; 
int		PowerData[1000]={0};
float	CaliedSpectrumCurveData[SPECTRUMWIDTH];
UINT8	IntensityCurveData[IMAGEWIDTH];
UINT8	PhaseCurveData[IMAGEWIDTH];

//images' arrays pointer
//INT16 * ReferenceBuff=NULL;
int AveragedReference[SPECTRUMWIDTH]={0};
INT16 * CaliBuff=NULL;
float * CaliResult=NULL;
float * FFTBuff=NULL;
float * FFTResult=NULL;
float * IntensityBuff=NULL;
UINT * IntensityImageRGB=NULL;
UINT8 * IntensityImage=NULL;
float * PhaseBuff=NULL;
UINT8 * PhaseWeightBuff=NULL;
UINT * PhaseImageRGB=NULL;
UINT8 * PhaseImage=NULL;
UINT * IntensitySmallImageRGB=NULL;
UINT8 * IntensitySmallImage=NULL;
UINT * PhaseSmallImageRGB=NULL;
UINT8 * PhaseSmallImage=NULL;


//threads loop's BOOL conditions
BOOL bImaq = FALSE;
BOOL bCali= FALSE;
BOOL bFFT= FALSE;
BOOL bDrawCurve= FALSE;
BOOL bDisplayIntensity= FALSE;
BOOL bDisplayPhase= FALSE;

//CurveDlg parameters
BOOL bRawSpectrum = FALSE;
BOOL bCaliedSpectrum = FALSE;
BOOL bPower=FALSE;
BOOL bAxialIntensity = FALSE;
BOOL bAxialPhase = FALSE;
int AxialLine_i = 0;

//IntensityDlg parameters
float IntensityGain;
int	OffSet=50;
BOOL bZoom = FALSE;
BOOL bExpand=FALSE;
BOOL bAOIEn = FALSE;
BOOL bLog=FALSE;
BOOL bHotColor=TRUE;
BOOL bAutosave=FALSE;
//BOOL bReconstruction=FALSE;
int LeftLateralIndex = 0;
int RightLateralIndex = ALineNum - 1;
int UpDepthIndex = 0;
int DownDepthIndex = IMAGEWIDTH-1;
CStringArray strPathNameArray;
int nIndex = 0;

//PhaseDlg parameters
int PhaseThresh=0;

//pixel information for displayed image
int	 intensity =0;
int  phase=0;
double depth =0.0;
double	voltageX = 0.0;
double	voltageY = 0.0;


//Events for threads communication and synchronization
EVENT CaliBuffEmpty=CreateEvent(NULL,FALSE,TRUE,NULL);
EVENT FFTBuffEmpty=CreateEvent(NULL,FALSE,TRUE,NULL);
EVENT IntensityBuffEmpty = CreateEvent(NULL,FALSE,TRUE,NULL);
EVENT PhaseBuffEmpty = CreateEvent(NULL,FALSE,TRUE,NULL);
EVENT CaliBuffNew=CreateEvent(NULL,FALSE,FALSE,NULL);
EVENT FFTBuffNew=CreateEvent(NULL,FALSE,FALSE,NULL);
EVENT IntensityBuffNew = CreateEvent(NULL,FALSE,FALSE,NULL);
EVENT PhaseBuffNew = CreateEvent(NULL,FALSE,FALSE,NULL);

extern UINT ImaqThreadProc(LPVOID);
extern UINT CaliThreadProc(LPVOID);
extern UINT FFTThreadProc(LPVOID);
extern UINT DisplayIntensityThreadProc(LPVOID);
extern UINT DisplayPhaseThreadProc(LPVOID);
extern UINT DrawCurveThreadProc(LPVOID);

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(COCTApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT,OnAppAbout)
END_MESSAGE_MAP()

COCTApp::COCTApp()
{
	m_hImaqThread = NULL;
	m_hCaliThread = NULL;
	m_hFFTThread = NULL;
	m_hDrawCurveThread = NULL;
	m_hDisplayIntensityThread =NULL;
	m_hDisplayPhaseThread =NULL;

	InitiateArrays();

}

COCTApp::~COCTApp()
{
	TerminateSDOCTThreads();
}

COCTApp theApp;

BOOL COCTApp::InitInstance()
{
	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//initialize the barthwindow for spectrum reshap
	for (float i =0; i<SPECTRUMWIDTH;i++) { 
		BarthWindow[int(i)]=	0.62-abs(0.48*(i/(SPECTRUMWIDTH-1) -0.5))+0.38*cos(2*PI*(i/(SPECTRUMWIDTH-1)-0.5));
	}


	//initialize the calibration parameters
	//for (int i =0; i<SPECTRUMWIDTH;i++) {
	//	LeftPointIndex[i]=i;
	//	LeftPointCoeff[i]=1;
	//}

	GenCaliCoeff(TRUE);

	COCTDlg dlg;
	pMainDlg = &dlg;
	INT_PTR nResponse = dlg.DoModal();	

	if (nResponse == IDOK)	{
	}

	else if (nResponse == IDCANCEL)	{
	}

	return FALSE;
}

void COCTApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void COCTApp::InitiateArrays()
{
	FreeArrays();
	CallocArrays();
}

void COCTApp::FreeArrays()
{	
	//calloc initialize the array to zeros as needed for calculation
	//if (NULL != ReferenceBuff) free(ReferenceBuff);     
	if (NULL != CaliBuff)  free(CaliBuff);   
	if (NULL != CaliResult)  free(CaliResult);   
	if (NULL != FFTBuff) free(FFTBuff);   
	if (NULL != FFTResult)  free(FFTResult);  
	if (NULL != IntensityBuff)  free(IntensityBuff);   
	if (NULL != IntensityImageRGB)	free(IntensityImageRGB);
	if (NULL != IntensityImage)	free(IntensityImage);
	if (NULL != PhaseBuff)  free(PhaseBuff);   
	if (NULL != PhaseWeightBuff)  free(PhaseWeightBuff);   
	if (NULL != PhaseImageRGB)	free(PhaseImageRGB);
	if (NULL != PhaseImage)	free(PhaseImage);
	if (NULL != IntensitySmallImage)	free(IntensitySmallImage);
	if (NULL != IntensitySmallImageRGB)	free(IntensitySmallImageRGB);
	if (NULL != PhaseSmallImage)	free(PhaseSmallImageRGB);
	if (NULL != PhaseSmallImage)	free(PhaseSmallImage);
}

void COCTApp::CallocArrays()
{
	//ReferenceBuff=(INT16 *) calloc(RawDataSize,2);
	CaliBuff=(INT16 *) calloc (RawDataSize, 2 );
	CaliResult= (float *) calloc(RawDataSize*2, 4);
	FFTBuff=(float *) calloc(RawDataSize*2,4);
	FFTResult=(float *)calloc(FrameSize*4, 4);
	IntensityBuff=(float *) calloc(FrameSize*2,4);	
	IntensityImageRGB=(UINT *) calloc (IMAGESIZE,4 );
	IntensityImage=(UINT8 *) calloc (IMAGESIZE,1);
	PhaseBuff=(float *) calloc(FrameSize*2,4);
	PhaseWeightBuff=(UINT8 *) calloc (IMAGESIZE,1);
	PhaseImageRGB=(UINT *) calloc (IMAGESIZE,4 );
	PhaseImage=(UINT8 *) calloc (IMAGESIZE,1);

	IntensitySmallImage=(UINT8 *) calloc (IMAGEQUATERSIZE,1);
	IntensitySmallImageRGB=(UINT *) calloc (IMAGEQUATERSIZE,4 );
	PhaseSmallImage=(UINT8 *) calloc (IMAGEQUATERSIZE,1);
	PhaseSmallImageRGB=(UINT *) calloc (IMAGEQUATERSIZE,4 );
}

//Phase linearization for calibration of the data.
void COCTApp::GenCaliCoeff(BOOL bAutoCali)
{
	//save the Imaq Thread status for resuming
	//BOOL bImaqWasRunning=bImaq;
	//PauseSDOCTThreads();
	
	//float Phase[SPECTRUMWIDTH]; //using Hilbert transform to get the Phase
	//float CurveData[FFTNUM];
	//float LinearPhase[SPECTRUMWIDTH]; //Linear Phase calculated.
	//CString strPathName=".\\Cali.RSC";


	////File reading dialog for saving the spectrum data for calibration
	//if (bAutoCali) {
	//	ifstream fin(strPathName);		
	//	for (int i = 0; i<SPECTRUMWIDTH; i++){
	//		fin >> CurveData[i*2];
	//		CurveData[i*2+1]=0;
	//	}
	//	fin.close();//strPathName
	//}

	//else{
	//	char szFilter[] = "Raw Spectrum Curves (*.RSC)|*.RSC||";
	//	CFileDialog fileDlg(TRUE, "RSC", "", OFN_EXPLORER | OFN_HIDEREADONLY, szFilter, NULL);
	//	int nResponse = fileDlg.DoModal();
	//	if(nResponse == IDOK)
	//	{
	//		strPathName = fileDlg.GetPathName();
	//		ifstream fin(strPathName);		
	//		for (int i = 0; i<SPECTRUMWIDTH; i++){
	//			fin >> CurveData[i*2];
	//			CurveData[i*2+1]=0;
	//		}
	//		fin.close();//strPathName
	//	}

	//	else return;
	//}

	////FFT transfrom (Hilbert traansform.
	//float out1[FFTNUM];
	//float out2[FFTNUM];

	////do FFT
	//fftwf_plan p1 = fftwf_plan_dft_1d(SPECTRUMWIDTH, (fftwf_complex *) CurveData , (fftwf_complex *) out1, FFTW_FORWARD, FFTW_ESTIMATE);
	//fftwf_execute(p1); /* repeat as needed */
	//fftwf_destroy_plan(p1);

	//for ( int i = 0; i< FFTNUM; i++)	{
	//	//out1[i]=out1[i]*2;		
	//	if ( i < 80 )
	//		out1[i]=0;
	//	if ( i >= SPECTRUMWIDTH +2 )
	//		out1[i] = 0;
	//}

	//fftwf_plan p2 = fftwf_plan_dft_1d(SPECTRUMWIDTH, (fftwf_complex *) out1 , (fftwf_complex *) out2, FFTW_BACKWARD, FFTW_ESTIMATE);
	//fftwf_execute(p2);	
	//fftwf_destroy_plan(p2);

	////get Phase
	//for ( int i = 0; i<SPECTRUMWIDTH; i++)	{
	//	Phase[i]=atan(out2[i*2+1]/out2[i*2]);
	//	if (out2[i*2+1]<0 && out2[i*2]<0)
	//		Phase[i]=Phase[i]-PI;
	//	if (out2[i*2+1]>0 && out2[i*2]<0)
	//		Phase[i]=Phase[i]+PI;
	//}

	//for (int i = 0; i< SPECTRUMWIDTH; i++) 
	//{
	//	if (Phase[i]-Phase[i+1]>= PI) 
	//	{
	//		for (int j = i+1; j< SPECTRUMWIDTH; j++)
	//			Phase[j]=Phase[j]+2*PI;
	//	}
	//}

	////calculate LinearPhase
	//LinearPhase[0]=Phase[0];
	//float step = (Phase[SPECTRUMWIDTH-1]-Phase[0])/(SPECTRUMWIDTH-1);
	//for (int i = 1; i < SPECTRUMWIDTH; i++)	{
	//	LinearPhase[i] = LinearPhase[i-1]+step;
	//}

	////initialize the start and end value of the left/right point
	//LeftPointIndex[0]= 0;//index of the left point for linear interpolation of the middle point
	//LeftPointCoeff[0]=1;//weight of the left point for linear interpolation of the middle point
	//RightPointIndex[0]=1;//index of the right point for linear interpolation of the middle point
	//RightPointCoeff[0]=0;//weight of the right point for linear interpolation of the middle point
	//LeftPointIndex[SPECTRUMWIDTH-1]= SPECTRUMWIDTH-2;
	//LeftPointCoeff[SPECTRUMWIDTH-1]= 0;
	//RightPointIndex[SPECTRUMWIDTH-1]= SPECTRUMWIDTH-1;
	//RightPointCoeff[SPECTRUMWIDTH-1]= 1;
	//
	////calculate the other values of the left/right point
	//for (int i = 1; i< SPECTRUMWIDTH-1; i++)	{
	//	for ( int j = 1; LinearPhase[i] > Phase[j]; j++) {
	//		LeftPointIndex[i]= j-1;
	//		LeftPointCoeff[i]=(Phase[j]-LinearPhase[i])/(Phase[j]-Phase[j-1]);
	//		RightPointIndex[i]=j;
	//		RightPointCoeff[i]=(LinearPhase[i]-Phase[j-1])/(Phase[j]-Phase[j-1]);
	//	}
	//}	

	////if(bImaqWasRunning)
	////	StartSDOCTThreads();
}

void COCTApp::StartImaqThread()
{
	DWORD ImaqThreadID;
	bImaq=TRUE;
	if ( NULL == m_hImaqThread )	{
		m_hImaqThread = ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE) ImaqThreadProc,
			pMainDlg->GetSafeHwnd(),THREAD_PRIORITY_HIGHEST,&ImaqThreadID);
	}
	else
		while (::ResumeThread(m_hImaqThread));
}

void COCTApp::StartCaliThread()
{
	DWORD CaliThreadID;
	bCali = TRUE;
	if ( NULL == m_hCaliThread )	{
		m_hCaliThread= ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)CaliThreadProc,
			pMainDlg->GetSafeHwnd(),THREAD_PRIORITY_NORMAL,&CaliThreadID);
	}
	else
		while (::ResumeThread(m_hCaliThread));
}

void COCTApp::StartFFTThread()
{
	DWORD FFTThreadID;
	bFFT = TRUE;
	if ( NULL == m_hFFTThread )	{
		m_hFFTThread= ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)FFTThreadProc,
			pMainDlg->GetSafeHwnd(), THREAD_PRIORITY_NORMAL,&FFTThreadID);
	}
	else
		while (::ResumeThread(m_hFFTThread));
}



void COCTApp::StartSDOCTThreads()
{	
	//bReconstruction = FALSE;
	StartImaqThread();
	StartCaliThread();
	StartFFTThread();

}

void COCTApp::TerminateImaqThread()
{
	if (NULL != m_hImaqThread)	{
		bImaq = FALSE; 
		//bCali = FALSE;
		while (::ResumeThread(m_hImaqThread));
		SetEvent(CaliBuffEmpty);

		if ( WAIT_TIMEOUT == WaitForSingleObject(m_hImaqThread, 5000) ) {
			MessageBox( pMainDlg->GetSafeHwnd(),"ImaqThread termination timeout", "SDOCT", MB_OK);
			TerminateThread(m_hImaqThread,0);			
			//CloseHandle(m_hImaqThread);
		}
		m_hImaqThread = NULL;
	}	
}

void COCTApp::TerminateCaliThread()
{
	if (NULL != m_hCaliThread) {
		bCali = FALSE; 
		//bFFT = FALSE;
		while (::ResumeThread(m_hCaliThread));
		SetEvent(CaliBuffNew);
		SetEvent(FFTBuffEmpty);
		if (WAIT_TIMEOUT==WaitForSingleObject(m_hCaliThread, 5000)){
			MessageBox( pMainDlg->GetSafeHwnd(),"Cali termination timeout", "SDOCT", MB_OK);
			TerminateThread(m_hCaliThread,0);
		}
		m_hCaliThread = NULL;
	}
}

void COCTApp::TerminateFFTThread()
{
	if (NULL != m_hFFTThread)
	{
		bFFT = FALSE;
		while (::ResumeThread(m_hFFTThread));
		SetEvent(FFTBuffNew);
		SetEvent(IntensityBuffEmpty);
		if (WAIT_TIMEOUT==WaitForSingleObject(m_hFFTThread, 5000)) {
			MessageBox( pMainDlg->GetSafeHwnd(),"FFT termination timeout", "SDOCT", MB_OK);
			TerminateThread(m_hFFTThread,0);
			//CloseHandle(m_hFFTThread);
		}
		m_hFFTThread = NULL;
	}
}

void COCTApp::TerminateSDOCTThreads()
{
	TerminateFFTThread();
	TerminateCaliThread();
	TerminateImaqThread();	
}

void COCTApp::PauseImaqThread()
{
	if (NULL != m_hImaqThread)	{
		::SuspendThread(m_hImaqThread);
		bImaq=FALSE;
	}
}

void COCTApp::PauseCaliThread()
{
	if (NULL != m_hCaliThread)	{
		::SuspendThread(m_hCaliThread);
		bCali =FALSE;
	}
}

void COCTApp::PauseFFTThread()
{
	if (NULL != m_hFFTThread)	{
		::SuspendThread(m_hFFTThread);
		bFFT=FALSE;
	}
}

void COCTApp::PauseSDOCTThreads()
{
	PauseImaqThread();
	PauseCaliThread();
	PauseFFTThread();
}

void COCTApp::StartDisplayIntensityThread()
{
	DWORD DisplayIntensityThreadID;
	bDisplayIntensity = TRUE;
	if ( NULL == m_hDisplayIntensityThread ) {
		m_hDisplayIntensityThread= ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)
			DisplayIntensityThreadProc,pMainDlg->m_pIntensityDlg->GetSafeHwnd(),
			THREAD_PRIORITY_NORMAL,&DisplayIntensityThreadID);
	}
	else
		while (::ResumeThread(m_hDisplayIntensityThread));
}

void COCTApp::TerminateDisplayIntensityThread()
{
	if (NULL != m_hDisplayIntensityThread)	{
		bDisplayIntensity = FALSE;
		while (::ResumeThread(m_hDisplayIntensityThread));
		SetEvent(IntensityBuffNew);
		SetEvent(PhaseBuffEmpty);
		if (WAIT_TIMEOUT==WaitForSingleObject(m_hDisplayIntensityThread, 10000)){
			MessageBox( pMainDlg->m_pIntensityDlg->GetSafeHwnd(),"DisplayIntenstiy termination timeout", "SDOCT", MB_OK);
			TerminateThread(m_hDisplayIntensityThread,0);
		}
		m_hDisplayIntensityThread=NULL;
	}
}

void COCTApp::PauseDisplayIntensityThread()
{
	if (NULL != m_hDisplayIntensityThread)	{
		::SuspendThread(m_hDisplayIntensityThread);
		bDisplayIntensity=FALSE;
	}
}

void COCTApp::StartDisplayPhaseThread()
{
	DWORD DisplayPhaseThreadID;
	bDisplayPhase = TRUE;
	if ( NULL == m_hDisplayPhaseThread ) {
		m_hDisplayPhaseThread= ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)
			DisplayPhaseThreadProc,pMainDlg->m_pPhaseDlg->GetSafeHwnd(),
			THREAD_PRIORITY_NORMAL,&DisplayPhaseThreadID);
	}
	else
		while (::ResumeThread(m_hDisplayPhaseThread));
}

void COCTApp::TerminateDisplayPhaseThread()
{
	if (NULL != m_hDisplayPhaseThread)
	{
		bDisplayPhase = FALSE;
		while (::ResumeThread(m_hDisplayPhaseThread));
		SetEvent(PhaseBuffNew);
		if (WAIT_TIMEOUT==WaitForSingleObject(m_hDisplayPhaseThread, 5000)) {
			MessageBox( pMainDlg->m_pPhaseDlg->GetSafeHwnd(),"DisplayPhase termination timeout", "SDOCT", MB_OK);
			TerminateThread(m_hDisplayPhaseThread,0);
		}
		m_hDisplayPhaseThread = NULL;
	}
}

void COCTApp::PauseDisplayPhaseThread()
{
	if (NULL != m_hDisplayPhaseThread)	{
		::SuspendThread(m_hDisplayPhaseThread);
		bDisplayPhase=FALSE;
	}
}

void COCTApp::StartDrawCurveThread()
{
	DWORD DrawCurveThreadID;
	bDrawCurve = TRUE;
	if ( NULL == m_hDrawCurveThread ) {
		m_hDrawCurveThread = ::CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)DrawCurveThreadProc,
			pMainDlg->m_pCurveDlg->GetSafeHwnd(),THREAD_PRIORITY_NORMAL,&DrawCurveThreadID);
	}
	else
		while (::ResumeThread(m_hDrawCurveThread));
}

void COCTApp::TerminateDrawCurveThread()
{
	if (m_hDrawCurveThread != NULL)	{
		bDrawCurve = FALSE;
		while (::ResumeThread(m_hDrawCurveThread));
		if (WAIT_TIMEOUT==WaitForSingleObject(m_hDrawCurveThread, 5000)){
			MessageBox( pMainDlg->m_pCurveDlg->GetSafeHwnd(),"DrawCurve termination timeout", "SDOCT", MB_OK);
			TerminateThread(m_hDrawCurveThread,0);
		}

		//CloseHandle(m_hDrawCurveThread);
		m_hDrawCurveThread = NULL;
	}
}

void COCTApp::PauseDrawCurveThread()
{
	if (NULL != m_hDrawCurveThread)	{
		::SuspendThread(m_hDrawCurveThread);
		bDrawCurve=FALSE;
	}
}

void COCTDlg::ValidateConf()
{
	ScanVMaxX = (ScanVMaxX > 10 ) ? 10 : ScanVMaxX;
	ScanVMaxX= (ScanVMaxX < -10) ? -10 : ScanVMaxX;
	ScanVMinX = (ScanVMinX > 10 ) ? 10 : ScanVMinX;
	ScanVMinX= (ScanVMinX < -10) ? -10 : ScanVMinX;

	ScanVMaxY = (ScanVMaxY > 10 ) ? 10 : ScanVMaxY;
	ScanVMaxY= (ScanVMaxY< -10) ? -10 : ScanVMaxY;
	ScanVMinY = (ScanVMinY > 10 ) ? 10 : ScanVMinY;
	ScanVMinY= (ScanVMinY< -10) ? -10 : ScanVMinY;


	AvgNum=AvgNum>5000 ? 5000 : AvgNum;
	AvgNum=AvgNum<1 ? 1 : AvgNum;
	m_ALineNum = (m_ALineNum <2)? 2 : m_ALineNum;
	m_ALineNum = (m_ALineNum >1000)? 1000 : m_ALineNum;
	m_ALineNum = ((int)(m_ALineNum/2))*2; //round(m_AlineNume) as in matlab.
	m_IterationNum=(m_IterationNum>10)? 10 : m_IterationNum;
}
