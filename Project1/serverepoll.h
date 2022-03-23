#pragma once
#ifndef SERVER_EPOLL
#define SERVER_EPOLL
#define EP_EVENTS_SIZE 1024
#define THREAD_POLL_SIZE 4

#include <sys/epoll.h>
#include <sys/socket.h>
#include <map>
#include "clienthandle.h"
#include "threadpool.h"
#include <unistd.h>
class ServerEpoll
{
private:

	struct epoll_event _epev;
	//��ʱ�¼�
	int _epollfd;
	//epoll�ļ�������
	struct epoll_event ep_events[EP_EVENTS_SIZE];
	//�������¼�����
	int _event_count;
	//�������¼�����
	int _listenfd;

	std::threadpool* serthreadpool;

	std::map<int, ClientHandle*> socket_clienthandle_map;
public:
	ServerEpoll(int _listenfd) {
		this->_listenfd = _listenfd;
		_epollfd = epoll_create(10);
		//����epoll������
		_epev.data.fd = _listenfd;
		_epev.events = EPOLLIN;
		//���ü����¼�
		epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listenfd, &_epev);
		//����ʼ_listenfd���¼���������
		serthreadpool = new std::threadpool(THREAD_POLL_SIZE);
	}
	~ServerEpoll(){
		delete serthreadpool;
	
	}
	void start() {
		printf("server start\n");
		while (1) {
			_event_count = epoll_wait(_epollfd, ep_events, EP_EVENTS_SIZE, -1);
			//�����ȴ��¼�
			for (int i = 0; i < _event_count; i++) {

				if (ep_events[i].data.fd == _listenfd) { // ���µ����ӽ���
					
					int newfd = accept(_listenfd, nullptr, nullptr);
					printf("newconnection %d\n", newfd);
					_epev.data.fd = newfd;
					_epev.events = EPOLLIN;
					epoll_ctl(_epollfd, EPOLL_CTL_ADD, newfd, &_epev);
					ClientHandle* clienthandle = new ClientHandle();
					socket_clienthandle_map[newfd] = clienthandle;

				}
				else if (ep_events[i].events & EPOLLIN) {
					printf("old \n");
					ClientHandle* clienthandle = socket_clienthandle_map[ep_events[i].data.fd];
					int nowfd = ep_events[i].data.fd;

					std::future<ClientHandle*> fu = serthreadpool->commit(ClientHandle::stahandle, clienthandle, nowfd);
					//��http��Ӧ��������̳߳��������

					if (fu.get()->getconnection()==false) {//���ӶϿ�

						epoll_ctl(_epollfd, EPOLL_CTL_DEL, nowfd, nullptr);
						//�Ƴ�����
						close(nowfd);
						socket_clienthandle_map.erase(nowfd);

						delete clienthandle;



					}



				}
			}
		}

	}



};
#endif // !1



