
// MFC_ServerDlg.h: 头文件
//

#pragma once

UINT ThreadConn(LPVOID pParam);
UINT ThreadListen(LPVOID pParam);

// CMFCServerDlg 对话框
class CMFCServerDlg : public CDialogEx
{
// 构造
public:
	CMFCServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_SERVER_DIALOG };
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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit m_display;
	CButton m_button1;
	CEdit m_portnum;
	afx_msg void OnBnClickedButton2();
};
