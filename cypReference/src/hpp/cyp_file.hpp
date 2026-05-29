#pragma once
#include <filesystem>
#include <ostream>
#include <fstream>
#include "cyp_network.hpp"
#include "cyp_hash.hpp"
#include "cyp_convert.hpp"

namespace cyp
{
	namespace file
	{
		bool ExistFile(const std::string& filePath);
		void CreateFile(const std::string& filePath, const std::string& fileContent = "");
		bool DeleteFile(const std::string& filePath);
		bool CreateDirectory(const std::string& directoryLoc);
		std::string ReadAllFile(const std::string& fileLoc);
		std::string GetFileName(std::string filePath);

		/*
		* Send and receive files.
		* Unencrypted packet is transmitted through TCP.
		* Hackers can easily obtain the file through packet sniffing, so it is recommended to use it only in a controlled environment.
		* If a file is transmitted through the corresponding class on a public network, 
		* it is possible to hide at least the internal format of the file if it is encryptedand transmitted in the form of 'compression (.zip)'.
		*/
		class FileCommunication : cyp::network::protocol::Tcp
		{
		private:
			static const int MB2 = 2000000;

			class Packet_FileHeader
			{
			public:
				char identifier[4] = { 0x00, 0x47, 0x02, 0x4F };
				char fileName[256] = { '\0', };
				unsigned long long fileSize = 0;	// unit, byte
			};

			void SendHeader(SOCKET& sendSocket, Packet_FileHeader& packet_fileHeader);
			void SendBody(SOCKET& sendSocket, const char* buffer, unsigned int packetSize);

		public:
			void SendFile(const u_short port, const std::string sendFilePath);
			void ReceiveFile(const std::string ip, const u_short port, const std::string saveFilePath, double& progress);

		};
	}
}
