#include"Proto.h"
//#include"InitSockLib.h"
int RecvBytes(SOCKET sock, int8_t* pBuff, int32_t nLen)
{
	int nRecvBytes = 0;
	while (nRecvBytes < nLen)  //小于指定的字节长度时继续收
	{
		int nRet = recv(sock, (char*)pBuff + nRecvBytes, nLen - nRecvBytes, 0);
		//check ..------注意修改
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
	//先收包头
	PkgHeader header;
	//等待全部数据获取
	int nRet = RecvBytes(sock, (int8_t*)&header, sizeof(header));
	if (nRet == 0 || nRet == SOCKET_ERROR)
	{
		return NULL;
	}

	int8_t* pBuff = new int8_t[sizeof(header) + header.m_nLen];
	memcpy(pBuff, &header, sizeof(header));
	//check....

	//说明还有数据，继续接收
	if (header.m_nLen != 0)
	{
		nRet = RecvBytes(sock, (int8_t*)pBuff + sizeof(header), header.m_nLen);
		//检查
	}

	return std::shared_ptr<PkgHeader>((PkgHeader*)pBuff, [](PkgHeader* p)    //1:02:11  释放时调用函数
		{delete (int8_t*)p; 
		});
}