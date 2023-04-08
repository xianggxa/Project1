#include "clienthandle.h"
#include <iostream>

ClientHandle::ClientHandle()
{
	_userrights = "visitor";//��ʼȨ��Ϊ�ο�
	_httpparsing = new HttpParsing();

	_httprespond = new HttpRespond(_httpparsing);
	connection = 1;
}

ClientHandle::~ClientHandle()
{
	delete _httpparsing;
	delete _httprespond;
}

 void ClientHandle::requesthandle(int connfd)
{
		_httpparsing->init(connfd);
		this->connfd = connfd;
		
		/*
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
		std::cout << "[body]" << _httpparsing->getBody() << std::endl;*/
		do {
			if (_httpparsing->getLen() <= 0 || _httpparsing->getError()) {
				connection = false;//�Ͽ�����
				printf("connection error 1\n");
				break;

			}
			else if (_httpparsing->getHeaders().find("Connection") == _httpparsing->getHeaders().end() || _httpparsing->getHeaders().find("Connection")->second == "close") {
				connection = false;//�Ͽ�����
				printf("connection close 2\n");
			}
			_httprespond->respond(connfd);
		} while (0);
		/*
		if (_httpparsing->getLen() <= 0 || _httpparsing->getRequestParams().find("Connection")!= _httpparsing->getRequestParams().end()&& _httpparsing->getRequestParams().find("Connection")->second=="close"||_httpparsing->getError()) {
			connection = false;//�Ͽ�����
			printf("connection close\n");
		}
		else {
			_httprespond->respond(connfd);

		}*/
		//if (!_httpparsing->getError())
			
		//else connection = 0;//���ִ���Ͽ�����
}

 void ClientHandle::requestinit(int connfd)
 {
	 _httpparsing->init(connfd);
 }

 ClientHandle* ClientHandle::stahandle(ClientHandle* ptr, int connfd)
 {
	 ptr->requesthandle(connfd);
	 ptr->send_content();
	 return ptr;
 }

 const bool& ClientHandle::getconnection() const
 {
	 return connection;
 }
 const int& ClientHandle::getfd() const
 {
	 return connfd;
 }

 void ClientHandle::send_content()
 {
	 //printf("connfd=%d\n", connfd);
	 Tools::_send(this->connfd, _httprespond->send_content, _httprespond->send_content_len);
	 _httprespond->send_content = nullptr;
	 _httprespond->send_content_len = 0;
 }

