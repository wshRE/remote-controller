
// ControllingDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Controlling.h"
#include "ControllingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ScreenDlg.h"
#include <memory>
#include <Proto.h>
#include "CmdDlg.h"
#include"Socket.h"
#include "CFileDlg.h"
using namespace std;

SOCKET sock;
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
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
//ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CControllingDlg 对话框



CControllingDlg::CControllingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONTROLLING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CControllingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CMD, m_bCmd);
	DDX_Control(pDX, IDC_BUTTON_SCREEN, m_bScreen);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_bFile);
}

BEGIN_MESSAGE_MAP(CControllingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LINL, &CControllingDlg::OnBnClickedButtonLinl)
	ON_BN_CLICKED(IDC_BUTTON_SCREEN, &CControllingDlg::OnBnClickedButtonScreen)
	ON_BN_CLICKED(IDC_BUTTON_CMD, &CControllingDlg::OnBnClickedButtonCmd)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CControllingDlg::OnBnClickedButtonFile)
END_MESSAGE_MAP()


// CControllingDlg 消息处理程序

BOOL CControllingDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CControllingDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CControllingDlg::OnPaint()
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
HCURSOR CControllingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//链接
void CControllingDlg::OnBnClickedButtonLinl()
{
	// TODO: 在此添加控件通知处理程序代码
	m_sockClient = socket(AF_INET,
		SOCK_STREAM, //数据流
		IPPROTO_TCP  //tcp协议
	);
	if (m_sockClient == INVALID_SOCKET)
	{
		AfxMessageBox("socket failed");
		return;
	}
	sock = m_sockClient;

	//连接服务器
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x9527);
	si.sin_addr.S_un.S_addr = inet_addr("192.168.202.128");
	//si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	int nRet = connect(m_sockClient, (sockaddr*)&si, sizeof(si));
	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox("connect failed");
		return;
	}
	else
	{
		AfxMessageBox("连接成功");
		GetDlgItem(IDC_BUTTON_LINL)->SetWindowText("已链接");
	}
	m_bCmd.EnableWindow(1); 
	m_bScreen.EnableWindow(1);
	m_bFile.EnableWindow(1);
}


//投屏页
void CControllingDlg::OnBnClickedButtonScreen()
{
	// TODO: 在此添加控件通知处理程序代码
	ScreenDlg dlg;
	dlg.m_sockClient = m_sockClient;
	dlg.DoModal();
}






BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//CMD页面
void CControllingDlg::OnBnClickedButtonCmd()
{
	CmdDlg dlg;
	dlg.m_sockClient = m_sockClient;
	dlg.DoModal();
}



//文件
void CControllingDlg::OnBnClickedButtonFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDlg dlg;
	dlg.m_sockClient = m_sockClient;
	dlg.DoModal();
}
