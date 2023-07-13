#pragma once
#include "Netcode.h"
#include "BasicTypedefs.h"
#include "TSQueue.h"
//#ifdef _WIN32
//#define __WIN32_WINNT 0x0a00
//#endif
//#define ASIO_STANDALONE
//#include <asio.hpp>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>

#include <iostream>
#include <atomic>
#include <thread>
#include <memory>
#include <WS2tcpip.h>
#include <map>
#include <chrono>
#include <windows.h>
#pragma comment (lib, "ws2_32.lib")

enum class ConnectionState : u8 {
	None,
	UnValidated,
	Validated,
	Playing,
	Disconnected,

	NumConnectionStates
};

#define MAX_CLIENT_CONNECTIONS 100

struct ClientConnection {
	ConnectionState state = ConnectionState::None;
	std::chrono::time_point<std::chrono::system_clock> timestampRecievedLastPacket;
};

class NetcodeServer
{
public:
	static void Init();
	static void StartServer();
	static void StopServer();

private:
	static void ServerLoop();
	static bool CompareSockAddr(sockaddr_in& hint1, sockaddr_in& hint2);
	static ClientConnection* FindClientConnection(sockaddr_in& hint);
	static void DoClientConnectionStateMachine(ClientConnection* connection, const char* rbuf);
	static ClientConnection* NewClientPacketRecieved(sockaddr_in& client, const char* rbuf);
	bool SetTimeoutAPC(HANDLE& apcHandleOut);

private:
	static std::atomic<bool> s_shouldServerThreadContinue;
	static std::unique_ptr<std::thread> s_serverThread;
	static std::map<sockaddr_in,ClientConnection> s_clientConnections;
	static u32 s_numClientActiveConnections;
};


class NetcodeClient
{
public:
	void Init();
	void StartClient();
	void StopClient();

private:
	void ClientLoop();

private:
	char m_rbuf[1024];
	sockaddr_in m_client;
	i32 m_clientSize;
	SOCKET m_in;
	std::atomic<bool> m_shouldClientThreadContinue = false;
	std::unique_ptr<std::thread> m_serverThread;
};