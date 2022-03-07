#pragma once
#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H


#include <string>
#include "httpparsing.h"
class ClientHandle{
	private:
		std::string _username;//�û�����
		
		std::string _userrights;//�û�Ȩ��

		HttpParsing* _httpparsing;//http��������

		bool connetion;
public:
	ClientHandle();

	~ClientHandle(); 

	void requesthandle(int connfd);

	static ClientHandle* stahandle(ClientHandle* ptr, int connfd);

	const bool& getconnection() const;


};
#endif // !1




