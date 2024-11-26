// CmdDlg.cpp: 实现文件
//

#include "pch.h"
#include "Controlling.h"
#include "afxdialogex.h"
#include "CmdDlg.h"
#include <memory>
#include <Proto.h>
#include <thread>
using namespace std;


// CmdDlg 对话框

IMPLEMENT_DYNAMIC(CmdDlg, CDialogEx)

CmdDlg::CmdDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CMD, pParent)
{
}

CmdDlg::~CmdDlg()
{
}

void CmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_CMD, m_cEdit);
}


BEGIN_MESSAGE_MAP(CmdDlg, CDialogEx)
END_MESSAGE_MAP()


// CmdDlg 消息处理程序



BOOL CmdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



