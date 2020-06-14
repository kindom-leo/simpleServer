#define WIN32_LEAN_AND_MEAN   //防止windows 中winsock1 与WinSock2.h 中宏定义冲突
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>
using std::cout;
using std::endl;
using std::cin;

#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 60000

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

struct LoginResult :DataHeader {
	LoginResult() {
		dataLen = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout :DataHeader {
	Logout() {
		dataLen = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char username[32] = {};
};

struct LogoutResult :DataHeader {
	LogoutResult() {
		dataLen = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//1 create socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_sock == INVALID_SOCKET) {
		cout << "create socket err:" << endl;
		return -1;
	}
	//2 connet server
	sockaddr_in _sin = {AF_INET,htons(SERVER_PORT)};
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.60.1");
	int ret = connect(_sock, reinterpret_cast<sockaddr*>(&_sin), sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		cout << "err: create socket failed!" << endl;
		return -1;
	}

	std::string s;
	int len = sizeof(sockaddr_in);
	while (true) {
		s.clear();
		//输入请求命令
		cout << "pleas input:" << endl;
		std::getline(cin, s);
		std::for_each(begin(s), end(s), [](auto& c) {c = std::tolower(c); });
		//处理请求命令
		if (s == "exit") {
			printf("receive command EXIT! mission complete!\n");
			break;
		} else if (s == "login") {
			//向服务器发送请求
			Login login;
			strcpy(login.m_username, "leopardln");
			strcpy(login.m_password, "ljt111104");
			send(_sock, (const char*)&login, sizeof(Login), 0);
			//接收服务器返回的数据
			LoginResult loginRes{};
			recv(_sock, (char*)&loginRes, sizeof(loginRes), 0);
			printf("login result: %d\n", loginRes.result);
		} else if (s == "logout") {
			//向服务器发送退出请求
			Logout logout;
			strcpy(logout.username, "leopardln");
			send(_sock, (const char*)&logout, sizeof(Logout), 0);
			//接收服务器返回的数据
			LogoutResult logoutRes{};
			recv(_sock, (char*)&logoutRes, sizeof(LogoutResult), 0);
			printf("login result: %d\n", logoutRes.result);
		}
		else{
			printf("input an invalid command !\n");
		}
	}

	//3 recv info from server 
	
	//4 close socket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}