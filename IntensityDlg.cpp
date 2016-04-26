#include "stdafx.h"
#include "OCT.h"
#include "IntensityDlg.h"
#include "OCTDlg.h"
#include "ImageWnd.h"
#include "Defines.h"
#include "niimaq.h"
#include <iostream>
#include <fstream> 
#include <tiffio.h>
#include "ExternVariables.h"
#include "intensitydlg.h"
#include ".\intensitydlg.h"
using namespace std;

//extern UINT DisplayIntensityThreadProc(LPVOID);

IMPLEMENT_DYNAMIC(CIntensityDlg, CDialog)

CIntensityDlg::CIntensityDlg()
{
	//initialize all the data
	m_pIntensityImgWnd = NULL;	
	bDisplayIntensity = FALSE;
}

CIntensityDlg::~CIntensityDlg()
{
	delete m_pIntensityImgWnd;
	m_pIntensityImgWnd = NULL;
}

BEGIN_MESSAGE_MAP(CIntensityDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_START_INTENSITY, OnBnClickedStartIntensity)
	ON_BN_CLICKED(IDC_PAUSE_INTENSITY, OnBnClickedPauseIntensity)
	ON_BN_CLICKED(IDC_SAVE_INTENSITY, OnBnClickedSaveIntensity)
	ON_BN_CLICKED(IDC_AOI, OnBnClickedCheckBox)
	ON_BN_CLICKED(IDC_ZOOM, OnBnClickedZoom)
	ON_BN_CLICKED(IDC_LOAD_INTENSITY, OnBnClickedLoadIntensity)
	ON_BN_CLICKED(IDC_LOG, OnBnClickedCheckBox)
	ON_BN_CLICKED(IDC_HOT_COLOR, OnBnClickedHotColor)
	ON_BN_CLICKED(IDC_AUTOSAVE, OnBnClickedCheckBox)
	ON_NOTIFY(UDN_DELTAPOS, IDC_FILE_SPIN, OnDeltaposFileSpin)
	ON_BN_CLICKED(IDC_RECON_INTENSITY, OnBnClickedReconIntensity)
	ON_BN_CLICKED(IDC_EXPAND, OnBnClickedCheckBox)
	ON_BN_CLICKED(IDC_REPLAY, OnBnClickedReplay)
	ON_BN_CLICKED(IDC_SNAP, OnBnClickedSnap)
END_MESSAGE_MAP()

void CIntensityDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_INTENSITY, intensity);
	DDX_Text(pDX, IDC_VOLTAGEX, voltageX);
	DDX_Text(pDX, IDC_VOLTAGEY, voltageY);
	DDX_Text(pDX, IDC_DEPTH, depth);
	DDX_Check(pDX, IDC_AOI, bAOIEn);
	DDX_Check(pDX,IDC_LOG, bLog);
	DDX_Check(pDX, IDC_ZOOM, bZoom);
	DDX_Check(pDX, IDC_EXPAND, bExpand);
	DDX_Check(pDX, IDC_AUTOSAVE, bAutosave);
	DDX_Slider(pDX, IDC_INTENSITY_GAIN, m_iIntensityGain);
	DDX_Slider(pDX, IDC_INTENSITY_OFFSET, OffSet);
	DDX_Text(pDX, IDC_FILE_INDEX, nIndex);
	DDX_Check(pDX, IDC_HOT_COLOR, bHotColor);
	DDX_Control(pDX, IDC_FILE_SPIN, m_FileSpin);
	CDialog::DoDataExchange(pDX);
}

void CIntensityDlg::OnCancel() 
{
	COCTDlg::bIntensityDlgEn = FALSE;
	theApp.TerminateDisplayIntensityThread();
	delete m_pIntensityImgWnd;
	m_pIntensityImgWnd=NULL;
	DestroyWindow();
}

void CIntensityDlg::OnOK() 
{
	UpdateData();
	nIndex=nIndex>strPathNameArray.GetSize() ? strPathNameArray.GetSize(): nIndex;
	m_FileSpin.SetPos(nIndex);
	UpdateData(FALSE);
	OnDeltaposFileSpin(NULL, NULL);
}

BOOL CIntensityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (bZoom)
		SetWindowPos(&wndTop,INTENSITY_DLG_POS_BIG,NULL);
	else
		SetWindowPos(&wndTop,INTENSITY_DLG_POS_SMALL,NULL);

	//reset the position of the dialog and initialize Controllers
	RECT rect;
	rect.left=100;
	rect.right=613+512*bZoom;
	rect.top = 2;
	rect.bottom=505+500*bZoom;
	m_pIntensityImgWnd = new CImageWnd;
	m_pIntensityImgWnd->Create(NULL,"Intensity Image",WS_BORDER | WS_VISIBLE | CS_HREDRAW | CS_VREDRAW,
		rect,this,ID_INTENSITY_IMAGE_WINDOW,NULL) ;
	m_iIntensityGain = 50;
	UpdateData(FALSE);
	IntensityGain = (float) m_iIntensityGain*GAINFACTOR/SPECTRUMWIDTH/IterationNum;
	return TRUE;
}

void CIntensityDlg::OnBnClickedStartIntensity()
{
	theApp.StartDisplayIntensityThread();	
	GetDlgItem(IDC_START_INTENSITY)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_INTENSITY)->ShowWindow(FALSE);
	GetDlgItem(IDC_PAUSE_INTENSITY)->ShowWindow(TRUE);
	GetDlgItem(IDC_PAUSE_INTENSITY)->EnableWindow(TRUE);
	GetDlgItem(IDC_FILE_SPIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_FILE_INDEX)->EnableWindow(FALSE);
	//GetDlgItem(IDC_AOI)->EnableWindow();

}

void CIntensityDlg::OnBnClickedPauseIntensity()
{
	theApp.PauseDisplayIntensityThread();
	GetDlgItem(IDC_PAUSE_INTENSITY)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSE_INTENSITY)->ShowWindow(FALSE);
	GetDlgItem(IDC_START_INTENSITY)->ShowWindow(TRUE);
	GetDlgItem(IDC_START_INTENSITY)->EnableWindow(TRUE);
}

void CIntensityDlg::OnBnClickedSaveIntensity()
{
	//save the status of threads for resuming.
	BOOL bDisplayIntensityWasRunning=bDisplayIntensity;
	OnBnClickedPauseIntensity();
	TIFF *image;

	char szFilter[] = " RAW Data (*.RAW)|*.RAW| TIFF Image (*.TIF)|*.TIF||";
	CFileDialog fileDlg(FALSE, "TIF.RAW", NULL, OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , szFilter, NULL);
	int nResponse = fileDlg.DoModal();
	if(nResponse == IDOK)
	{
		//save Raw Data or reconstructed Image data Depend on the File extension name.	
		CString strPathName = fileDlg.GetPathName();
		if (strPathName.Right(3)== "TIF") {		
			image = TIFFOpen(strPathName, "w");
			TIFFSetField(image, TIFFTAG_IMAGEWIDTH, IMAGEWIDTH );
			TIFFSetField(image, TIFFTAG_IMAGELENGTH, IMAGEHEIGHT);
			TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, IMAGEHEIGHT);
			TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
			TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
			TIFFSetField(image, TIFFTAG_COMPRESSION, 1);
			TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFWriteEncodedStrip(image, 0, IntensityImage, IMAGESIZE);
			TIFFClose(image);
		}

		//save the raw data
		else {
			BOOL bCaliWasRunning=bCali;
			theApp.PauseCaliThread();
			image = TIFFOpen(strPathName, "w");
			TIFFSetField(image, TIFFTAG_IMAGEWIDTH, SPECTRUMWIDTH );
			TIFFSetField(image, TIFFTAG_IMAGELENGTH, ALineNum*IterationNum);
			TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 16);
			TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
			TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, IterationNum);
			TIFFSetField(image, TIFFTAG_COMPRESSION, 1);
			TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

			//write the images
			int imageOffset = 0;
			int stripSize = IterationNum * SPECTRUMWIDTH*2;
			for ( int stripCount = 0; stripCount < ALineNum; stripCount++ )
				imageOffset+=TIFFWriteEncodedStrip(image, stripCount,CaliBuff + imageOffset, stripSize)/2; 
				// '/2' is because bits per sample is 16 = 2 bytes.
			TIFFClose(image);
			
			if (bCaliWasRunning)
					theApp.StartCaliThread();
		}
	}	
			
	if (bDisplayIntensityWasRunning)	
		OnBnClickedStartIntensity();
}

	
void CIntensityDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData(TRUE);
	IntensityGain = (float)m_iIntensityGain*GAINFACTOR/SPECTRUMWIDTH/IterationNum;
	SetEvent(IntensityBuffNew);
	if (!bDisplayIntensity)
		theApp.StartDisplayIntensityThread();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CIntensityDlg::OnBnClickedCheckBox()
{
	UpdateData(TRUE);
}

void CIntensityDlg::OnBnClickedHotColor()
{
	UpdateData(TRUE);
	if (bHotColor)
		imgPlot ((GUIHNDL)(m_pIntensityImgWnd->GetSafeHwnd()), IntensityImageRGB, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_COLOR_RGB32);
	else
		imgPlot ((GUIHNDL)(m_pIntensityImgWnd->GetSafeHwnd()), IntensityImage, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_MONO_8);
}



void CIntensityDlg::OnBnClickedLoadIntensity()
{
	OnBnClickedPauseIntensity();
	char szFilter[] =  " TIFF Image (*.TIF)|*.TIF||";
	CFileDialog fileDlg(TRUE, "TIF", NULL, OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter, NULL);
	char FileNameBuff[2000];
	memset(FileNameBuff,0,2000);
	fileDlg.m_ofn.lpstrFile=FileNameBuff;
	fileDlg.m_ofn.nMaxFile=2000;
	int nResponse = fileDlg.DoModal();
	POSITION pos =fileDlg.GetStartPosition();
	if (NULL==pos) return;
	if(nResponse == IDOK)	{
		strPathNameArray.RemoveAll();
		while (pos) 
			strPathNameArray.Add(fileDlg.GetNextPathName(pos));

		if (strPathNameArray.IsEmpty()) return;

		//set the range of spin controller;
		m_FileSpin.SetRange(1, strPathNameArray.GetSize());
		GetDlgItem(IDC_FILE_SPIN)->EnableWindow();
		GetDlgItem(IDC_FILE_INDEX)->EnableWindow();
		nIndex=1;
		m_FileSpin.SetPos(nIndex);
		UpdateData(FALSE);

		pMainDlg->OnBnClickedPauseImaq();
		theApp.TerminateDisplayIntensityThread();
		GetDlgItem(IDC_START_INTENSITY)->EnableWindow();
		LoadTiffImage();
	}
}

void CIntensityDlg::OnBnClickedReconIntensity()
{	
	OnBnClickedPauseIntensity();
	char szFilter[] =  " RAW Data (*.RAW)|*.RAW||";
	CFileDialog fileDlg(TRUE, "RAW", NULL, OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter, NULL);
	char FileNameBuff[2000];
	memset(FileNameBuff,0,2000);
	fileDlg.m_ofn.lpstrFile=FileNameBuff;
	fileDlg.m_ofn.nMaxFile=2000;
	int nResponse = fileDlg.DoModal();
	POSITION pos =fileDlg.GetStartPosition();
	if (NULL==pos) return;
	if(nResponse == IDOK)	{
		strPathNameArray.RemoveAll();
		while (pos) 
			strPathNameArray.Add(fileDlg.GetNextPathName(pos));

		if (strPathNameArray.IsEmpty()) return;

		//set the range of spin controller;
		m_FileSpin.SetRange(1, strPathNameArray.GetSize());
		GetDlgItem(IDC_FILE_SPIN)->EnableWindow();
		GetDlgItem(IDC_FILE_INDEX)->EnableWindow();
		nIndex=1;
		m_FileSpin.SetPos(nIndex);

		//theApp.TerminateSDOCTThreads();
		pMainDlg->OnBnClickedPauseImaq();
		//bReconstruction = TRUE;
		theApp.StartCaliThread();
		theApp.StartFFTThread();
		theApp.StartDisplayIntensityThread();
	}
}


void CIntensityDlg::OnDeltaposFileSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (pNMHDR) {
        LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
		m_FileSpin.SetPos(pNMUpDown->iPos);
		nIndex=pNMUpDown->iPos;
		UpdateData(FALSE);
	}

	if ((strPathNameArray[nIndex-1]).Right(3)== "TIF") 
		LoadTiffImage();
	else 
		LoadRawImage();
}

void CIntensityDlg::OnBnClickedReplay()
{
	int  nIndexMax=strPathNameArray.GetSize();
	char FileIndex[4];
	for ( nIndex=0; nIndex<nIndexMax; ) {
		nIndex++;
		m_FileSpin.SetPos(nIndex);
		UpdateData(FALSE);
		if ((strPathNameArray[nIndex-1]).Right(3)== "TIF") 
			LoadTiffImage();
		else 
			LoadRawImage();
	}
}

void CIntensityDlg::LoadTiffImage()
{
		TIFF *image;
		image = TIFFOpen(strPathNameArray[nIndex-1], "r");
		int stripSize = TIFFStripSize (image);
		int stripMax = TIFFNumberOfStrips (image);
		int imageOffset = 0;
		for (int stripCount = 0; stripCount < stripMax; stripCount++)
			imageOffset+=TIFFReadEncodedStrip (image, stripCount,IntensityImage + imageOffset ,stripSize);
		TIFFClose(image);

		//HotColor display .
		for (int i = 0; i<ALineNum;i++){
			for (int j = UpDepthIndex; j<= DownDepthIndex; j++){ 
				*(IntensityImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  HotColorTable[*(IntensityImage+(i + LeftLateralIndex)*IMAGEWIDTH+j)];	
			}
		}

		if (!bZoom) {
			for (int i = 0; i<ALineNum ;i=i+2){
				for (int j = UpDepthIndex; j<= DownDepthIndex; j=j+2){ 
					*(IntensitySmallImage+(i + LeftLateralIndex)/2*IMAGEHALFWIDTH+j/2) = *(IntensityImage+(i + LeftLateralIndex)*IMAGEWIDTH+j);
					//if hot color enabled, get the image array for hot color
					if (bHotColor)
						*(IntensitySmallImageRGB+(i + LeftLateralIndex)/2*IMAGEHALFWIDTH+j/2) =  *(IntensityImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j);
				}
			}
		}

		//if DrawCurve thread is running and IntensityCurveData is needed for display intensity Curve along ALine
		if (bDrawCurve || bAxialIntensity)
			memcpy(IntensityCurveData, IntensityImage+(AxialLine_i+LeftLateralIndex)*IMAGEWIDTH,  IMAGEWIDTH);

		if ( bHotColor ){
			if (bZoom)
				imgPlot((GUIHNDL)(m_pIntensityImgWnd->GetSafeHwnd()), IntensityImageRGB, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_COLOR_RGB32);
			else
				imgPlot((GUIHNDL)(m_pIntensityImgWnd->GetSafeHwnd()), IntensitySmallImageRGB, 0 , 0, IMAGEHALFWIDTH,IMAGEHALFHEIGHT , 0,0,  IMGPLOT_COLOR_RGB32);
		}
		else {
			if (bZoom)
				imgPlot((GUIHNDL)(m_pIntensityImgWnd->GetSafeHwnd()), IntensityImage, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_MONO_8);
			else
				imgPlot((GUIHNDL)(m_pIntensityImgWnd->GetSafeHwnd()), IntensitySmallImage, 0 , 0, IMAGEHALFWIDTH,IMAGEHALFHEIGHT, 0,0,  IMGPLOT_MONO_8);
		}		

}
 
void CIntensityDlg::LoadRawImage()
{
	TIFF *image;
	image = TIFFOpen(strPathNameArray[nIndex-1], "r");
	//read scan and acquisition information from the images
	int stripSize = TIFFStripSize(image);
	int stripMax = TIFFNumberOfStrips(image);
	pMainDlg->m_ALineNum=stripMax;
	pMainDlg->m_IterationNum=stripSize/SPECTRUMWIDTH/2;
	pMainDlg->UpdateData(FALSE);
	if (ALineNum!= pMainDlg->m_ALineNum || IterationNum!= pMainDlg->m_IterationNum)
		pMainDlg->ReconfigureScan();
	
	if (bCali){
		WaitForSingleObject(CaliBuffEmpty,INFINITE);
		int imageOffset = 0;
		for ( int stripCount = 0; stripCount < stripMax; stripCount++ )
			imageOffset+=TIFFReadEncodedStrip(image, stripCount,CaliBuff+imageOffset ,stripSize)/2; 
		// '/2' is because bits per sample is 16 = 2 bytes.
		TIFFClose(image);
		SetEvent(CaliBuffNew);	
	}
}

void CIntensityDlg::OnBnClickedZoom()
{
	UpdateData(TRUE);
	if (pMainDlg->bPhaseDlgEn)
		pMainDlg->m_pPhaseDlg->ResizeWindow();
	ResizeWindow();

}

void CIntensityDlg::ResizeWindow()
{

	if (bZoom) {
		SetWindowPos(&wndTop,INTENSITY_DLG_POS_BIG,NULL);
		m_pIntensityImgWnd->SetWindowPos(&wndTop,IMAGE_WND_POS_BIG,NULL);
	}
	else {
		SetWindowPos(&wndTop,INTENSITY_DLG_POS_SMALL,NULL);
		m_pIntensityImgWnd->SetWindowPos(&wndTop,IMAGE_WND_POS_SMALL,NULL);
	}
}
void CIntensityDlg::OnBnClickedSnap()
{
	//save the status of threads for resuming.
	BOOL bDisplayIntensityWasRunning=bDisplayIntensity;
	OnBnClickedPauseIntensity();
	BOOL bCaliWasRunning=bCali;
	theApp.PauseCaliThread();
	//save Raw Data or reconstructed Image data Depend on the File extension name.	
	CTime currentTime=CTime::GetCurrentTime();
	CString strTime=currentTime.Format("%H_%M_%S");

	//save the Calibrated data

	TIFF *image;
	image = TIFFOpen(DefaultFilePath+strTime+".RAW", "w");//what is the path name???
	TIFFSetField(image, TIFFTAG_IMAGEWIDTH, SPECTRUMWIDTH );
	TIFFSetField(image, TIFFTAG_IMAGELENGTH, ALineNum*IterationNum);
	TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 16);
	TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, IterationNum);
	TIFFSetField(image, TIFFTAG_COMPRESSION, 1);
	TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	//write the images
	int imageOffset = 0;
	int32 stripSize = IterationNum * SPECTRUMWIDTH*2;
	for ( int stripCount = 0; stripCount < ALineNum; stripCount++ )
		imageOffset+=TIFFWriteEncodedStrip(image, stripCount, CaliBuff + imageOffset, stripSize)/2; 
	// '/2' is because bits per sample is 16 = 2 bytes.

	TIFFClose(image);
	if (bCaliWasRunning)
		theApp.StartCaliThread();

	if (bDisplayIntensityWasRunning)	
		OnBnClickedStartIntensity();

}
