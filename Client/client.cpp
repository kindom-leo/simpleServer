#define WIN32_LEAN_AND_MEAN   //防止windows 中winsock1 与WinSock2.h 中宏定义冲突
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

struct DataPackage {
	int		m_age;
	char	m_chArrName[32];
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
		cout << "pleas input:" << endl;
		std::getline(cin, s);
		if (s == "exit") {
			break;
		}else{
			send(_sock, s.c_str(), s.size(), 0);
		}
		char szBuf[256] = { 0 };
		int nLen = recv(_sock, szBuf, 256, 0);
		DataPackage* dp = reinterpret_cast<DataPackage*>(szBuf);
		if (nLen > 0) {
			std::for_each(begin(s), end(s), [](auto& c) {return c = std::tolower(c); });
			if (s == "getinfo")
				cout << "recv msg from server: name = " << dp->m_chArrName << ",age = " << dp->m_age << endl;
			else
				cout << "recv msg from server :" << szBuf << endl;
		}
	}

	//3 recv info from server 
	
	//4 close socket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}