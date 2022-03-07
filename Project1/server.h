#pragma once
#ifndef SERVER
#define SERVER

#include "serverepoll.h"
#include "serversocket.h"

#define PORT 80

class Server {
public:
	void run() {
		ServerSocket* serversocket = new ServerSocket(PORT);
		serversocket->init();
		ServerEpoll* serverepoll = new ServerEpoll(serversocket->getfd());
		serverepoll->start();

	}



};

#endif // SERVER
