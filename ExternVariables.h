#ifndef EXTERNVARIABLES_H
#define EXTERNVARIABLES_H
#include "Defines.h"
#include "OCT.h"
#include "OCTDlg.h"

//the pointer to the main dialog for SDOCT 
extern COCTDlg * pMainDlg;

//configurations for camera 
extern int		CamGain;
extern int		IntTime;
extern int		ScanFreq;

//configuration for scanner
extern double	ScanVMaxX,ScanVMaxY;
extern double	ScanVMinX,ScanVMinY;



//Acquisition parameters
extern int		IterationNum;
extern int		ALineNum;
extern BOOL		bScanDirection;
extern int		RawDataSize;
extern int		FrameSize;
extern BOOL		bScanBackward;

//configurations for continuous and 3D imaging
extern int		FrameThickness;
extern int		FrameNum;
//extern int		MotorStepsize;
extern int		AvgNum;
extern float	Dispersion;
extern BOOL		bReconstructImage;
extern BOOL		bStepX;
extern BOOL		bStepY;
//extern BOOL		bEnableStage;
extern BOOL		bImqFrames;
extern BOOL		bGetRef;
extern CString	DefaultFilePath;

//calibration co efficiencies.
extern UINT16	LeftPointIndex[SPECTRUMWIDTH];
extern UINT16	RightPointIndex[SPECTRUMWIDTH];
extern float	LeftPointCoeff[SPECTRUMWIDTH];
extern float	RightPointCoeff[SPECTRUMWIDTH];
//extern float	OddPixGain[SPECTRUMWIDTH];
extern float	BarthWindow[SPECTRUMWIDTH];

//curve drawing arrays
extern UINT16 RawSpectrumCurveData[SPECTRUMWIDTH]; 
extern float  CaliedSpectrumCurveData[SPECTRUMWIDTH];
extern UINT8  IntensityCurveData[IMAGEWIDTH];
extern UINT8  PhaseCurveData[IMAGEWIDTH];
extern int  PowerData[1000];
extern int	AveragedReference[SPECTRUMWIDTH];

//images' arrays pointers
//extern INT16 * ReferenceBuff;
extern INT16 * CaliBuff;
extern float * CaliResult;
extern float * FFTBuff;
extern float * FFTResult;
extern float * IntensityBuff;
extern UINT * IntensityImageRGB;
extern UINT8 * IntensityImage;
extern float * PhaseBuff;
extern UINT8 * PhaseWeightBuff;
extern UINT * PhaseImageRGB;
extern UINT8 * PhaseImage;
extern UINT * IntensitySmallImageRGB;
extern UINT8 * IntensitySmallImage;
extern UINT * PhaseSmallImageRGB;
extern UINT8 * PhaseSmallImage;


//threads loop's BOOL conditions
extern BOOL bImaq;
extern BOOL bCali;
extern BOOL bFFT;
extern BOOL bDrawCurve;
extern BOOL bDisplayIntensity;
extern BOOL bDisplayPhase;


//CurveDlg parameters
extern BOOL bRawSpectrum;
extern BOOL bCaliedSpectrum;
extern BOOL bAxialIntensity;
extern BOOL bAxialPhase;
extern BOOL bPower;
extern int	AxialLine_i;

//IntensityDlg parameters
extern float	IntensityGain;
extern int OffSet;
extern BOOL		bZoom;
extern BOOL		bExpand;
extern BOOL		bAOIEn;
extern BOOL		bLog;
extern BOOL		bHotColor;
extern BOOL		bAutosave;
//extern BOOL		bReconstruction;
extern int		LeftLateralIndex;
extern int		RightLateralIndex;
extern int		UpDepthIndex;
extern int		DownDepthIndex;
extern CStringArray strPathNameArray;
extern int nIndex;
extern int PhaseThresh;

//pixel information for displayed image
extern int		intensity;
extern int		phase;
extern double	voltageX;
extern double	voltageY;
extern double	depth;

extern int HotColorTable[256];
extern int ColdColorTable[256];
//events for communication between threads
extern EVENT CaliBuffEmpty;
extern EVENT FFTBuffEmpty;
extern EVENT IntensityBuffEmpty;
extern EVENT PhaseBuffEmpty ;
extern EVENT CaliBuffNew;
extern EVENT FFTBuffNew;
extern EVENT IntensityBuffNew;
extern EVENT PhaseBuffNew;

#endif
