#include "pch.h"
#include "framework.h"
#include "SampleGrabberCallback.h"


SampleGrabberCallback::SampleGrabberCallback()
{
	m_bGetPicture = FALSE;
	//Get template path
	//GetTempPath(MAX_PATH,m_chTempPath);
	GetCurrentDirectory(MAX_PATH, m_chTempPath);
	StringCchCat(m_chTempPath, MAX_PATH, TEXT("\\PICS"));
	CreateDirectory(m_chTempPath, NULL);
}

ULONG STDMETHODCALLTYPE SampleGrabberCallback::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE SampleGrabberCallback::Release()
{
	return 2;
}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::QueryInterface(REFIID riid,void** ppvObject)
{
	if (NULL == ppvObject) return E_POINTER;
	if (riid == __uuidof(IUnknown))
	{
		*ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
	}
	if (riid == IID_ISampleGrabberCB)
	{
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}
	return E_NOTIMPL;

}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::SampleCB(double Time, IMediaSample *pSample)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
	HRESULT hr = S_FALSE;
	if(!pBuffer)
		return E_POINTER;
	if(TRUE == m_bGetPicture)  //≈–∂œ «∑Ò–Ë“™ΩÿÕº
	{
		//SaveBitmap(pBuffer,BufferLen);
		SaveRaw(pBuffer, BufferLen);
		m_bGetPicture = FALSE;
		hr = S_OK;
	}else
	{
		hr = S_FALSE;
	}
	
	for (int i = 0; i < BufferLen - 3; i++) {
		if (pBuffer[i] == 0xff && pBuffer[i + 1] == 0x02) {
			TRACE("find RES tag, len = %d!\r\n", pBuffer[i + 2]*256 + pBuffer[i + 3]);
		}
	}
	//ALG_HandleImage(pBuffer, BufferLen, m_lWidth, m_lHeight, m_iBitCount);
	return hr;
}

BOOL SampleGrabberCallback::SaveRaw(BYTE* pBuffer, long lBufferSize)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchCopy(m_chSwapStr, MAX_PATH, m_chTempPath);
	StringCchPrintf(m_chDirName, MAX_PATH, TEXT("\\%04i%02i%02i%02i%02i%02i%03i.raw"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
		sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	StringCchCat(m_chSwapStr, MAX_PATH, m_chDirName);
	//MessageBox(NULL,chTempPath,TEXT("Message"),MB_OK);
	//create picture file
	HANDLE hf = CreateFile(m_chSwapStr, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, 0, NULL);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}
	//Write the file Data
	DWORD dwWritten = 0;
	WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
	CloseHandle(hf);
	return 0;
}

//only valid when RGB24\RGB32...
BOOL SampleGrabberCallback::SaveBitmap(BYTE * pBuffer, long lBufferSize )
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchCopy(m_chSwapStr,MAX_PATH,m_chTempPath);
	StringCchPrintf(m_chDirName,MAX_PATH,TEXT("\\%04i%02i%02i%02i%02i%02i%03i.bmp"),
					sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,
					sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	StringCchCat(m_chSwapStr,MAX_PATH,m_chDirName);
	//MessageBox(NULL,chTempPath,TEXT("Message"),MB_OK);
	//create picture file
	HANDLE hf = CreateFile(m_chSwapStr,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,
		CREATE_ALWAYS,0,NULL);
	if(hf == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	BITMAPFILEHEADER bfh;  //Set bitmap header
	ZeroMemory(&bfh,sizeof(bfh));
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh) + lBufferSize + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	// Write the file header.
	DWORD dwWritten = 0;
	WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );    
	// Write the file Format
	BITMAPINFOHEADER bih;
	ZeroMemory(&bih,sizeof(bih));
	bih.biSize = sizeof( bih );
	bih.biWidth = m_lWidth;
	bih.biHeight = m_lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = m_iBitCount;  //Specifies the number of bits per pixel (bpp)
	WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );
	//Write the file Data
	WriteFile( hf, pBuffer, lBufferSize, &dwWritten, NULL );
	CloseHandle( hf );
	return 0;
}