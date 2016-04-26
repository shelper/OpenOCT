#pragma once
#include "niimaq.h"
#include "nidaqmx.h"
#include "Defines.h"
#include "ExternVariables.h"

#define IMAQErrChk(fCall) if (err1 = (fCall), err1 < 0) {goto Error1;} else
#define DAQmxErrChk(fCall) if(err2 = (fCall), err2 < 0) {goto Error2;} else

class CImgAcq
{

		SESSION_ID Sid;
		INTERFACE_ID Iid;
		CString intfName;
		BUFLIST_ID Bid;
	TaskHandle TriggerTaskHandle, ScanVoltageTaskHandle;

	int err1;
	int err2;
	//scan parameters
	int SampleNumPerChan, SampleNumPerBuff, ScanVoltageNum, ScanLineNum;	
	int tmpCamGain,	tmpIntTime, tmpScanFreq;
	double tmpScanVMaxX, tmpScanVMinX,tmpScanVMaxY, tmpScanVMinY;

	double *ScanVoltage;

public:
	CImgAcq();
	~CImgAcq();
	void * ImaqBuff;
	BOOL GetImg();
	BOOL Initialize();
};
