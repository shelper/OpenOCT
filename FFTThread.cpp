#include "stdafx.h"
#include "fftw3.h"
#include "afxmt.h"
#include "Defines.h"
#include "ExternVariables.h"

UINT FFTThreadProc( LPVOID hWnd )
{
	int pn =  FFTNUM;	//point numbers for FFT

	//before start the thread, set the event for the prior thread to start correctly
	SetEvent(FFTBuffEmpty);

	////setup FFT plan 
	//fftwf_r2r_kind fftkind = FFTW_R2HC;
	//fftwf_plan p = fftwf_plan_many_r2r(1, &pn, ALineNum*IterationNum,
	//	FFTBuff, NULL,		1,  FFTNUM,
	//	FFTResult, NULL,		1,  FFTNUM,
	//	&fftkind, FFTW_MEASURE);

	//setup FFT plan for complex data.
	fftwf_plan p = fftwf_plan_many_dft(	1, &pn, ALineNum*IterationNum,
										(fftwf_complex *)FFTBuff, NULL,		
										1,  SPECTRUMWIDTH,
										(fftwf_complex *)FFTResult, NULL,		
										1,  FFTNUM,
										FFTW_FORWARD, FFTW_MEASURE);

	while (bFFT) 
	{   
		WaitForSingleObject(FFTBuffNew,  INFINITE) ;
			//MessageBox((HWND)hWnd,"FFTBuffNew timeout", "SDOCT", MB_OK);
		fftwf_execute(p);	
		
		//FFT is done
		SetEvent(FFTBuffEmpty);

		//if Display Intensity thread is running and IntensityCurveData is needed for display Intensity Curve along ALine
		if (bDisplayIntensity)
		{
			WaitForSingleObject(IntensityBuffEmpty,  INFINITE);
				//MessageBox((HWND)hWnd,"IntensityBuffEmpty timeout", "SDOCT", MB_OK);
			for ( int i = 0; i< ALineNum * IterationNum;i++){
				memcpy(IntensityBuff+i* FFTNUM,FFTResult+i* FFTNUM*2, FFTNUM*4);
			}

			//memcpy(IntensityBuff, FFTResult, RawDataSize*8);

			//Intensity data buff (IntensityBuff) is full and new.
			SetEvent(IntensityBuffNew);
		}		
	}

	fftwf_destroy_plan(p);
	theApp.m_hFFTThread=NULL;

	return 0;
}
