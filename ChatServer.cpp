#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define NUM 1024

// �����ӵĿͻ�������
SOCKET clientSocket[NUM];

// �����ӿͻ�������
int connectedClientNums = { 0 };



// ͨ��
void communication(int idx) {
	char buff[256];
	int r;
	char temp[255];
	while (1) {
		r = recv(clientSocket[idx], buff, 255, NULL);
		if (r > 0) {
			buff[r] = 0; // ��ӽ����� ��'\0
			printf(">>:%s\n", buff);
			memset(temp, 0, 255); // �������
			sprintf(temp, "%d:%s", idx, buff);

			// �㲥��Ϣ
			for (int i = 0; i < connectedClientNums; i++) {
				send(clientSocket[i], temp, strlen(temp), NULL);
				printf("������ת����Ϣ��-%d: %s\n", i, temp);
			}
		}
	}
}

int main() {

	// 1. ȷ��Э��汾��Ϣ
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("ȷ��Э��汾��Ϣ����%d\n", GetLastError());
		return -1;
	}
	printf("ȷ��Э��汾��Ϣ�ɹ���\n");

	// 2. ����socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {
		printf("����socketʧ�ܣ�%d\n", GetLastError());
		return -1;
	}
	printf("����socket�ɹ���\n");

	// 3. ȷ��������Э���ַ��
	SOCKADDR_IN sAddr = { 0 };
	sAddr.sin_family = AF_INET; // socket������һ������
	sAddr.sin_addr.S_un.S_addr = inet_addr("10.4.30.167");
	sAddr.sin_port = htons(9527); 

	// 4. ��
	int r = bind(serverSocket, (sockaddr*)&sAddr, sizeof(sAddr));
	if (-1 == r) {

		// 8. �ر�socket
		closesocket(serverSocket);

		// 9. ����Э��汾��Ϣ
		WSACleanup();
		printf("��ʧ��: %d\n", GetLastError());
		return -1;
	}
	printf("�󶨳ɹ���\n");

	// 5. ����
	r = listen(serverSocket, 10);
	if (-1 == r) {

		// 8. �ر�socket
		closesocket(serverSocket);

		// 9. ����Э��汾��Ϣ
		WSACleanup();
		printf("����ʧ��: %d\n", GetLastError());
		return -1;
	}
	printf("�����ɹ���\n");

	// 6. �������� 
	for (int i = 0; i < NUM; i++) {
		clientSocket[i] = accept(serverSocket, NULL, NULL);
		
		if (-1 == clientSocket[i]) {

			// 8. �ر�socket
			closesocket(serverSocket);

			// 9. ����Э��汾��Ϣ
			WSACleanup();
			printf("����ʧ��: %d\n", GetLastError());
			return -1;
		}
		printf("�ͻ������ӳɹ���\n");

		// ����ͨ��
		connectedClientNums++;
		printf("----���߿ͻ���������%d", connectedClientNums);
		CreateThread(NULL, NULL,
			(LPTHREAD_START_ROUTINE) communication, (LPVOID) i, 
			NULL, NULL);

	}

	// 8. �ر�socket
	closesocket(serverSocket);

	// 9. ����Э��汾��Ϣ
	WSACleanup();

	return 0;
}