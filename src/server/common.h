#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>

#include <vector>
#include <map>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "include/pthread.h"
#include <time.h>

using namespace std;

#ifndef LINUX_OS
#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"lib/x64/pthreadVC2.lib")
#endif

#if defined(_WIN64)
	#include <windows.h>
	#define WIN64_OS
	typedef long long SMVD_INT;
#elif defined(_WIN32)
	#include <windows.h>
	#define WIN32_OS
#else
	typedef int SMVD_INT;
#endif

#ifdef linux
	#define LINUX_OS
#endif
#ifdef LINUX_OS
	#include  <unistd.h>
	#include  <sys/types.h>       /* basic system data types */
	#include  <sys/socket.h>      /* basic socket definitions */
	#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
	#include  <arpa/inet.h>       /* inet(3) functions */
	#include <netdb.h> /*gethostbyname function */
	#include <errno.h>
	typedef int SMVD_INT;
	#ifndef IPPROTO_TCP 
		#define IPPROTO_TCP 0
	#endif
#endif
//心跳消息标志
#ifndef HeartBeatMessage
#define HeartBeatMessage   "keepalive"//保活消息
#endif
//服务端多长时间向客户端发送一次心跳消息(单位s)
#ifndef HeartBeatMessageInterval
#define HeartBeatMessageInterval 10
#endif
//定义服务端支持的最大连接数
#ifndef MAX_LISTEN
#define MAX_LISTEN 4
#endif
//定义数据接收缓冲区大小
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 50000
#endif

//回调函数标示
#ifndef CALLBACK
#define CALLBACK
#endif

	//调用者通过设置回调函数，可以获取网络通信过程中的详细错误
typedef  int (CALLBACK* RecvDataCBFun)(char* pBuf,int len);
//定义网络通信地址信息结构体
typedef struct 
{
	string ip;
	int port;
}SMVD_SOCKETADDR,*PSMVD_SOCKETADDR;

#endif
