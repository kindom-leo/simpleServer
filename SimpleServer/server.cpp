#define WIN32_LEAN_AND_MEAN //解决Windows.h 和WinSock2.h 之间的冲突宏，win下有效
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include <string.h>
#include <string>
#include <algorithm>
#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 60000
#define CONNECT_NUMS 5


struct DataPackage {
	int		m_age;
	char	m_chArrName[32];
};
int main() {
	WORD ver = MAKEWORD(2, 1);
	WSADATA data;
	WSAStartup(ver, &data);
	//1 create a socket 
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock) {
		perror("sock()");
		return -1;
	}

	//2 bind ip and port
	sockaddr_in sa{ AF_INET,htons(SERVER_PORT),INADDR_ANY };
	int n = bind(sock, (sockaddr*)&sa, sizeof(sockaddr_in));
	if (SOCKET_ERROR == n) {
		perror("bind()");
		return -1;
	}

	//3 listen
	n = listen(sock, CONNECT_NUMS);
	if (SOCKET_ERROR == n) {
		perror("listen()");
		return -1;
	}

	//4 accept
	sockaddr_in sa_client = {};
	n = sizeof(sockaddr_in);
	
	SOCKET sock_client = INVALID_SOCKET;
	sock_client = accept(sock, (sockaddr*)&sa_client, &n);
	if (INVALID_SOCKET == sock_client) {
		perror("accept()");
		return -1;
	}
	fprintf(stdout, "new client comming :Ip = %s\n", inet_ntoa(sa_client.sin_addr));
	char cmdMsg[128];
	while(true){
		memset(cmdMsg, 0, sizeof(cmdMsg));
		int nlen = recv(sock_client, cmdMsg, sizeof(cmdMsg), 0);
		if (nlen <= 0) {
			fprintf(stdout, "client has quit , mission complete!");
			break;
		}
		std::for_each(std::begin(cmdMsg), std::end(cmdMsg), [](char& c) { c = std::tolower(c); });
		if (0 == strcmp(cmdMsg, "getinfo")) {
			DataPackage dp{ 38,"kindom-leo" };
			send(sock_client, (const char*)&dp, sizeof(DataPackage), 0);
		}  else {
			char msgBuf[] = "hello , i am server! what you wanna do?";
			send(sock_client, msgBuf, strlen(msgBuf) + 1, 0);
		}
		
	}
	

	//6 close socket
	closesocket(sock);
	WSACleanup();
}