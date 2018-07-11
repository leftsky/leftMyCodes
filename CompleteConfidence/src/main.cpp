
#include "Socket.h"
#ifdef _USE_LIBEVENT_
#include "event2/event.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/thread.h"
#endif
#include "WebSocket.h"
#include "LeftMyCodes/MyCodes.h"

#include <iostream>
#include <regex>
#include "easylogging/easylogging++.h"

#define IP		"127.0.0.1"
#define PORT	20002

INITIALIZE_EASYLOGGINGPP

#ifdef _USE_LIBEVENT_

void socket_event_cb(bufferevent *bev, short events, void *arg) {
	if (events & BEV_EVENT_EOF) {
		LOG(INFO) << "connection closed";
	}
	else if (events & BEV_EVENT_ERROR)
		LOG(INFO) << "some other error";
	bufferevent_free(bev);
}

void socket_read_cb(bufferevent *bev, void *arg) {
	size_t len = 0;
    char time[20];
	char msg[4096];
	char RtMsg[1024];

    memset(msg, 0, sizeof(msg));
    memset(RtMsg, 0, sizeof(RtMsg));
	len = bufferevent_read(bev, msg, sizeof(msg) - 1);
	msg[len] = '\0';
	std::regex MagicKey("[\\S,\\s]+Sec-WebSocket-Key[\\S,\\s]+");
	if (std::regex_match(msg, MagicKey)) {
		CCWebSocket::GetAnswerStr(msg, RtMsg, 1024);
	}
	else {
        if (CCWebSocket::ReadPacket(msg, len, RtMsg, 1024) == NULL)
        {
            CCWebSocket::PackageAddHead("ReadPacket is NULL", strlen("ReadPacket is NULL"), RtMsg, 1024);
            LOG(INFO) << "ReadPacket is NULL";
            bufferevent_write(bev, RtMsg, strlen(RtMsg));
            return;
        }
        RtMsg[1024 - 1] = '\0';
		LOG(INFO) << "Recv >>> " << strlen(RtMsg) << " :: " << RtMsg;
		snprintf(msg, 4069, "Recv: %d %s \r\n %s", strlen(RtMsg), leftName::GetTimeStr(time, 20), RtMsg);
		//snprintf(msg, 4069, "%s", RtMsg);
        CCWebSocket::PackageAddHead(msg, strlen(msg), RtMsg, 1024);
	}
	LOG(INFO) << "RtMsg >>> " << strlen(RtMsg) << " :: " << RtMsg;
	bufferevent_write(bev, RtMsg, strlen(RtMsg));
}

void listener_cb(evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sock, int socklen, void *arg) {
	char ip[16];
	event_base *base = (event_base*)arg;
	sockaddr_in *clientAddress = (sockaddr_in*)sock;
	unsigned short port = clientAddress->sin_port;
	inet_ntop(AF_INET, IPFromAddr((*clientAddress)), ip, 16);
	LOG(INFO) << "Accept " << ip << "::" << port;
	bufferevent *bev = bufferevent_socket_new(base, fd,
		BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void EasyLoggingInit() {
	el::Configurations conf("easylogging.conf");
	el::Loggers::reconfigureLogger("default", conf);
	el::Loggers::reconfigureAllLoggers(conf);
}

void InitAll() {
	EasyLoggingInit();
	LeftSocket::InitializeSocket();
}

int WebSocketMain(char *ip, short port) {
	InitAll();
	LeftThrNo ThrNo = 0;
	LOG(INFO) << "You chose " << ip << ":" << port;
	LOG(INFO) << "I will try to build it ";
#ifdef __linux__
	evthread_use_pthreads();
#endif
#ifdef _WIN32
	evthread_use_windows_threads();
#endif
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	inet_pton(AF_INET, ip, IPFromAddr(sin));
	event_base *base = event_base_new();
	evconnlistener *listener
		= evconnlistener_new_bind(base, listener_cb, base,
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
			500, (struct sockaddr*)&sin,
			sizeof(struct sockaddr_in));
	event_base_dispatch(base);
	evconnlistener_free(listener);
	event_base_free(base);
	LeftSocket::CleanSocket();
	return 0;
}

int main(int argc, char **argv) {
	WebSocketMain(IP, PORT);
	return 0;
}

#else



typedef struct _CLIENT_INFO {
    sockaddr_in ClientAddress;
    LeftSokt	Socket;
} ClientInfo, *pClientInfo;

bool socket_read_cb(char* InBuf, int InBufLen, char *OutBuf) {
    size_t len = 0;
    char time[20];
    char msg[4096];
    char RtMsg[1024];

    memset(msg, 0, sizeof(msg));
    memset(RtMsg, 0, sizeof(RtMsg));
    snprintf(msg, 4069, InBuf);
    len = strlen(msg);
    msg[len] = '\0';
    std::regex MagicKey("[\\S,\\s]+Sec-WebSocket-Key[\\S,\\s]+");
    if (std::regex_match(msg, MagicKey)) {
        CCWebSocket::GetAnswerStr(msg, RtMsg, 1024);
    }
    else {
        if (CCWebSocket::ReadPacket(msg, len, RtMsg, 1024) == NULL)
        {
            CCWebSocket::PackageAddHead("ReadPacket is NULL", strlen("ReadPacket is NULL"), RtMsg, 1024);
            LOG(INFO) << "ReadPacket is NULL";
            //memcpy(OutBuf, RtMsg, sizeof(RtMsg));
            snprintf(OutBuf, 4069, RtMsg);
            return false;
        }
        RtMsg[1024 - 1] = '\0';
        LOG(INFO) << "Recv >>> " << strlen(RtMsg) << " :: " << RtMsg;
        snprintf(msg, 4069, "Recv: %d %s \r\n %s", strlen(RtMsg), leftName::GetTimeStr(time, 20), RtMsg);
        //snprintf(msg, 4069, "%s", RtMsg);
        CCWebSocket::PackageAddHead(msg, strlen(msg), RtMsg, 1024);
    }
    //LOG(INFO) << "RtMsg >>> " << strlen(RtMsg) << " :: " << RtMsg;
    //memcpy(OutBuf, RtMsg, sizeof(RtMsg));
    snprintf(OutBuf, 4069, RtMsg);
    return true;
}



LeftThrReturn OneConn(LeftThrArgv pCltIf) {
    ClientInfo CltInfo = *(pClientInfo)pCltIf;
    char ip[16];
    char time[20];
    char OutBuf[4069];
    unsigned short port = CltInfo.ClientAddress.sin_port;
    inet_ntop(AF_INET, IPFromAddr(CltInfo.ClientAddress), ip, 16);

    LOG(INFO) << "Accept " << ip << ":" << port;
    LOG(INFO) << "Now time: " << leftName::GetTimeStr(time, 20);
    char buf[4096];
    int bytes;
    for (;;) {
        LeftSleep(100);
        memset(buf, 0, sizeof(buf));
        bytes = recv(CltInfo.Socket, buf, sizeof(buf), 0);
        if (bytes == SOCKET_ERROR || !bytes) {
            //break;
            continue;
        }
        //LOG(INFO) << ip << ":" << port << " >>> " << buf;
        
        socket_read_cb(buf, bytes, OutBuf);
        send(CltInfo.Socket, OutBuf, strlen(OutBuf), 0);
        if (bytes == SOCKET_ERROR || !bytes) {
            //break;
            continue;
        }
    }
    LeftCloseSocket(CltInfo.Socket);
    LOG(INFO) << "lost " << ip << ":" << port;
    LOG(INFO) << "Now time: " << leftName::GetTimeStr(time, 20);

    EndThread;
    return 0;
}

int server(char* ip, short port) {
    do {
        if (LeftSocket::InitializeSocket()) {
            LOG(INFO) << "InitializeSocket failed.";
            break;
        }

        LeftSokt serverSocket;
        if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            LOG(INFO) << "Create socket failed.";
            break;
        }
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(sockaddr_in));
        serverAddress.sin_family = AF_INET;
        inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
        serverAddress.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddress,
            sizeof(serverAddress)) == SOCKET_ERROR) {
            LOG(INFO) << "Bind failed.";
            break;
        }
        LOG(INFO) << "Bind at " << ip << ":" << port;

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            LOG(INFO) << "Listen failed.";
            break;
        }

        LeftSokt clientSocket;
        struct sockaddr_in clientAddress;
        memset(&clientAddress, 0, sizeof(clientAddress));
        LeftSoktLen addrlen = sizeof(clientAddress);
        LeftThrNo Id = 0;
        ClientInfo* CltInfo = NULL;
        for (;;) {
            if ((clientSocket = accept(serverSocket,
                (sockaddr*)&clientAddress, &addrlen)) == INVALID_SOCKET) {
                LOG(INFO) << "Accept failed.";
                break;
            }
            CltInfo = new ClientInfo;
            CltInfo->ClientAddress = clientAddress;
            CltInfo->Socket = clientSocket;
            CURRENCY_StartThread(OneConn, (LeftThrArgv)CltInfo, Id);
        }

    } while (false);
    LeftSocket::CleanSocket();
    return 0;
}

int client(char* ip, int port) {
    using namespace std;
    do {
        if (LeftSocket::InitializeSocket()) {
            cout << "InitializeSocket failed." << endl;
            break;
        }

        LeftSokt sock;
        if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            cout << "Create socket failed." << endl;
            break;
        }
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(sockaddr_in));
        inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            cout << "Connect failed." << endl;
            break;
        }
        char buf[4096];
        memset(buf, 0, sizeof(buf));
        while (1) {
            cout << ">";
            cin.getline(buf, 4069, '\n');
            if (send(sock, buf, (int)strlen(buf), 0) == SOCKET_ERROR) {
                cout << "Send failed" << endl;
                break;
            }
            memset(buf, 0, sizeof(buf));
            int bytes;
            if ((bytes = recv(sock, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
                cout << "Recv failed" << endl;
                break;
            }
            cout << "<< " << buf << endl;

        }
    } while (false);
    LeftSocket::CleanSocket();
    return 0;
}


int main(int argc, char* argv[]) {
    using namespace std;

    short port;
    char ip[20];
    switch (argc) {
    case 4:
        snprintf(ip, 20, argv[2]);
        port = atoi(argv[3]);
        LOG(INFO) << "You chose " << ip << ":" << port;
        LOG(INFO) << "I'll try to build it like a ";
        switch (hash_(argv[1])) {
        case "--s"_hash: LOG(INFO) << "server"; return server(ip, port);
        case "--c"_hash: LOG(INFO) << "client"; return client(ip, port);
        default: break;
        }
        break;
    default: break;
    }

    LOG(INFO) << "TCPTest Version 1.0.1 (c) left";
    LOG(INFO) << "Use like WebSocket mode(--s/--c) ip port";
    return -1;

}


#endif
