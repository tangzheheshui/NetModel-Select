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
	struct sockaddr_in			addrServer;
	SOCKET						g_fd_ArrayC[MAX_LISTEN];
	unsigned int				g_SendHeartTime[MAX_LISTEN];
	fd_set						m_fdRead;
	int							m_iNumSock;
	pthread_t					m_iThreadId;	//>�߳̾��
private:

	/*
	  function : �洢���ܵ�������
		@
	*/
	int PushDataToReceiveQueue(const string &data,int whichclient);

	/*
	  function : ͨ��socket��ȡip��ַ
		@
	*/
	bool GetAddressBySocket(int peersocket, SMVD_SOCKETADDR &peeraddr);

public:

	/*
	  function : ���߼�
		@
	*/
	int MainLogic();

	/*
	  function : ��ʼ�����绷��
		@
	*/
	static void SetEnvironment();

	/*
	  function : �������绷��
		@
	*/
	static void ResetEnvironment();

	/*
	  brief : �����׽���
		#mode������������
	*/

	int CreateServer(char* serverIp,int port, DWORD mode);
	/*
	  function : ��ͻ��˷�������
		@whichclient���ͻ���id
	*/
	int SendData2Client(const char* buf,int len,int whichclient);
	
};

#endif