#include "stdafx.h"
#include "imgacq.h"
#include "niimaq.h"
#include "nidaqmx.h"
#include "Defines.h"
#include "OCT.h"
#include "ExternVariables.h"

CImgAcq::CImgAcq()
{
	//setup initial values of data acquisition tasks
	intfName="img1";
	Sid = 0;
	Iid =0;
	Bid =0;
	err1=0;
	err2=0;
	ScanVoltageTaskHandle =0;
	TriggerTaskHandle=0;
	ScanLineNum=ALineNum*IterationNum;
    ScanVoltageNum=ScanLineNum*2;
	SampleNumPerBuff=ScanLineNum*(SPECTRUMWIDTH);
	SampleNumPerChan=SampleNumPerBuff/2;
	ScanVoltage=(double *) calloc(ScanLineNum*4,8);
}

BOOL CImgAcq::Initialize()
{	
	//read in the system configurations
	bScanDirection=FORWARD;
	tmpCamGain=CamGain;
	tmpIntTime=IntTime;
	tmpScanFreq=ScanFreq;

	tmpScanVMaxX=ScanVMaxX;
	tmpScanVMinX=ScanVMinX;
	tmpScanVMaxX=ScanVMaxY;
	tmpScanVMinX=ScanVMinY;

	
	////iterative scan, calculate the scanvoltageX;
	//double	ScanVStepX=(tmpScanVMaxX-tmpScanVMinX)/(ALineNum-1);
	//double	ScanVStepY=(tmpScanVMaxY-tmpScanVMinY)/(ALineNum-1);
	//for ( int i = 0;i<ScanVoltageNum ;i=i+2){
	//	*(ScanVoltage + i) = tmpScanVMinX + ScanVStepX*(i/2/IterationNum);
	//	*(ScanVoltage + ScanVoltageNum + i) = tmpScanVMaxX - ScanVStepX * (i/2/IterationNum);
	//	*(ScanVoltage + i+1) = tmpScanVMinY + ScanVStepY*(i/2/IterationNum);
	//	*(ScanVoltage + ScanVoltageNum + i+1) = tmpScanVMaxY - ScanVStepY * (i/2/IterationNum);
	//}

	//non-iterative scan, calculate the scanvoltageX;
	double	ScanVStepX=(tmpScanVMaxX-tmpScanVMinX)/(ScanLineNum-1);
	double	ScanVStepY=(tmpScanVMaxY-tmpScanVMinY)/(ScanLineNum-1);
	for ( int i = 0;i<ScanVoltageNum ;i=i+2){
		*(ScanVoltage + i) = tmpScanVMinX + ScanVStepX*(i/2);
		*(ScanVoltage + ScanVoltageNum + i) = tmpScanVMaxX - ScanVStepX * (i/2);
		*(ScanVoltage + i+1) = tmpScanVMinY + ScanVStepY*(i/2);
		*(ScanVoltage + ScanVoltageNum + i+1) = tmpScanVMaxY - ScanVStepY* (i/2);
	}
	
	//IMAQ initialization
	IMAQErrChk(imgInterfaceOpen (intfName, &Iid));
	IMAQErrChk(imgSessionOpen (Iid, &Sid));
	IMAQErrChk(imgSessionTriggerConfigure2(Sid, IMG_SIGNAL_RTSI, 0, IMG_TRIG_POLAR_ACTIVEH, 2000000, IMG_TRIG_ACTION_CAPTURE));
	IMAQErrChk(imgSetAttribute (Sid, IMG_ATTR_ROI_HEIGHT, ALineNum*IterationNum));
	IMAQErrChk(imgCreateBufList(1, &Bid));
	IMAQErrChk(imgCreateBuffer(Sid, IMG_HOST_FRAME, RawDataSize*2, &ImaqBuff));
	IMAQErrChk(imgSetBufferElement(Bid, 0, IMG_BUFF_ADDRESS, (uInt32)ImaqBuff));
	IMAQErrChk(imgSetBufferElement(Bid, 0, IMG_BUFF_SIZE,  RawDataSize*2));
	IMAQErrChk(imgSetBufferElement(Bid, 0, IMG_BUFF_COMMAND, IMG_CMD_STOP));
	IMAQErrChk(imgSessionConfigure(Sid, Bid));
	
	
	//IMAQErrChk(imgSetCameraAttributeNumeric(Sid,"Digital Gain", (double)tmpCamGain/100));
	//char strOPR[8] = "OPR ";
	//char strIntTime[3];	
	//itoa(tmpIntTime,strIntTime,10);
	//strcat(strOPR,strIntTime);
	//IMAQErrChk(imgSetCameraAttributeString(Sid,"Operational Setting", strOPR));

	//IMAQErrChk(imgSetCameraAttributeString(Sid,"Trigger Mode", "Mode 1 Trig"));
	//IMAQErrChk(imgSessionLineTrigSource(Sid, IMG_EXT_TRIG1, IMG_TRIG_POLAR_ACTIVEH, 0));
	
	//create trigger task
	DAQmxErrChk(DAQmxConnectTerms("/Dev1/PFI12","/Dev1/RTSI0", DAQmx_Val_DoNotInvertPolarity));
	DAQmxErrChk(DAQmxCreateTask("TriggerTask",&TriggerTaskHandle));
	DAQmxErrChk(DAQmxCreateCOPulseChanFreq(TriggerTaskHandle, "Dev1/Ctr0", "TriggerTask", DAQmx_Val_Hz, DAQmx_Val_Low, 0, tmpScanFreq , 0.5));
	DAQmxErrChk(DAQmxCfgImplicitTiming(TriggerTaskHandle,DAQmx_Val_FiniteSamps,1));

	//create ScanVoltage task and channels
	//voltage output  for x and y
	DAQmxErrChk(DAQmxCreateTask("ScanVoltageTask",&ScanVoltageTaskHandle));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(ScanVoltageTaskHandle,"Dev1/ao0","ScanVoltageX", -10,10,DAQmx_Val_Volts,NULL));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(ScanVoltageTaskHandle,"Dev1/ao1","ScanVoltageY", -10,10,DAQmx_Val_Volts,NULL));
	DAQmxErrChk(DAQmxCfgDigEdgeStartTrig (ScanVoltageTaskHandle, "PFI12", DAQmx_Val_Rising));
	DAQmxErrChk(DAQmxCfgSampClkTiming(ScanVoltageTaskHandle,"PFI0",tmpScanFreq , DAQmx_Val_Rising,DAQmx_Val_FiniteSamps, ScanLineNum));

	//error handling for IMAQ
Error1:
	if (err1 <0)	{	
		MessageBox( pMainDlg->GetSafeHwnd(),"Imaq Constructor Error: Initialize()", "DOCT", MB_OK);
		return 1;
	}

	//error handling for DAQ
Error2:
	if (err2 <0)	{	
		char errBuff[2048];
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessageBox( pMainDlg->GetSafeHwnd(), errBuff, "DOCT", MB_OK);
		return 1;
	}
	return 0;
}

CImgAcq::~CImgAcq(void)
{
	//reset IMAQ session ID-> buffer->buffer ID -> interface ID 
	//reset IMAQ session ID-> buffer->buffer ID -> interface ID 
	if ( Sid != 0){
		imgSessionStopAcquisition(Sid);
		imgClose (Sid, TRUE);
		Sid = 0;
	}
	if (ImaqBuff != NULL){
		imgDisposeBuffer(ImaqBuff);
		ImaqBuff = NULL ;
	}
	if (Bid != 0){
		imgDisposeBufList(Bid, FALSE);
		Bid =0 ;
	}
	if(Iid != 0){
		imgClose (Iid, TRUE);
		Iid =0;
	}

	//reset DAQ
	if( TriggerTaskHandle!=0 ) {
		DAQmxStopTask(TriggerTaskHandle);
		DAQmxClearTask(TriggerTaskHandle);
		TriggerTaskHandle = 0;
	}
	
	//reset scan voltage task 1 -x 
	if(ScanVoltageTaskHandle !=0)
	{
		DAQmxStopTask(ScanVoltageTaskHandle);
		DAQmxClearTask(ScanVoltageTaskHandle);
		ScanVoltageTaskHandle= 0;
	}

	free(ScanVoltage);

}

BOOL CImgAcq::GetImg()
{
	//check and update system configuration if necessary
	//camera configurations
	if ( tmpCamGain != CamGain || tmpIntTime != IntTime ){
		tmpCamGain=CamGain;
		tmpIntTime=IntTime;
		IMAQErrChk(imgSessionStopAcquisition(Sid));
		//need to be modified according to the camera
		//IMAQErrChk(imgSetCameraAttributeNumeric(Sid,"Digital Gain", (double)tmpCamGain/100));

		//char strOPR[8] = "OPR ";
		//char strIntTime[3];	
		//itoa(tmpIntTime,strIntTime,10);
		//strcat(strOPR,strIntTime);
		//IMAQErrChk(imgSetCameraAttributeString(Sid,"Operational Setting", strOPR));

		//the trigger does not work last time on the camera (because of whatever the problem is) 
		//a outside trigger from BNC cable has to be connected to the camera to provide the trigger.
		//IMAQErrChk(imgSetCameraAttributeString(Sid,"Trigger Mode", "Mode 1 Trig"));
		//IMAQErrChk(imgSessionLineTrigSource(Sid, IMG_EXT_TRIG1, IMG_TRIG_POLAR_ACTIVEH, 0));
		IMAQErrChk(imgSessionConfigure(Sid, Bid));

	}

	//scan voltage 1-x
	if (tmpScanVMaxX != ScanVMaxX || tmpScanVMinX!=ScanVMinX ){
		tmpScanVMinX=ScanVMinX;
		tmpScanVMaxX=ScanVMaxX;
		////-iterative scan
		//double	ScanVStepX=(tmpScanVMaxX-tmpScanVMinX)/(ALineNum-1);
		//for ( int i = 0;i<ScanVoltageNum ;i=i+2){
		//	*(ScanVoltage + i) = tmpScanVMinX + ScanVStepX*(i/2/IterationNum);
		//	*(ScanVoltage + ScanVoltageNum + i) = tmpScanVMaxX - ScanVStepX * (i/2/IterationNum);
		//}

		//non-iterative scan
		double	ScanVStepX=(tmpScanVMaxX-tmpScanVMinX)/(ScanLineNum-1);
		for ( int i = 0;i<ScanVoltageNum ;i=i+2){
			*(ScanVoltage + i) = tmpScanVMinX + ScanVStepX*(i/2);
			*(ScanVoltage + ScanVoltageNum + i) = tmpScanVMaxX - ScanVStepX * (i/2);
		}		
		//DAQmxErrChk(DAQmxWriteAnalogF64(ScanVoltageTaskHandle, ScanLineNum,TRUE,DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByScanNumber,ScanVoltage+bScanDirection*ScanVoltageNum, NULL, NULL));
	}

	//scan voltage 2-y
	if (tmpScanVMaxY != ScanVMaxY|| tmpScanVMinY!=ScanVMinY){
		tmpScanVMinY=ScanVMinY;
		tmpScanVMaxY=ScanVMaxY;
		////iterative scan
		//double	ScanVStepY=(tmpScanVMaxY-tmpScanVMinY)/(ALineNum-1);
		//for ( int i = 0;i<ScanVoltageNum ;i=i+2){
		//	*(ScanVoltage + i+1) = tmpScanVMinY + ScanVStepY*(i/2/IterationNum);
		//	*(ScanVoltage + ScanVoltageNum + i+1) = tmpScanVMaxY - ScanVStepY * (i/2/IterationNum);
		//}
		//non-iterative scan
		double	ScanVStepY=(tmpScanVMaxY-tmpScanVMinY)/(ScanLineNum-1);
		for ( int i = 0;i<ScanVoltageNum ;i=i+2){
			*(ScanVoltage + i+1) = tmpScanVMinY + ScanVStepY*(i/2);
			*(ScanVoltage + ScanVoltageNum + i+1) = tmpScanVMaxY - ScanVStepY* (i/2);
		}
		//DAQmxErrChk(DAQmxWriteAnalogF64(ScanVoltageTaskHandle, ScanLineNum,TRUE,DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByScanNumber,ScanVoltage+bScanDirection*ScanVoltageNum, NULL, NULL));
	}

	////Scan frequency / Trigger frequency
	//if (tmpScanFreq!= ScanFreq){
	//	tmpScanFreq=ScanFreq;
	//	//stop the trigger task first
	//	if( TriggerTaskHandle!=0 ) {
	//		DAQmxStopTask(TriggerTaskHandle);
	//		DAQmxClearTask(TriggerTaskHandle);
	//		TriggerTaskHandle = 0;
	//	}		
	//	//re-setup the trigger task
	//	DAQmxErrChk(DAQmxCreateTask("TriggerTask",&TriggerTaskHandle));
	//	DAQmxErrChk(DAQmxCreateCOPulseChanFreq(TriggerTaskHandle, "Dev1/Ctr0", "TriggerTask", DAQmx_Val_Hz, DAQmx_Val_Low, 0, tmpScanFreq , 0.5));
	//	DAQmxErrChk(DAQmxCfgImplicitTiming(TriggerTaskHandle,DAQmx_Val_ContSamps,NULL));
	//}

	IMAQErrChk(imgSessionAcquire(Sid, TRUE, NULL));	
	DAQmxErrChk(DAQmxWriteAnalogF64(ScanVoltageTaskHandle, ScanLineNum,TRUE,DAQmx_Val_WaitInfinitely, DAQmx_Val_GroupByScanNumber,ScanVoltage+bScanDirection*ScanVoltageNum, NULL, NULL));
	DAQmxErrChk(DAQmxStartTask(TriggerTaskHandle));
	DAQmxErrChk(DAQmxWaitUntilTaskDone(ScanVoltageTaskHandle,1));
	DAQmxErrChk(DAQmxStopTask(ScanVoltageTaskHandle));	
	IMAQErrChk(imgSessionReleaseBuffer(Sid));
	DAQmxErrChk(DAQmxStopTask(TriggerTaskHandle));
Error1:
	if (err1 <0)	{	
		MessageBox( pMainDlg->GetSafeHwnd(),"Imaq Constructor Error: GetImg()", "DOCT", MB_OK);
		//PostMessage((HWND) hWnd,WM_COMMAND,ID_ERROR_IMAQ,NULL);
		return 1;
	}

Error2:
	if (err2 <0)	{	
		char errBuff[2048];
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		MessageBox( pMainDlg->GetSafeHwnd(),errBuff, "DOCT", MB_OK);
		return 1;
	}
	return 0;
}
