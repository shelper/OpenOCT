#include "stdafx.h"
#include "afxmt.h"
#include "OCT.h"
#include "Defines.h"
#include "ExternVariables.h"
#include <math.h>
#include "fftw-3.1.2\fftw3.h"

//void HilbertTranform(float * InputArray, float * OutputArray);

UINT CaliThreadProc(LPVOID hWnd)
{
	//the index for left and right pixels for interpolation.
	int CaliBuffOffset;
	//float HilbertResult[SPECTRUMWIDTH*2];
	float DispersionCoeff[SPECTRUMWIDTH];
	float tempDispersion;

	//float OddPixOffset=8.32;
	//float PixValue;
	//before start the thread, set the event for the prior thread to start correctly
	SetEvent(CaliBuffEmpty);
	while (bCali) {
		//calculate dispersion compensation coeff
		if ( tempDispersion != Dispersion ){
			tempDispersion=Dispersion;
			for (int i=0;i< SPECTRUMWIDTH;i++) {
				DispersionCoeff[i]=Dispersion*(pow(i/687.5-1,2) - 1);
			}
		}
		//calibration algorithm, use linear interpolation, do the calculation when the data is ready
		WaitForSingleObject(CaliBuffNew,  INFINITE);				
		//MessageBox((HWND)hWnd,"CaliBuffNew timeout", "SDOCT", MB_OK);

		////Dispersion compensation 
		//for (int i=0;i< ALineNum * IterationNum ;i++) {				
		//	CaliResultOffest = i*SPECTRUMWIDTH ;
		//	for (int j=0;j< SPECTRUMWIDTH;j++) {
		//		*(CaliResult + CaliResultOffest+j) = *(CaliBuff + CaliResultOffest+j) * BarthWindow [j];
		//	}
		//	HilbertTranform(CaliResult+CaliResultOffest,HilbertResult);
		//	for (int j=0;j< SPECTRUMWIDTH;j++) {
		//		*(CaliResult + CaliResultOffest+j)=*(HilbertResult + 2*j) * cos(DispersionCoeff[j])	- *(HilbertResult + 2*j+1) * sin(DispersionCoeff[j]);
		//		*(CaliResult + CaliResultOffest+j)=*(CaliResult + CaliResultOffest+j) /SPECTRUMWIDTH;
		//	}
		//}

		//Dispersion Compensation in complex domain
		for (int i=0;i< ALineNum * IterationNum ;i++) {				
			CaliBuffOffset = i*SPECTRUMWIDTH;
			for (int j=0;j< SPECTRUMWIDTH;j++) {
				*(CaliResult + 2*(CaliBuffOffset+j))=(*(CaliBuff + (CaliBuffOffset+j))- *(AveragedReference+j))* BarthWindow [j]*cos(DispersionCoeff[j]);
				//*(CaliResult + 2*(CaliBuffOffset+j)) = *(CaliBuff + CaliBuffOffset +j) ;
				*(CaliResult + 2*(CaliBuffOffset+j)+1) = *(CaliResult + 2*(CaliBuffOffset+j))*tan(DispersionCoeff[j]);
			}
		}
		
		////Calibration of the wavenumber linearility
		//for (int i=0;i< ALineNum * IterationNum ;i++) {				
		//	CaliResultOffest = i*SPECTRUMWIDTH ;
		//	for (int j=0;j< SPECTRUMWIDTH;j++) 
		//		*(CaliResult + CaliResultOffest+j) =  
		//		(  *(CaliBuff + CaliResultOffest+LeftPointIndex[j]) )    *  LeftPointCoeff[j] +
		//		(  *(CaliBuff + CaliResultOffest+RightPointIndex[j]) )  *  RightPointCoeff[j];
		//}

		//When quit calibration thread, also quit FFT thread
		if (bDrawCurve && bCaliedSpectrum ) 
			memcpy(CaliedSpectrumCurveData,CaliResult+AxialLine_i*IterationNum*SPECTRUMWIDTH, SPECTRUMWIDTH*4);

		//if FFT thread is running, copy calibrated data to FFT data buffer (FFTBuff)
		if (bFFT) {
			WaitForSingleObject(FFTBuffEmpty, INFINITE);				
				//MessageBox((HWND)hWnd,"FFTBuffEmpty timeout", "SDOCT", MB_OK);

			for ( int i = 0; i< ALineNum * IterationNum;i++)
				memcpy(FFTBuff+i* SPECTRUMWIDTH*2,CaliResult+i* SPECTRUMWIDTH*2, SPECTRUMWIDTH*8);

			//FFT data buff (FFTBuff) is full and new.
			SetEvent(FFTBuffNew);
		}

		//Calibration is done
		SetEvent(CaliBuffEmpty);
	}

	theApp.m_hCaliThread=NULL;
	return 0;	
}

//void  HilbertTranform(float * InputArray, float * OutputArray)
//{
//	//float Phase[SPECTRUMWIDTH]; //using Hilbert transform to get the Phase
//	float out1[SPECTRUMWIDTH*2];
//	//out2 = fftwf_malloc(sizeof(fftwf_complex) * SPECTRUMWIDTH);
//
//	//do FFT
//	//fftw_plan fftw_plan_dft_r2c_1d(int n, double *in, fftw_complex *out, unsigned flags);
//	fftwf_plan p1 = fftwf_plan_dft_r2c_1d(SPECTRUMWIDTH, InputArray , (fftwf_complex *) out1, FFTW_ESTIMATE );
//	fftwf_execute(p1); /* repeat as needed */
//	fftwf_destroy_plan(p1);
//
//	for ( int i = 0; i< SPECTRUMWIDTH*2; i++)	{
//		if (i<SPECTRUMWIDTH)
//			out1[i]=out1[i]*2;	
//		else if (i>SPECTRUMWIDTH+1)
//			out1[i]=0;
//	}
//
//	fftwf_plan p2 = fftwf_plan_dft_1d(SPECTRUMWIDTH, (fftwf_complex *) out1 , (fftwf_complex *) OutputArray, FFTW_BACKWARD, FFTW_ESTIMATE);
//	fftwf_execute(p2);
//	fftwf_destroy_plan(p2);
//	//return (float *) out2;
//
//	////get Phase
//	//for ( int i = 0; i<SPECTRUMWIDTH; i++)	{
//	//	Phase[i]=atan(out2[i*2+1]/out2[i*2]);
//	//	if (out2[i*2+1]<0 && out2[i*2]<0)
//	//		Phase[i]=Phase[i]-PI;
//	//	if (out2[i*2+1]>0 && out2[i*2]<0)
//	//		Phase[i]=Phase[i]+PI;
//	//}
//}
