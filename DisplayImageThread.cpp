
#include "stdafx.h"
#include "NIImaq.h"
#include "afxmt.h"

#define EVENT HANDLE

extern EVENT ImageDataReady;

extern UINT16 AxialLine_i;
extern CCriticalSection Section;

extern BOOL bDisplayImage;
//extern UINT16 IntensityBuff[IMG_SIZE];
extern float FFTResult[IMG_SIZE + 2* IMG_WIDTH];

UINT DisplayImageThreadProc(LPVOID hParentWnd)
{

	//INT16 * pCaliBuff;
	//pCaliBuff = CaliBuff;
	//INT16 tmpIntensityLine[IMG_WIDTH];
	//INT16 tmpPhaseLine[IMG_WIDTH];
	while (bDisplayImage) 
	{
		WaitForSingleObject(ImageDataReady,INFINITE);
		Section.Lock();
		FFTResult[1]=0;
		//display Intensity if Intensity Window Exist
		//Display Phase Image if Phase window Exist
		//if (WAIT_OBJECT_0==WaitForSingleObject(IntensityBuffFull,500))
		//	Sleep(50);
		Section.Unlock();

		//display phase
	}
	return 0;
}