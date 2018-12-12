#ifndef _H_TCP_SERVER_
#define _H_TCP_SERVER_
#include "common.h"


class CTCPSocketServer
{
public:
	CTCPSocketServer();
	~CTCPSocketServer();
public:
	pthread_mutex_t             m_csData;
	pthread_mutex_t				m_csSend;
	pthread_mutex_t				m_csSocket;
	map<int, vector<string> >   m_mapRecvData;
private:
	SOCKET						m_sockServer;
	void*						m_hThread;
	SOCKET						m_fd_ArrayC[MAX_LISTEN];
	unsigned int				m_sendHeartTime[MAX_LISTEN];
	fd_set						m_fdRead;
	int							m_iNumSock;
private:

	/*
	  function : 存储接受到的数据
		@
	*/
	int PushDataToReceiveQueue(const string &data,int whichclient);

	/*
	  function : 通过socket获取ip地址
		@
	*/
	bool GetAddressBySocket(int peersocket, SMVD_SOCKETADDR &peeraddr);

public:

	/*
	  function : 主逻辑
		@
	*/
	int MainLogic();

	/*
	  function : 初始化网络环境
		@
	*/
	static void SetEnvironment();

	/*
	  function : 重置网络环境
		@
	*/
	static void ResetEnvironment();

	/*
	  brief : 创建套接字
		#mode：阻塞非阻塞
	*/

	int CreateServer(char* serverIp,int port, DWORD mode);
	/*
	  function : 向客户端发送数据
		@whichclient：客户端id
	*/
	int SendData2Client(const char* buf,int len,int whichclient);
	
};

#endif