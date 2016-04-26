#include "stdafx.h"
#include "OCT.h"
#include "CurveDlg.h"
#include "ddraw.h"
#include "Defines.h"
#include "ExternVariables.h"

static RECT * lpRect;

void DrawGrid(HDC hdc,RECT* lpRect)
{
	HPEN hpen=CreatePen(PS_DOT,1,RGB(192,192,192));
	HBRUSH hbrush=CreateSolidBrush(RGB(255,255,255));

	//set the background of the curve area(client rect) white
	FillRect(hdc, lpRect,hbrush);
	SelectObject(hdc, hpen);

	// grid in horizontal
	for(int i = 1; i < 10; i++){
		MoveToEx(hdc, lpRect->left, lpRect->top + i * 255 / 10,NULL);
		LineTo(hdc, lpRect->right, lpRect->top + i * 255 / 10);
	}

	// grid in vertical
	for(int k = 1; k < 10; k++)	{
		MoveToEx(hdc,lpRect->left + IMAGEWIDTH * k / 10, lpRect->bottom,NULL);
		LineTo(hdc,lpRect->left + IMAGEWIDTH * k / 10, lpRect->top);
	}
	DeleteObject(hpen);
	DeleteObject(hbrush);
}

UINT DrawCurveThreadProc(LPVOID hWnd)
{
	//initiate and create DDraw object;
	LPDIRECTDRAW7 lpDDraw = NULL;
	LPDIRECTDRAWSURFACE7 lpDDSPrimary = NULL;
	LPDIRECTDRAWSURFACE7 lpDDSBack = NULL;
	LPDIRECTDRAWSURFACE7 lpDDSGrid = NULL;
	LPDIRECTDRAWCLIPPER lpClipper = NULL;
	
	//create DDRAW objects
	DirectDrawCreateEx(NULL, (VOID**)&lpDDraw, IID_IDirectDraw7, NULL);
	lpDDraw->SetCooperativeLevel(GetDlgItem((HWND) hWnd, ID_CURVE_IMAGE),DDSCL_NORMAL);
	
	//get curve area(Client Rect)
	lpRect = new RECT;
	GetClientRect(GetDlgItem((HWND) hWnd, ID_CURVE_IMAGE),lpRect);
	
	//set up the DDRAW primary surface
	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof( ddsd ) );
	ddsd.dwSize         = sizeof( ddsd );
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	lpDDraw->CreateSurface(&ddsd, &lpDDSPrimary, NULL); 

	//setup parameters for the primary surface
	ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth        = IMAGEWIDTH;
	ddsd.dwHeight       = 255;

	//create back surface
	lpDDraw->CreateSurface(&ddsd, &lpDDSBack, NULL);
	//create Grid Surface
	lpDDraw->CreateSurface(&ddsd, &lpDDSGrid, NULL);
	//create Clipper
	lpDDraw->CreateClipper(0, &lpClipper, NULL);
	lpClipper->SetHWnd(0,GetDlgItem((HWND) hWnd, ID_CURVE_IMAGE));
	lpDDSPrimary->SetClipper(lpClipper);

	//draw grid to Grid surface
	HDC hdc;
	lpDDSGrid->GetDC(&hdc);
	DrawGrid(hdc, lpRect);
	lpDDSGrid->ReleaseDC(hdc);

	//initiates the DCs for drawing curves.
	UINT px,py;
	POINT p={0,0};
	HPEN hRawSpectrumPen=CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN hCaliedSpectrumPen=CreatePen(PS_SOLID,1,RGB(255,0,0));
	HPEN hAxialIntensityPen=CreatePen(PS_SOLID,1,RGB(0,255,0));
	HPEN hAxialPhasePen=CreatePen(PS_SOLID,1,RGB(0,0,255));
	HPEN hPowerPen=CreatePen(PS_SOLID,1,RGB(128,128,128));

	//draw the curves.
	while (bDrawCurve)
	{	
		//blit the grids to the Back surface.
		lpDDSBack->Blt(NULL, lpDDSGrid, NULL, DDBLT_WAIT, NULL);	
		//get DC to start the drawing of the curves in back surface.
		lpDDSBack->GetDC(&hdc);

		if (bRawSpectrum){
			SelectObject(hdc, hRawSpectrumPen);
			MoveToEx(hdc,0,256-(*RawSpectrumCurveData)*SPECTRUMGAIN,NULL);
			for(px = 0; px < IMAGEWIDTH; px++)		{	
				py=256-(*(RawSpectrumCurveData+px*SPECTRUMSTEP)*SPECTRUMGAIN);
				LineTo(hdc,px,py);
			}
		}

		if (bCaliedSpectrum){
			SelectObject(hdc, hCaliedSpectrumPen);
			MoveToEx(hdc,0,128-(int)(*CaliedSpectrumCurveData*SPECTRUMGAIN),NULL);
			for(px = 0; px < IMAGEWIDTH ; px++)		{	
				py=128-(int)(*(CaliedSpectrumCurveData+px*SPECTRUMSTEP)*SPECTRUMGAIN);
				LineTo(hdc,px,py);
			}
		}

		if (bAxialIntensity){
			SelectObject(hdc, hAxialIntensityPen);
			MoveToEx(hdc,0,256-(*IntensityCurveData),NULL);
			for(px = 1; px <IMAGEWIDTH ; px++)	{	
				py=256-*(IntensityCurveData+px);
				LineTo(hdc,px,py);
			}
		}
		
		if (bAxialPhase) {
			SelectObject(hdc, hAxialPhasePen);
			MoveToEx(hdc,0,256-(*PhaseCurveData),NULL);
			for(px = 1; px < IMAGEWIDTH ; px++)	{	
				py=256-*(PhaseCurveData+px);
				LineTo(hdc,px,py);
			}
		}			
		
		if (bPower) {
			SelectObject(hdc, hPowerPen);
			MoveToEx(hdc,0,256-PowerData[0]*4/SPECTRUMWIDTH*SPECTRUMGAIN,NULL);
			for(px = 1; px < 1000 ; px++)	{	
				py=256-PowerData[px]*4/SPECTRUMWIDTH*SPECTRUMGAIN;
				LineTo(hdc,px,py);
			}
		}

		//done with drawing curves, release all the DC.
		lpDDSBack->ReleaseDC(hdc);

		//get the position of the curve display area from client to the whole screen
		p.x=0;p.y=0;
		ClientToScreen(GetDlgItem((HWND) hWnd, ID_CURVE_IMAGE),&p);
		lpRect->left = p.x; lpRect->top = p.y; 
		lpRect->right = IMAGEWIDTH + p.x; lpRect->bottom = 256 + p.y;

		//blit the image from DDRAW buffer to the front screen.
		lpDDSPrimary->Blt(lpRect,lpDDSBack, NULL, DDBLT_WAIT, NULL);
		//Sleep(100);
	}

	//release resources
	delete lpRect;
	lpRect = NULL;
	DeleteObject(hRawSpectrumPen);
	DeleteObject(hCaliedSpectrumPen);
	DeleteObject(hAxialIntensityPen);
	DeleteObject(hAxialPhasePen);

	lpClipper->Release();
	lpDDSGrid->Release();
	lpDDSBack->Release();
	lpDDSPrimary->Release();
	lpDDraw->Release();	

	theApp.m_hDrawCurveThread=NULL;
	return 0;
} 


