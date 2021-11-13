#include "cyp_communication.hpp"

namespace cyp
{
	namespace communication
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
			closesocket(_listenSocket);
			closesocket(_serverSocket);
			closesocket(_clientSocket);

			WSACleanup();
		}

		void Tcp::openServer(const u_short port)
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
			addrServer.sin_port = htons(port);
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

			closesocket(_listenSocket);
		}

		void Tcp::openClient(const std::string& serverIp, const u_short port)
		{
			_clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET)
			{
				throw "error : _clientSocket stop";
			}

			SOCKADDR_IN addrServer;
			ZeroMemory(&addrServer, sizeof(addrServer));

			addrServer.sin_family = PF_INET;
			addrServer.sin_port = htons(port);
			if (inet_pton(PF_INET, serverIp.data(), &(addrServer.sin_addr.s_addr)) != INET_PTON_SUCCESS)
			{
				throw "error : can't inet_pton init";
			}

			if (connect(_clientSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) != 0)
			{
				throw "error : can't connection";
			}
		}

		std::string Tcp::receiveServer()
		{
			char msgbuf[512];
			std::string output;

			int size = recv(_serverSocket, msgbuf, 512, 0);
			if (size < 0)
			{
				throw "recvfrom";
			}
			msgbuf[size] = '\0';
			output = msgbuf;
			return output;
		}

		std::string Tcp::receiveClient()
		{
			char msgbuf[512];
			std::string output;

			int size = recv(_clientSocket, msgbuf, 512, 0);
			if (size < 0)
			{
				throw "recvfrom";
			}
			msgbuf[size] = '\0';
			output = msgbuf;
			return output;
		}

		void Tcp::sendServerToClient(const std::string& message)
		{
			if (send(_serverSocket, message.data(), static_cast<int>(message.length()), 0) == SOCKET_ERROR)
			{
				throw "error : server error send";
			}
		}

		void Tcp::sendClientToServer(const std::string& message)
		{
			if (send(_clientSocket, message.data(), static_cast<int>(message.length()), 0) == SOCKET_ERROR)
			{
				throw "error : client error send";
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
			closesocket(_sendSocket);
			closesocket(_recvSocket);

			WSACleanup();
		}

		void Udp::open_send(const std::string& ip, const u_short port)
		{
			_sendSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

			ZeroMemory(&_recvAddr, sizeof(_recvAddr));
			_dummyAddrSize = sizeof(_dummyAddr);

			// send
			_recvAddr.sin_family = PF_INET;
			_recvAddr.sin_port = htons(port);
			if (inet_pton(PF_INET, ip.data(), &_recvAddr.sin_addr.s_addr) != INET_PTON_SUCCESS)
			{
				throw "error : can't inet_pton init";
			}
		}

		void Udp::open_receive(const u_short port)
		{
			_recvSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

			ZeroMemory(&_dummyAddr, sizeof(_dummyAddr));
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

		bool Udp::send(const std::string& message)
		{
			if (sendto(_sendSocket, message.data(), static_cast<int>(message.length()), 0,
				(struct sockaddr*)&_recvAddr, sizeof(_recvAddr)) == SOCKET_ERROR)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		std::string Udp::receive()
		{
			std::string output;
			char msgbuf[512];
			int addrlen = sizeof(_recvAddr);
			int size = recvfrom(_recvSocket, msgbuf, 512, 0, (struct sockaddr*)&_recvAddr, &addrlen);
			if (size < 0)
			{
				throw "recvfrom";
			}
			msgbuf[size] = '\0';
			output = msgbuf;
			return output;
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
			closesocket(_sendSocket);
			closesocket(_recvSocket);

			WSACleanup();
		}

		void Udp_multicast::open_send(const std::string& ip_multicast, const u_short port_multicast)
		{
			_sendSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

			ZeroMemory(&_sendAddr, sizeof(_sendAddr));

			// receive
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

		void Udp_multicast::open_receive(const std::string& ip_multicast, const u_short port_multicast)
		{
			_recvSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

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

		bool Udp_multicast::send(const std::string& message)
		{
			if (sendto(_sendSocket, message.data(), static_cast<int>(message.length()), 0,
				(struct sockaddr*)&_recvAddr, sizeof(_recvAddr)) == SOCKET_ERROR)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		std::string Udp_multicast::receive()
		{
			std::string output;
			char msgbuf[512];
			int addrlen = sizeof(_recvAddr);
			int size = recvfrom(_recvSocket, msgbuf, 512, 0, (struct sockaddr*)&_recvAddr, &addrlen);
			if (size < 0) 
			{
				throw "recvfrom";
			}
			msgbuf[size] = '\0';
			output = msgbuf;
			return output;
		}
	}

}