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
//������Ϣ��־
#ifndef HeartBeatMessage
#define HeartBeatMessage   "keepalive"//������Ϣ
#endif
//����˶೤ʱ����ͻ��˷���һ��������Ϣ(��λs)
#ifndef HeartBeatMessageInterval
#define HeartBeatMessageInterval 10
#endif
//��������֧�ֵ����������
#ifndef MAX_LISTEN
#define MAX_LISTEN 4
#endif
//�������ݽ��ջ�������С
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 50000
#endif

//�ص�������ʾ
#ifndef CALLBACK
#define CALLBACK
#endif

	//������ͨ�����ûص����������Ի�ȡ����ͨ�Ź����е���ϸ����
typedef  int (CALLBACK* RecvDataCBFun)(char* pBuf,int len);
//��������ͨ�ŵ�ַ��Ϣ�ṹ��
typedef struct 
{
	string ip;
	int port;
}SMVD_SOCKETADDR,*PSMVD_SOCKETADDR;

#endif
