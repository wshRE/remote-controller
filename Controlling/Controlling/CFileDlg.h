#pragma once
#include "afxdialogex.h"


// CFileDlg 对话框

class CFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CTreeCtrl m_tFileTree;   //文件树
		afx_msg void OnItemexpandingTreeFile(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkTreeFile(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_lTreeFile;   //列表框
		afx_msg void OnRdblclkListFile(NMHDR* pNMHDR, LRESULT* pResult);  
		afx_msg void OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult);


	//文件
	SOCKET m_sockClient;
	afx_msg void OnRclickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	void FindSocketChild(CString csPath);
	void GetSocketDriveStrings(int nSize, char* szBuff);
	BOOL FindSocketFile(CString csPath);
	afx_msg void OnDown();//下载
	afx_msg void OnUp();  //上传
	CString SelectFloderPath();//选择文件夹
	CString SelectFilesPath();//选择文件


	void DelChild(HTREEITEM hmyItem);               //清空子节点
	CString GetItemPath(HTREEITEM hItem);           //获取路径
	void PathFileReadANDShow(CString csPath, HTREEITEM hItem); //获取路径下的文件并插入
	bool IsFolderEmpty(CString strPath); //判断文件夹是否为空
};
