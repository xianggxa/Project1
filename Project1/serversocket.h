#pragma once
#ifndef SERVER_SOCKET
#define SERVER_SOCKET

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXLINK 2048
class ServerSocket {
private:
	int listenfd;
	struct sockaddr_in serveraddr;

public:
	ServerSocket(uint16_t port) {
		bzero(&serveraddr, sizeof serveraddr);
		serveraddr.sin_family = AF_INET;
		//ipv4Э���
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		//INADDR_ANY��ָ����������ip��htonl������ַ�����ַת��Ϊlong������
		serveraddr.sin_port = htons(port);
		//��һ���޷��Ŷ�������ֵת��Ϊ�����ֽ���
		
	}
	~ServerSocket(){}
	int init() {
         listenfd = socket(AF_INET, SOCK_STREAM, 0);
		//ipv4Э��غ�tcp��
		if (-1 == listenfd) {
			perror("socket����ʧ��");
			return -1;
		}
		if (-1 == bind(listenfd, (struct sockaddr*)&serveraddr, sizeof serveraddr)) {
			perror("�󶨶˿�ʧ��");
			return -1;
		}
		if (-1 == listen(listenfd, MAXLINK)) {
			perror("�����˿�ʧ��");
			return -1;
		}

	}
	const int& getfd () const {
		return listenfd;

	}

};
#endif // !1