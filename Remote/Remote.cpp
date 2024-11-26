#include <afx.h>
#include <iostream>
using namespace std;
#include "InitSockLib.h"
#include "Proto.h"
#include"tool.h"
#include<string>
#include <thread>
#include "afxdialogex.h"

BOOL Init();                           //初始化
void SendScreen();                     //屏幕发送
void MouseMove(LONG nX, LONG nY);      //鼠标指定移动
void MouseClick(int nType);            //鼠标操作
void InitCMD();
void CmdDeal();
void Read();
void Write();
void GetChild(char* szBuff);           //文件

SOCKET sockClient; 
//屏幕数据
int nWidth;
int nHeight;
//管道
HANDLE m_hParentRead;
HANDLE m_hChildWrite;
HANDLE m_hChildRead;
HANDLE m_hParentWrite;
char pData[MAXBYTE];   //输入的数据

int main()
{
    BOOL bn = Init();
    if (!bn) {
        return 0;
    }

    //暂时不开线程  仅发送界面数据
    while (true)
    {
        //接受命令
        auto pPkg = RecvPackage(sockClient);   //tcp收包  ----以前的代码
        switch (pPkg->m_pt)
        {
        //屏幕命令
        case C2R_SCREEN: { 
            SendScreen();//返回屏幕数据
            break;
        }
        //鼠标命令
        case C2R_MOUSE_MOVE: {
            C2RMouseMove* pScrPkg = (C2RMouseMove*)pPkg.get();
            MouseMove(pScrPkg->m_nX / pScrPkg->m_nWidth * nWidth, pScrPkg->m_nY / pScrPkg->m_nHeight*nHeight /** 65535 / (nHeight - 1)*/);
            break;
        }
        case C2R_MOUSE_CLICK_DOWN: {
            MouseClick(MOUSEEVENTF_LEFTDOWN);
            break;
        }
        case C2R_MOUSE_CLICK_UP: {
            MouseClick(MOUSEEVENTF_LEFTUP);
            break;
        }
        //键盘命令
        case C2R_KEYBOARD: {
            C2RKyeboard* pScrPkg = (C2RKyeboard*)pPkg.get();
            keybd_event(pScrPkg->m_nDownAscii, 0, 0, 0);
            keybd_event(pScrPkg->m_nDownAscii, 0, KEYEVENTF_KEYUP, 0);
            break;
        }
        //CMD命令
        case C2R_CMD: {
            C2RCmd* pScrPkg = (C2RCmd*)pPkg.get();
            int nDataLen = pScrPkg->m_nLen;
            //初次获取返回值
            if (nDataLen == 0) {
                InitCMD();
                thread(&Read).detach();
                break;
            }
            uint8_t* pData1 = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据
            ZeroMemory(pData, MAXBYTE);
            strcpy_s(pData, nDataLen, (char*)pData1);
            //CmdDeal();
            Write();
            break;
        }
        //盘符申请
        case C2R_DRIVER: {
            char szBuff[MAXBYTE] = {};
            GetLogicalDriveStrings(MAXBYTE, szBuff);
            //返回
            //构造返回包
            shared_ptr<PkgHeader> pPkg = MakePackage<R2CDriver>(strlen(szBuff), (uint8_t*)szBuff);
            //返回数据
            SendPackage(sockClient, pPkg);
            break;
        }
         //文件存在
        case C2R_FE: {
            char szBuff[MAXBYTE];
            C2RFe* pScrPkg = (C2RFe*)pPkg.get();
            uint32_t nDataLen = pScrPkg->m_nLen;
            uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据

            ZeroMemory(szBuff, MAXBYTE);
            memcpy((char*)szBuff, (char*)pData, nDataLen);
            //strcpy_s(szBuff, nDataLen, (char*)pData);

            CFileFind finder;
            BOOL bWorking = finder.FindFile(((LPCSTR)szBuff));
            finder.Close();
            //返回
            int nRE = bWorking ? 1 : 0;
            shared_ptr<PkgHeader> pPkg = MakePackage<R2CFe>(sizeof(int), nRE);
            SendPackage(sockClient, pPkg);
            break;
        }
         //文件目录
        case C2R_CHF: {
            char szBuff[MAXBYTE];
            C2RChf* pScrPkg = (C2RChf*)pPkg.get();
            uint32_t nDataLen = pScrPkg->m_nLen;
            uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据

            ZeroMemory(szBuff, MAXBYTE);
            memcpy((char*)szBuff, (char*)pData, nDataLen);
            GetChild(szBuff);
            break;
        }
        //文件下载通知
        case C2R_DOWN: {
            char szBuff[MAXBYTE];
            C2RDown* pScrPkg = (C2RDown*)pPkg.get();
            uint32_t nDataLen = pScrPkg->m_nLen;
            uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据

            ZeroMemory(szBuff, MAXBYTE);
            memcpy((char*)szBuff, (char*)pData, nDataLen);

            //返回文件大小
            FILE* f;
            f = fopen(szBuff, "rb");   //打开文件
            if (f == NULL) {
                //返回0
                shared_ptr<PkgHeader> pPkg = MakePackage<R2CDownSize>(sizeof(int), 0);
                SendPackage(sockClient, pPkg);
                break;
            }
            fseek(f, 0, SEEK_END);
            int nEnd = ftell(f);//文件大小
            rewind(f); //重置指针
            //发送大小
            shared_ptr<PkgHeader> pPkg = MakePackage<R2CDownSize>(sizeof(int), nEnd);
            SendPackage(sockClient, pPkg);
            fclose(f); //关闭


            HANDLE hFile = CreateFile(szBuff,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                cout << "失败" << endl;
                return 0;
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
                R2CDown rc((uint8_t*)buf, sizeof(buf));
                int nRe = send(sockClient,
                    (char*)&rc, sizeof(PkgHeader) + sizeof(buf), 0);
                nTol += nRe;
                cout << "文件大小: " << nEnd << "  已发送： " << nTol /*- sizeof(PkgHeader)*/ << endl;
            }
            CloseHandle(hFile);
            break;
        }
        //上传的文件路径
        case C2R_UP_PATH: {
            //路径
            char szBuff[MAXBYTE];
            C2RUpPath* pScrPkg = (C2RUpPath*)pPkg.get();
            uint32_t nDataLen = pScrPkg->m_nLen;
            uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据
            ZeroMemory(szBuff, MAXBYTE);
            memcpy((char*)szBuff, (char*)pData, nDataLen);

            //大小
            std::shared_ptr<PkgHeader> pPkg1 = RecvPackage(sockClient);
            if (pPkg1->m_pt != C2R_UP_SIZE) {
                continue;
            }
            R2CDownSize* pScrPkg1 = (R2CDownSize*)pPkg1.get();
            int nSize = pScrPkg1->m_nSize;

            //创建文件
            HANDLE hFile = CreateFile(szBuff,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                continue;
            }
            int nRecv = 0;
            while (nRecv < nSize) {
                int8_t buf[0x1000];
                ZeroMemory(buf, 0x1000);
                //收数据
                std::shared_ptr<PkgHeader> pPkg2 = RecvPackage(sockClient);
                if (pPkg2->m_pt != C2R_UP) {
                    continue;
                }
                C2RUp* pScrPkg = (C2RUp*)pPkg2.get();
                uint32_t nDataLen = pScrPkg->m_nLen;
                uint8_t* pData = (uint8_t*)pScrPkg + sizeof(PkgHeader);//取得数据

                DWORD dwBytesWrited = 0;
                memcpy((char*)buf, (char*)pData, nDataLen);
                WriteFile(hFile, buf, nDataLen, &dwBytesWrited, NULL);
                nRecv += nDataLen;
                cout << "当前已下载：" << nRecv<<" 共："<< nSize << endl;
            }
            CloseHandle(hFile);
            break;
        }
        default:
            break;
        }
    }
}



BOOL Init() {
    HDC hdc = ::GetDC(NULL);
    nWidth = GetDeviceCaps(hdc, HORZRES);//获取屏幕水平分辨率
    nHeight = GetDeviceCaps(hdc, VERTRES);//获取屏幕垂直分辨率
    //nWidth =1920;
    //nHeight = 1080;

    SOCKET sockListen = socket(AF_INET,
        SOCK_STREAM, //数据流
        IPPROTO_TCP  //tcp协议
    );
    if (sockListen == INVALID_SOCKET)
    {
        cout << "socket failed" << endl;
        return 0;
    }
    sockaddr_in si;
    si.sin_family = AF_INET;
    si.sin_port = htons(0x9527);
    si.sin_addr.S_un.S_addr = inet_addr("192.168.202.128");
    //si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


    int nRet = ::bind(sockListen, (sockaddr*)&si, sizeof(si));
    if (nRet != 0)
    {
        cout << "bind failed" << endl;
        return 0;
    }
    //监听
    nRet = listen(sockListen, SOMAXCONN);
    if (nRet == SOCKET_ERROR)
    {
        cout << "listen failed" << endl;
        return 0;
    }
    cout << "监听端口9527h， 等待控制端连接 。。。。 " << endl;
    sockaddr_in siClient = {};  //接收控制端的连接
    int nLen = sizeof(siClient);
    sockClient = accept(sockListen, (sockaddr*)&siClient, &nLen);//接收外部包
    if (sockClient == INVALID_SOCKET)
    {
        cout << "accept failed" << endl;
        return 0;
    }
    printf("ip:%s port:%d 连接进入 。。。。 \r\n",
        inet_ntoa(siClient.sin_addr),
        ntohs(siClient.sin_port));
    return TRUE;
}


//文件
void GetChild(char* szBuff) {
    CString cPath = szBuff;
    cPath.Delete(strlen(szBuff) - 1, 1);
    CFileFind finder;
    BOOL bWorking = finder.FindFile(szBuff);
    while (bWorking)
    {
        CString cName;
        CString cType;
        CString cTime;

        bWorking = finder.FindNextFile();   //下一个文件
        cName = finder.GetFileName();  //文件名
        //文件类型判断
        if (finder.IsSystem() || finder.IsDots()) {
            continue;
        }

        //时间插入
        FILETIME pTimeStamp = {};
        finder.GetLastWriteTime(&pTimeStamp);
        SYSTEMTIME lpSystemTime = {};
        FileTimeToSystemTime(&pTimeStamp, &lpSystemTime);
        std::string stime = std::to_string(lpSystemTime.wYear) + "-" + std::to_string(lpSystemTime.wMonth) + "-" + std::to_string(lpSystemTime.wDay) /*+ "  " + std::to_string(lpSystemTime.wHour) + ":" + std::to_string(lpSystemTime.wMinute) + ":" + std::to_string(lpSystemTime.wSecond)*/;
        cTime = stime.c_str();



        SHFILEINFO sTypeName = {};
        SHGetFileInfo(cPath + cName, 0, &sTypeName, MAXBYTE, SHGFI_TYPENAME);
        cType = sTypeName.szTypeName;

        char* szName = cName.GetBuffer(cName.GetLength());
        char* szType = cType.GetBuffer(cType.GetLength());
        char* szTime = cTime.GetBuffer(cTime.GetLength());

        R2CChf rc((uint8_t*)szName, (uint8_t*)szType, (uint8_t*)szTime);
        send(sockClient,
            (char*)&rc, sizeof(PkgHeader) + rc.m_nLen, 0);
    }
    //发送结束包
     //打包返回
    shared_ptr<PkgHeader> pPkg = MakePackage<R2CChfend>(0);
    //发送
    SendPackage(sockClient, pPkg);
    finder.Close();
}




//屏幕
void SendScreen() {
    //获取屏幕数据
    HDC hDcScr = ::GetDC(NULL);
    //每个像素点占多少个字节
    int nBytesPerixel = GetDeviceCaps(hDcScr, BITSPIXEL) / 8/*八个位一个字节*/; //RGB 888

    //创建与指定设备兼容的内存设备上下文DC
    HDC hDcMem = ::CreateCompatibleDC(hDcScr);
    HBITMAP hBmpMem = ::CreateCompatibleBitmap(hDcScr, nWidth, nHeight);  //创建兼容位图
    ::SelectObject(hDcMem, hBmpMem);  //写入位图

    //拷贝
    ::BitBlt(hDcMem/*目标DC*/, 0, 0, nWidth, nHeight, hDcScr/*来源*/, 0, 0, SRCCOPY);

    //获取位图数据
    uint32_t nDataLen = nWidth * nHeight * nBytesPerixel;  //总的占用字节数
    uint8_t* pScrData = new uint8_t[nDataLen];

    ::GetBitmapBits(hBmpMem, nDataLen, pScrData);

    ReleaseDC(NULL, hDcScr); //清理资源 释放

    //构造返回包
    shared_ptr<PkgHeader> pPkg = MakePackage<R2CScreen>(nDataLen, nWidth, nHeight, pScrData, nDataLen);

    //返回数据
    SendPackage(sockClient, pPkg);

    ReleaseDC(NULL, hDcMem);
    delete[] pScrData;
}

//鼠标指定移动
void MouseMove(LONG nX, LONG nY) {
    int newpos[2]{};
    newpos[0] = nX;
    newpos[1] = nY;
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE,
        newpos[0],  
        newpos[1],  
        0,
        0);
}

//鼠标操作
void MouseClick(int nType) {
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = nType | MOUSEEVENTF_ABSOLUTE;
    inputs[0].mi.dx = 0;
    inputs[0].mi.dy = 0;

    int nSize = ARRAYSIZE(inputs);
    UINT uSent;
    uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

//cmd
void InitCMD() {
    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;  //允许继承
    //父子管道
    if (!CreatePipe(&m_hParentRead, &m_hChildWrite, &sa, 0))
    {
        cout << "管道创建失败" << endl;
        return;
    }
    else
    {
        cout << "管道创建成功" << endl;
    }

    //子进程读 父进程写
    if (!CreatePipe(&m_hChildRead, &m_hParentWrite, &sa, 0))
    {
        cout << "管道创建失败" << endl;
        return;
    }
    else
    {
        cout << "管道创建成功" << endl;
    }

    //创建子进程
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES; //使用句柄--------使其他参数起作用
    si.hStdInput = m_hChildRead;
    si.hStdOutput = m_hChildWrite;
    si.hStdError = m_hChildWrite;

    ZeroMemory(&pi, sizeof(pi));

    // Start the child process. 
    if (!CreateProcess(NULL, // No module name (use command line). 
        (LPSTR)"cmd.exe", // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        TRUE,            // Set handle inheritance to FALSE. -----允许继承，这样才能绑定父子
        CREATE_NO_WINDOW,  // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi)             // Pointer to PROCESS_INFORMATION structure.
        )
    {
        cout << "管道创建失败" << endl;
        return;
    }
    else
    {
        cout << "管道创建成功" << endl;

    }
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void CmdDeal() {
    Write();
    Read();
}

void Read() {
    //判断管道中是否有数据可读
  /*
  ReadFile 拿数据时会把数据移出管道，PeekNamedPipe不会
  */
    DWORD dwBytesAvail = 0;//当前管道内的数据量
    string m_strOut;
    while (PeekNamedPipe(m_hParentRead/*句柄*/, NULL/*缓冲区*/, 0/*大小*/, NULL, &dwBytesAvail, NULL))
    {
        //从管道读取数据
        char szBuff[MAXBYTE] = {};
        ZeroMemory(szBuff, MAXBYTE);
        if (dwBytesAvail > 0)//数据大于0
        {
            DWORD dwBytesReaded = 0;
            //管道内无数据时会被阻塞,因此需要判断管道内是否有数据据
            ReadFile(m_hParentRead, szBuff, sizeof(szBuff) - 1, &dwBytesReaded, NULL);
            m_strOut += szBuff;//记录历史
        }
        else {
            if (m_strOut.empty()) {
                Sleep(50);
                continue;
            }
            //显示到界面
            if (m_strOut == "" || *(m_strOut.end() - 1) != '>') {
                continue;
            }
            //构造返回包
            shared_ptr<PkgHeader> pPkg = MakePackage<R2CCmd>(strlen(m_strOut.c_str()), (uint8_t*)(m_strOut.c_str()));

            //返回数据
            SendPackage(sockClient, pPkg);
            m_strOut.clear();
        }
    }
}

void Write() {
    string strData(pData);
    //strData += "\r\n";  //回车
    //从管道读取数据
    DWORD dwBytesWrited = 0;
    WriteFile(m_hParentWrite, strData.c_str(),   /*管道满的时候会阻塞*/
        strData.size(),
        &dwBytesWrited, NULL);
}