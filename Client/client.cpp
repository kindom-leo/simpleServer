#define WIN32_LEAN_AND_MEAN   //��ֹwindows ��winsock1 ��WinSock2.h �к궨���ͻ
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
	CMD_LOGOUT,
	CMD_ERROR
};

struct DataHeader {
	unsigned short dataLen;
	unsigned short cmd;
};

struct Login {
	char 	m_username[32];
	char	m_password[32];
};

struct LoginResult {
	int result;
};

struct Logout {
	char username[32];
};

struct LogoutResult {
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
		//������������
		cout << "pleas input:" << endl;
		std::getline(cin, s);
		std::for_each(begin(s), end(s), [](auto& c) {c = std::tolower(c); });
		//������������
		if (s == "exit") {
			printf("receive command EXIT! mission complete!\n");
			break;
		} else if (s == "login") {
			//���������������
			Login login{ "leopardln","ljt111104" };
			DataHeader dh{ sizeof(login),CMD_LOGIN };
			send(_sock, (const char*)&dh, sizeof(DataHeader), 0);
			send(_sock, (const char*)&login, sizeof(Login), 0);
			//���շ��������ص�����
			DataHeader dhres{};
			LoginResult loginRes{};
			recv(_sock, (char*)&dhres, sizeof(DataHeader), 0);
			recv(_sock, (char*)&loginRes, sizeof(loginRes), 0);
			printf("login result: %d\n", loginRes.result);
		} else if (s == "logout") {
			//������������˳�����
			Logout logout{"leopardln"};
			DataHeader dh{sizeof(Logout),CMD_LOGOUT};
			send(_sock, (const char*)&dh, sizeof(DataHeader), 0);
			send(_sock, (const char*)&logout, sizeof(Login), 0);
			//���շ��������ص�����
			DataHeader dhres{};
			LogoutResult logoutRes{};
			recv(_sock, (char*)&dhres, sizeof(DataHeader), 0);
			recv(_sock, (char*)&logoutRes, sizeof(LogoutResult), 0);
			printf("login result: %d\n", logoutRes.result);
		}
		else{
			printf("receive invalid command ! mission complete!\n");
			break;
		}
	}

	//3 recv info from server 
	
	//4 close socket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}