#include "MyTask.h"
#include "TcpSockeServer.h"

void* MyTask::ThreadProc(LPVOID lpParam)
{
	MyTask* mytask = static_cast<MyTask*>(lpParam);
	CTCPSocketServer* server = mytask->m_sockServer;
	while (1)
	{
		//pthread_mutex_lock(&server->m_csData);
		for (map<int, vector<string> >::iterator it = server->m_mapRecvData.begin(); it != server->m_mapRecvData.end();)
		{
			pthread_mutex_lock(&server->m_csData);
			vector<string>& datas = it->second;
			pthread_mutex_unlock(&server->m_csData);
			string oneRequest = "";
			bool isfulldata = false;
			int len = 0;
			//组装接收到的消息包，得到完整的一个客户请求
			if (datas.size() > 0)
			{
				memcpy(&len, datas[0].c_str(), sizeof(int));
			}

			size_t totallen = 0;
			for (int i = 0; i < datas.size(); ++i)
			{
				totallen += datas[i].length();
			}
			if (1)//totallen == len
			{
				string oneRequest = "";
				for (int i = 0; i < datas.size(); ++i)
				{
					oneRequest.append(datas[i]);
				}
				printf("recv:%s\n",oneRequest.c_str());
				isfulldata = true;

				//回复
				string strAnswer = "I am server\n";
				server->SendData2Client(strAnswer.c_str(), strAnswer.length(), it->first);
			}
			if (isfulldata)
			{
				server->m_mapRecvData.erase(it++);
				//pthread_mutex_unlock(&server->m_csData);
			}
			else
			{
				++it;
			}
		}
		//pthread_mutex_unlock(&server->m_csData);
	}
	
	return ((void *)0);
}

MyTask::MyTask()
{
	m_sockServer = new CTCPSocketServer();
}

MyTask::~MyTask()
{
}

bool MyTask::MainTask()
{
	bool ret = false;
	CTCPSocketServer::SetEnvironment();
	int bCreateServer = m_sockServer->CreateServer("192.168.1.210", 65432, 0);
	if (bCreateServer == 0)//>成功
	{
		ret = true;
		//>创建线程 
		pthread_t iThreadId;
		int iTemp = pthread_create(&iThreadId, NULL, ThreadProc, this); 
	}
	else
	{
		printf("MyTask::MainTask create socket failed");
	}
	return ret;

}

