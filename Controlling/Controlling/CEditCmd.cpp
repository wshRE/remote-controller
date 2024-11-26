// CEditCmd.cpp: 实现文件
//

#include "pch.h"
#include "Controlling.h"
#include <memory>
#include <Proto.h>
#include "CmdDlg.h"
#include <thread>
#include"Socket.h"
using namespace std;

IMPLEMENT_DYNAMIC(CEditCmd, CEdit)

CEditCmd::CEditCmd()
{
	std::thread(&CEditCmd::Thread, this).detach();
	shared_ptr<PkgHeader> pPkg = MakePackage<C2RCmd>(0);
	SendPackage(sock, pPkg);//发送
}

CEditCmd::~CEditCmd()
{
	//管道关闭
}


BEGIN_MESSAGE_MAP(CEditCmd, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CEditCmd 消息处理程序




void CEditCmd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (/*m_bCmd == TRUE &&*/ nChar == VK_RETURN) {
		CString cBuff;
		this->GetWindowText(cBuff);  //获取全部文本
		int nIndex = cBuff.ReverseFind('>'); //截取输入的文本
		if (nIndex != -1)
		{
			//右侧数据提取
			cBuff = cBuff.Right((cBuff.GetLength() - 1) - nIndex);
			cBuff += "\r\n";
			char* szBuff = cBuff.GetBuffer(cBuff.GetLength());
			//构造返回包
			shared_ptr<PkgHeader> pPkg = MakePackage<C2RCmd>(strlen(szBuff), (uint8_t*)szBuff);
			SendPackage(sock, pPkg);//发送
			m_bCmd = FALSE;//获取返回
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CEditCmd::Thread()
{
	while (true) {
		//接收执行数据
		std::shared_ptr<PkgHeader> pPkg = RecvPackage(sock);
		if (pPkg->m_pt != R2C_CMD) {
			return;
		}
		R2CCmd* pScrPkg = (R2CCmd*)pPkg.get();
		uint32_t nDataLen = pScrPkg->m_nLen;
		uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据

		char Buff[0x1000];
		ZeroMemory(Buff, 0x1000);
		strcpy_s(Buff, nDataLen, (char*)pData);


		CString oldStr;
		this->GetWindowText(oldStr);
		oldStr.Append(Buff);      
		this->SetWindowText(oldStr);
	}
}


void CEditCmd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
