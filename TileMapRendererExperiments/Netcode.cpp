#include "Netcode.h"
#define _SECOND 10000000

std::atomic<bool> NetcodeServer::s_shouldServerThreadContinue = false;
std::unique_ptr<std::thread> NetcodeServer::s_serverThread;
std::map<sockaddr_in, ClientConnection> NetcodeServer::s_clientConnections;
u32 NetcodeServer::s_numClientActiveConnections = 0;

void NetcodeServer::Init()
{
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0) {
		std::cout << "can't start winsock\n";
		return;
	}

	
}


void NetcodeServer::StartServer()
{
	/*if (s_shouldServerThreadContinue) {
		return;
	}
	s_shouldServerThreadContinue = true;
	s_serverThread = std::make_unique<std::thread>(&ServerLoop);*/
}

void NetcodeServer::StopServer()
{
	/*if (!s_shouldServerThreadContinue)
	{
		std::cout << "already stopped\n";
		return;
	}
	s_shouldServerThreadContinue = false;
	s_serverThread->join();*/
}

void NetcodeServer::ServerLoop()
{
	//SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	//sockaddr_in serverHint;
	//serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	//serverHint.sin_family = AF_INET;
	//serverHint.sin_port = htons(54000); // little to big endian

	//if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {
	//	std::cout << "can't bind socket " << WSAGetLastError() << std::endl;
	//	return;
	//}

	//sockaddr_in client;
	//i32 clientSize = sizeof(client);
	//ZeroMemory(&client, sizeof(client));



	//char rbuf[1024];
	//while (s_shouldServerThreadContinue)
	//{
	//	ZeroMemory(rbuf, 1024);
	//	
	//	int bytesIn = recvfrom(in, rbuf, 1024, 0, (sockaddr*)&client, &clientSize);

	//	ClientConnection* foundClient = FindClientConnection(client);
	//	if (!foundClient) {
	//		foundClient = NewClientPacketRecieved(client, rbuf);
	//	}
	//	foundClient->timestampRecievedLastPacket = std::chrono::system_clock::now();
	//	DoClientConnectionStateMachine(foundClient, rbuf);

	//	/*if (bytesIn == SOCKET_ERROR){
	//		cout << "Error recieving from client" << WSAGetLastError() << endl;
	//		continue;
	//	}

	//	char clientIp[256];
	//	ZeroMemory(clientIp, 256);

	//	inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
	//	cout << "message recieved from " << clientIp << " : " << rbuf << "\n";*/
	//}
	//closesocket(in);
	//WSACleanup();
}

bool NetcodeServer::CompareSockAddr(sockaddr_in& hint1, sockaddr_in& hint2)
{
	return hint1.sin_addr.S_un.S_addr == hint2.sin_addr.S_un.S_addr;
}

ClientConnection* NetcodeServer::FindClientConnection(sockaddr_in& hint)
{
	//if (s_clientConnections.find(hint) == s_clientConnections.end()) {
	//	// not found
	//	return nullptr;
	//}
	//else {
	//	// found
	//	return &s_clientConnections[hint];
	//}
	return nullptr;
}

void NetcodeServer::DoClientConnectionStateMachine(ClientConnection* connection, const char* rbuf)
{
}

ClientConnection* NetcodeServer::NewClientPacketRecieved(sockaddr_in& client, const char* rbuf)
{
	/*ClientConnection connection;
	connection.state = ConnectionState::UnValidated;
	s_clientConnections[client] = connection;*/
	return nullptr;

	//return &s_clientConnections[client];
}

bool NetcodeServer::SetTimeoutAPC(HANDLE& handleOut)
{
	//handleOut = CreateWaitableTimer(
	//	NULL,                   // Default security attributes
	//	TRUE,                  // Create auto-reset timer
	//	TEXT("Game server timeout check"));       // Name of waitable timer
	//__int64 qwDueTime = -5 * _SECOND;
	//LARGE_INTEGER   liDueTime;

	//// Copy the relative time into a LARGE_INTEGER.
	//liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
	//liDueTime.HighPart = (LONG)(qwDueTime >> 32);
	return true;
}












void NetcodeClient::Init()
{
}

void NetcodeClient::StartClient()
{
}

void NetcodeClient::StopClient()
{
}

void NetcodeClient::ClientLoop()
{
}
