#pragma once
#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H


#include <string>
#include "httpparsing.h"
class ClientHandle{
	private:
		std::string _username;//用户姓名
		
		std::string _userrights;//用户权限

		HttpParsing* _httpparsing;//http解析对象

		bool connetion;
public:
	ClientHandle();

	~ClientHandle(); 

	void requesthandle(int connfd);

	static ClientHandle* stahandle(ClientHandle* ptr, int connfd);

	const bool& getconnection() const;


};
#endif // !1




