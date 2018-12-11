#include "TcpSockeServer.h"


void* ServerThreadProc(void* lpParam)
{
	CTCPSocketServer* pThis = (CTCPSocketServer*)lpParam;
	pThis->MainLogic();
	 return ((void *)0);  
}

CTCPSocketServer::CTCPSocketServer()
: m_hThread(NULL)
, m_sockServer(0)
, m_iNumSock(0)
{
	
	for(int i=0;i<MAX_LISTEN;++i)
	{
		g_fd_ArrayC[i] =0;
	}
	
	pthread_mutex_init(&m_csData, NULL);
	pthread_mutex_init(&m_csSend, NULL);
	pthread_mutex_init(&m_csSocket, NULL);
}
CTCPSocketServer::~CTCPSocketServer()
{
	pthread_mutex_destroy(&m_csData); 
	pthread_mutex_destroy(&m_csSocket); 
	pthread_mutex_destroy(&m_csSend);
	pthread_cancel(m_iThreadId);
}

void  CTCPSocketServer::SetEnvironment()
{
	WSAData  wsData;
	WSAStartup(MAKEWORD(2,2),&wsData);
}

void CTCPSocketServer::ResetEnvironment()
{
	WSACleanup();
}

int CTCPSocketServer::CreateServer(char* serverIp,int port , DWORD mode)
{
	addrServer.sin_family=AF_INET;
	addrServer.sin_addr.S_un.S_addr=inet_addr(serverIp);
	addrServer.sin_port=htons(port);
	m_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int ret = 0;
	//非阻塞模式设定
	ret = ioctlsocket(m_sockServer, FIONBIO, &mode);

	ret = bind(m_sockServer, (sockaddr*)&addrServer, sizeof(addrServer));
	if (ret != 0)
	{
		char str[200] = {0};
		sprintf(str,"CTCPSocketServer::CreateServer bind socket fail,ip=%s,port=%d\n",serverIp,port);
		printf(str);
	}
	ret = listen(m_sockServer, MAX_LISTEN);
	
	//创建是否成功
	if (ret == 0)
	{ 
		ret = pthread_create(&m_iThreadId, NULL, ServerThreadProc, this); //创建线程
		printf("create socket success\n");
    }
	else
	{
		printf("create socket error\n");
		closesocket(m_sockServer);
	}
	return 0;
}

int CTCPSocketServer::MainLogic()
{
	int nRes = 0;
	int i = 0;
	while (1)
	{
		FD_ZERO(&m_fdRead);//设置count =0;
		FD_SET(m_sockServer, &m_fdRead); //设置索引为count的值

		for (i = 0; i < m_iNumSock; ++i)
		{
			if (g_fd_ArrayC[i] != 0)
			{
				FD_SET(g_fd_ArrayC[i], &m_fdRead);
			}
		}
		//连接请求和接到报文
		nRes = select(0, &m_fdRead, NULL, NULL, NULL);
		//nRes = select(MAX_LISTEN+1, &fdread, NULL, NULL, NULL);
		if (nRes < 0)
		{
			printf("error\n");//服务器socket失效
			break;
		}
		else if (nRes == 0)
		{
			printf("select timeout\n");
			continue;
		}
		if (FD_ISSET(m_sockServer, &m_fdRead))
		{
			SOCKET AcceptSocket;
			int nSize = sizeof(sockaddr);
			sockaddr_in  addrClient;
			AcceptSocket = accept(m_sockServer, (sockaddr*)&addrClient, &nSize);
			if (m_iNumSock == MAX_LISTEN)
			{
				printf("服务器端连接已经满\n");
				const char* pBuf = "refuse";
				send(AcceptSocket, pBuf, (int)strlen(pBuf), 0);
				closesocket(AcceptSocket);
				continue;
			}
			if (AcceptSocket == WSAEWOULDBLOCK)
			{
				printf("非阻塞模式设定accept调用不正确\n");
				continue;
			}
			else if (AcceptSocket == INVALID_SOCKET)
			{
				printf("accept failed\n");
				continue;
			}
			pthread_mutex_lock(&m_csSocket);
			for (i = 0; i < MAX_LISTEN; ++i)
			{
				if (g_fd_ArrayC[i] == 0)
				{
					g_fd_ArrayC[i] = AcceptSocket;
					m_iNumSock++;
					break;
				}
			}
			pthread_mutex_unlock(&m_csSocket);
			printf("new client:port[%d]IP [%s] AcceptSocket=%d\n", ntohs(addrClient.sin_port), 
				inet_ntoa(addrClient.sin_addr), (int)AcceptSocket);
		}//if(FD_ISSET())
		for (i = 0; i < m_iNumSock; ++i)
		{
			pthread_mutex_lock(&m_csSocket);
			SOCKET whichclient = g_fd_ArrayC[i];
			pthread_mutex_unlock(&m_csSocket);
			if (whichclient == 0)
			{
				continue;
			}
			if (FD_ISSET(whichclient, &m_fdRead))
			{
				char recvBuff[BUFFER_SIZE] = { 0 };
				nRes = recv(whichclient, recvBuff, sizeof(recvBuff)-1, 0);
				if (nRes <= 0)
				{
					printf("client has closed\n");
					m_iNumSock--;
					closesocket(whichclient);
					pthread_mutex_lock(&m_csSocket);
					FD_CLR(g_fd_ArrayC[i], &m_fdRead);
					g_fd_ArrayC[i] = 0;
					pthread_mutex_unlock(&m_csSocket);
				}
				else
				{
					string temp;
					temp.assign(recvBuff, nRes);
					PushDataToReceiveQueue(temp, (int)whichclient);
				}//end else
			}//end if FD_ISSET
		}//end for
	}//while(true)
	printf("关闭服务器端socket\n");
	closesocket(m_sockServer);
	return 0;
}


int CTCPSocketServer::PushDataToReceiveQueue(const string &data,int whichclient)
{
	pthread_mutex_lock(&m_csData);
	m_mapRecvData[whichclient].push_back(data);
	printf("m_mapRecvData size = %d\n", (int)m_mapRecvData.size());
	pthread_mutex_unlock(&m_csData);
	return 0;
}


int CTCPSocketServer::SendData2Client(const char* buf,int len,int whichclient)
{
	int nLeft = len;
	if(nLeft == 0)
	{
		return 0;
	}
	int nlndex=0;
	int nError =0;
	pthread_mutex_lock(&m_csSend);
	while(nLeft>0)
	{

		nError=send(whichclient,buf+nlndex,nLeft,0);
		if(nError <0)
		{
#ifndef LINUX_OS
			closesocket(whichclient);
#else
			close(whichclient);
#endif
			break;
		}
		else if(nError==0)//数据全部发送完成
		{
			nLeft = 0;
			break;
		}
		else
		{
			nLeft-=nError;
			nlndex+=nError;
		}
	}

	pthread_mutex_unlock(&m_csSend);
	if(nLeft > 0)
	{
		pthread_mutex_lock(&m_csSocket);
		for(int i=0;i<MAX_LISTEN;++i)
		{
			if(g_fd_ArrayC[i] == whichclient)
			{
				FD_CLR(g_fd_ArrayC[i],&m_fdRead);
				g_fd_ArrayC[i] =0;
			}
		}
		pthread_mutex_unlock(&m_csSocket);
	}
	
	return nLeft > 0;
}

bool CTCPSocketServer::GetAddressBySocket(int peersocket, SMVD_SOCKETADDR &peeraddr)
{
	sockaddr_in peeraddress;
	int nAddrLen = sizeof(peeraddress);
	//根据套接字获取地址信息
	if (::getpeername(peersocket, (sockaddr*)&peeraddress, &nAddrLen) != 0)
	{
		printf("Get IP address by socket failed!n");
		return false;
	}
	peeraddr.ip = inet_ntoa(peeraddress.sin_addr);
	peeraddr.port = ntohs(peeraddress.sin_port);
	//读取IP和Port
	// cout<<"IP: "<<::inet_ntoa(m_address.sin_addr)<<"  PORT: "<<ntohs(m_address.sin_port)<<endl;
	return true;
}
