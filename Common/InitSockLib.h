#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#pragma comment (lib,"ws2_32.lib")
class CInitSockLib
{
public:
    CInitSockLib();
    ~CInitSockLib();
private:
    static CInitSockLib m_initsocklib;
};