#include "stdafx.h"
#include "niimaq.h"
#include "afxmt.h"
#include <math.h>
#include "OCT.h"
#include "Defines.h"
#include <tiffio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ExternVariables.h"


//HotColor Table for Hotcolor display
int HotColorTable[256]={
	RGB(	0	,	0	,	3	),
	RGB(	0	,	0	,	5	),
	RGB(	0	,	0	,	8	),
	RGB(	0	,	0	,	11	),
	RGB(	0	,	0	,	13	),
	RGB(	0	,	0	,	16	),
	RGB(	0	,	0	,	19	),
	RGB(	0	,	0	,	21	),
	RGB(	0	,	0	,	24	),
	RGB(	0	,	0	,	27	),
	RGB(	0	,	0	,	29	),
	RGB(	0	,	0	,	32	),
	RGB(	0	,	0	,	35	),
	RGB(	0	,	0	,	37	),
	RGB(	0	,	0	,	40	),
	RGB(	0	,	0	,	43	),
	RGB(	0	,	0	,	45	),
	RGB(	0	,	0	,	48	),
	RGB(	0	,	0	,	50	),
	RGB(	0	,	0	,	53	),
	RGB(	0	,	0	,	56	),
	RGB(	0	,	0	,	58	),
	RGB(	0	,	0	,	61	),
	RGB(	0	,	0	,	64	),
	RGB(	0	,	0	,	66	),
	RGB(	0	,	0	,	69	),
	RGB(	0	,	0	,	72	),
	RGB(	0	,	0	,	74	),
	RGB(	0	,	0	,	77	),
	RGB(	0	,	0	,	80	),
	RGB(	0	,	0	,	82	),
	RGB(	0	,	0	,	85	),
	RGB(	0	,	0	,	88	),
	RGB(	0	,	0	,	90	),
	RGB(	0	,	0	,	93	),
	RGB(	0	,	0	,	96	),
	RGB(	0	,	0	,	98	),
	RGB(	0	,	0	,	101	),
	RGB(	0	,	0	,	104	),
	RGB(	0	,	0	,	106	),
	RGB(	0	,	0	,	109	),
	RGB(	0	,	0	,	112	),
	RGB(	0	,	0	,	114	),
	RGB(	0	,	0	,	117	),
	RGB(	0	,	0	,	120	),
	RGB(	0	,	0	,	122	),
	RGB(	0	,	0	,	125	),
	RGB(	0	,	0	,	128	),
	RGB(	0	,	0	,	130	),
	RGB(	0	,	0	,	133	),
	RGB(	0	,	0	,	135	),
	RGB(	0	,	0	,	138	),
	RGB(	0	,	0	,	141	),
	RGB(	0	,	0	,	143	),
	RGB(	0	,	0	,	146	),
	RGB(	0	,	0	,	149	),
	RGB(	0	,	0	,	151	),
	RGB(	0	,	0	,	154	),
	RGB(	0	,	0	,	157	),
	RGB(	0	,	0	,	159	),
	RGB(	0	,	0	,	162	),
	RGB(	0	,	0	,	165	),
	RGB(	0	,	0	,	167	),
	RGB(	0	,	0	,	170	),
	RGB(	0	,	0	,	173	),
	RGB(	0	,	0	,	175	),
	RGB(	0	,	0	,	178	),
	RGB(	0	,	0	,	181	),
	RGB(	0	,	0	,	183	),
	RGB(	0	,	0	,	186	),
	RGB(	0	,	0	,	189	),
	RGB(	0	,	0	,	191	),
	RGB(	0	,	0	,	194	),
	RGB(	0	,	0	,	197	),
	RGB(	0	,	0	,	199	),
	RGB(	0	,	0	,	202	),
	RGB(	0	,	0	,	205	),
	RGB(	0	,	0	,	207	),
	RGB(	0	,	0	,	210	),
	RGB(	0	,	0	,	213	),
	RGB(	0	,	0	,	215	),
	RGB(	0	,	0	,	218	),
	RGB(	0	,	0	,	220	),
	RGB(	0	,	0	,	223	),
	RGB(	0	,	0	,	226	),
	RGB(	0	,	0	,	228	),
	RGB(	0	,	0	,	231	),
	RGB(	0	,	0	,	234	),
	RGB(	0	,	0	,	236	),
	RGB(	0	,	0	,	239	),
	RGB(	0	,	0	,	242	),
	RGB(	0	,	0	,	244	),
	RGB(	0	,	0	,	247	),
	RGB(	0	,	0	,	250	),
	RGB(	0	,	0	,	252	),
	RGB(	0	,	0	,	255	),
	RGB(	0	,	3	,	255	),
	RGB(	0	,	5	,	255	),
	RGB(	0	,	8	,	255	),
	RGB(	0	,	11	,	255	),
	RGB(	0	,	13	,	255	),
	RGB(	0	,	16	,	255	),
	RGB(	0	,	19	,	255	),
	RGB(	0	,	21	,	255	),
	RGB(	0	,	24	,	255	),
	RGB(	0	,	27	,	255	),
	RGB(	0	,	29	,	255	),
	RGB(	0	,	32	,	255	),
	RGB(	0	,	35	,	255	),
	RGB(	0	,	37	,	255	),
	RGB(	0	,	40	,	255	),
	RGB(	0	,	43	,	255	),
	RGB(	0	,	45	,	255	),
	RGB(	0	,	48	,	255	),
	RGB(	0	,	50	,	255	),
	RGB(	0	,	53	,	255	),
	RGB(	0	,	56	,	255	),
	RGB(	0	,	58	,	255	),
	RGB(	0	,	61	,	255	),
	RGB(	0	,	64	,	255	),
	RGB(	0	,	66	,	255	),
	RGB(	0	,	69	,	255	),
	RGB(	0	,	72	,	255	),
	RGB(	0	,	74	,	255	),
	RGB(	0	,	77	,	255	),
	RGB(	0	,	80	,	255	),
	RGB(	0	,	82	,	255	),
	RGB(	0	,	85	,	255	),
	RGB(	0	,	88	,	255	),
	RGB(	0	,	90	,	255	),
	RGB(	0	,	93	,	255	),
	RGB(	0	,	96	,	255	),
	RGB(	0	,	98	,	255	),
	RGB(	0	,	101	,	255	),
	RGB(	0	,	104	,	255	),
	RGB(	0	,	106	,	255	),
	RGB(	0	,	109	,	255	),
	RGB(	0	,	112	,	255	),
	RGB(	0	,	114	,	255	),
	RGB(	0	,	117	,	255	),
	RGB(	0	,	120	,	255	),
	RGB(	0	,	122	,	255	),
	RGB(	0	,	125	,	255	),
	RGB(	0	,	128	,	255	),
	RGB(	0	,	130	,	255	),
	RGB(	0	,	133	,	255	),
	RGB(	0	,	135	,	255	),
	RGB(	0	,	138	,	255	),
	RGB(	0	,	141	,	255	),
	RGB(	0	,	143	,	255	),
	RGB(	0	,	146	,	255	),
	RGB(	0	,	149	,	255	),
	RGB(	0	,	151	,	255	),
	RGB(	0	,	154	,	255	),
	RGB(	0	,	157	,	255	),
	RGB(	0	,	159	,	255	),
	RGB(	0	,	162	,	255	),
	RGB(	0	,	165	,	255	),
	RGB(	0	,	167	,	255	),
	RGB(	0	,	170	,	255	),
	RGB(	0	,	173	,	255	),
	RGB(	0	,	175	,	255	),
	RGB(	0	,	178	,	255	),
	RGB(	0	,	181	,	255	),
	RGB(	0	,	183	,	255	),
	RGB(	0	,	186	,	255	),
	RGB(	0	,	189	,	255	),
	RGB(	0	,	191	,	255	),
	RGB(	0	,	194	,	255	),
	RGB(	0	,	197	,	255	),
	RGB(	0	,	199	,	255	),
	RGB(	0	,	202	,	255	),
	RGB(	0	,	205	,	255	),
	RGB(	0	,	207	,	255	),
	RGB(	0	,	210	,	255	),
	RGB(	0	,	213	,	255	),
	RGB(	0	,	215	,	255	),
	RGB(	0	,	218	,	255	),
	RGB(	0	,	220	,	255	),
	RGB(	0	,	223	,	255	),
	RGB(	0	,	226	,	255	),
	RGB(	0	,	228	,	255	),
	RGB(	0	,	231	,	255	),
	RGB(	0	,	234	,	255	),
	RGB(	0	,	236	,	255	),
	RGB(	0	,	239	,	255	),
	RGB(	0	,	242	,	255	),
	RGB(	0	,	244	,	255	),
	RGB(	0	,	247	,	255	),
	RGB(	0	,	250	,	255	),
	RGB(	0	,	252	,	255	),
	RGB(	0	,	255	,	255	),
	RGB(	4	,	255	,	255	),
	RGB(	8	,	255	,	255	),
	RGB(	12	,	255	,	255	),
	RGB(	16	,	255	,	255	),
	RGB(	20	,	255	,	255	),
	RGB(	24	,	255	,	255	),
	RGB(	28	,	255	,	255	),
	RGB(	32	,	255	,	255	),
	RGB(	36	,	255	,	255	),
	RGB(	40	,	255	,	255	),
	RGB(	44	,	255	,	255	),
	RGB(	48	,	255	,	255	),
	RGB(	52	,	255	,	255	),
	RGB(	56	,	255	,	255	),
	RGB(	60	,	255	,	255	),
	RGB(	64	,	255	,	255	),
	RGB(	68	,	255	,	255	),
	RGB(	72	,	255	,	255	),
	RGB(	76	,	255	,	255	),
	RGB(	80	,	255	,	255	),
	RGB(	84	,	255	,	255	),
	RGB(	88	,	255	,	255	),
	RGB(	92	,	255	,	255	),
	RGB(	96	,	255	,	255	),
	RGB(	100	,	255	,	255	),
	RGB(	104	,	255	,	255	),
	RGB(	108	,	255	,	255	),
	RGB(	112	,	255	,	255	),
	RGB(	116	,	255	,	255	),
	RGB(	120	,	255	,	255	),
	RGB(	124	,	255	,	255	),
	RGB(	128	,	255	,	255	),
	RGB(	131	,	255	,	255	),
	RGB(	135	,	255	,	255	),
	RGB(	139	,	255	,	255	),
	RGB(	143	,	255	,	255	),
	RGB(	147	,	255	,	255	),
	RGB(	151	,	255	,	255	),
	RGB(	155	,	255	,	255	),
	RGB(	159	,	255	,	255	),
	RGB(	163	,	255	,	255	),
	RGB(	167	,	255	,	255	),
	RGB(	171	,	255	,	255	),
	RGB(	175	,	255	,	255	),
	RGB(	179	,	255	,	255	),
	RGB(	183	,	255	,	255	),
	RGB(	187	,	255	,	255	),
	RGB(	191	,	255	,	255	),
	RGB(	195	,	255	,	255	),
	RGB(	199	,	255	,	255	),
	RGB(	203	,	255	,	255	),
	RGB(	207	,	255	,	255	),
	RGB(	211	,	255	,	255	),
	RGB(	215	,	255	,	255	),
	RGB(	219	,	255	,	255	),
	RGB(	223	,	255	,	255	),
	RGB(	227	,	255	,	255	),
	RGB(	231	,	255	,	255	),
	RGB(	235	,	255	,	255	),
	RGB(	239	,	255	,	255	),
	RGB(	243	,	255	,	255	),
	RGB(	247	,	255	,	255	),
	RGB(	251	,	255	,	255	),
	RGB(	255	,	255	,	255	)
};


UINT DisplayIntensityThreadProc(LPVOID hWnd)
{
	//PixValue for per pixel Intensity calculation
	float PixImg, PixReal;
	float PixValue;
	int PixValueArray[10]={0};
	int IntensityBuffOffset;
	//int TotalFrameNum=0;
	int TmpAvgNum=1;
	//TIFF image is used for saving the raw data and reconstructed images.
	TIFF *image;	
	int nTmpIndex = 0;
	int TimeTick1,TimeTick2;//show the frame rate be calculating the time interval between sequential acquisitions
	char FrameRate[32]; 
	char AvgRate[32];

	//before start the thread, set the event for the prior thread to start correctly
	SetEvent(IntensityBuffEmpty);

	while (bDisplayIntensity) {
		//calculate the frame rate, reciprocal of the time interval
		TimeTick1=TimeTick2;
		TimeTick2=GetTickCount();
		if (bImaq) {
			sprintf(FrameRate, "%.2f", 1000.0 /(float)(TimeTick2-TimeTick1));
			SetWindowText(GetDlgItem((HWND)hWnd,IDC_FRAME_RATE), FrameRate);
			SetWindowText(GetDlgItem((HWND)hWnd,IDC_FILE_INDEX), itoa(TmpAvgNum,AvgRate,10));
		}

		WaitForSingleObject(IntensityBuffNew, INFINITE);
		//MessageBox( (HWND)hWnd,"IntensityBuffNew timeout", "SDOCT", MB_OK);

		for (int i = 0; i<ALineNum ;i++){
			IntensityBuffOffset=i* IterationNum;
			for (int j = UpDepthIndex; j<= DownDepthIndex; j++){ 
				PixValue=0;
				for (int n = 0; n < IterationNum;n++) {
					////get the real and image part and do the sqrt to get the Intensity per pixel
					//PixReal=*(IntensityBuff + (i* IterationNum+n) * FFTNUM+j);
					//PixImg =*(IntensityBuff + (i* IterationNum+n+1) * FFTNUM-j);

					// get the real image by the result from complex to complex transform
					PixReal=*(IntensityBuff + (IntensityBuffOffset+n) * FFTNUM+j*2);
					PixImg =*(IntensityBuff + (IntensityBuffOffset+n) * FFTNUM+j*2+1);
					PixValue= PixValue+ IntensityGain * sqrt(PixReal*PixReal+PixImg*PixImg);	

				}
				if (bLog && PixValue)
					PixValue= 20* log10((float) PixValue);

				PixValue=PixValue - OffSet+50;
				PixValue=PixValue> 255 ? 255 : PixValue;
				PixValue=PixValue< 0 ? 0 : PixValue;

				//dealing saturation, set maximum 255 for display
				if (1==TmpAvgNum) {				
					*(IntensityImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) = (int)PixValue;
					if (bHotColor)
						*(IntensityImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  HotColorTable[int(PixValue)];
				}

				else {
					*(IntensityImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) = (PixValue + (*(IntensityImage+(i + LeftLateralIndex)*IMAGEWIDTH+j))*(TmpAvgNum-1))/TmpAvgNum;
					if (bHotColor)
						*(IntensityImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  HotColorTable[*(IntensityImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) ];
				}
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

		//if DrawCurve thread is running and IntensityCurveData is needed for display Intensity Curve along ALine
		if (bDrawCurve || bAxialIntensity)
			memcpy(IntensityCurveData, IntensityImage+(AxialLine_i+LeftLateralIndex)*IMAGEWIDTH,  IMAGEWIDTH);

		//copy IntensityImage to PhaseBuff for Intensity weighted Phase display
		if (bDisplayPhase) {
			WaitForSingleObject(PhaseBuffEmpty,  INFINITE);
			//MessageBox((HWND)hWnd,"PhaseBuffEmpty timeout", "SDOCT", MB_OK);

			memcpy(PhaseBuff, IntensityBuff,  FrameSize*8);
			memcpy(PhaseWeightBuff, IntensityImage, IMAGESIZE);

			//Phase data buff (PhaseBuff and PhaseWeightBuff) is full and new.
			SetEvent(PhaseBuffNew);
		}
		//Intensity image reconstruction is done
		SetEvent(IntensityBuffEmpty);

		//display the image
		if ( bHotColor ){
			if (bZoom)
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_INTENSITY_IMAGE_WINDOW), IntensityImageRGB, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_COLOR_RGB32);
			else
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_INTENSITY_IMAGE_WINDOW), IntensitySmallImageRGB, 0 , 0, IMAGEHALFWIDTH,IMAGEHALFHEIGHT , 0,0,  IMGPLOT_COLOR_RGB32);
		}
		else {
			if (bZoom)
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_INTENSITY_IMAGE_WINDOW), IntensityImage, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_MONO_8);
			else
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_INTENSITY_IMAGE_WINDOW), IntensitySmallImage, 0 , 0, IMAGEHALFWIDTH,IMAGEHALFHEIGHT, 0,0,  IMGPLOT_MONO_8);
		}

		//TotalFrameNum++;

		//if continuous acquisition and autosave then reconstructed images
		if ( bAutosave&& strPathNameArray.GetSize() ) {
			//set the parameters of TIFF images for saving images .
			image = TIFFOpen(strPathNameArray[nTmpIndex]+ "Structure.TIF", "w");
			TIFFSetField(image, TIFFTAG_IMAGEWIDTH, IMAGEWIDTH);
			TIFFSetField(image, TIFFTAG_IMAGELENGTH, IMAGEHEIGHT);
			TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
			TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
			TIFFSetField(image, TIFFTAG_COMPRESSION, 1);
			TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
			//save the image using TIFF format
			TIFFWriteEncodedStrip(image, 0, IntensityImage, IMAGESIZE);
			TIFFClose(image);	
			nTmpIndex++;
			if (strPathNameArray.GetSize() == nTmpIndex)
				nTmpIndex=0;				
		}
		TmpAvgNum++;
		if (TmpAvgNum>AvgNum) 
			TmpAvgNum=1;
	}

	theApp.m_hDisplayIntensityThread=NULL;
	return 0;
}
