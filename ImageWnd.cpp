#include "stdafx.h"
#include "OCT.h"
#include "ImageWnd.h"
#include "OCTDlg.h"
#include "IntensityDlg.h"
#include "ExternVariables.h"
#include <math.h>

IMPLEMENT_DYNAMIC(CImageWnd, CWnd)

CImageWnd::CImageWnd()
{
}

CImageWnd::~CImageWnd()
{	
}


BEGIN_MESSAGE_MAP(CImageWnd, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CImageWnd::OnLButtonDown(UINT nFlags,CPoint point)
{
	//get the AxialLine_i when mouse down.
	CWnd::OnLButtonDown(nFlags,point);
	tmpVMinX=voltageX;
	tmpVMinY=voltageY;
	if (bZoom) {
		StartPoint.x=point.x;
		StartPoint.y=point.y;
	}
	else {
		StartPoint.x=point.x>511?511:point.x;
		StartPoint.y=point.y>499?499:point.y;
	}
	if (bImaq && bDisplayIntensity && !bAOIEn){
		CPen ALinePen;
		CDC * pDC=GetDC();
		ALinePen.CreatePen(PS_SOLID,1,RGB(255,0,0));
		pDC->SelectObject(&ALinePen);	
		pDC->MoveTo(0,point.y);
		if (bZoom){
			pDC->LineTo(IMAGEWIDTH,point.y);
			int tmpALine_i = (StartPoint.y-LeftLateralIndex)<0 ? 0 : (StartPoint.y-LeftLateralIndex) ;
			AxialLine_i = (tmpALine_i >(ALineNum-1)) ? (ALineNum-1) : tmpALine_i ;
		}
		else{
			pDC->LineTo(IMAGEHALFWIDTH,point.y);
			int tmpALine_i = (StartPoint.y*2-LeftLateralIndex)<0 ? 0 : (StartPoint.y*2-LeftLateralIndex) ;
			AxialLine_i = (tmpALine_i >(ALineNum-1)) ? (ALineNum-1) : tmpALine_i ;
		}
	}
}

void CImageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	//draw the rectangle to circumvent the Area of Interests
	CWnd::OnMouseMove(nFlags, point);
	if (bZoom){	
		if (bDisplayIntensity)
			intensity = *(IntensityImage + point.y * IMAGEWIDTH+ point.x);
		if (bDisplayPhase)
			phase = *(PhaseImage + point.y * IMAGEWIDTH + point.x);

		voltageX = ScanVMinX + (point.y-LeftLateralIndex)  * (ScanVMaxX-ScanVMinX)/(ALineNum-1);
		voltageY = ScanVMinY + (point.y-LeftLateralIndex)  * (ScanVMaxY-ScanVMinY)/(ALineNum-1);
		depth = point.x*DEPTHFACTOR;
	}
	else{
		if (bDisplayIntensity)
			intensity = *(IntensitySmallImage + (point.y>499?499:point.y) * IMAGEHALFWIDTH +(point.x>511?511:point.x));
		if (bDisplayPhase)
			phase = *(PhaseSmallImage +(point.y>499?499:point.y) * IMAGEHALFWIDTH +(point.x>511?511:point.x));

		voltageX = ScanVMinX + ((point.y>499?499:point.y)*2-LeftLateralIndex)  * (ScanVMaxX-ScanVMinX)/(ALineNum-1);
		voltageY = ScanVMinY + ((point.y>499?499:point.y)*2-LeftLateralIndex)  * (ScanVMaxY-ScanVMinY)/(ALineNum-1);

		depth = point.x*DEPTHFACTOR*2;

	}

	if (COCTDlg::bIntensityDlgEn)
		pMainDlg->m_pIntensityDlg->UpdateData(FALSE);

	if (COCTDlg::bPhaseDlgEn)
		pMainDlg->m_pPhaseDlg->UpdateData(FALSE);

	if ( (nFlags & WM_LBUTTONDOWN ) && bAOIEn && bImaq && bDisplayIntensity ){
		CPen AreaPen;
		CDC * pDC=GetDC();
		AreaPen.CreatePen(PS_DOT,1,RGB(255,255,255));
		pDC->SelectObject(AreaPen);
		if (bZoom) {
			pDC->MoveTo(StartPoint.x,StartPoint.y);
			pDC->LineTo(StartPoint.x,point.y);
			pDC->LineTo(point.x,point.y);
			pDC->LineTo(point.x,StartPoint.y);
			pDC->LineTo(StartPoint.x,StartPoint.y);
		}
		else {
			pDC->MoveTo(StartPoint.x,StartPoint.y);
			pDC->LineTo(StartPoint.x,(point.y>499?499:point.y));
			pDC->LineTo((point.x>511?511:point.x),(point.y>499?499:point.y));
			pDC->LineTo((point.x>511?511:point.x),StartPoint.y);
			pDC->LineTo(StartPoint.x,StartPoint.y);
		}
	}

}


void CImageWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	//reconfigure the whole image arrays and threads
	CWnd::OnLButtonUp(nFlags,point);

	if (bAOIEn) {
		if (bZoom){
			EndPoint.x=point.x;
			EndPoint.y=point.y;
			UpDepthIndex = min(StartPoint.x ,EndPoint.x );
			DownDepthIndex = max(StartPoint.x ,EndPoint.x );
			pMainDlg->m_ALineNum=abs(EndPoint.y-StartPoint.y);
		}
		else {
			EndPoint.x=point.x>511?511:point.x;
			EndPoint.y=point.y>499?499:point.y;
			UpDepthIndex = min(StartPoint.x*2 ,EndPoint.x*2 );
			DownDepthIndex = max(StartPoint.x*2 ,EndPoint.x*2 );
			pMainDlg->m_ALineNum=abs(EndPoint.y-StartPoint.y)*2;
		}
		//show the pixel information.
		ScanVMaxX = max(voltageX,tmpVMinX);
		ScanVMinY = min(voltageY,tmpVMinY);

		//if bZoon, reset ALineNum=1000 to magnify the image laterally
		if (bExpand) 
			pMainDlg->m_ALineNum=IMAGEHEIGHT;

		pMainDlg->ReconfigureScan();

		//reset Dialog parameters.
		bAOIEn=FALSE;
		pMainDlg->m_pIntensityDlg->UpdateData(FALSE);

	}
}

