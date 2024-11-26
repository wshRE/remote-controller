// ScreenDlg.cpp: 实现文件
//

#include "pch.h"
#include "Controlling.h"
#include "afxdialogex.h"
#include "ScreenDlg.h"
#include <Proto.h>
#include <thread>
#include <tool.h>

// ScreenDlg 对话框

IMPLEMENT_DYNAMIC(ScreenDlg, CDialogEx)
ScreenDlg::ScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SCREEN, pParent)
{

}
ScreenDlg::~ScreenDlg()
{


}
void ScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(ScreenDlg, CDialogEx)
	ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// ScreenDlg 消息处理程序



BOOL ScreenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
    CRect rcClient;
    GetClientRect(&rcClient);
    nRecvWidth = rcClient.Width();
    nRecvHeight = rcClient.Height();
    SetTimer(1,15,NULL);
    //std::thread(&ScreenDlg::ScreenShow,this).detach();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

//DWORD WINAPI ScreenShow(LPVOID lpParameter){
//    while (m_bIsRun) {
//        SendPackage(sc->m_sockClient, MakePackage<C2RScreen>(0));  //发送请求
//        std::shared_ptr<PkgHeader> pPkg = RecvPackage(sc->m_sockClient);
//        switch (pPkg->m_pt)
//        {
//            //投屏
//        case R2C_SCREEN:
//        {
//            //屏幕数据包
//            R2CScreen* pScrPkg = (R2CScreen*)pPkg.get();
//
//            //将数据写入屏幕
//            CDC* pDC = sc->GetDC();
//
//            //数据转dc
//            CDC dcMem;
//            dcMem.CreateCompatibleDC(pDC);
//            //创建位图
//            CBitmap bmpMem;
//            bmpMem.CreateCompatibleBitmap(pDC, pScrPkg->m_nWidth, pScrPkg->m_nHeight);
//            dcMem.SelectObject(bmpMem);//位图写入DC
//
//            uint32_t nDataLen = pScrPkg->m_nLen - (sizeof(R2CScreen) - sizeof(PkgHeader));   //计算数据长度
//            uint8_t* pData = (uint8_t*)pScrPkg + sizeof(R2CScreen);
//            bmpMem.SetBitmapBits(nDataLen/*数量*/, pData/*缓冲区*/);
//
//            pDC->StretchBlt(0, 0, sc->nRecvWidth, sc->nRecvHeight,//拷贝到客户区
//                &dcMem, 0, 0, pScrPkg->m_nWidth, pScrPkg->m_nHeight, //拷贝整个屏幕
//                SRCCOPY); //颜色不做任何修改
//
//            //ReleaseDC(NULL,pDC);
//            break;
//        }
//        default:
//            break;
//        }
//        Sleep(50);
//    }
//    Log("[end]","sss");
//    return 0;
//}
//void ScreenDlg::ScreenShow()
//{
//        while (m_bIsRun) {
//        SendPackage(m_sockClient, MakePackage<C2RScreen>(0));  //发送请求
//        std::shared_ptr<PkgHeader> pPkg = RecvPackage(m_sockClient);
//        switch (pPkg->m_pt)
//        {
//            //投屏
//        case R2C_SCREEN:
//        {
//            //屏幕数据包
//            R2CScreen* pScrPkg = (R2CScreen*)pPkg.get();
//
//            //将数据写入屏幕
//            CDC* pDC = GetDC();
//
//            //数据转dc
//            CDC dcMem;
//            dcMem.CreateCompatibleDC(pDC);
//            //创建位图
//            CBitmap bmpMem;
//            bmpMem.CreateCompatibleBitmap(pDC, pScrPkg->m_nWidth, pScrPkg->m_nHeight);
//            dcMem.SelectObject(bmpMem);//位图写入DC
//
//            uint32_t nDataLen = pScrPkg->m_nLen - (sizeof(R2CScreen) - sizeof(PkgHeader));   //计算数据长度
//            uint8_t* pData = (uint8_t*)pScrPkg + sizeof(R2CScreen);
//            bmpMem.SetBitmapBits(nDataLen/*数量*/, pData/*缓冲区*/);
//
//            pDC->StretchBlt(0, 0, nRecvWidth, nRecvHeight,//拷贝到客户区
//                &dcMem, 0, 0, pScrPkg->m_nWidth, pScrPkg->m_nHeight, //拷贝整个屏幕
//                SRCCOPY); //颜色不做任何修改
//
//            ReleaseDC(pDC);
//            break;
//        }
//        default:
//            break;
//        }
//        Sleep(50);
//    }
//    Log("[end]","sss");
////}



//定时器
void ScreenDlg::OnTimer(UINT_PTR nIDEvent)
{
    SendPackage(m_sockClient, MakePackage<C2RScreen>(0));  //发送请求
    std::shared_ptr<PkgHeader> pPkg = RecvPackage(m_sockClient);

    switch (pPkg->m_pt)
    {
        //投屏
    case R2C_SCREEN:
    {
        //屏幕数据包
        R2CScreen* pScrPkg = (R2CScreen*)pPkg.get();

        //将数据写入屏幕
        CDC* pDC = GetDC();

        //数据转dc
        CDC dcMem;
        dcMem.CreateCompatibleDC(pDC);
        //创建位图
        CBitmap bmpMem;
        bmpMem.CreateCompatibleBitmap(pDC, pScrPkg->m_nWidth, pScrPkg->m_nHeight);
        dcMem.SelectObject(bmpMem);//位图写入DC

        uint32_t nDataLen = pScrPkg->m_nLen - (sizeof(R2CScreen) - sizeof(PkgHeader));   //计算数据长度
        uint8_t* pData = (uint8_t*)pScrPkg + sizeof(R2CScreen);
        bmpMem.SetBitmapBits(nDataLen/*数量*/, pData/*缓冲区*/);

        pDC->StretchBlt(0, 0, nRecvWidth, nRecvHeight,//拷贝到客户区
            &dcMem, 0, 0, pScrPkg->m_nWidth, pScrPkg->m_nHeight, //拷贝整个屏幕
            SRCCOPY); //颜色不做任何修改

        ReleaseDC(pDC);
        break;
    }
    default:
        break;
    }

	CDialogEx::OnTimer(nIDEvent);
}




//鼠标按下
void ScreenDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    std::shared_ptr<PkgHeader> pPkg = MakePackage<C2RMouseClickDown>(sizeof(uint32_t) * 4, point.x, point.y, nRecvWidth, nRecvHeight);
    SendPackage(m_sockClient, pPkg);
    CDialogEx::OnLButtonDown(nFlags, point);
}

//鼠标抬起
void ScreenDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    std::shared_ptr<PkgHeader> pPkg = MakePackage<C2RMouseClickUp>(sizeof(uint32_t) * 4, point.x, point.y, nRecvWidth, nRecvHeight);
    SendPackage(m_sockClient, pPkg);
    CDialogEx::OnLButtonUp(nFlags, point);
}

//鼠标移动
void ScreenDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    //ClientToScreen(&point);
    std::shared_ptr<PkgHeader> pPkg = MakePackage<C2RMouseMove>(sizeof(uint32_t) * 4, point.x, point.y, nRecvWidth, nRecvHeight);
    SendPackage(m_sockClient, pPkg);
    CDialogEx::OnMouseMove(nFlags, point);
}


//键盘按下
void ScreenDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    std::shared_ptr<PkgHeader> pPkg = MakePackage<C2RKyeboard>(sizeof(uint32_t), nChar);
    SendPackage(m_sockClient, pPkg);
    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}
