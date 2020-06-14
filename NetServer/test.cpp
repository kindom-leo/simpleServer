#define WIN32_LEAN_AND_MEAN   //防止windows 中winsock1 与WinSock2.h 中宏定义冲突
#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include<functional>
using namespace std;
#pragma comment(lib,"ws2_32.lib")


template <typename InputIterator,typename T>
void Find(InputIterator first, InputIterator last, const T& t) {
	if constexpr (is_same_v<typename iterator_traits<InputIterator>::value_type, int>) {
		cout << "int" << endl;
	} else if constexpr(is_same_v<typename iterator_traits<InputIterator>::value_type, double>) {
		cout << "double" << endl;
	}
}
int main() {
	char buf[] = { "HELLO,WORLD" };
	transform(begin(buf), end(buf),begin(buf), [](auto c) {return tolower(c); });
	cout << buf << endl;
	char c = 'A';
	cout << tolower(c) << endl;
	cout << c << endl;
	return 0;
}