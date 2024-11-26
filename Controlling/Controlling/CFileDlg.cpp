// CFileDlg.cpp: 实现文件
//

#include "pch.h"
#include "Controlling.h"
#include "afxdialogex.h"
#include "CFileDlg.h"
#include <memory>
#include <Proto.h>
#include <vector>
using namespace std;
CString csLastFile; // 最后一个双击的路径
vector<m_ChildFile> m_vChild;
HTREEITEM hNodeLastFile;    //最后一个双击的句柄
// CFileDlg 对话框

IMPLEMENT_DYNAMIC(CFileDlg, CDialogEx)

CFileDlg::CFileDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILE, pParent)
{

}

CFileDlg::~CFileDlg()
{
}

void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_lTreeFile);
	DDX_Control(pDX, IDC_TREE_FILE, m_tFileTree);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_FILE, &CFileDlg::OnDblclkTreeFile)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_FILE, &CFileDlg::OnItemexpandingTreeFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileDlg::OnDblclkListFile)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CFileDlg::OnRclickListFile)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_FILE, &CFileDlg::OnRdblclkListFile)
	ON_COMMAND(ID_DOWN, &CFileDlg::OnDown)
	ON_COMMAND(ID_UP, &CFileDlg::OnUp)
END_MESSAGE_MAP()


// CFileDlg 消息处理程序


BOOL CFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//获取全部盘符
	char szBuff[MAXBYTE] = {};
	GetSocketDriveStrings(MAXBYTE, szBuff);
	//盘符拆分
	int nBuff = 0;
	char szDrive[MAXBYTE] = {};
	int nDrive = 0;
	HTREEITEM hTree;
	while (szBuff[nBuff]) {
		nDrive = 0;
		while (szBuff[nBuff]) {
			szDrive[nDrive] = szBuff[nBuff];
			nDrive++;
			nBuff++;
		}
		szDrive[nDrive] = '\0';

		/*MFC_Dir_A_S(szDrive);*/
		//添加所有盘符并添加读取中的提示
		hTree = m_tFileTree.InsertItem(szDrive);
		m_tFileTree.InsertItem("读取中", hTree);
		nBuff++;
	}

	//设置风格，树带线
	m_tFileTree.ModifyStyle(0,
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EX_FADEINOUTEXPANDOS
	);


	//文本框设置
	DWORD dwStyle = m_lTreeFile.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）  
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）  
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件  
	m_lTreeFile.SetExtendedStyle(dwStyle); //设置扩展风格  
	int nColIdx = 0;
	m_lTreeFile.InsertColumn(nColIdx++, "名称", LVCFMT_CENTER, 200);
	m_lTreeFile.InsertColumn(nColIdx++, "修改日期", LVCFMT_CENTER, 200);
	m_lTreeFile.InsertColumn(nColIdx++, "类型", LVCFMT_CENTER, 200);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//展开或收缩
void CFileDlg::OnItemexpandingTreeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	/*规则：
	1.展开：
		1.删掉读取中，获取数据并写入
		2.判断子文件夹下是否有文件夹
			1.有则添加 待写入
			2.没有就不添加
	2.收起：
		1.删除所有子树
		2.添加 读取中
	*/
	HTREEITEM hNode = pNMTreeView->itemNew.hItem;  //获取句柄
	//如果是展开
	if (pNMTreeView->action == TVE_EXPAND) {
		//删除所有子节点
		DelChild(hNode);
		/*标记：插入新的项*/
		CString csPath = GetItemPath(hNode); //获取路径
		//获取文件并插入
		PathFileReadANDShow(csPath, hNode);
	}
	//如果是收起
	else if (pNMTreeView->action == TVE_COLLAPSE) {
		//删除所有子节点
		DelChild(hNode);
		m_tFileTree.InsertItem("读取中", hNode);  //分展开还是收起
	}
	*pResult = 0;
}
//列表双击
void CFileDlg::OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 1.获取选中的文件名
	NMLISTVIEW* pMListView = (NMLISTVIEW*)pNMHDR;
	int nIndex = pMListView->iItem; //选择的行
	CString csFileName = m_lTreeFile.GetItemText(nIndex, 0);
	//AfxMessageBox(a);

	// 2.获取当前树控件的路径
	//csFileName = csLastFile  + csFileName + '\\';
	//AfxMessageBox(csFileName);

	//3.树跳转至新的文件夹内
	if (m_tFileTree.ItemHasChildren(hNodeLastFile))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_tFileTree.GetChildItem(hNodeLastFile);
		while (hChildItem != NULL)
		{
			hNextItem = m_tFileTree.GetNextItem(hChildItem, TVGN_NEXT);
			CString csName = m_tFileTree.GetItemText(hNextItem);
			if (csName == csFileName) {
				m_tFileTree.Expand(hNextItem, TVE_EXPAND); //展开
			}
			hChildItem = hNextItem;
		}
	}
	*pResult = 0;
}
//列表右键
void CFileDlg::OnRclickListFile(NMHDR* pNMHDR, LRESULT* pResult)

{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//下载
	if (pNMListView->iItem != -1) {
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu mnContext;
		mnContext.LoadMenu(IDR_MENU_DOWN);
		CMenu* popup = mnContext.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	//上传
	else {
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu mnContext;
		mnContext.LoadMenu(IDR_MENU_UP);
		CMenu* popup = mnContext.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	*pResult = 0;
}

//下载
void CFileDlg::OnDown()
{
	//获取选中的信息
	// 1.获取选中的文件名
		//获取选中的信息
	POSITION m_pstion = m_lTreeFile.GetFirstSelectedItemPosition();
	//选择项的id
	int m_nIndex = m_lTreeFile.GetNextSelectedItem(m_pstion);
	//名称
	CString cName = m_lTreeFile.GetItemText(m_nIndex, 0);
	//路径
	CString szLoad = csLastFile + cName;
	char* szBuff = szLoad.GetBuffer(szLoad.GetLength());

	//获取保存路径
	CString cDownPath = SelectFloderPath();
	CString cSave = cDownPath + "\\" + cName;

	//下载
	shared_ptr<PkgHeader> pPkg = MakePackage<C2RDown>(strlen(szBuff), (uint8_t*)szBuff);
	SendPackage(m_sockClient, pPkg);

	//获取文本大小
	std::shared_ptr<PkgHeader> pPkg1 = RecvPackage(m_sockClient);
	if (pPkg1->m_pt != R2C_DOWN_SIZE) {
		return;
	}
	R2CDownSize* pScrPkg = (R2CDownSize*)pPkg1.get();
	int nSize = pScrPkg->m_nSize;


	//创建文件
	HANDLE hFile = CreateFile(cSave,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}
	int nRecv = 0;
	int8_t buf[0x1000];
	while (nRecv < nSize) {
		ZeroMemory(buf, 0x1000);
		//收数据
		std::shared_ptr<PkgHeader> pPkg2 = RecvPackage(m_sockClient);
		if (pPkg2->m_pt != R2C_DOWN) {
			continue;
		}
		R2CDown* pScrPkg = (R2CDown*)pPkg2.get();
		uint32_t nDataLen = pScrPkg->m_nLen;
		uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据


		DWORD dwBytesWrited = 0;
		memcpy((char*)buf, (char*)pData, nDataLen);
		WriteFile(hFile, buf, nDataLen, &dwBytesWrited, NULL);
		nRecv += nDataLen;
		//Log("out", nRecv);
	}
	CloseHandle(hFile);
}
//上传
void CFileDlg::OnUp()
{
	/*
	1. 获取选择的文件及其路径
	2.发送文件名
	3.发送文件大小
	4.发送文件
	*/
	//1.
	CString cPath = SelectFilesPath();                          //文件路径
	int nIndex = cPath.ReverseFind('\\');
	CString cName = cPath.Right(cPath.GetLength() - nIndex - 1);//文件名

	//发送文件名（包括保存路径）
	cName = csLastFile + cName;
	char* szBuff = cName.GetBuffer(cName.GetLength());
	shared_ptr<PkgHeader> pPkg = MakePackage<C2RUpPath>(strlen(szBuff), (uint8_t*)szBuff);
	SendPackage(m_sockClient, pPkg);


	//2.返回文件大小
	FILE* f;
	f = fopen(cPath, "rb");   //打开文件
	if (f == NULL) {
		//返回0
		shared_ptr<PkgHeader> pPkg3 = MakePackage<C2RUpSize>(sizeof(int), 0);
		SendPackage(m_sockClient, pPkg3);
		return ;
	}
	fseek(f, 0, SEEK_END);
	int nEnd = ftell(f);//文件大小
	rewind(f); //重置指针
	//发送大小
	shared_ptr<PkgHeader> pPkg3 = MakePackage<C2RUpSize>(sizeof(int), nEnd);
	SendPackage(m_sockClient, pPkg3);
	fclose(f); //关闭


	//发送文件
	HANDLE hFile = CreateFile(cPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return ;
	}

	//每次读一个分页
	int nTol = 0;
	DWORD dwBytesReaded = 0;
	while (true)
	{
		dwBytesReaded = 0;
		uint8_t buf[0x1000];
		ReadFile(hFile, buf, sizeof(buf), &dwBytesReaded, NULL);
		if (dwBytesReaded <= 0)
		{
			break;
		}
		C2RUp rc((uint8_t*)buf, sizeof(buf));
		int nRe = send(m_sockClient,
			(char*)&rc, sizeof(PkgHeader) + sizeof(buf), 0);
		nTol += nRe;
	}
	CloseHandle(hFile);
}

//返回文件夹路径
CString CFileDlg::SelectFloderPath()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择处理结果存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	return strFolderPath;
}
//返回文件路径
CString CFileDlg::SelectFilesPath()
{
	//0.打开文件选择框
	//1.获取来源地址
	CFileDialog    dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, NULL);
	dlg.m_ofn.lpstrTitle = "请选择文件";
	CArray<CString, CString> aryFilename;
	if (dlg.DoModal() == IDOK) {

		POSITION posFile = dlg.GetStartPosition();
		while (posFile != NULL)
		{
			aryFilename.Add(dlg.GetNextPathName(posFile));
		}
	}
	return CStringA(*(aryFilename.GetData()));
}

//删除树中的所有子节点
void CFileDlg::DelChild(HTREEITEM hmyItem)
{
	if (m_tFileTree.ItemHasChildren(hmyItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_tFileTree.GetChildItem(hmyItem);

		while (hChildItem != NULL)
		{
			hNextItem = m_tFileTree.GetNextItem(hChildItem, TVGN_NEXT);
			m_tFileTree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
}
//获取树中某个节点的路径
CString CFileDlg::GetItemPath(HTREEITEM hItem)
{
	CString ItemPath;
	ItemPath.Empty();
	CString TemPath(_T(""));

	while (TRUE)
	{
		HTREEITEM hParentItem = m_tFileTree.GetParentItem(hItem);
		if (hParentItem == NULL)//没有父节点
		{
			ItemPath = m_tFileTree.GetItemText(hItem);
			ItemPath.Delete(ItemPath.GetLength() - 1, 1);
			ItemPath = ItemPath + TemPath;

			break;
		}
		else//有父节点
		{
			TemPath = _T("\\") + m_tFileTree.GetItemText(hItem) + TemPath;
			hItem = m_tFileTree.GetParentItem(hItem);
		}
	}
	return ItemPath + "\\";
}



//废弃
void CFileDlg::OnRdblclkListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}



//Socket
//查找子节点
void CFileDlg::FindSocketChild(CString csPath)
{
	//发申请包
	char* szBuff = csPath.GetBuffer(csPath.GetLength());

	shared_ptr<PkgHeader> pPkg1 = MakePackage<C2RChf>(strlen(szBuff), (uint8_t*)szBuff);
	//发送
	SendPackage(m_sockClient, pPkg1);

	vector<m_ChildFile> vChil;
	//循环收包
	while (true) {
		std::shared_ptr<PkgHeader> pPkg = RecvPackage(m_sockClient);
		if (pPkg->m_pt == R2C_CHFEND) {
			m_vChild = vChil;
			return;
		}
		if (pPkg->m_pt != R2C_CHF) {
			return;
		}
		R2CChf* pScrPkg = (R2CChf*)pPkg.get();
		uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader) + sizeof(int8_t) * 3;//取得数据

		char szName[MAXBYTE];
		char szType[MAXBYTE];
		char szTime[MAXBYTE];
		ZeroMemory(szName, MAXBYTE);
		ZeroMemory(szType, MAXBYTE);
		ZeroMemory(szTime, MAXBYTE);
		memcpy((char*)szName, (char*)pData, pScrPkg->nLenName);
		memcpy((char*)szType, (char*)pData + pScrPkg->nLenName, pScrPkg->nLenType);
		memcpy((char*)szTime, (char*)pData + pScrPkg->nLenName + pScrPkg->nLenType, pScrPkg->nLenTime);
		m_ChildFile ch(szName, szType, szTime);
		vChil.push_back(ch);
	}
}
//获取盘符
void CFileDlg::GetSocketDriveStrings(int nSize, char* szBuff)
{
	//发申请包
	shared_ptr<PkgHeader> pPkg1 = MakePackage<C2RDriver>(0);
	//发送
	SendPackage(m_sockClient, pPkg1);

	//获取返回值
	//收返回包
	std::shared_ptr<PkgHeader> pPkg = RecvPackage(m_sockClient);
	if (pPkg->m_pt != R2C_DRIVER) {
		return;
	}
	R2CCmd* pScrPkg = (R2CCmd*)pPkg.get();
	uint32_t nDataLen = pScrPkg->m_nLen;
	uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据

	ZeroMemory(szBuff, MAXBYTE);
	memcpy((char*)szBuff, (char*)pData, nDataLen);
}
//查找路径是否存在
BOOL CFileDlg::FindSocketFile(CString csPath)
{	//发申请包
	char* szBuff = csPath.GetBuffer(csPath.GetLength());

	shared_ptr<PkgHeader> pPkg1 = MakePackage<C2RFe>(strlen(szBuff), (uint8_t*)szBuff);
	//发送
	SendPackage(m_sockClient, pPkg1);

	std::shared_ptr<PkgHeader> pPkg = RecvPackage(m_sockClient);
	if (pPkg->m_pt != R2C_FE) {
		return false;
	}
	R2CFe* pScrPkg = (R2CFe*)pPkg.get();
	return pScrPkg->m_IsExist;
}




//获取路径下的文件并插入到树 ----需要api
void CFileDlg::PathFileReadANDShow(CString csPath, HTREEITEM hItem)
{
	BOOL bWorking = FindSocketFile(csPath + '*');
	FindSocketChild(csPath + '*');

	for (auto i : m_vChild) {
		if (strcmp(i.m_SzType, "文件夹") == 0) {
			HTREEITEM hItem1 = m_tFileTree.InsertItem(i.m_SzName, hItem);
			if (!IsFolderEmpty(csPath + '\\' + i.m_SzName)) {//如果文件夹非空
				m_tFileTree.InsertItem("读取中", hItem1);
			}
		}
	}
}


//判断文件夹是否为空------需要api
bool CFileDlg::IsFolderEmpty(CString strPath)
{

	CString strFilePaht = strPath + "\\*.*";
	BOOL bRet = FindSocketFile(strFilePaht);
	return bRet;
}


//树双击------------需要api
void CFileDlg::OnDblclkTreeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	m_lTreeFile.DeleteAllItems();
	int nNextRow = 0;
	//1.获取路径
	CString csPath = GetItemPath(m_tFileTree.GetSelectedItem());
	csLastFile = csPath;
	hNodeLastFile = m_tFileTree.GetSelectedItem();
	//2.读取路径下的所有文件并显示在文本框
	//3.循环输出所有的 名称 修改日期  类型  大小

	BOOL bWorking = FindSocketFile(csPath + '*');
	FindSocketChild(csPath + '*');

	for (auto i : m_vChild) {
		nNextRow = m_lTreeFile.GetItemCount();// 下一行的索引
		m_lTreeFile.InsertItem(nNextRow, i.m_SzName); //文件名插入
		m_lTreeFile.SetItemText(nNextRow, 1, i.m_SzTime); //时间插入
		m_lTreeFile.SetItemText(nNextRow, 2, i.m_SzType); //类型插入


		if (strcmp(i.m_SzType, "文件夹") == 0) {
			HTREEITEM hItem1 = m_tFileTree.InsertItem(i.m_SzName, hNodeLastFile);
			if (!IsFolderEmpty(csPath + '\\' + i.m_SzName)) {//如果文件夹非空
				m_tFileTree.InsertItem("读取中", hItem1);
			}
		}
	}
}