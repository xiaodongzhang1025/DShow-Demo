#include "pch.h"
#include "framework.h"
#include "CaptureVideo.h"

SampleGrabberCallback g_sampleGrabberCB;  //CallBack

CaptureVideo::CaptureVideo()
{
	//COM Library Initialize
	if (FAILED(CoInitialize(NULL)))
	{
		TRACE(L"CoInitialize Failed!\r\n");
		return;
	}
	//initialize member variable
	m_nCaptureDeviceNumber = 0;
	m_pDevFilter = NULL;
	m_pCaptureGB = NULL;
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pMediaEvent = NULL;
	m_pSampGrabber = NULL;
	m_pVideoWindow = NULL;

	m_pProcAmp = NULL;
	m_pCameraControl = NULL;

	m_App = NULL;
	m_bGetOneShot = FALSE;
	m_bConnect = FALSE;
	InitializeEnv();
}

CaptureVideo::~CaptureVideo()
{
	CloseInterface();
	CoUninitialize();
}

HRESULT CaptureVideo::EnumAllDevices(HWND hCombox)
{
	if (!hCombox)
		return S_FALSE;
	ICreateDevEnum *pDevEnum;
	IEnumMoniker   *pEnumMon;
	IMoniker	   *pMoniker;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,
			IID_ICreateDevEnum,(LPVOID*)&pDevEnum);
	if (SUCCEEDED(hr))
	{
		m_nCaptureDeviceNumber = 0;
		memset(m_pCapDeviceName, 0, sizeof(m_pCapDeviceName));

		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnumMon, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;
			return hr;
		}
		pEnumMon->Reset();
		ULONG cFetched;
		while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr == S_OK)
		{
			IPropertyBag *pProBag;
			hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pProBag);
			if (SUCCEEDED(hr))
			{
				VARIANT varTemp;
				varTemp.vt = VT_BSTR;
				hr = pProBag->Read(L"FriendlyName",&varTemp,NULL);
				if (SUCCEEDED(hr))
				{
					//int nStrSize = WideCharToMultiByte(CP_ACP,0,varTemp.bstrVal,-1,0,0,NULL,FALSE);
					//char *strName = new char[nStrSize];
					//WideCharToMultiByte(CP_ACP,0,varTemp.bstrVal,-1,strName,nStrSize,NULL,FALSE);
					//m_vecCapDeviceName.push_back(string(strName)); //push the device name to vector
					
					StringCchCopy(m_pCapDeviceName[m_nCaptureDeviceNumber],MAX_PATH,varTemp.bstrVal);
					//::SendMessage(hCombox, CB_ADDSTRING, m_nCaptureDeviceNumber,(LPARAM)varTemp.bstrVal);
					::SendMessage(hCombox, CB_INSERTSTRING, m_nCaptureDeviceNumber,(LPARAM)varTemp.bstrVal);
					m_nCaptureDeviceNumber++; //Device number add 1
					SysFreeString(varTemp.bstrVal);
				}
				pProBag->Release();
			}
			pMoniker->Release();
		}
		pEnumMon->Release();
	}
	return hr;
}

HRESULT CaptureVideo::InitializeEnv()
{
	HRESULT hr;

	//Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
						  IID_IGraphBuilder,(LPVOID*)&m_pGraphBuilder);
	if(FAILED(hr))
		return hr;
	
	//Create the capture graph builder
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,
						  IID_ICaptureGraphBuilder2,(LPVOID*)&m_pCaptureGB);
	if(FAILED(hr))
		return hr;

	//Obtain interfaces for media control and Video Window
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl,(LPVOID*)&m_pMediaControl);
	if(FAILED(hr))
		return hr;

	hr = m_pGraphBuilder->QueryInterface(IID_IVideoWindow,(LPVOID*)&m_pVideoWindow);
	if(FAILED(hr))
		return hr;

	hr = m_pGraphBuilder->QueryInterface(IID_IMediaEventEx,(LPVOID*)&m_pMediaEvent);
	if(FAILED(hr))
		return hr;

	hr = m_pMediaEvent->SetNotifyWindow((OAHWND)m_App, WM_GRAPHNOTIFY, 0);

	if(FAILED(hr))
		return hr;
	m_pCaptureGB->SetFiltergraph(m_pGraphBuilder);
	if(FAILED(hr))
		return hr;
	return hr;
}

void CaptureVideo::CloseInterface()
{
	m_bGetOneShot = FALSE;

	if (m_pMediaControl)
		m_pMediaControl->Stop();
	if(m_pVideoWindow)
	{
		m_pVideoWindow->get_Visible(FALSE);
		m_pVideoWindow->put_Owner(NULL);
	}

	if(m_pMediaEvent)
		m_pMediaEvent->SetNotifyWindow(NULL,WM_GRAPHNOTIFY,0);

	//release interface
	ReleaseInterface(m_pVmrFilter);
	ReleaseInterface(m_pDevFilter);
	ReleaseInterface(m_pCaptureGB);
	ReleaseInterface(m_pGraphBuilder);
	ReleaseInterface(m_pMediaControl);
	ReleaseInterface(m_pMediaEvent);
	ReleaseInterface(m_pSampGrabber);
	ReleaseInterface(m_pVideoWindow);
}

HRESULT CaptureVideo::BindFilter(int deviceID, IBaseFilter **pBaseFilter)
{
	ICreateDevEnum *pDevEnum;
	IEnumMoniker   *pEnumMon;
	IMoniker	   *pMoniker;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,(LPVOID*)&pDevEnum);
	if (SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnumMon, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;
			return hr;
		}
		pEnumMon->Reset();
		ULONG cFetched;
		int index = 0;
#if 0		
		while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr == S_OK)
		{
			IPropertyBag *pProBag;
			hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pProBag);
			if (SUCCEEDED(hr))
			{
				VARIANT varTemp;
				varTemp.vt = VT_BSTR;
				hr = pProBag->Read(L"FriendlyName",&varTemp,NULL);
				if (SUCCEEDED(hr))
				{
					TCHAR tmpName[MAX_PATH];
					StringCchCopy(tmpName,MAX_PATH,varTemp.bstrVal);
					if (_tcscmp(m_pCapDeviceName[deviceID],tmpName) == 0)
					{
						pMoniker->BindToObject(0,0,IID_IBaseFilter,(LPVOID*)pBaseFilter);
						break;
					}
					SysFreeString(varTemp.bstrVal);
				}
			}
			pMoniker->Release();
			index++;
		}
#else
		while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr == S_OK, index<= deviceID)
		{
			IPropertyBag *pProBag;
			hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,(LPVOID*)&pProBag);
			if (SUCCEEDED(hr))
			{
				if (index == deviceID)
				{
					pMoniker->BindToObject(0,0,IID_IBaseFilter,(LPVOID*)pBaseFilter);
				}
			}
			pMoniker->Release();
			index++;
		}
#endif
		pEnumMon->Release();
	}
	return hr;
}

HRESULT CaptureVideo::SetupVideoWindow(LONG nLeft, LONG nTop, LONG nWidth, LONG nHeight)
{
	HRESULT hr;
	hr = m_pVideoWindow->put_Owner((OAHWND)m_App);
	if (FAILED(hr))
		return hr;

	hr = m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if(FAILED(hr))
		return hr;

	ResizeVideoWindow(nLeft,nTop,nWidth,nHeight);

	hr = m_pVideoWindow->put_Visible(OATRUE);
	if(FAILED(hr))
		return hr;

	return hr;
}

void CaptureVideo::ResizeVideoWindow(LONG nLeft, LONG nTop, LONG nWidth, LONG nHeight)
{
	if(m_pVideoWindow)
	{
		m_pVideoWindow->SetWindowPosition(nLeft,nTop,nWidth,nHeight);
	}
}

void FreeMediaType(AM_MEDIA_TYPE* pmt)
{
	if (pmt == NULL)
	{
		return;
	}

	if (pmt->cbFormat != 0)
	{
		CoTaskMemFree((PVOID)pmt->pbFormat);
		// Strictly unnecessary but tidier
		pmt->cbFormat = 0;
		pmt->pbFormat = NULL;
	}

	if (pmt->pUnk != NULL)
	{
		pmt->pUnk->Release();
		pmt->pUnk = NULL;
	}
}

HRESULT CaptureVideo::ListDeviceRes(int deviceID, HWND hCombox) {
	HRESULT hr;
	IAMStreamConfig* pConfig = NULL;
	if (!hCombox)
		return S_FALSE;
	m_nVideoFmtFrameCnt = 0;
	memset(m_pVideoFmtFrameInfo, 0, sizeof(m_pVideoFmtFrameInfo));
	//&MEDIATYPE_Video,如果包括其他媒体类型，第二个参数设置为0
	hr = m_pCaptureGB->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		m_pDevFilter, IID_IAMStreamConfig, (void**)&pConfig);
	if (FAILED(hr))
		return hr;
	int iCount = 0, iSize = 0;
	hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
	// Check the size to make sure we pass in the correct structure.
	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		// Use the video capabilities structure.
		for (int iFormat = 0; iFormat < iCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE* pmtConfig = NULL;
			hr = pConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
			if (SUCCEEDED(hr))
			{
				//(pmtConfig->subtype == MEDIASUBTYPE_RGB24) &&
				if ((pmtConfig->majortype == MEDIATYPE_Video) &&
					(pmtConfig->formattype == FORMAT_VideoInfo) &&
					(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
					(pmtConfig->pbFormat != NULL))
				{
					VIDEOINFOHEADER* pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
					// pVih contains the detailed format information.
					LONG lWidth = pVih->bmiHeader.biWidth;
					LONG lHeight = pVih->bmiHeader.biHeight;
					BOOL bFind = FALSE;
					LONG lFps = 1*1000*1000*1000/(pVih->AvgTimePerFrame*100);//100ns
					char tmpName[MAX_PATH] = { 0 };
					memset(tmpName, 0, sizeof(tmpName));
					uint32_t fmtType = VIDEO_FMT_UNDEF;
					if (pmtConfig->subtype == MEDIASUBTYPE_MJPG) {
						TRACE(L"iFormat = %d, subtype = MJPEG, width = %d, height = %d, fps = %d\n", iFormat, lWidth, lHeight, lFps);
						sprintf(tmpName, "MJPEG %dx%d %d", lWidth, lHeight, lFps);
						fmtType = VIDEO_FMT_MJPEG;
					}else if (pmtConfig->subtype == MEDIASUBTYPE_YUY2) {
						TRACE(L"iFormat = %d, subtype = YUY2, width = %d, height = %d\n", iFormat, lWidth, lHeight, lFps);
						sprintf(tmpName, "YUY2 %dx%d %d", lWidth, lHeight, lFps);
						fmtType = VIDEO_FMT_YUY2;
					}else if(pmtConfig->subtype == MEDIASUBTYPE_H264) {
						TRACE(L"iFormat = %d, subtype = H264, width = %d, height = %d\n", iFormat, lWidth, lHeight, lFps);
						sprintf(tmpName, "H264 %dx%d %d", lWidth, lHeight, lFps);
						fmtType = VIDEO_FMT_H264;
					}else {
						TRACE(L"iFormat = %d, subtype = ?, width = %d, height = %d\n", iFormat, lWidth, lHeight, lFps);
						sprintf(tmpName, "???? %dx%d %d", lWidth, lHeight, lFps);
						fmtType = VIDEO_FMT_UNDEF;
					}
					BOOL repeatFlag = FALSE;
					for (int i = 0; i < m_nVideoFmtFrameCnt; i++) {
						if (memcmp(tmpName, m_pVideoFmtFrameInfo[i].mName, strlen(tmpName)) == 0) {
							repeatFlag = TRUE;
							break;
						}
					}
					if (repeatFlag) {
						continue;
					}
					sprintf(m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mName, tmpName);
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].iFormat = iFormat;
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mFmtType = fmtType;
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mIndex = m_nVideoFmtFrameCnt;
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mWidth = lWidth;
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mHeight = lHeight;
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mFps = lFps;
					m_pVideoFmtFrameInfo[m_nVideoFmtFrameCnt].mSubType = pmtConfig->subtype;

					TCHAR tmpNameStr[MAX_PATH] = {0};
					mbstowcs(tmpNameStr, tmpName, MAX_PATH);
					::SendMessage(hCombox, CB_INSERTSTRING, m_nVideoFmtFrameCnt, (LPARAM)tmpNameStr);
					m_nVideoFmtFrameCnt++;
				}

				// Delete the media type when you are done.
				FreeMediaType(pmtConfig);
			}
		}
	}
	return hr;
}

HRESULT CaptureVideo::OpenDevice(int deviceID, int resIndex, LONG nLeft, LONG nTop, LONG nWide, LONG nHeight)
{
	HRESULT hr;
	
	if (m_bConnect)
	{
		CloseInterface();
		InitializeEnv();
	}

	//bind device filter
	hr = BindFilter(deviceID,&m_pDevFilter);
	if (FAILED(hr))
		return hr;

	hr = m_pGraphBuilder->AddFilter(m_pDevFilter,L"Video Filter");
	if (FAILED(hr))
		return hr;

	hr = CoCreateInstance(CLSID_VideoMixingRenderer9, 0,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pVmrFilter);
	if (FAILED(hr))
		return hr;

	hr = m_pGraphBuilder->AddFilter(m_pVmrFilter, L"VMR9");
	if (FAILED(hr))
		return hr;

	IBaseFilter* pSampleGrabberFilter;
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&pSampleGrabberFilter);
	if (FAILED(hr))
		return hr;

	hr = m_pGraphBuilder->AddFilter(pSampleGrabberFilter,L"Sample Grabber");
	if (FAILED(hr))
		return hr;

	hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&m_pSampGrabber);
	if (FAILED(hr))
		return hr;

#if 0
	//set media type
	AM_MEDIA_TYPE mediaType;
	ZeroMemory(&mediaType,sizeof(AM_MEDIA_TYPE));
	AM_MEDIA_TYPE* pmt = NULL;
	pmt = &mediaType;
	if (resIndex == -1) {
		//Find the current bit depth
		HDC hdc = GetDC(NULL);
		int iBitDepth = GetDeviceCaps(hdc, BITSPIXEL);
		g_sampleGrabberCB.m_iBitCount = iBitDepth;
		ReleaseDC(NULL, hdc);
		//Set the media type
		pmt->majortype = MEDIATYPE_Video;
		switch (iBitDepth)
		{
		case  8:
			pmt->subtype = MEDIASUBTYPE_RGB8;
			break;
		case 16:
			pmt->subtype = MEDIASUBTYPE_RGB555;
			break;
		case 24:
			pmt->subtype = MEDIASUBTYPE_RGB24;
			break;
		case 32:
			pmt->subtype = MEDIASUBTYPE_RGB32;
			break;
		default:
			return E_FAIL;
		}
	}else{
		g_sampleGrabberCB.m_iBitCount = 0;
		pmt->majortype = MEDIATYPE_Video;
		pmt->subtype = m_pVideoFmtFrameInfo[resIndex].mSubType;//MEDIASUBTYPE_MJPG;
		pmt->pbFormat = (BYTE *)malloc(sizeof(VIDEOINFOHEADER));
		if (pmt->pbFormat) {
			pmt->cbFormat = sizeof(VIDEOINFOHEADER);
			VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)pmt->pbFormat;
			vih->bmiHeader.biWidth = m_pVideoFmtFrameInfo[resIndex].mWidth;
			vih->bmiHeader.biHeight = m_pVideoFmtFrameInfo[resIndex].mHeight;
			vih->AvgTimePerFrame = 1 * 1000 * 1000 * 1000 / (m_pVideoFmtFrameInfo[resIndex].mFps * 100);
		}
	}
	pmt->formattype = FORMAT_VideoInfo;
	hr = m_pSampGrabber->SetMediaType(pmt);
	if (resIndex != -1 && pmt->pbFormat != NULL) {
		free(pmt->pbFormat);
	}

#else
	AM_MEDIA_TYPE* pmt = NULL;
	IAMStreamConfig* pConfig = NULL;
	m_pCaptureGB->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
	m_pDevFilter, IID_IAMStreamConfig, (void**)&pConfig);
	VIDEO_STREAM_CONFIG_CAPS scc;
	pConfig->GetStreamCaps(m_pVideoFmtFrameInfo[resIndex].iFormat, &pmt, (BYTE*)&scc);
	pConfig->SetFormat(pmt);
#endif

	hr = m_pCaptureGB->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
		m_pDevFilter, pSampleGrabberFilter, m_pVmrFilter);
	if(FAILED(hr))
		return hr;

	hr = m_pSampGrabber->GetConnectedMediaType(pmt);
	if(FAILED(hr))
		return hr;

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)pmt->pbFormat;
	g_sampleGrabberCB.m_lWidth = vih->bmiHeader.biWidth;
	g_sampleGrabberCB.m_lHeight = vih->bmiHeader.biHeight;

	if (resIndex != -1) {
		TRACE(L"bitCnt = %d, width = %d, height = %d\n", g_sampleGrabberCB.m_iBitCount, g_sampleGrabberCB.m_lWidth, g_sampleGrabberCB.m_lHeight);
	}
	// Configure the Sample Grabber
	hr = m_pSampGrabber->SetOneShot(FALSE);
	if (FAILED(hr))
		return hr;
	hr = m_pSampGrabber->SetBufferSamples(TRUE);
	if (FAILED(hr))
		return hr;
	// 1 = Use the BufferCB callback method.
	hr = m_pSampGrabber->SetCallback(&g_sampleGrabberCB,1);

	//set capture video Window
	SetupVideoWindow(nLeft,nTop,nWide,nHeight);
	TRACE(L"nLeft = %d, nTop = %d, nWide = %d, nHeight = %d\n", nLeft, nTop, nWide, nHeight);
	hr = m_pMediaControl->Run();
	if(FAILED(hr))
		return hr;

#if 0
	if (pmt->cbFormat != 0)
	{
		CoTaskMemFree((PVOID)pmt->pbFormat);
		pmt->cbFormat = 0;
		pmt->pbFormat = NULL;
	}
	if (pmt->pUnk != NULL)
	{
		pmt->pUnk->Release();
		pmt->pUnk = NULL;
	}
#else
	FreeMediaType(pmt);
#endif
	hr = m_pDevFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&m_pProcAmp);
	if(FAILED(hr))    
	{ 
		m_pProcAmp = NULL;
	}

	hr = m_pDevFilter->QueryInterface(IID_IAMCameraControl, (void**)&m_pCameraControl);
	if(FAILED(hr))    
	{ 
		m_pCameraControl = NULL;
	}

	m_bConnect = TRUE;
	return hr;

}

void CaptureVideo::GrabOneFrame(BOOL bGrab)
{
	m_bGetOneShot = bGrab;
	g_sampleGrabberCB.m_bGetPicture = bGrab;//for save data in BufferCB

	CapturePicture();
	//HandleGraphCapturePicture();
}

void CaptureVideo::CapturePicture(void)
{
	IBasicVideo* pBasicVideo = NULL;
	HRESULT hr = m_pGraphBuilder->QueryInterface(IID_IBasicVideo, (void**)&pBasicVideo);
	if (FAILED(hr)) {
		return;
	}

	//hr = m_pMediaControl->Pause();
	//FILTER_STATE fs;
	//hr = m_pMediaControl->GetState(10, (OAFilterState*)&fs);
	//if (fs == State_Running) {
	//	TRACE("still running\n");
	//}
	long bitmapSize;
	char* buffer;
	hr = pBasicVideo->GetCurrentImage(&bitmapSize, 0);
	if (SUCCEEDED(hr))
	{
		buffer = new char[bitmapSize];
		hr = pBasicVideo->GetCurrentImage(&bitmapSize, (long*)buffer);
		if (FAILED(hr)) {
			delete []buffer;
			ReleaseInterface(pBasicVideo);
			return;
		}
		BITMAPFILEHEADER hdr;  //定义位图变量
		LPBITMAPINFOHEADER lpbi; //是个指针,指向BMP头的
		lpbi = (LPBITMAPINFOHEADER)buffer;
		int nColor = 0;
		if (lpbi->biBitCount <= 8)
		{
			nColor = 1 << lpbi->biBitCount;
		}
		ZeroMemory(&hdr, sizeof(hdr));
		// Fill in the fields of the file header 
		//hdr.bfType = ((WORD)('M' << 8) | 'B');// 位图类别is always "BM"
		hdr.bfType = 'MB';// 位图类别is always "BM"
		hdr.bfSize = bitmapSize + sizeof(hdr);  //BMP图像文件的大小
		hdr.bfReserved1 = 0;  //总为0
		hdr.bfReserved2 = 0;   //总为0
		//hdr.bfOffBits = (DWORD)(sizeof(BITMAPFILEHEADER) +  lpbi->biSize + nColor * sizeof(RGBQUAD));
		hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(LPBITMAPINFOHEADER) + nColor * sizeof(RGBQUAD);
#if 0
		FILE* fp = fopen("1.bmp", "wb");
		fwrite(&hdr, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(buffer, bitmapSize, 1, fp);
		fclose(fp);
#else
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		TCHAR m_chSwapStr[MAX_PATH] = {0};
		TCHAR m_chDirName[MAX_PATH] = {0};
		TCHAR m_chTempPath[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, m_chTempPath);
		StringCchCat(m_chTempPath, MAX_PATH, TEXT("\\PICS"));
		CreateDirectory(m_chTempPath, NULL);
		StringCchCopy(m_chSwapStr, MAX_PATH, m_chTempPath);
		StringCchPrintf(m_chDirName, MAX_PATH, TEXT("\\%04i%02i%02i%02i%02i%02i%03i.bmp"),
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
			sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
		StringCchCat(m_chSwapStr, MAX_PATH, m_chDirName);
		//MessageBox(NULL,chTempPath,TEXT("Message"),MB_OK);
		//create picture file
		HANDLE hf = CreateFile(m_chSwapStr, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
			CREATE_ALWAYS, 0, NULL);
		if (hf == INVALID_HANDLE_VALUE)
		{
			delete []buffer;
			ReleaseInterface(pBasicVideo);
			return;
		}
		DWORD dwWritten = 0;
		WriteFile(hf, &hdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
		WriteFile(hf, buffer, bitmapSize, &dwWritten, NULL);
		CloseHandle(hf);
#endif
		delete []buffer;
		ReleaseInterface(pBasicVideo);
	}

	//hr = m_pMediaControl->Run();
}

//SAME WITH BufferCB
HRESULT CaptureVideo::HandleGraphCapturePicture(void)
{
	//////////////////////////////////////////////////////////////////////////
	HRESULT hr = 0;
	long evCode = 0;
	long lBufferSize = 0;
	BYTE *p;
#if 0
	hr = m_pMediaEvent->WaitForCompletion(INFINITE, &evCode); //
	if (SUCCEEDED(hr))
	{
		switch(evCode)
		{
		case EC_COMPLETE:
			m_pSampGrabber->GetCurrentBuffer(&lBufferSize, NULL);
			p = new BYTE[lBufferSize];
			m_pSampGrabber->GetCurrentBuffer(&lBufferSize, (LONG*)p);// get Current buffer
			g_sampleGrabberCB.SaveBitmap(p,lBufferSize); //save bitmap
			delete []p;
			break;
		default:
			break;
		}
	}
#else
	m_pSampGrabber->GetCurrentBuffer(&lBufferSize, NULL);
	p = new BYTE[lBufferSize];
	m_pSampGrabber->GetCurrentBuffer(&lBufferSize, (LONG*)p);// get Current buffer
	g_sampleGrabberCB.SaveBitmap(p, lBufferSize); //save bitmap
	delete []p;
#endif
	//////////////////////////////////////////////////////////////////////////
	return hr;
}