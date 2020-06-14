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

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

struct DataHeader {
	unsigned short dataLen;
	unsigned short cmd;
};

struct Login :DataHeader {
	Login() {
		dataLen = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char 	m_username[32] = {};
	char	m_password[32] = {};
};

struct LoginResult :DataHeader{
	LoginResult() {
		dataLen = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout :DataHeader{
	Logout() {
		dataLen = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char username[32] = {};
};

struct LogoutResult :DataHeader{
	LogoutResult() {
		dataLen = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
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
	
	while(true){
		DataHeader dh{};
		int nlen = recv(sock_client, (char*)&dh, sizeof(DataHeader), 0);
		if (nlen <= 0) {
			fprintf(stdout, "client has quit , mission complete!");
			break;
		}
		
		switch (dh.cmd) {
		case CMD_LOGIN:
		{
			Login login{};
			LoginResult logres{ };
			recv(sock_client, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
			printf("recv cmd :CMD_LOGIN ,data length : %d  username: %s  password: %s\n", 
				login.dataLen,login.m_username,login.m_password);
			send(sock_client, (const char*)&logres, sizeof(LoginResult), 0);
		}
			break;
		case CMD_LOGOUT:
		{
			Logout logout{};
			LogoutResult logoutres;
			recv(sock_client, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
			printf("recv cmd :CMD_LOGOUT ,data length : %d  username: %s\n",
				logout.dataLen, logout.username);
			send(sock_client, (const char*)&logoutres, sizeof(LogoutResult), 0);
		}
			break;
		default:
			DataHeader dh{ CMD_ERROR,0 };
			send(sock_client, (const char*)&dh, sizeof(DataHeader), 0);
			break;
		}		
	}
	

	//6 close socket
	closesocket(sock);
	WSACleanup();
}