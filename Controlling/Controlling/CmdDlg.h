#pragma once
#include "afxdialogex.h"
#include "CEditCmd.h"


// CmdDlg 对话框

class CmdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CmdDlg)

public:
	CmdDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CmdDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CMD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	SOCKET m_sockClient;
	virtual BOOL OnInitDialog();
	CEditCmd m_cEdit;
};
