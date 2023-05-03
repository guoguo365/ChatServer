#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define NUM 1024

// 可连接的客户端总数
SOCKET clientSocket[NUM];

// 已连接客户端数量
int connectedClientNums = { 0 };



// 通信
void communication(int idx) {
	char buff[256];
	int r;
	char temp[255];
	while (1) {
		r = recv(clientSocket[idx], buff, 255, NULL);
		if (r > 0) {
			buff[r] = 0; // 添加结束符 ’'\0
			printf(">>:%s\n", buff);
			memset(temp, 0, 255); // 清空数组
			sprintf(temp, "%d:%s", idx, buff);

			// 广播消息
			for (int i = 0; i < connectedClientNums; i++) {
				send(clientSocket[i], temp, strlen(temp), NULL);
				printf("服务器转发消息到-%d: %s\n", i, temp);
			}
		}
	}
}

int main() {

	// 1. 确定协议版本信息
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("确定协议版本信息错误：%d\n", GetLastError());
		return -1;
	}
	printf("确定协议版本信息成功！\n");

	// 2. 创建socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {
		printf("创建socket失败：%d\n", GetLastError());
		return -1;
	}
	printf("创建socket成功！\n");

	// 3. 确定服务器协议地址蹙
	SOCKADDR_IN sAddr = { 0 };
	sAddr.sin_family = AF_INET; // socket函数第一个参数
	sAddr.sin_addr.S_un.S_addr = inet_addr("10.4.30.167");
	sAddr.sin_port = htons(9527); 

	// 4. 绑定
	int r = bind(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
	if (-1 == r) {

		// 8. 关闭socket
		closesocket(serverSocket);

		// 9. 清理协议版本信息
		WSACleanup();
		printf("绑定失败: %d\n", GetLastError());
		return -1;
	}
	printf("绑定成功！\n");

	// 5. 监听
	r = listen(serverSocket, 10);
	if (-1 == r) {

		// 8. 关闭socket
		closesocket(serverSocket);

		// 9. 清理协议版本信息
		WSACleanup();
		printf("监听失败: %d\n", GetLastError());
		return -1;
	}
	printf("监听成功！\n");

	// 6. 接受连接 
	for (int i = 0; i < NUM; i++) {
		clientSocket[i] = accept(serverSocket, NULL, NULL);
		
		if (-1 == clientSocket[i]) {

			// 8. 关闭socket
			closesocket(serverSocket);

			// 9. 清理协议版本信息
			WSACleanup();
			printf("连接失败: %d\n", GetLastError());
			return -1;
		}
		printf("客户端连接成功！\n");

		// 并发通信
		connectedClientNums++;
		printf("----在线客户端数量：%d", connectedClientNums);
		CreateThread(NULL, NULL,
			(LPTHREAD_START_ROUTINE) communication, (LPVOID) i, 
			NULL, NULL);

	}

	// 8. 关闭socket
	closesocket(serverSocket);

	// 9. 清理协议版本信息
	WSACleanup();

	return 0;
}