#ifndef __CAPTUREVIDEO_H__
#define __CAPTUREVIDEO_H__
#include "SampleGrabberCallback.h"

#define VIDEO_FMT_FOURCC(a, b, c, d) \
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define VIDEO_FMT_MJPEG		VIDEO_FMT_FOURCC('M', 'J', 'P', 'G')
#define VIDEO_FMT_YUY2		VIDEO_FMT_FOURCC('Y', 'U', 'Y', '2')
#define VIDEO_FMT_H264		VIDEO_FMT_FOURCC('H', '2', '6', '4')
#define VIDEO_FMT_UNDEF		VIDEO_FMT_FOURCC('?', '?', '?', '?')

typedef struct VideoFmtFrameInfo {
	char mName[MAX_PATH];
	GUID mSubType;
	uint32_t mFmtType;//V4L2_PIX_FMT_H264
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mFps;
	uint32_t mIndex;
	uint32_t iFormat;
}VideoFmtFrameInfoSt;

class CaptureVideo
{
public:
	CaptureVideo();
	~CaptureVideo();
	HRESULT InitializeEnv(); //initialize environment
	HRESULT EnumAllDevices(HWND hCombox);  //Enumeration all devices
	void CloseInterface(); //close all interface
	HRESULT ListDeviceRes(int deviceID, HWND hCombox);
	HRESULT OpenDevice(int deviceID, int resIndex, LONG nLeft, LONG nTop, LONG nWide, LONG nHeight);
	HRESULT BindFilter(int deviceID, IBaseFilter **pBaseFilter);
	HRESULT SetupVideoWindow(LONG nLeft, LONG nTop, LONG nWidth, LONG nHeight);
	void ResizeVideoWindow(LONG nLeft, LONG nTop, LONG nWidth, LONG nHeight);
	void GrabOneFrame(BOOL bGrab); // use ISampleGrabberCallbackCB get a picture
	void CapturePicture(void);
	HRESULT HandleGraphCapturePicture(void); //capture picture  use GetCurrentBuffer

private:
	IGraphBuilder *m_pGraphBuilder;
	ICaptureGraphBuilder2 *m_pCaptureGB;
	IMediaControl *m_pMediaControl;
	IBaseFilter *m_pDevFilter;
	IBaseFilter* m_pVmrFilter;
	ISampleGrabber *m_pSampGrabber;  
	IMediaEventEx *m_pMediaEvent;
public:
	IAMVideoProcAmp *m_pProcAmp;
	IAMCameraControl *m_pCameraControl;

	int m_nCaptureDeviceNumber;      //Device Count
	//vector<string> m_vecCapDeviceName; //the Device name
	TCHAR m_pCapDeviceName[10][MAX_PATH]; //the Device name
	HWND m_App;
	BOOL m_bGetOneShot;
	BOOL m_bConnect;
	IVideoWindow *m_pVideoWindow;

	int m_nVideoFmtFrameCnt;
	VideoFmtFrameInfoSt m_pVideoFmtFrameInfo[32];
};


#endif  //__CAPTUREVIDEO_H__