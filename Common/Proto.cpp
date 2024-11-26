#include"Proto.h"
//#include"InitSockLib.h"
int RecvBytes(SOCKET sock, int8_t* pBuff, int32_t nLen)
{
	int nRecvBytes = 0;
	while (nRecvBytes < nLen)  //С��ָ�����ֽڳ���ʱ������
	{
		int nRet = recv(sock, (char*)pBuff + nRecvBytes, nLen - nRecvBytes, 0);
		//check ..------ע���޸�
		//if (nRet == 0) {
		//	return 0;
		//}
		nRecvBytes += nRet;
	}
	return nRecvBytes;
}

void SendPackage(SOCKET sock, std::shared_ptr<PkgHeader> pPkg)
{
	send(sock, (char*)pPkg.get(), sizeof(PkgHeader) + pPkg->m_nLen,0);
}

int SendPackageRe(SOCKET sock, std::shared_ptr<PkgHeader> pPkg)
{
	return send(sock, (char*)pPkg.get(), sizeof(PkgHeader) + pPkg->m_nLen, 0);
}

std::shared_ptr<PkgHeader> RecvPackage(SOCKET sock)
{
	//���հ�ͷ
	PkgHeader header;
	//�ȴ�ȫ�����ݻ�ȡ
	int nRet = RecvBytes(sock, (int8_t*)&header, sizeof(header));
	if (nRet == 0 || nRet == SOCKET_ERROR)
	{
		return NULL;
	}

	int8_t* pBuff = new int8_t[sizeof(header) + header.m_nLen];
	memcpy(pBuff, &header, sizeof(header));
	//check....

	//˵���������ݣ���������
	if (header.m_nLen != 0)
	{
		nRet = RecvBytes(sock, (int8_t*)pBuff + sizeof(header), header.m_nLen);
		//���
	}

	return std::shared_ptr<PkgHeader>((PkgHeader*)pBuff, [](PkgHeader* p)    //1:02:11  �ͷ�ʱ���ú���
		{delete (int8_t*)p; 
		});
}