#include "clienthandle.h"
#include <iostream>

ClientHandle::ClientHandle()
{
	_userrights = "visitor";//初始权限为游客
	_httpparsing = new HttpParsing();
}

ClientHandle::~ClientHandle()
{
	delete _httpparsing;
}

 void ClientHandle::requesthandle(int connfd)
{
		_httpparsing->init(connfd);
		if (_httpparsing->getLen() <= 0 || _httpparsing->getRequestParams().find("Connection")!= _httpparsing->getRequestParams().end()&& _httpparsing->getRequestParams().find("Connection")->second=="close") {
			connetion = 0;//断开连接
		}
		std::cout << "[method]" << _httpparsing->getMethod() << std::endl;
		std::cout << "[url]" << _httpparsing->getUrl() << std::endl;
		std::cout << "[request params]" << std::endl;
		for (auto& p : _httpparsing->getRequestParams()) {
			std::cout << "++ key: " << p.first << std::endl;
			std::cout << "   value: " << p.second << std::endl;
		}
		std::cout << "[protocol]" << _httpparsing->getProtocol() << std::endl;
		std::cout << "[version]" << _httpparsing->getVersion() << std::endl;
		std::cout << "[request headers]" << std::endl;
		for (auto& h : _httpparsing->getHeaders()) {
			std::cout << "++ key: " << h.first << std::endl;
			std::cout << "   value: " << h.second << std::endl;
		}
		std::cout << std::endl;
		std::cout << "[body]" << _httpparsing->getBody() << std::endl;
}

 ClientHandle* ClientHandle::stahandle(ClientHandle* ptr, int connfd)
 {
	 ptr->requesthandle(connfd);
	 return ptr;
 }

 const bool& ClientHandle::getconnection() const
 {
	 return connetion;
 }
