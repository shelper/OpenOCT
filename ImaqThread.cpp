#include "stdafx.h"
#include "afxmt.h"
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "OCT.h"
#include "Defines.h"
#include "ImgAcq.h"
#include <tiffio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ExternVariables.h"
#include <math.h>


UINT ImaqThreadProc(LPVOID hWnd) 
{	
	//Variables for image/file saving
	char index[4], direction;	
	TIFF *image;
	CString strFramesFilePath ;
	int FileIndex=0;
	int TmpAvgNum=1;
	int ImaqBuffOffset, CaliBuffOffset, tmpAxialLine_i;
	bool  bScan;

	//initialize CImgAcq class for Image acquisition
	CImgAcq * pImgAcq=new CImgAcq();
	if (pImgAcq->Initialize()){
		delete pImgAcq;	
		theApp.m_hImaqThread=NULL;
		return 0;
	}

	while(bImaq)	{
		if ( pImgAcq->GetImg()) {
			delete pImgAcq;
			theApp.m_hImaqThread=NULL;
			return 0;
		}

		bScan = (bScanDirection==FORWARD) || bScanBackward;

		//frame shift by resetting the scanning voltage to enable complex 3d / arbitrary direction imaging.
		if (bScan) {
			if ( TmpAvgNum==AvgNum ) {
				if (bStepX) {
					ScanVMinX=ScanVMinX+FrameThickness*FRAME2VOLTAGESHIFT;
					ScanVMaxX=ScanVMaxX+FrameThickness*FRAME2VOLTAGESHIFT;
				}
				if (bStepY){
					ScanVMinY=ScanVMinY+FrameThickness*FRAME2VOLTAGESHIFT;
					ScanVMaxY=ScanVMaxY+FrameThickness*FRAME2VOLTAGESHIFT;
				}
				////move the translation stage
				//if ( bEnableStage ) {
				//	if (MotorStepsize>0)
				//		pMainDlg->m_TStage.MoveJog(0,1);	
				//	else 
				//		pMainDlg->m_TStage.MoveJog(0,2);
				//}
			}

			//get reference
			if (bGetRef){
				for (int j=0;j< SPECTRUMWIDTH;j++)  {
					for (int i=0;i< ALineNum * IterationNum ;i++) {				
						AveragedReference[j] =  AveragedReference[j] + *((UINT16 *)(pImgAcq->ImaqBuff)+i*SPECTRUMWIDTH+j) ;
					}
					AveragedReference[j] = AveragedReference[j] / (ALineNum * IterationNum);
				}
				bGetRef=FALSE;
			}

			//if the DrawCurveThread is running, and RawSpectrum is checked for display, copy AxialLine Spectrum for the thread.
			if (bDrawCurve && bRawSpectrum ) {
				tmpAxialLine_i=bScanDirection? (ALineNum -1- AxialLine_i) : AxialLine_i;
				memcpy(RawSpectrumCurveData,(UINT16 *)(pImgAcq->ImaqBuff)+ (tmpAxialLine_i* IterationNum)*SPECTRUMWIDTH, SPECTRUMWIDTH*2);
			}

			if (bDrawCurve && bPower ) {
				for (int i=0;i<ALineNum;i++) {
					PowerData[i] = 0;
					for (int j=0;j<SPECTRUMWIDTH;j++) 
						PowerData[i]= PowerData[i]+ abs(* ((INT16 *) (pImgAcq->ImaqBuff)+ (i* IterationNum)*SPECTRUMWIDTH +j)-2048.0);	
				}
			}

			//if bReconstructImage checked, bCali ==1, thus reconstruct the images, otherwise, do not reconstruct the images to save time/speed up.
			if ( bCali )		{
				WaitForSingleObject(CaliBuffEmpty,  INFINITE);	
				TmpAvgNum++;
				for (int i=0;i< ALineNum * IterationNum ;i++) {				
					ImaqBuffOffset = i*SPECTRUMWIDTH;
					CaliBuffOffset =bScanDirection?  ((ALineNum*IterationNum-i-1)*SPECTRUMWIDTH) : (i*SPECTRUMWIDTH);
					for (int j=0;j< SPECTRUMWIDTH;j++) 
						//*(CaliBuff+CaliBuffOffset+j) =  *((INT16 *)(pImgAcq->ImaqBuff)+ImaqBuffOffset+j) - *(AveragedReference+j);
						*(CaliBuff+CaliBuffOffset+j) =  *((INT16 *)(pImgAcq->ImaqBuff)+ImaqBuffOffset+j);
				}
				SetEvent(CaliBuffNew);
			}

			//continuous acquisition, 
			if (bImqFrames) {	
				//check if the file path is ready for continuous acquisition
				if (strFramesFilePath.CompareNoCase(DefaultFilePath)){
					strFramesFilePath=DefaultFilePath;
					FileIndex=0;				
				}

				//separate forward and backward scanning.
				FileIndex++;	
				itoa(FileIndex,index,10);
				//save data as TIFF if continuous acquisition
				direction= bScanDirection? 'B' : 'F';
				image = TIFFOpen(strFramesFilePath+index+direction+".RAW", "w");					
				TIFFSetField(image, TIFFTAG_IMAGEWIDTH, SPECTRUMWIDTH);
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
					imageOffset+=TIFFWriteEncodedStrip(image, stripCount, CaliBuff + imageOffset, stripSize)/2; 
					// '/2' is because bits per sample is 16 = 2 bytes.
				TIFFClose(image);	

				//when FramNum Images saved, stop continuous acquisition and return
				if ( FileIndex>=FrameNum )		{
					FileIndex=0;
					bImqFrames=FALSE;
					pMainDlg->GetDlgItem(IDC_IMAQ_FRAMES_STOP)->EnableWindow(FALSE);
					pMainDlg->GetDlgItem(IDC_IMAQ_FRAMES_STOP)->ShowWindow(FALSE);
					pMainDlg->GetDlgItem(IDC_IMAQ_FRAMES_START)->ShowWindow(TRUE);
					pMainDlg->GetDlgItem(IDC_IMAQ_FRAMES_START)->EnableWindow(TRUE);
					break;
				}
			}
			else	FileIndex=0;

			if (TmpAvgNum>AvgNum)			TmpAvgNum=1;
			//if  (FrameInterval)		Sleep(FrameInterval);
		}

		bScanDirection=!bScanDirection;	
		//switching between backward and forward.
	}

	//exit this thread by deleting the ImgAcq classes.and set the handle of thread to Null;
	delete pImgAcq;
	theApp.m_hImaqThread=NULL;
	return 0;
}