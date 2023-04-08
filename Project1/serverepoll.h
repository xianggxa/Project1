#pragma once
#ifndef SERVER_EPOLL
#define SERVER_EPOLL
#define EP_EVENTS_SIZE 1024
#define THREAD_POLL_SIZE 20
#define TIMER_ARRAY_SIZE 100

#include <sys/epoll.h>
#include <sys/socket.h>
#include <map>
#include "clienthandle.h"
#include "threadpool.h"
#include <unistd.h>
#include "timer.h"
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

	std::map<int, std::pair<ClientHandle*, heap_timer_node*> > socket_clienthandle_map;

	timer_heap* server_timer;//��ʱ��

	std::mutex mtx;
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

		server_timer = new timer_heap(TIMER_ARRAY_SIZE);
	}
	~ServerEpoll(){
		delete serthreadpool;
	
	}
	void handle_Timeout_event() {//����ʱ����
		while (!server_timer->is_empty()) {
			heap_timer_node* n_node = server_timer->get_top();
			if (!n_node->validFlag) {
				server_timer->del_top();
				continue;
			}
			
			if (n_node->isTimeout()) {
				int dfd = n_node->connfd;
				if (socket_clienthandle_map.find(dfd) != socket_clienthandle_map.end()) {
					std::lock_guard<std::mutex> mylock_guard(mtx);
					printf("close connfd %d\n", dfd);
					auto  clientpair = socket_clienthandle_map[dfd];
					//delete clientpair.first;
					epoll_ctl(_epollfd, EPOLL_CTL_DEL, dfd, nullptr);
					//�Ƴ�����
					close(dfd);
					socket_clienthandle_map.erase(dfd);
				}
				server_timer->del_top();

			}
			else break;

		}

	}
	void start() {
		printf("server start\n");
		while (1) {
			_event_count = epoll_wait(_epollfd, ep_events, EP_EVENTS_SIZE, -1);
			//�����ȴ��¼�
			printf("event num =%d\n", _event_count);
			for (int i = 0; i < _event_count; i++) {

				if (ep_events[i].data.fd == _listenfd) { // ���µ����ӽ���
					
					int newfd = accept(_listenfd, nullptr, nullptr);
					printf("newconnection %d\n", newfd);
					_epev.data.fd = newfd;
					_epev.events = EPOLLIN | EPOLLET;
					epoll_ctl(_epollfd, EPOLL_CTL_ADD, newfd, &_epev);
					ClientHandle* clienthandle = new ClientHandle();
					heap_timer_node* clientnode = new heap_timer_node(newfd);

					socket_clienthandle_map[newfd] = { clienthandle,clientnode};
					//server_timer->add_timer(clientnode);

				}
				else if (ep_events[i].events & EPOLLIN) {
					
					auto  clientpair = socket_clienthandle_map[ep_events[i].data.fd];
					int nowfd = ep_events[i].data.fd;
					printf("old %d\n",nowfd);

					server_timer->del_timer(clientpair.second);
					heap_timer_node* clientnode = new heap_timer_node(ep_events[i].data.fd);
					server_timer->add_timer(clientnode);
					socket_clienthandle_map[ep_events[i].data.fd].second = clientnode;
					printf("add work\n");


					//clientpair.first->requestinit(nowfd);
					std::future<ClientHandle*> fu = serthreadpool->commit(ClientHandle::stahandle, clientpair.first, nowfd);
					//��http��Ӧ��������̳߳��������
					
					ClientHandle* _handle = fu.get();
					if (_handle->getconnection()==false) {//���ӶϿ�
						printf("close fd:%d\n", nowfd);
						int dfd = _handle->getfd();
						if (socket_clienthandle_map.find(dfd) != socket_clienthandle_map.end()) {
						std::lock_guard<std::mutex> mylock_guard(mtx);
						auto  clientpair = socket_clienthandle_map[ep_events[i].data.fd];
						server_timer->del_timer(clientpair.second);
						
						epoll_ctl(_epollfd, EPOLL_CTL_DEL, dfd, nullptr);
						//�Ƴ�����
						close(dfd);
						socket_clienthandle_map.erase(dfd);
						
						delete _handle;

						
						}

						
					}
					
					/*
					//��д�¼����
					_epev.data.fd = nowfd;

					_epev.events = EPOLLET | EPOLLOUT;

					epoll_ctl(_epollfd, EPOLL_CTL_MOD, nowfd, &_epev);
					*/



				}/*
				else if (ep_events[i].events & EPOLLOUT) {
					auto  clientpair = socket_clienthandle_map[ep_events[i].data.fd];
					int nowfd = ep_events[i].data.fd;
					clientpair.first->send_content();
					server_timer->del_timer(clientpair.second);
					if (clientpair.first->getconnection() == false) {
						
						if (socket_clienthandle_map.find(nowfd) != socket_clienthandle_map.end()) {
							//std::lock_guard<std::mutex> mylock_guard(mtx);
							epoll_ctl(_epollfd, EPOLL_CTL_DEL, nowfd, nullptr);
							//�Ƴ�����
							close(nowfd);
							delete clientpair.first;
							socket_clienthandle_map.erase(nowfd);
						}
						
						
					}
					else {
						heap_timer_node* clientnode = new heap_timer_node(ep_events[i].data.fd);
						server_timer->add_timer(clientnode);
						socket_clienthandle_map[ep_events[i].data.fd].second = clientnode;
						_epev.data.fd = nowfd;
						_epev.events = EPOLLIN | EPOLLET;
						epoll_ctl(_epollfd, EPOLL_CTL_MOD, nowfd, &_epev);
					}

					
				}*/
			}
			//printf("epoll wait\n");
			handle_Timeout_event();
		}

	}



};
#endif // !1



