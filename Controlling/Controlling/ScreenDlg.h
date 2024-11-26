#pragma once
#include "afxdialogex.h"


// ScreenDlg 对话框

class ScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ScreenDlg)

public:
	ScreenDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ScreenDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCREEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void ScreenShow();
	HANDLE g_hThread = NULL;
	BOOL m_bIsRun = TRUE;
	int nRecvWidth ;
	int	nRecvHeight;
	SOCKET m_sockClient;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
