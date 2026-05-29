#include "cyp_network.hpp"

namespace cyp
{
	namespace network
	{
		namespace protocol
		{
			Tcp::Tcp()
			{
				if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != NO_ERROR)
				{
					throw "error : WSAStartup failed";
				}
			}

			Tcp::~Tcp()
			{
				if (_listenSocket != INVALID_SOCKET) closesocket(_listenSocket);
				if (_serverSocket != INVALID_SOCKET) closesocket(_serverSocket);
				if (_clientSocket != INVALID_SOCKET) closesocket(_clientSocket);

				WSACleanup();
			}

			SOCKET& Tcp::OpenServer(const u_short serverPort)
			{
				_listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

				if (_listenSocket == INVALID_SOCKET)
				{
					throw "error : _listenSocket stop";
				}

				SOCKADDR_IN addrServer, addrClient;
				ZeroMemory(&addrServer, sizeof(addrServer));
				ZeroMemory(&addrClient, sizeof(addrClient));
				int addrSize = sizeof(addrClient);

				addrServer.sin_family = AF_INET;
				addrServer.sin_port = htons(serverPort);
				addrServer.sin_addr.s_addr = htonl(INADDR_ANY);

				if (bind(_listenSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == BIND_ERROR)
				{
					throw "error bind failed";
				}

				if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
				{
					throw "error : listen failed";
				}

				_serverSocket = accept(_listenSocket, (SOCKADDR*)&addrClient, &addrSize);

				if (_serverSocket == INVALID_SOCKET)
				{
					throw "error : accept failed";
				}

				return _serverSocket;
			}

			SOCKET& Tcp::OpenClient(const std::string& serverIp, const u_short serverPort)
			{
				_clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

				if (_clientSocket == INVALID_SOCKET)
				{
					throw "error : _clientSocket stop";
				}

				SOCKADDR_IN addrServer;
				ZeroMemory(&addrServer, sizeof(addrServer));

				addrServer.sin_family = PF_INET;
				addrServer.sin_port = htons(serverPort);
				if (inet_pton(PF_INET, serverIp.data(), &(addrServer.sin_addr.s_addr)) != INET_PTON_SUCCESS)
				{
					throw "error : can't inet_pton init";
				}

				if (connect(_clientSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) != 0)
				{
					throw "error : can't connection";
				}

				return _clientSocket;
			}

			int Tcp::ReceiveServer(char* msgBuf, int readLen)
			{
				if (msgBuf == nullptr || readLen <= 0)
				{
					throw "error : invalid receive buffer";
				}

				return recv(_serverSocket, msgBuf, readLen, 0);
			}

			int Tcp::ReceiveClient(char* msgBuf, int readLen)
			{
				if (msgBuf == nullptr || readLen <= 0)
				{
					throw "error : invalid receive buffer";
				}

				return recv(_clientSocket, msgBuf, readLen, 0);
			}

			void Tcp::SendServerToClient(const char* msgBuf, int msgLen)
			{
				if (msgBuf == nullptr || msgLen <= 0)
				{
					throw "error : invalid send buffer";
				}

				int totalSendSize = 0;

				while (totalSendSize < msgLen)
				{
					int sendSize = send(_serverSocket, msgBuf + totalSendSize, msgLen - totalSendSize, 0);

					if (sendSize <= 0)
					{
						throw "error : server error send";
					}

					totalSendSize += sendSize;
				}
			}

			void Tcp::SendClientToServer(const char* msgBuf, int msgLen)
			{
				if (msgBuf == nullptr || msgLen <= 0)
				{
					throw "error : invalid send buffer";
				}

				int totalSendSize = 0;

				while (totalSendSize < msgLen)
				{
					int sendSize = send(_clientSocket, msgBuf + totalSendSize, msgLen - totalSendSize, 0);

					if (sendSize <= 0)
					{
						throw "error : client error send";
					}

					totalSendSize += sendSize;
				}
			}


			Udp::Udp()
			{
				if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != NO_ERROR)
				{
					throw "error : WSAStartup failed";
				}
			}

			Udp::~Udp()
			{
				if (_sendSocket != INVALID_SOCKET) closesocket(_sendSocket);
				if (_recvSocket != INVALID_SOCKET) closesocket(_recvSocket);

				WSACleanup();
			}

			void Udp::Open_send(const std::string& ip, const u_short port)
			{
				_sendSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (_sendSocket == INVALID_SOCKET)
				{
					throw "error : _sendSocket stop";
				}

				ZeroMemory(&_recvAddr, sizeof(_recvAddr));

				// send
				_recvAddr.sin_family = PF_INET;
				_recvAddr.sin_port = htons(port);
				if (inet_pton(PF_INET, ip.data(), &_recvAddr.sin_addr.s_addr) != INET_PTON_SUCCESS)
				{
					throw "error : can't inet_pton init";
				}
			}

			void Udp::Open_receive(const u_short port)
			{
				_recvSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (_recvSocket == INVALID_SOCKET)
				{
					throw "error : _recvSocket stop";
				}

				ZeroMemory(&_sendAddr, sizeof(_sendAddr));

				// recv
				_sendAddr.sin_family = PF_INET;
				_sendAddr.sin_port = htons(port);
				_sendAddr.sin_addr.s_addr = htonl(INADDR_ANY);

				if (bind(_recvSocket, (SOCKADDR*)&_sendAddr, sizeof(_sendAddr)) == BIND_ERROR)
				{
					throw "error : bind failed";
				}
			}

			void Udp::Send(const char* msgBuf, int msgLen)
			{
				if (msgBuf == nullptr || msgLen <= 0)
				{
					throw "error : invalid send buffer";
				}

				if (sendto(_sendSocket, msgBuf, msgLen, 0,
					(struct sockaddr*)&_recvAddr, sizeof(_recvAddr)) == SOCKET_ERROR)
				{
					throw "error : Send Fail";
				}
			}

			void Udp::Receive(char* msgBuf, int msgLen)
			{
				if (msgBuf == nullptr || msgLen <= 0)
				{
					throw "error : invalid receive buffer";
				}

				int addrlen = sizeof(_recvAddr);
				int size = recvfrom(_recvSocket, msgBuf, msgLen, 0, (struct sockaddr*)&_recvAddr, &addrlen);
				if (size < 0)
				{
					throw "recvfrom";
				}
			}

			Udp_multicast::Udp_multicast()
			{
				if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != NO_ERROR)
				{
					throw "error : WSAStartup failed";
				}
			}

			Udp_multicast::~Udp_multicast()
			{
				if (_sendSocket != INVALID_SOCKET) closesocket(_sendSocket);
				if (_recvSocket != INVALID_SOCKET) closesocket(_recvSocket);

				WSACleanup();
			}

			void Udp_multicast::Open_send(const std::string& ip_multicast, const u_short port_multicast)
			{
				_sendSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (_sendSocket == INVALID_SOCKET)
				{
					throw "error : _sendSocket stop";
				}

				ZeroMemory(&_sendAddr, sizeof(_sendAddr));

				// Receive
				_recvAddr.sin_family = PF_INET;
				_recvAddr.sin_port = htons(port_multicast);
				if (inet_pton(PF_INET, ip_multicast.data(), &_recvAddr.sin_addr.s_addr) != INET_PTON_SUCCESS)
				{
					throw "error : can't inet_pton init";
				}

				if (setsockopt(_sendSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&_setsockoptBuf, sizeof(_setsockoptBuf)) < 0)
				{
					throw "error : setsockopt";
				}
			}

			void Udp_multicast::Open_receive(const std::string& ip_multicast, const u_short port_multicast)
			{
				_recvSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (_recvSocket == INVALID_SOCKET)
				{
					throw "error : _recvSocket stop";
				}

				if (setsockopt(_recvSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&_setsockoptBuf, sizeof(_setsockoptBuf)) < 0)
				{
					throw "error : Reusing ADDR failed";
				}

				ZeroMemory(&_sendAddr, sizeof(_sendAddr));
				_sendAddr.sin_family = PF_INET;
				_sendAddr.sin_addr.s_addr = htonl(INADDR_ANY);
				_sendAddr.sin_port = htons(port_multicast);

				// bind to send address
				if (bind(_recvSocket, (struct sockaddr*)&_sendAddr, sizeof(_sendAddr)) < 0)
				{
					throw "bind";
					return;
				}

				// use setsockopt() to request that the kernel join a multicast group
				if (inet_pton(PF_INET, ip_multicast.data(), &_imr_receive.imr_multiaddr.s_addr) != INET_PTON_SUCCESS)
				{
					throw "error : can't inet_pton init";
				}
				_imr_receive.imr_interface.s_addr = htonl(INADDR_ANY);
				if (setsockopt(_recvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&_imr_receive, sizeof(_imr_receive)) < 0)
				{
					throw "error : setsockopt";
				}
			}

			void Udp_multicast::Send(char* msgBuf, int msgLen)
			{
				if (msgBuf == nullptr || msgLen <= 0)
				{
					throw "error : invalid send buffer";
				}

				if (sendto(_sendSocket, msgBuf, msgLen, 0,
					(struct sockaddr*)&_recvAddr, sizeof(_recvAddr)) == SOCKET_ERROR)
				{
					throw "error : send Fail";
				}
			}

			void Udp_multicast::Receive(char* msgbuf, int msgLen)
			{
				if (msgbuf == nullptr || msgLen <= 0)
				{
					throw "error : invalid receive buffer";
				}

				int addrlen = sizeof(_recvAddr);
				if (recvfrom(_recvSocket, msgbuf, msgLen, 0, (struct sockaddr*)&_recvAddr, &addrlen) < 0)
				{
					throw "recvfrom";
				}
			}
		}

		namespace supporter
		{
			void Reverse(char* start, const int size)
			{
				if (start == nullptr || size <= 0)
				{
					return;
				}

				char* startPoint = start;
				char* endPoint = startPoint + size;
				std::reverse(startPoint, endPoint);
			}
		}
	}

}
