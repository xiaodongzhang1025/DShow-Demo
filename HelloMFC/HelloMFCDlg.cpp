
// HelloMFCDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "HelloMFC.h"
#include "HelloMFCDlg.h"
#include "afxdialogex.h"

#include "locale.h"
#include "CaptureVideo.h"
CaptureVideo g_CaptureVideo;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Log(CHAR* pBuf)
{
	int len = strlen(pBuf);
	WCHAR* pBufW = new WCHAR[len + 1];
	wmemset(pBufW, 0, len + 1);
	mbstowcs(pBufW, pBuf, len);

	CEdit* pEdit = (CEdit*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT_LOG);
	int iLen = pEdit->GetWindowTextLength();
	pEdit->SetSel(iLen, iLen, TRUE);
	//pEdit->SetSel(-1,-1);
	pEdit->ReplaceSel(pBufW);
	delete pBufW;

	FILE* pMyFile = NULL;
	if (fopen_s(&pMyFile, "HelloMFCLog.txt", "a") == 0)
	{
		fwrite(pBuf, sizeof(CHAR), len, pMyFile);
		fclose(pMyFile);
	}
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHelloMFCDlg 对话框



CHelloMFCDlg::CHelloMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HELLOMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHelloMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VIDEO_DEV, mComboBoxVideoDevs);
	DDX_Control(pDX, IDC_EDIT_LOG, mEditLog);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_hVideoCapture);
	DDX_Control(pDX, IDC_COMBO_VIDEO_INFO, mComboBoxDevInfo);
}

BEGIN_MESSAGE_MAP(CHelloMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHelloMFCDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHelloMFCDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CHelloMFCDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CHelloMFCDlg::OnBnClickedButtonImage)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRIGHTNESS, &CHelloMFCDlg::OnNMCustomdrawSliderBrightness)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONTRAST, &CHelloMFCDlg::OnNMCustomdrawSliderContrast)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HUE, &CHelloMFCDlg::OnNMCustomdrawSliderHue)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SATURATION, &CHelloMFCDlg::OnNMCustomdrawSliderSaturation)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GAIN, &CHelloMFCDlg::OnNMCustomdrawSliderGain)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_WHITE_BALANCE, &CHelloMFCDlg::OnNMCustomdrawSliderWhiteBalance)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BLACK_LIGHT_COMPENSATION, &CHelloMFCDlg::OnNMCustomdrawSliderBlackLightCompensation)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EXPOSURE, &CHelloMFCDlg::OnNMCustomdrawSliderExposure)
	ON_BN_CLICKED(IDC_CHECK_BRIGHTNESS, &CHelloMFCDlg::OnBnClickedCheckBrightness)
	ON_BN_CLICKED(IDC_CHECK_CONTRAST, &CHelloMFCDlg::OnBnClickedCheckContrast)
	ON_BN_CLICKED(IDC_CHECK_HUE, &CHelloMFCDlg::OnBnClickedCheckHue)
	ON_BN_CLICKED(IDC_CHECK_SATURATION, &CHelloMFCDlg::OnBnClickedCheckSaturation)
	ON_BN_CLICKED(IDC_CHECK_GAIN, &CHelloMFCDlg::OnBnClickedCheckGain)
	ON_BN_CLICKED(IDC_CHECK_WHITE_BALANCE, &CHelloMFCDlg::OnBnClickedCheckWhiteBalance)
	ON_BN_CLICKED(IDC_CHECK_BLACK_LIGHT_COMPENSATION, &CHelloMFCDlg::OnBnClickedCheckBlackLightCompensation)
	ON_BN_CLICKED(IDC_CHECK_EXPOSURE, &CHelloMFCDlg::OnBnClickedCheckExposure)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CHelloMFCDlg::OnBnClickedButtonDefault)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_INFO, &CHelloMFCDlg::OnCbnSelchangeComboVideoInfo)
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_DEV, &CHelloMFCDlg::OnCbnSelchangeComboVideoDev)
END_MESSAGE_MAP()


// CHelloMFCDlg 消息处理程序

BOOL CHelloMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);
	//ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	SetWindowText(L"HelloMFC");
	setlocale(LC_ALL, "chs");
	{
		int btnW = 88;
		int btnH = 30;
		int dlgW = 1060;
		int dlgH = 750;
		CRect tmpRect;
		GetClientRect(&tmpRect);
		::MoveWindow(m_hWnd, tmpRect.left, tmpRect.top, dlgW, dlgH, TRUE);

		tmpRect.left = 10; tmpRect.right = 90; tmpRect.top = 10; tmpRect.bottom = 30;
		GetDlgItem(IDC_STATIC_TIP)->MoveWindow(tmpRect);
		tmpRect.left = 90; tmpRect.right = 250; tmpRect.top = 10; tmpRect.bottom = 30;
		GetDlgItem(IDC_COMBO_VIDEO_DEV)->MoveWindow(tmpRect);

		tmpRect.left = 410; tmpRect.right = 490; tmpRect.top = 10; tmpRect.bottom = 30;
		GetDlgItem(IDC_STATIC_TIP2)->MoveWindow(tmpRect);
		tmpRect.left = 490; tmpRect.right = 650; tmpRect.top = 10; tmpRect.bottom = 30;
		GetDlgItem(IDC_COMBO_VIDEO_INFO)->MoveWindow(tmpRect);

		tmpRect.left = 10; tmpRect.right = 10 + 640 + 20; tmpRect.top = 90; tmpRect.bottom = 90 + 480 + 30;
		GetDlgItem(IDC_STATIC_GROUP)->MoveWindow(tmpRect);
		tmpRect.left = 20; tmpRect.right = 20 + 640; tmpRect.top = 110; tmpRect.bottom = 110 + 480;
		GetDlgItem(IDC_STATIC_IMAGE)->MoveWindow(tmpRect);
		tmpRect.left = 10; tmpRect.right = dlgW - 30; tmpRect.top = 110 + 480 + 20; tmpRect.bottom = dlgH - 50;
		GetDlgItem(IDC_EDIT_LOG)->MoveWindow(tmpRect);

		tmpRect.left = 10 + 670; tmpRect.right = tmpRect.left + 260; tmpRect.top = 90; tmpRect.bottom = 90 + 480 + 30;
		GetDlgItem(IDC_STATIC_PROPERTY)->MoveWindow(tmpRect);

		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_BRIGHTNESS)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 1; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_CONTRAST)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 2; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_HUE)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 3; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_SATURATION)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 4; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_GAIN)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 5; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_WHITE_BALANCE)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 6; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_BLACK_LIGHT_COMPENSATION)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 680; tmpRect.right = tmpRect.left + 50; tmpRect.top = 20 + 90 + 40 * 7; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_STATIC_EXPOSURE)->MoveWindow(tmpRect);

		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_BRIGHTNESS)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 1; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_CONTRAST)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 2; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_HUE)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 3; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_SATURATION)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 4; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_GAIN)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 5; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_WHITE_BALANCE)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 6; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION)->MoveWindow(tmpRect);
		tmpRect.left = 10 + 730; tmpRect.right = tmpRect.left + 140; tmpRect.top = 20 + 90 + 40 * 7; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_SLIDER_EXPOSURE)->MoveWindow(tmpRect);

		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_BRIGHTNESS)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 1; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_CONTRAST)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 2; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_HUE)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 3; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_SATURATION)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 4; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_GAIN)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 5; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_WHITE_BALANCE)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 6; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_BLACK_LIGHT_COMPENSATION)->MoveWindow(tmpRect);
		tmpRect.left = 15 + 870; tmpRect.right = tmpRect.left + 40; tmpRect.top = 20 + 90 + 40 * 7; tmpRect.bottom = tmpRect.top + 20;
		GetDlgItem(IDC_CHECK_EXPOSURE)->MoveWindow(tmpRect);

		tmpRect.left = 10 + 730 + (140 - btnW) / 2; tmpRect.right = tmpRect.left + btnW; tmpRect.top = 600 - btnH - 20; tmpRect.bottom = tmpRect.top + btnH;
		GetDlgItem(IDC_BUTTON_DEFAULT)->MoveWindow(tmpRect);

		tmpRect.left = dlgW - 20 - btnW; tmpRect.right = dlgW - 20; tmpRect.top = 90; tmpRect.bottom = tmpRect.top + btnH;
		GetDlgItem(IDOK)->MoveWindow(tmpRect);
		tmpRect.left = dlgW - 20 - btnW; tmpRect.right = dlgW - 20; tmpRect.top = 90 + 70 * 1; tmpRect.bottom = tmpRect.top + btnH;
		GetDlgItem(IDC_BUTTON_STOP)->MoveWindow(tmpRect);
		tmpRect.left = dlgW - 20 - btnW; tmpRect.right = dlgW - 20; tmpRect.top = 90 + 70 * 2; tmpRect.bottom = tmpRect.top + btnH;
		GetDlgItem(IDC_BUTTON_IMAGE)->MoveWindow(tmpRect);
		tmpRect.left = dlgW - 20 - btnW; tmpRect.right = dlgW - 20; tmpRect.top = 90 + 70 * 6; tmpRect.bottom = tmpRect.top + btnH;
		GetDlgItem(IDCANCEL)->MoveWindow(tmpRect);



		int delayTime = 50;
		mToolTipBrightness.Create(this);
		mToolTipBrightness.AddTool(GetDlgItem(IDC_SLIDER_BRIGHTNESS), TTS_ALWAYSTIP);
		mToolTipBrightness.SetDelayTime(delayTime);//ms

		mToolTipContrast.Create(this);
		mToolTipContrast.AddTool(GetDlgItem(IDC_SLIDER_CONTRAST), TTS_ALWAYSTIP);
		mToolTipContrast.SetDelayTime(delayTime);//ms

		mToolTipHue.Create(this);
		mToolTipHue.AddTool(GetDlgItem(IDC_SLIDER_HUE), TTS_ALWAYSTIP);
		mToolTipHue.SetDelayTime(delayTime);//ms

		mToolTipSaturation.Create(this);
		mToolTipSaturation.AddTool(GetDlgItem(IDC_SLIDER_SATURATION), TTS_ALWAYSTIP);
		mToolTipSaturation.SetDelayTime(delayTime);//ms

		mToolTipGain.Create(this);
		mToolTipGain.AddTool(GetDlgItem(IDC_SLIDER_GAIN), TTS_ALWAYSTIP);
		mToolTipGain.SetDelayTime(delayTime);//ms

		mToolTipWhiteBalance.Create(this);
		mToolTipWhiteBalance.AddTool(GetDlgItem(IDC_SLIDER_WHITE_BALANCE), TTS_ALWAYSTIP);
		mToolTipWhiteBalance.SetDelayTime(delayTime);//ms

		mToolTipBlackLight.Create(this);
		mToolTipBlackLight.AddTool(GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION), TTS_ALWAYSTIP);
		mToolTipBlackLight.SetDelayTime(delayTime);//ms

		mToolTipExposure.Create(this);
		mToolTipExposure.AddTool(GetDlgItem(IDC_SLIDER_EXPOSURE), TTS_ALWAYSTIP);
		mToolTipExposure.SetDelayTime(delayTime);//ms
	}

	m_videoOpen = FALSE;
	g_CaptureVideo.m_App = m_hVideoCapture;
	g_CaptureVideo.EnumAllDevices(mComboBoxVideoDevs.m_hWnd); //Enum All Camera
	int nGetComboxCount = mComboBoxVideoDevs.GetCount();
	if (nGetComboxCount == 0)
		mComboBoxVideoDevs.EnableWindow(FALSE);
	else {
		mComboBoxVideoDevs.SetCurSel(0);
		OnCbnSelchangeComboVideoDev();
	}

	if (g_CaptureVideo.m_nCaptureDeviceNumber == 0)
	{
		AfxMessageBox(L"没有摄像头设备");
	}
	SetTimer(1, 300, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHelloMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHelloMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHelloMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHelloMFCDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//AfxMessageBox(L"YouPressedOkButton!!!");
	myCapStart();
	//CDialogEx::OnOK();
}


void CHelloMFCDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	OnBnClickedButtonStop();
	CDialogEx::OnCancel();
}


void CHelloMFCDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	myCapStop();
}


void CHelloMFCDlg::OnBnClickedButtonImage()
{
	// TODO: 在此添加控件通知处理程序代码
	myCapGetFrame();
}


void CHelloMFCDlg::OnBnClickedButtonDefault()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pCameraControl != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pCameraControl->GetRange(CameraControl_Exposure, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_EXPOSURE);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pCameraControl->Get(CameraControl_Exposure, &Val, &Flags);
			g_CaptureVideo.m_pCameraControl->Set(CameraControl_Exposure, Default, Flags);
			pSlider->SetPos(Default);
		}
	}
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Brightness, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_BRIGHTNESS);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Brightness, Default, Flags);
			pSlider->SetPos(Default);
		}

		hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Contrast, &Min, &Max, &Step, &Default, &Flags);
		pCheck = (CButton*)GetDlgItem(IDC_CHECK_CONTRAST);
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Contrast, Default, Flags);
			pSlider->SetPos(Default);
		}

		hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Hue, &Min, &Max, &Step, &Default, &Flags);
		pCheck = (CButton*)GetDlgItem(IDC_CHECK_HUE);
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HUE);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Hue, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Hue, Default, Flags);
			pSlider->SetPos(Default);
		}

		hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Saturation, &Min, &Max, &Step, &Default, &Flags);
		pCheck = (CButton*)GetDlgItem(IDC_CHECK_SATURATION);
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SATURATION);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Saturation, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Saturation, Default, Flags);
			pSlider->SetPos(Default);
		}

		hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Gain, &Min, &Max, &Step, &Default, &Flags);
		pCheck = (CButton*)GetDlgItem(IDC_CHECK_GAIN);
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Gain, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Gain, Default, Flags);
			pSlider->SetPos(Default);
		}

		hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_WhiteBalance, &Min, &Max, &Step, &Default, &Flags);
		pCheck = (CButton*)GetDlgItem(IDC_CHECK_WHITE_BALANCE);
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WHITE_BALANCE);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_WhiteBalance, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_WhiteBalance, Default, Flags);
			pSlider->SetPos(Default);
		}

		hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_BacklightCompensation, &Min, &Max, &Step, &Default, &Flags);
		pCheck = (CButton*)GetDlgItem(IDC_CHECK_BLACK_LIGHT_COMPENSATION);
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_BacklightCompensation, &Val, &Flags);
			g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_BacklightCompensation, Default, Flags);
			pSlider->SetPos(Default);
		}
	}
}

void CHelloMFCDlg::OnNMCustomdrawSliderBrightness(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_BRIGHTNESS))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Brightness, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipBrightness.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderContrast(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_CONTRAST))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Contrast, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipContrast.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderHue(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_HUE))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HUE);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Hue, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipHue.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderSaturation(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_SATURATION))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SATURATION);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Saturation, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipSaturation.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderGain(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_GAIN))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Gain, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipGain.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderWhiteBalance(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_WHITE_BALANCE))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WHITE_BALANCE);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_WhiteBalance, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipWhiteBalance.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderBlackLightCompensation(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_BLACK_LIGHT_COMPENSATION))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_BacklightCompensation, Val, VideoProcAmp_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipBlackLight.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnNMCustomdrawSliderExposure(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_videoOpen == TRUE && ((CButton*)GetDlgItem(IDC_CHECK_EXPOSURE))->GetCheck() == FALSE)
	{
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
		int Val = pSlider->GetPos();
		int Min = pSlider->GetRangeMin();
		int Max = pSlider->GetRangeMax();
		g_CaptureVideo.m_pCameraControl->Set(CameraControl_Exposure, Val, CameraControl_Flags_Manual);

		CString str;
		str.Format(L"%d", Val);
		mToolTipExposure.UpdateTipText(str, pSlider);
	}
	*pResult = 0;
}


void CHelloMFCDlg::OnBnClickedCheckBrightness()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Brightness, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_BRIGHTNESS);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Brightness, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Brightness, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}


void CHelloMFCDlg::OnBnClickedCheckContrast()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Contrast, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_CONTRAST);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Contrast, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Contrast, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}


void CHelloMFCDlg::OnBnClickedCheckHue()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Hue, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_HUE);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HUE);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Hue, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Hue, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Hue, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}


void CHelloMFCDlg::OnBnClickedCheckSaturation()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Saturation, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_SATURATION);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SATURATION);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Saturation, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Saturation, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Saturation, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}


void CHelloMFCDlg::OnBnClickedCheckGain()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Gain, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_GAIN);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Gain, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Gain, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_Gain, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}


void CHelloMFCDlg::OnBnClickedCheckWhiteBalance()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_WhiteBalance, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_WHITE_BALANCE);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WHITE_BALANCE);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_WhiteBalance, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_WhiteBalance, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_WhiteBalance, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}

void CHelloMFCDlg::OnBnClickedCheckBlackLightCompensation()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pProcAmp != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_BacklightCompensation, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_STATIC_BLACK_LIGHT_COMPENSATION);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_BacklightCompensation, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_BacklightCompensation, 0, VideoProcAmp_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pProcAmp->Set(VideoProcAmp_BacklightCompensation, Val, VideoProcAmp_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}

void CHelloMFCDlg::OnBnClickedCheckExposure()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_CaptureVideo.m_pCameraControl != NULL && m_videoOpen == TRUE)
	{
		long Min, Max, Step, Default, Flags, Val;

		HRESULT hr = g_CaptureVideo.m_pCameraControl->GetRange(CameraControl_Exposure, &Min, &Max, &Step, &Default, &Flags);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_EXPOSURE);
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
		if (SUCCEEDED(hr))
		{
			hr = g_CaptureVideo.m_pCameraControl->Get(CameraControl_Exposure, &Val, &Flags);
			if (pCheck->GetCheck() == TRUE)
			{
				g_CaptureVideo.m_pCameraControl->Set(CameraControl_Exposure, 0, CameraControl_Flags_Auto);
				pSlider->EnableWindow(FALSE);
			}
			else {
				g_CaptureVideo.m_pCameraControl->Set(CameraControl_Exposure, Val, CameraControl_Flags_Manual);
				pSlider->EnableWindow(TRUE);
			}

		}
	}
}

int CHelloMFCDlg::myCapStart()
{
	int iGetCurSel = mComboBoxVideoDevs.GetCurSel();
	CRect rect;
	m_hVideoCapture.GetClientRect(&rect);
	int iInfoSel = mComboBoxDevInfo.GetCurSel();
	HRESULT hr = g_CaptureVideo.OpenDevice(iGetCurSel, iInfoSel, 0, 0, rect.Width(), rect.Height());
	if (SUCCEEDED(hr))
	{
		m_videoOpen = TRUE;
		//获取属性值
		if (g_CaptureVideo.m_pProcAmp != NULL)
		{
			long Min, Max, Step, Default, Flags, Val;
			// 亮度.            
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Brightness, &Min, &Max, &Step, &Default, &Flags);
			CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
			CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_BRIGHTNESS);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}
			//对比度          
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Contrast, &Min, &Max, &Step, &Default, &Flags);
			pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
			pCheck = (CButton*)GetDlgItem(IDC_CHECK_CONTRAST);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Contrast, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}
			//色调          
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Hue, &Min, &Max, &Step, &Default, &Flags);
			pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HUE);
			pCheck = (CButton*)GetDlgItem(IDC_CHECK_HUE);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Hue, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}

			//饱和度      
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Saturation, &Min, &Max, &Step, &Default, &Flags);
			pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SATURATION);
			pCheck = (CButton*)GetDlgItem(IDC_CHECK_SATURATION);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Saturation, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}
			//增益         
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_Gain, &Min, &Max, &Step, &Default, &Flags);
			pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
			pCheck = (CButton*)GetDlgItem(IDC_CHECK_GAIN);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_Gain, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}
			//白平衡         
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_WhiteBalance, &Min, &Max, &Step, &Default, &Flags);
			pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WHITE_BALANCE);
			pCheck = (CButton*)GetDlgItem(IDC_CHECK_WHITE_BALANCE);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_WhiteBalance, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}

			//逆光补偿    
			hr = g_CaptureVideo.m_pProcAmp->GetRange(VideoProcAmp_BacklightCompensation, &Min, &Max, &Step, &Default, &Flags);
			pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION);
			pCheck = (CButton*)GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pProcAmp->Get(VideoProcAmp_BacklightCompensation, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}
			Invalidate(FALSE);
			Invalidate(TRUE);
		}

		if (g_CaptureVideo.m_pCameraControl != NULL)
		{
			long Min, Max, Step, Default, Flags, Val;
			hr = g_CaptureVideo.m_pCameraControl->GetRange(CameraControl_Exposure, &Min, &Max, &Step, &Default, &Flags);
			CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
			CButton* pCheck = (CButton*)GetDlgItem(IDC_CHECK_EXPOSURE);
			if (SUCCEEDED(hr))
			{
				if (Flags & VideoProcAmp_Flags_Auto)
				{
					pCheck->EnableWindow(TRUE);
				}
				else {
					pCheck->EnableWindow(FALSE);
				}
				hr = g_CaptureVideo.m_pCameraControl->Get(CameraControl_Exposure, &Val, &Flags);
				if (Flags == VideoProcAmp_Flags_Auto)
				{
					pCheck->SetCheck(TRUE);
					pSlider->EnableWindow(FALSE);
				}
				else {
					pCheck->SetCheck(FALSE);
					pSlider->EnableWindow(TRUE);
				}

				pSlider->SetRange(Min, Max);
				pSlider->SetPos(Val);
				pSlider->SetPageSize(Step);
			}
			else {
				pCheck->EnableWindow(FALSE);
				pCheck->SetCheck(FALSE);
				pSlider->EnableWindow(FALSE);
			}

			Invalidate(FALSE);
			Invalidate(TRUE);
		}

		return TRUE;
	}
	else {
		return FALSE;
	}

}

int CHelloMFCDlg::myCapStop()
{
	g_CaptureVideo.CloseInterface();
	m_videoOpen = FALSE;
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
	pSlider->EnableWindow(FALSE);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
	pSlider->EnableWindow(FALSE);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HUE);
	pSlider->EnableWindow(FALSE);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SATURATION);
	pSlider->EnableWindow(FALSE);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
	pSlider->EnableWindow(FALSE);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WHITE_BALANCE);
	pSlider->EnableWindow(FALSE);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BLACK_LIGHT_COMPENSATION);
	pSlider->EnableWindow(FALSE);
	return TRUE;

}
int CHelloMFCDlg::myCapGetFrame()
{
	g_CaptureVideo.GrabOneFrame(TRUE);
	return TRUE;
}


void CHelloMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		//TRACE("Timer1 300ms\r\n");
		if (m_videoOpen == TRUE)
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_VIDEO_DEV)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_IMAGE)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_DEFAULT)->EnableWindow(TRUE);
			CHAR tmpBuf[1000];
			WCHAR tmpBufW[1000];
			memset(tmpBuf, 0, 1000);
			wmemset(tmpBufW, 0, 1000);
		}
		else {
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_VIDEO_DEV)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_IMAGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_DEFAULT)->EnableWindow(FALSE);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CHelloMFCDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		//判断是否按下键盘F2键
		if (pMsg->wParam == VK_F2)
		{
			mEditLog.SetWindowTextW(L"");
			return TRUE;
		}
	}
	if (pMsg->message == WM_MOUSEMOVE)
	{
		CRect tmpRect;
		GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(&tmpRect);
		//ScreenToClient(&tmpRect);
		CPoint pt = pMsg->pt;
		if (pt.x >= tmpRect.left && pt.x < tmpRect.right && pt.y >= tmpRect.top && pt.y < tmpRect.bottom)
		{
			//g_MouseInImageX = pt.x - tmpRect.left;
			//g_MouseInImageY = pt.y - tmpRect.top;
			//g_MouseInImage = TRUE;
			//TRACE("%d, %d\r\n", g_MouseInImageX, g_MouseInImageY);
		}
		else {
			//g_MouseInImage = FALSE;
		}
	}

	mToolTipBrightness.RelayEvent(pMsg);
	mToolTipContrast.RelayEvent(pMsg);
	mToolTipHue.RelayEvent(pMsg);
	mToolTipSaturation.RelayEvent(pMsg);
	mToolTipGain.RelayEvent(pMsg);
	mToolTipWhiteBalance.RelayEvent(pMsg);
	mToolTipBlackLight.RelayEvent(pMsg);
	mToolTipExposure.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}


LRESULT CHelloMFCDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (message == WM_DEVICECHANGE)
	{
		if (m_videoOpen == FALSE)
		{
			mComboBoxDevInfo.ResetContent();

			mComboBoxVideoDevs.ResetContent();
			g_CaptureVideo.m_App = m_hVideoCapture;
			g_CaptureVideo.EnumAllDevices(mComboBoxVideoDevs.m_hWnd); //Enum All Camera
			int nGetComboxCount = mComboBoxVideoDevs.GetCount();
			if (nGetComboxCount == 0)
				mComboBoxVideoDevs.EnableWindow(FALSE);
			else
				mComboBoxVideoDevs.SetCurSel(0);
		}
	}
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}



void CHelloMFCDlg::OnCbnSelchangeComboVideoDev()
{
	mComboBoxDevInfo.ResetContent();
	int iGetCurSel = mComboBoxVideoDevs.GetCurSel();
	CRect rect;
	m_hVideoCapture.GetClientRect(&rect);
	HRESULT hr = g_CaptureVideo.OpenDevice(iGetCurSel, -1, 0, 0, rect.Width(), rect.Height());
	if (SUCCEEDED(hr))
	{
		g_CaptureVideo.ListDeviceRes(iGetCurSel, mComboBoxDevInfo.m_hWnd);
		mComboBoxDevInfo.SetCurSel(0);
		int nGetComboxCount = mComboBoxDevInfo.GetCount();
		if (nGetComboxCount == 0)
			mComboBoxDevInfo.EnableWindow(FALSE);
		else
			mComboBoxDevInfo.SetCurSel(0);

		Invalidate(FALSE);
		Invalidate(TRUE);
	}
	g_CaptureVideo.CloseInterface();

	m_videoOpen = FALSE;
	// TODO: 在此添加控件通知处理程序代码
}


void CHelloMFCDlg::OnCbnSelchangeComboVideoInfo()
{
	// TODO: 在此添加控件通知处理程序代码
}
