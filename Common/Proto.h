#pragma once
#include <stdint.h>
#include<iostream>
#include<memory>
#include"InitSockLib.h"
#define NAMELEN 64
#define MSGLEN  256


//���͵İ�����
enum PkgType {
	C2R_SCREEN,        //���ƶ� �� �����ƶ�  ������Ļ����
	R2C_SCREEN,        //�����ƶ�   �� ���ƶ� ������Ļ����

	//������ɵ������ƶ�
	C2R_MOUSE_MOVE,    //���ƶ� �� �����ƶ�  ��������ƶ���Ϣ
	C2R_MOUSE_CLICK_DOWN,   //���ƶ� �� �����ƶ�  ������갴��
	C2R_MOUSE_CLICK_UP,   //���ƶ� �� �����ƶ�  ������갴��


	C2R_KEYBOARD,      //���ƶ� �� �����ƶ�  ���ͼ��̰�����Ϣ

	C2R_CMD,           //���ƶ� �� �����ƶ�  ����cmd �ı�
	R2C_CMD,           //�����ƶ�   �� ���ƶ� ����cmdִ�н��

	//�ļ��漰�ϴ����� Ŀ¼��ȡ....

	//�̷�
	C2R_DRIVER,  //�����̷�
	R2C_DRIVER,   //����

	//�ļ��Ƿ����
	C2R_FE,      
	R2C_FE,

	//�����ļ�
	C2R_CHF,
	R2C_CHF,
	R2C_CHFEND, //�ļ����ͽ���

	//����
	C2R_DOWN,
	R2C_DOWN,      //
	R2C_DOWN_SIZE,  //�������ص���

	//�ϴ�
	C2R_UP_PATH,   //�����ϴ���λ��
	C2R_UP_SIZE,   //�����ϴ��ļ��Ĵ�С
	C2R_UP,		   //�����ϴ��ļ�
	R2C_UP,        //�����ϴ����ļ�


		//C2R_
	//R2C_


		//C2R_
	//R2C_


		//C2R_
	//R2C_
};


//����ʽ
#pragma pack(push)
#pragma pack(1)

//��ͷ
struct PkgHeader
{
	PkgType		m_pt;					//������
	int			m_nLen;					//���ݳ���
};



//����--��������
struct C2RDown:public PkgHeader {
	C2RDown(uint8_t* pScreenData)
	{
		m_pt = C2R_DOWN;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  
};
//�������ص����ݴ�С
struct R2CDownSize :public PkgHeader {
	R2CDownSize(int n)
	{
		m_pt = R2C_DOWN_SIZE;
		m_nSize = n;
		m_nLen = sizeof(int);
	}
	int m_nSize;
};
//��������ݰ�
struct R2CDown :public PkgHeader {
	R2CDown(uint8_t* pScreenData,int nLen)
	{
		m_pt = R2C_DOWN;
		m_nLen = nLen;
		memcpy((char*)m_szMsg, (char*)pScreenData, m_nLen);
	}
	uint8_t m_szMsg[0x1000];  //�̷�����
};

//�����ϴ���λ��
struct C2RUpPath :public PkgHeader {
	C2RUpPath(uint8_t* pScreenData)
	{
		m_pt = C2R_UP_PATH;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];
};
//�����ϴ��ļ��Ĵ�С
struct C2RUpSize :public PkgHeader {
	C2RUpSize(int n)
	{
		m_pt = C2R_UP_SIZE;
		m_nSize = n;
		m_nLen = sizeof(int);
	}
	int m_nSize;
};
//�����ϴ��ļ�
struct C2RUp :public PkgHeader {
	C2RUp(uint8_t* pScreenData, int nLen)
	{
		m_pt = C2R_UP;
		m_nLen = nLen;
		//strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
		memcpy((char*)m_szMsg, (char*)pScreenData, m_nLen);
	}
	uint8_t m_szMsg[0x1000];  //�̷�����
};
  

//�̷�
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
	uint8_t m_szMsg[MAXBYTE];  //�̷�����
};

//�ļ��Ƿ����
struct C2RFe :public PkgHeader {
	C2RFe(uint8_t* pScreenData)
	{
		m_pt = C2R_FE;
		m_nLen = strlen((char*)pScreenData)+1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  //�̷�����
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

//���ļ�
struct C2RChf :public PkgHeader {
	C2RChf(uint8_t* pScreenData)
	{
		m_pt = C2R_CHF;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  //�̷�����

};
struct R2CChf :public PkgHeader {
	R2CChf(uint8_t* SzName, uint8_t* SzType, uint8_t* SzTime) {
		m_pt = R2C_CHF;
		m_nLen = strlen((char*)SzName) + strlen((char*)SzType) + strlen((char*)SzTime) +3 + 3;
		nLenName = strlen((char*)SzName) + 1;
		nLenType = strlen((char*)SzType) + 1;
		nLenTime = strlen((char*)SzTime) + 1;
		memcpy((uint8_t*)this/*�����׵�ַ*/ + sizeof(*this)/*�����С*/, SzName, nLenName);  //�ļ���
		memcpy((uint8_t*)this/*�����׵�ַ*/ + sizeof(*this)/*�����С*/ + nLenName, SzType, nLenType);  //�ļ���
		memcpy((uint8_t*)this/*�����׵�ַ*/ + sizeof(*this)/*�����С*/ + nLenName  + nLenType, SzTime, nLenTime);  //�ļ���
	}
	int8_t nLenName;
	int8_t nLenType;
	int8_t nLenTime;
	//uint8_t m_SzName[MAXBYTE];  //�ļ���	
	//uint8_t m_SzType[MAXBYTE];  //�ļ�����
	//uint8_t m_SzTime[MAXBYTE];  //����޸�ʱ��
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
	char m_SzName[MAXBYTE];  //�ļ���
	char m_SzType[MAXBYTE];  //�ļ�����
	char m_SzTime[MAXBYTE];  //����޸�ʱ��
};



//���ƶ� �� �����ƶ�  ������Ļ����
struct C2RScreen :public PkgHeader
{
	C2RScreen()
	{
		m_pt = C2R_SCREEN;
		m_nLen = 0;
	}
};
//������Ļ����
struct R2CScreen :public PkgHeader
{
	R2CScreen(uint32_t nWidth, uint32_t nHeight, uint8_t* pScreenData, uint32_t nLen)
	{
		m_pt = R2C_SCREEN;
		m_nLen = sizeof(R2CScreen) + nLen - sizeof(PkgHeader);   //R2CScreen �Ҳ����� PkgHeader�����ݳ���
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		memcpy((uint8_t*)this/*�����׵�ַ*/ + sizeof(*this)/*�����С*/, pScreenData, nLen);  //д����Ļ����
	}

	uint32_t m_nWidth; //���
	uint32_t m_nHeight; //�߶�
	//uint8_t m_data[1];
};

//���ƶ� �� �����ƶ�  ��������ƶ���Ϣ�����꣩
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


//���ƶ� �� �����ƶ�  �����������Ϣ(�������)
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



//���ƶ� �� �����ƶ�  ���ͼ��̰�����Ϣ
struct C2RKyeboard :public PkgHeader {
	C2RKyeboard(uint32_t nDownAscii) :m_nDownAscii(nDownAscii) {
		m_pt = C2R_KEYBOARD;
		m_nLen = sizeof(C2RKyeboard) - sizeof(PkgHeader);
	}
	uint32_t m_nDownAscii; //����İ�����ascii
};



//���ƶ� �� �����ƶ�  ����cmd �ı�
struct C2RCmd :public PkgHeader {
	C2RCmd(uint8_t* pScreenData = nullptr)
	{
		m_pt = C2R_CMD;
		if (pScreenData == nullptr) {
			m_nLen = 0;
		}
		else {
			//m_nLen = sizeof(C2RCmd) + nLen - sizeof(PkgHeader);   //R2CScreen �Ҳ����� PkgHeader�����ݳ���
			m_nLen = strlen((char*)pScreenData) + 1;
			strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
		}
	}
	uint8_t m_szMsg[MAXBYTE];  //��Ϣ
};

//�����ƶ�   �� ���ƶ� ����cmdִ�н��
struct R2CCmd :public PkgHeader {
	R2CCmd(uint8_t* pScreenData)
	{
		m_pt = R2C_CMD;
		m_nLen = strlen((char*)pScreenData) + 1;
		strcpy_s((char*)m_szMsg, m_nLen, (char*)pScreenData);
	}
	uint8_t m_szMsg[MAXBYTE];  //��Ϣ
};


#pragma pack(pop)


//44��00   ���췵�ذ�
template<typename T, typename...TS>
std::shared_ptr<PkgHeader>MakePackage(uint32_t nLen, TS ...ts) {
	uint8_t* pBuf = new uint8_t[nLen + sizeof(T)]; //���뻺����
	//check....���ܴ�������
	T* pPkg = new(pBuf) T(ts...);
	return std::shared_ptr<PkgHeader>(pPkg, [](PkgHeader* p) {delete[](uint8_t*)p; /*�ͷ�*/});
}




//�հ�
std::shared_ptr<PkgHeader> RecvPackage(SOCKET sock);
int RecvBytes(SOCKET sock, int8_t* pBuff, int32_t nLen);

//����
void SendPackage(SOCKET sock, std::shared_ptr<PkgHeader> pPkg);
int SendPackageRe(SOCKET sock, std::shared_ptr<PkgHeader> pPkg);