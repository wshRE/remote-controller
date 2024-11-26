#pragma once
#include <stdint.h>
#include<iostream>
#include<memory>
#include"InitSockLib.h"
#define NAMELEN 64
#define MSGLEN  256


//传送的包类型
enum PkgType {
	C2R_SCREEN,        //控制端 向 被控制端  申请屏幕数据
	R2C_SCREEN,        //被控制端   向 控制端 发送屏幕数据

	//鼠标仅完成单击和移动
	C2R_MOUSE_MOVE,    //控制端 向 被控制端  发送鼠标移动信息
	C2R_MOUSE_CLICK_DOWN,   //控制端 向 被控制端  发送鼠标按下
	C2R_MOUSE_CLICK_UP,   //控制端 向 被控制端  发送鼠标按下


	C2R_KEYBOARD,      //控制端 向 被控制端  发送键盘按键信息

	C2R_CMD,           //控制端 向 被控制端  发送cmd 文本
	R2C_CMD,           //被控制端   向 控制端 发送cmd执行结果

	//文件涉及上传下载 目录获取....

	//盘符
	C2R_DRIVER,  //申请盘符
	R2C_DRIVER,   //返回

	//文件是否存在
	C2R_FE,      
	R2C_FE,

	//请求文件
	C2R_CHF,
	R2C_CHF,
	R2C_CHFEND, //文件发送结束

	//下载
	C2R_DOWN,
	R2C_DOWN,      //
	R2C_DOWN_SIZE,  //返回下载的量

	//上传
	C2R_UP_PATH,   //发送上传的位置
	C2R_UP_SIZE,   //发送上传文件的大小
	C2R_UP,		   //发送上传文件
	R2C_UP,        //接收上传的文件


		//C2R_
	//R2C_


		//C2R_
	//R2C_


		//C2R_
	//R2C_
};


//包格式
#pragma pack(push)
#pragma pack(1)

//包头
struct PkgHeader
{
	PkgType		m_pt;					//包类型
	int			m_nLen;					//数据长度
};



//下载--附带链接
struct C2RDown:public PkgHeader {
	C2RDown(uint8_t* pScreenData)
	{
		m_pt = C2R_DOWN;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  
};
//返回下载的数据大小
struct R2CDownSize :public PkgHeader {
	R2CDownSize(int n)
	{
		m_pt = R2C_DOWN_SIZE;
		m_nSize = n;
		m_nLen = sizeof(int);
	}
	int m_nSize;
};
//传输的数据包
struct R2CDown :public PkgHeader {
	R2CDown(uint8_t* pScreenData,int nLen)
	{
		m_pt = R2C_DOWN;
		m_nLen = nLen;
		memcpy((char*)m_szMsg, (char*)pScreenData, m_nLen);
	}
	uint8_t m_szMsg[0x1000];  //盘符数组
};

//发送上传的位置
struct C2RUpPath :public PkgHeader {
	C2RUpPath(uint8_t* pScreenData)
	{
		m_pt = C2R_UP_PATH;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];
};
//发送上传文件的大小
struct C2RUpSize :public PkgHeader {
	C2RUpSize(int n)
	{
		m_pt = C2R_UP_SIZE;
		m_nSize = n;
		m_nLen = sizeof(int);
	}
	int m_nSize;
};
//发送上传文件
struct C2RUp :public PkgHeader {
	C2RUp(uint8_t* pScreenData, int nLen)
	{
		m_pt = C2R_UP;
		m_nLen = nLen;
		//strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
		memcpy((char*)m_szMsg, (char*)pScreenData, m_nLen);
	}
	uint8_t m_szMsg[0x1000];  //盘符数组
};
  

//盘符
struct C2RDriver :public PkgHeader {
	C2RDriver() {
		m_pt = C2R_DRIVER;
		m_nLen = 0;
	}
};
struct R2CDriver :public PkgHeader {
	R2CDriver(uint8_t* pScreenData)
	{
		m_pt = R2C_DRIVER;

		m_nLen = 0;
		while (pScreenData[m_nLen]) {
			while (pScreenData[m_nLen]) {
				m_nLen++;
			}
			m_nLen++;
		}
		//strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
		memcpy((char*)m_szMsg, (char*)pScreenData, m_nLen);
	}
	uint8_t m_szMsg[MAXBYTE];  //盘符数组
};

//文件是否存在
struct C2RFe :public PkgHeader {
	C2RFe(uint8_t* pScreenData)
	{
		m_pt = C2R_FE;
		m_nLen = strlen((char*)pScreenData)+1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  //盘符数组
};
struct R2CFe :public PkgHeader {
	R2CFe(int n)
	{
		m_pt = R2C_FE;
		m_IsExist = n;
		m_nLen = sizeof(int32_t);
	}
	int32_t m_IsExist;
};

//子文件
struct C2RChf :public PkgHeader {
	C2RChf(uint8_t* pScreenData)
	{
		m_pt = C2R_CHF;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  //盘符数组

};
struct R2CChf :public PkgHeader {
	R2CChf(uint8_t* SzName, uint8_t* SzType, uint8_t* SzTime) {
		m_pt = R2C_CHF;
		m_nLen = strlen((char*)SzName) + strlen((char*)SzType) + strlen((char*)SzTime) +3 + 3;
		nLenName = strlen((char*)SzName) + 1;
		nLenType = strlen((char*)SzType) + 1;
		nLenTime = strlen((char*)SzTime) + 1;
		memcpy((uint8_t*)this/*对象首地址*/ + sizeof(*this)/*对象大小*/, SzName, nLenName);  //文件名
		memcpy((uint8_t*)this/*对象首地址*/ + sizeof(*this)/*对象大小*/ + nLenName, SzType, nLenType);  //文件名
		memcpy((uint8_t*)this/*对象首地址*/ + sizeof(*this)/*对象大小*/ + nLenName  + nLenType, SzTime, nLenTime);  //文件名
	}
	int8_t nLenName;
	int8_t nLenType;
	int8_t nLenTime;
	//uint8_t m_SzName[MAXBYTE];  //文件名	
	//uint8_t m_SzType[MAXBYTE];  //文件类型
	//uint8_t m_SzTime[MAXBYTE];  //最后修改时间
};



struct R2CChfend :public PkgHeader {
	R2CChfend()
	{
		m_pt = R2C_CHFEND;
		m_nLen = 0;
	}
};
struct m_ChildFile {
	m_ChildFile(){}
	m_ChildFile( char* SzName,char* SzType,char* SzTime) {
		strcpy_s((char*)m_SzName, strlen((char*)SzName) + 1, (char*)SzName);
		strcpy_s((char*)m_SzType, strlen((char*)SzType) + 1, (char*)SzType);
		strcpy_s((char*)m_SzTime, strlen((char*)SzTime) + 1, (char*)SzTime);
	}
	char m_SzName[MAXBYTE];  //文件名
	char m_SzType[MAXBYTE];  //文件类型
	char m_SzTime[MAXBYTE];  //最后修改时间
};



//控制端 向 被控制端  申请屏幕数据
struct C2RScreen :public PkgHeader
{
	C2RScreen()
	{
		m_pt = C2R_SCREEN;
		m_nLen = 0;
	}
};
//返回屏幕数据
struct R2CScreen :public PkgHeader
{
	R2CScreen(uint32_t nWidth, uint32_t nHeight, uint8_t* pScreenData, uint32_t nLen)
	{
		m_pt = R2C_SCREEN;
		m_nLen = sizeof(R2CScreen) + nLen - sizeof(PkgHeader);   //R2CScreen 且不包含 PkgHeader的数据长度
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		memcpy((uint8_t*)this/*对象首地址*/ + sizeof(*this)/*对象大小*/, pScreenData, nLen);  //写入屏幕数据
	}

	uint32_t m_nWidth; //宽度
	uint32_t m_nHeight; //高度
	//uint8_t m_data[1];
};

//控制端 向 被控制端  发送鼠标移动信息（坐标）
struct C2RMouseMove :public PkgHeader {
	C2RMouseMove(uint32_t nX, uint32_t nY, uint32_t nWidth,uint32_t nHeight) :m_nX(nX), m_nY(nY), m_nWidth(nWidth), m_nHeight(nHeight) {
		m_pt = C2R_MOUSE_MOVE;
		m_nLen = sizeof(C2RMouseMove) - sizeof(PkgHeader);
	}
	uint32_t m_nX;
	uint32_t m_nY;
	uint32_t m_nWidth;
	uint32_t m_nHeight;
};


//控制端 向 被控制端  发送鼠标点击信息(点击坐标)
struct C2RMouseClickDown :public PkgHeader {
	C2RMouseClickDown(uint32_t nX, uint32_t nY, uint32_t nWidth, uint32_t nHeight) :m_nX(nX), m_nY(nY), m_nWidth(nWidth), m_nHeight(nHeight) {
		m_pt = C2R_MOUSE_CLICK_DOWN;
		m_nLen = sizeof(C2RMouseMove) - sizeof(PkgHeader);
	}
	uint32_t m_nX;
	uint32_t m_nY;
	uint32_t m_nWidth;
	uint32_t m_nHeight;
};
struct C2RMouseClickUp :public PkgHeader {
	C2RMouseClickUp(uint32_t nX, uint32_t nY, uint32_t nWidth, uint32_t nHeight) :m_nX(nX), m_nY(nY), m_nWidth(nWidth), m_nHeight(nHeight) {
		m_pt = C2R_MOUSE_CLICK_UP;
		m_nLen = sizeof(C2RMouseMove) - sizeof(PkgHeader);
	}
	uint32_t m_nX;
	uint32_t m_nY;
	uint32_t m_nWidth;
	uint32_t m_nHeight;
};



//控制端 向 被控制端  发送键盘按键信息
struct C2RKyeboard :public PkgHeader {
	C2RKyeboard(uint32_t nDownAscii) :m_nDownAscii(nDownAscii) {
		m_pt = C2R_KEYBOARD;
		m_nLen = sizeof(C2RKyeboard) - sizeof(PkgHeader);
	}
	uint32_t m_nDownAscii; //点击的按键的ascii
};



//控制端 向 被控制端  发送cmd 文本
struct C2RCmd :public PkgHeader {
	C2RCmd(uint8_t* pScreenData = nullptr)
	{
		m_pt = C2R_CMD;
		if (pScreenData == nullptr) {
			m_nLen = 0;
		}
		else {
			//m_nLen = sizeof(C2RCmd) + nLen - sizeof(PkgHeader);   //R2CScreen 且不包含 PkgHeader的数据长度
			m_nLen = strlen((char*)pScreenData) + 1;
			strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
		}
	}
	uint8_t m_szMsg[MAXBYTE];  //消息
};

//被控制端   向 控制端 发送cmd执行结果
struct R2CCmd :public PkgHeader {
	R2CCmd(uint8_t* pScreenData)
	{
		m_pt = R2C_CMD;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  //消息
};


#pragma pack(pop)


//44：00   构造返回包
template<typename T, typename...TS>
std::shared_ptr<PkgHeader>MakePackage(uint32_t nLen, TS ...ts) {
	uint8_t* pBuf = new uint8_t[nLen + sizeof(T)]; //申请缓冲区
	//check....可能存在问题
	T* pPkg = new(pBuf) T(ts...);
	return std::shared_ptr<PkgHeader>(pPkg, [](PkgHeader* p) {delete[](uint8_t*)p; /*释放*/});
}




//收包
std::shared_ptr<PkgHeader> RecvPackage(SOCKET sock);
int RecvBytes(SOCKET sock, int8_t* pBuff, int32_t nLen);

//发包
void SendPackage(SOCKET sock, std::shared_ptr<PkgHeader> pPkg);
int SendPackageRe(SOCKET sock, std::shared_ptr<PkgHeader> pPkg);