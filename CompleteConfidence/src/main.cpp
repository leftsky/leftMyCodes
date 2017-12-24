
#include "Socket.h"
#include "event2/event.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/thread.h"
#include "WebSocket.h"
#include "LeftMyCodes/MyCodes.h"

#include <iostream>
#include <regex>
#include "easylogging/easylogging++.h"

#define IP		"127.0.0.1"
#define PORT	20002

INITIALIZE_EASYLOGGINGPP

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
	char msg[4096];
	char RtMsg[1024];
	len = bufferevent_read(bev, msg, sizeof(msg) - 1);
	msg[len] = '\0';
	std::regex MagicKey("[\\S,\\s]+Sec-WebSocket-Key[\\S,\\s]+");
	if (std::regex_match(msg, MagicKey)) {
		CCWebSocket::GetAnswerStr(msg, RtMsg, 1024);
	}
	else {
		CCWebSocket::ReadPacket(msg, len, RtMsg, 1024);
		LOG(INFO) << "Recv >>> " << strlen(RtMsg) << " :: " << RtMsg;
		snprintf(msg, 4069, RtMsg);
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
