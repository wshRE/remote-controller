#pragma once
#include<Windows.h>
//»’÷æ ‰≥ˆ

template<typename...TS>
void Log(const char* szFmt, TS... ts) {
    char szBuff[1024] = {};
    wsprintf(szBuff, szFmt, ts...);
    OutputDebugString(szBuff);
}


template<typename...TS>
void Log(const char* szFmt) {
    OutputDebugString(szFmt);
}