#include "stdafx.h"
#include "niimaq.h"
#include "afxmt.h"
#include <math.h>
#include "OCT.h"
#include <tiffio.h>
#include "Defines.h"
#include "ExternVariables.h"


int JetColorTable[256]={	
		RGB(	251	,	255	,	255	),
		RGB(	243	,	255	,	255	),
		RGB(	235	,	255	,	255	),
		RGB(	227	,	255	,	255	),
		RGB(	219	,	255	,	255	),
		RGB(	211	,	255	,	255	),
		RGB(	203	,	255	,	255	),
		RGB(	195	,	255	,	255	),
		RGB(	187	,	255	,	255	),
		RGB(	179	,	255	,	255	),
		RGB(	171	,	255	,	255	),
		RGB(	163	,	255	,	255	),
		RGB(	155	,	255	,	255	),
		RGB(	147	,	255	,	255	),
		RGB(	139	,	255	,	255	),
		RGB(	131	,	255	,	255	),
		RGB(	124	,	255	,	255	),
		RGB(	116	,	255	,	255	),
		RGB(	108	,	255	,	255	),
		RGB(	100	,	255	,	255	),
		RGB(	92	,	255	,	255	),
		RGB(	84	,	255	,	255	),
		RGB(	76	,	255	,	255	),
		RGB(	68	,	255	,	255	),
		RGB(	60	,	255	,	255	),
		RGB(	52	,	255	,	255	),
		RGB(	44	,	255	,	255	),
		RGB(	36	,	255	,	255	),
		RGB(	28	,	255	,	255	),
		RGB(	20	,	255	,	255	),
		RGB(	12	,	255	,	255	),
		RGB(	4	,	255	,	255	),
		RGB(	0	,	252	,	255	),
		RGB(	0	,	247	,	255	),
		RGB(	0	,	242	,	255	),
		RGB(	0	,	236	,	255	),
		RGB(	0	,	231	,	255	),
		RGB(	0	,	226	,	255	),
		RGB(	0	,	220	,	255	),
		RGB(	0	,	215	,	255	),
		RGB(	0	,	210	,	255	),
		RGB(	0	,	205	,	255	),
		RGB(	0	,	199	,	255	),
		RGB(	0	,	194	,	255	),
		RGB(	0	,	189	,	255	),
		RGB(	0	,	183	,	255	),
		RGB(	0	,	178	,	255	),
		RGB(	0	,	173	,	255	),
		RGB(	0	,	167	,	255	),
		RGB(	0	,	162	,	255	),
		RGB(	0	,	157	,	255	),
		RGB(	0	,	151	,	255	),
		RGB(	0	,	146	,	255	),
		RGB(	0	,	141	,	255	),
		RGB(	0	,	135	,	255	),
		RGB(	0	,	130	,	255	),
		RGB(	0	,	125	,	255	),
		RGB(	0	,	120	,	255	),
		RGB(	0	,	114	,	255	),
		RGB(	0	,	109	,	255	),
		RGB(	0	,	104	,	255	),
		RGB(	0	,	98	,	255	),
		RGB(	0	,	93	,	255	),
		RGB(	0	,	88	,	255	),
		RGB(	0	,	82	,	255	),
		RGB(	0	,	77	,	255	),
		RGB(	0	,	72	,	255	),
		RGB(	0	,	66	,	255	),
		RGB(	0	,	61	,	255	),
		RGB(	0	,	56	,	255	),
		RGB(	0	,	50	,	255	),
		RGB(	0	,	45	,	255	),
		RGB(	0	,	40	,	255	),
		RGB(	0	,	35	,	255	),
		RGB(	0	,	29	,	255	),
		RGB(	0	,	24	,	255	),
		RGB(	0	,	19	,	255	),
		RGB(	0	,	13	,	255	),
		RGB(	0	,	8	,	255	),
		RGB(	0	,	3	,	255	),
		RGB(	0	,	0	,	252	),
		RGB(	0	,	0	,	247	),
		RGB(	0	,	0	,	242	),
		RGB(	0	,	0	,	236	),
		RGB(	0	,	0	,	231	),
		RGB(	0	,	0	,	226	),
		RGB(	0	,	0	,	220	),
		RGB(	0	,	0	,	215	),
		RGB(	0	,	0	,	210	),
		RGB(	0	,	0	,	205	),
		RGB(	0	,	0	,	199	),
		RGB(	0	,	0	,	194	),
		RGB(	0	,	0	,	189	),
		RGB(	0	,	0	,	183	),
		RGB(	0	,	0	,	178	),
		RGB(	0	,	0	,	173	),
		RGB(	0	,	0	,	167	),
		RGB(	0	,	0	,	162	),
		RGB(	0	,	0	,	157	),
		RGB(	0	,	0	,	151	),
		RGB(	0	,	0	,	146	),
		RGB(	0	,	0	,	141	),
		RGB(	0	,	0	,	135	),
		RGB(	0	,	0	,	130	),
		RGB(	0	,	0	,	125	),
		RGB(	0	,	0	,	120	),
		RGB(	0	,	0	,	114	),
		RGB(	0	,	0	,	109	),
		RGB(	0	,	0	,	104	),
		RGB(	0	,	0	,	98	),
		RGB(	0	,	0	,	93	),
		RGB(	0	,	0	,	88	),
		RGB(	0	,	0	,	82	),
		RGB(	0	,	0	,	77	),
		RGB(	0	,	0	,	72	),
		RGB(	0	,	0	,	66	),
		RGB(	0	,	0	,	61	),
		RGB(	0	,	0	,	56	),
		RGB(	0	,	0	,	50	),
		RGB(	0	,	0	,	45	),
		RGB(	0	,	0	,	40	),
		RGB(	0	,	0	,	35	),
		RGB(	0	,	0	,	29	),
		RGB(	0	,	0	,	24	),
		RGB(	0	,	0	,	19	),
		RGB(	0	,	0	,	13	),
		RGB(	0	,	0	,	8	),
		RGB(	0	,	0	,	3	),
		RGB(	3	,	0	,	0	),
		RGB(	8	,	0	,	0	),
		RGB(	13	,	0	,	0	),
		RGB(	19	,	0	,	0	),
		RGB(	24	,	0	,	0	),
		RGB(	29	,	0	,	0	),
		RGB(	35	,	0	,	0	),
		RGB(	40	,	0	,	0	),
		RGB(	45	,	0	,	0	),
		RGB(	50	,	0	,	0	),
		RGB(	56	,	0	,	0	),
		RGB(	61	,	0	,	0	),
		RGB(	66	,	0	,	0	),
		RGB(	72	,	0	,	0	),
		RGB(	77	,	0	,	0	),
		RGB(	82	,	0	,	0	),
		RGB(	88	,	0	,	0	),
		RGB(	93	,	0	,	0	),
		RGB(	98	,	0	,	0	),
		RGB(	104	,	0	,	0	),
		RGB(	109	,	0	,	0	),
		RGB(	114	,	0	,	0	),
		RGB(	120	,	0	,	0	),
		RGB(	125	,	0	,	0	),
		RGB(	130	,	0	,	0	),
		RGB(	135	,	0	,	0	),
		RGB(	141	,	0	,	0	),
		RGB(	146	,	0	,	0	),
		RGB(	151	,	0	,	0	),
		RGB(	157	,	0	,	0	),
		RGB(	162	,	0	,	0	),
		RGB(	167	,	0	,	0	),
		RGB(	173	,	0	,	0	),
		RGB(	178	,	0	,	0	),
		RGB(	183	,	0	,	0	),
		RGB(	189	,	0	,	0	),
		RGB(	194	,	0	,	0	),
		RGB(	199	,	0	,	0	),
		RGB(	205	,	0	,	0	),
		RGB(	210	,	0	,	0	),
		RGB(	215	,	0	,	0	),
		RGB(	220	,	0	,	0	),
		RGB(	226	,	0	,	0	),
		RGB(	231	,	0	,	0	),
		RGB(	236	,	0	,	0	),
		RGB(	242	,	0	,	0	),
		RGB(	247	,	0	,	0	),
		RGB(	252	,	0	,	0	),
		RGB(	255	,	3	,	0	),
		RGB(	255	,	8	,	0	),
		RGB(	255	,	13	,	0	),
		RGB(	255	,	19	,	0	),
		RGB(	255	,	24	,	0	),
		RGB(	255	,	29	,	0	),
		RGB(	255	,	35	,	0	),
		RGB(	255	,	40	,	0	),
		RGB(	255	,	45	,	0	),
		RGB(	255	,	50	,	0	),
		RGB(	255	,	56	,	0	),
		RGB(	255	,	61	,	0	),
		RGB(	255	,	66	,	0	),
		RGB(	255	,	72	,	0	),
		RGB(	255	,	77	,	0	),
		RGB(	255	,	82	,	0	),
		RGB(	255	,	88	,	0	),
		RGB(	255	,	93	,	0	),
		RGB(	255	,	98	,	0	),
		RGB(	255	,	104	,	0	),
		RGB(	255	,	109	,	0	),
		RGB(	255	,	114	,	0	),
		RGB(	255	,	120	,	0	),
		RGB(	255	,	125	,	0	),
		RGB(	255	,	130	,	0	),
		RGB(	255	,	135	,	0	),
		RGB(	255	,	141	,	0	),
		RGB(	255	,	146	,	0	),
		RGB(	255	,	151	,	0	),
		RGB(	255	,	157	,	0	),
		RGB(	255	,	162	,	0	),
		RGB(	255	,	167	,	0	),
		RGB(	255	,	173	,	0	),
		RGB(	255	,	178	,	0	),
		RGB(	255	,	183	,	0	),
		RGB(	255	,	189	,	0	),
		RGB(	255	,	194	,	0	),
		RGB(	255	,	199	,	0	),
		RGB(	255	,	205	,	0	),
		RGB(	255	,	210	,	0	),
		RGB(	255	,	215	,	0	),
		RGB(	255	,	220	,	0	),
		RGB(	255	,	226	,	0	),
		RGB(	255	,	231	,	0	),
		RGB(	255	,	236	,	0	),
		RGB(	255	,	242	,	0	),
		RGB(	255	,	247	,	0	),
		RGB(	255	,	252	,	0	),
		RGB(	255	,	255	,	4	),
		RGB(	255	,	255	,	12	),
		RGB(	255	,	255	,	20	),
		RGB(	255	,	255	,	28	),
		RGB(	255	,	255	,	36	),
		RGB(	255	,	255	,	44	),
		RGB(	255	,	255	,	52	),
		RGB(	255	,	255	,	60	),
		RGB(	255	,	255	,	68	),
		RGB(	255	,	255	,	76	),
		RGB(	255	,	255	,	84	),
		RGB(	255	,	255	,	92	),
		RGB(	255	,	255	,	100	),
		RGB(	255	,	255	,	108	),
		RGB(	255	,	255	,	116	),
		RGB(	255	,	255	,	124	),
		RGB(	255	,	255	,	131	),
		RGB(	255	,	255	,	139	),
		RGB(	255	,	255	,	147	),
		RGB(	255	,	255	,	155	),
		RGB(	255	,	255	,	163	),
		RGB(	255	,	255	,	171	),
		RGB(	255	,	255	,	179	),
		RGB(	255	,	255	,	187	),
		RGB(	255	,	255	,	195	),
		RGB(	255	,	255	,	203	),
		RGB(	255	,	255	,	211	),
		RGB(	255	,	255	,	219	),
		RGB(	255	,	255	,	227	),
		RGB(	255	,	255	,	235	),
		RGB(	255	,	255	,	243	),
		RGB(	255	,	255	,	251	)
};


UINT DisplayPhaseThreadProc(LPVOID hWnd)
{
	float PixPhaseSum;
	int PixPhase;
	float PixImg1, PixReal1,PixImg2,PixReal2,PixImg,PixReal;
	TIFF *image;	
	int nTmpIndex = 0;
	int PhaseALineNum=ALineNum-1;

	//BOOL bZoom;

	SetEvent(PhaseBuffEmpty);
	while (bDisplayPhase) 	{			
		//if zoom the imaging window

		WaitForSingleObject(PhaseBuffNew,  INFINITE);
		//MessageBox((HWND)hWnd,"PhaseBuffNew timeout", "SDOCT", MB_OK);
		if (1<IterationNum) {
			for (int i = 0; i< ALineNum;i++){
				for (int j = UpDepthIndex; j< DownDepthIndex; j++){ 
					if (*(PhaseWeightBuff+(i + LeftLateralIndex)*IMAGEWIDTH+j) > PhaseThresh ) {
						PixPhaseSum=0;				
						for (int n = 0; n < (IterationNum-1);n++) {
							PixReal1=*(PhaseBuff + (i* IterationNum+n) * FFTNUM+j*2);
							PixImg1 =*(PhaseBuff + (i* IterationNum+n) * FFTNUM+j*2+1);
							PixReal2=*(PhaseBuff + (i* IterationNum+n+1) * FFTNUM+j*2);
							PixImg2 =*(PhaseBuff + (i* IterationNum+n+1) * FFTNUM+j*2+1);
							PixReal  = PixReal1*PixReal2+PixImg1*PixImg2;
							PixImg = PixImg1*PixReal2-PixReal1*PixImg2;
							PixPhaseSum=PixPhaseSum+atan(PixImg/PixReal);
						}
						PixPhase=(PixPhaseSum/IterationNum+PI)*40.5845;
						if( PixImg>0 && PixReal<0)
							PixPhase+=127;
						if (PixImg<0 && PixReal<0)
							PixPhase-=127;
						*(PhaseImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) = PixPhase;
						*(PhaseImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  JetColorTable[PixPhase];
					}

					else {
						*(PhaseImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) = 127;
						*(PhaseImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  JetColorTable[127];
					}
				}				
			}
		}
		else{
			for (int i = 0; i< PhaseALineNum;i++){
				for (int j = UpDepthIndex; j< DownDepthIndex; j++){ 
					if (*(PhaseWeightBuff+(i + LeftLateralIndex)*IMAGEWIDTH+j) > PhaseThresh ) {						
						PixReal1=*(PhaseBuff + i * FFTNUM+j*2);
						PixImg1 =*(PhaseBuff + i * FFTNUM+j*2+1);
						PixReal2=*(PhaseBuff + (i+1) * FFTNUM+j*2);
						PixImg2 =*(PhaseBuff + (i+1) * FFTNUM+j*2+1);
						PixReal  = PixReal1*PixReal2+PixImg1*PixImg2;
						PixImg = PixImg1*PixReal2-PixReal1*PixImg2;

						PixPhase=(atan(PixImg/PixReal)+PI)*40.5845;//40.5845*2PI=255
						if( PixImg>0 && PixReal<0)
							PixPhase+=127;
						if (PixImg<0 && PixReal<0)
							PixPhase-=127;

						*(PhaseImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) = PixPhase;
						*(PhaseImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  JetColorTable[PixPhase];
					}

					else {
						*(PhaseImage+(i + LeftLateralIndex)*IMAGEWIDTH+j) = 127;
						*(PhaseImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j) =  JetColorTable[127];
					}
				}
			}

		}
	
		//TotalFrameNum++;

		if (!bZoom) {
			for (int i = 0; i<ALineNum ;i=i+2){
				for (int j = UpDepthIndex; j<= DownDepthIndex; j=j+2){ 
					*(PhaseSmallImage+(i + LeftLateralIndex)/2*IMAGEHALFWIDTH+j/2) = *(PhaseImage+(i + LeftLateralIndex)*IMAGEWIDTH+j);
					//if hot color enabled, get the image array for hot color
					if (bHotColor)
						*(PhaseSmallImageRGB+(i + LeftLateralIndex)/2*IMAGEHALFWIDTH+j/2) =  *(PhaseImageRGB+(i + LeftLateralIndex)*IMAGEWIDTH+j);
				}
			}
		}



		if (bDrawCurve || bAxialPhase)
			memcpy(PhaseCurveData, PhaseImage + (AxialLine_i+LeftLateralIndex)*IMAGEWIDTH, IMAGEWIDTH);

		SetEvent(PhaseBuffEmpty);

		if (bHotColor) {
			if (bZoom)
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_PHASE_IMAGE_WINDOW), PhaseImageRGB, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_COLOR_RGB32);
			else
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_PHASE_IMAGE_WINDOW), PhaseSmallImageRGB, 0 , 0, IMAGEHALFWIDTH,IMAGEHALFHEIGHT , 0,0,  IMGPLOT_COLOR_RGB32);
		}

		else {
			if (bZoom)
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_PHASE_IMAGE_WINDOW), PhaseImage, 0 , 0, IMAGEWIDTH,IMAGEHEIGHT, 0,0,  IMGPLOT_MONO_8);
			else
				imgPlot((GUIHNDL)GetDlgItem((HWND)hWnd,ID_PHASE_IMAGE_WINDOW), PhaseSmallImage, 0 , 0, IMAGEHALFWIDTH,IMAGEHALFHEIGHT , 0,0,  IMGPLOT_MONO_8);
		}

		if ( bAutosave && strPathNameArray.GetSize() ) {			
			//set the parameters of TIFF images for saving images .
			image = TIFFOpen(strPathNameArray[nIndex-1]+"Phase.TIF", "w");
			TIFFSetField(image, TIFFTAG_IMAGEWIDTH, IMAGEWIDTH);
			TIFFSetField(image, TIFFTAG_IMAGELENGTH, IMAGEHEIGHT);
			TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
			TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
			TIFFSetField(image, TIFFTAG_COMPRESSION, 1);
			TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
			//save the image using TIFF format
			TIFFWriteEncodedStrip(image, 0, PhaseImage, IMAGESIZE);
			TIFFClose(image);	
			nTmpIndex++;
			if (strPathNameArray.GetSize() == nTmpIndex)
				nTmpIndex=0;
		}
	}
	theApp.m_hDisplayPhaseThread=NULL;
	return 0;
}

 