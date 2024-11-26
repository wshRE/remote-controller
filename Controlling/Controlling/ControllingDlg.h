
// ControllingDlg.h: 头文件
//

#pragma once


// CControllingDlg 对话框
class CControllingDlg : public CDialogEx
{
// 构造
public:
	CControllingDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTROLLING_DIALOG };
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
	afx_msg void OnBnClickedButtonLinl();
	SOCKET m_sockClient;
	CButton m_bCmd;
	CButton m_bScreen;
	CButton m_bFile;
	afx_msg void OnBnClickedButtonScreen();
	afx_msg void OnBnClickedButtonCmd();
	afx_msg void OnBnClickedButtonFile();
};
