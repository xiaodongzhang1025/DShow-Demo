
// HelloMFCDlg.h: 头文件
//

#pragma once


// CHelloMFCDlg 对话框
class CHelloMFCDlg : public CDialogEx
{
// 构造
public:
	CHelloMFCDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELLOMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonImage();
	CComboBox mComboBoxVideoDevs;
	CComboBox mComboBoxDevInfo;
	CEdit mEditLog;
	CStatic m_hVideoCapture;
	int m_videoOpen;
	CToolTipCtrl mToolTipBrightness;
	CToolTipCtrl mToolTipContrast;
	CToolTipCtrl mToolTipHue;
	CToolTipCtrl mToolTipSaturation;
	CToolTipCtrl mToolTipGain;
	CToolTipCtrl mToolTipWhiteBalance;
	CToolTipCtrl mToolTipBlackLight;
	CToolTipCtrl mToolTipExposure;
	int myCapStart();
	int myCapStop();
	int myCapGetFrame();
	afx_msg void OnNMCustomdrawSliderBrightness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderContrast(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderHue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderSaturation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderGain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderWhiteBalance(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderBlackLightCompensation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderExposure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCheckBrightness();
	afx_msg void OnBnClickedCheckContrast();
	afx_msg void OnBnClickedCheckHue();
	afx_msg void OnBnClickedCheckSaturation();
	afx_msg void OnBnClickedCheckGain();
	afx_msg void OnBnClickedCheckWhiteBalance();
	afx_msg void OnBnClickedCheckBlackLightCompensation();
	afx_msg void OnBnClickedCheckExposure();
	afx_msg void OnStnClickedStaticBlackLightCompensation();
	afx_msg void OnBnClickedButtonDefault();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchangeComboVideoInfo();
	afx_msg void OnCbnSelchangeComboVideoDev();
};
