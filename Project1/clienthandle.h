#pragma once
#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H


#include <string>
#include "httpparsing.h"
#include "HttpRespond.h"

class ClientHandle{
	private:
		std::string _username;//�û�����
		
		std::string _userrights;//�û�Ȩ��

		HttpParsing* _httpparsing;//http��������

		HttpRespond* _httprespond;//http��Ӧ����

		bool connection;

		int connfd;
public:
	ClientHandle();

	~ClientHandle(); 

	void requesthandle(int connfd);
	
	void requestinit(int connfd);

	static ClientHandle* stahandle(ClientHandle* ptr, int connfd);

	const bool& getconnection() const;

	const int& getfd() const;

	void send_content();


};
#endif // !1




