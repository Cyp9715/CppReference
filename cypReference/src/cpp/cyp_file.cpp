#include "cyp_file.hpp"

#include <cmath>
#include <vector>

namespace cyp
{
	namespace file
	{
		bool ExistFile(const std::string& filePath)
		{
			std::filesystem::path p(filePath);
			return std::filesystem::exists(p) ? true : false;
		}

		bool DeleteFile(const std::string& filePath)
		{
			return std::filesystem::remove(filePath) ? true : false;
		}

		bool CreateDirectory(const std::string& directoryLoc)
		{
			return std::filesystem::create_directory(directoryLoc) ? true : false;
		}

		void CreateFile(const std::string& filePath, const std::string& fileContent)
		{
			std::ofstream file(filePath);
			file << fileContent;
			file.close();
		}

		std::string ReadAllFile(const std::string& fileLoc)
		{
			std::ifstream file(fileLoc, std::ios::binary);

			if (!file.is_open())
			{
				throw "error : cypReference::file::readAllFile";
			}

			file.seekg(0, std::ios::end);
			std::streampos endPos = file.tellg();
			std::streamoff fileSize = static_cast<std::streamoff>(endPos);
			if (fileSize < 0)
			{
				throw "error : cypReference::file::readAllFile";
			}

			std::string buffer(static_cast<size_t>(fileSize), '\0');
			file.seekg(0, std::ios::beg);

			if (!buffer.empty())
			{
				file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
				if (file.gcount() != static_cast<std::streamsize>(buffer.size()))
				{
					throw "error : cypReference::file::readAllFile";
				}
			}

			return buffer;
		}

		std::string GetFileName(std::string filePath)
		{
			size_t pos = filePath.find_last_of("/\\");
			return pos == std::string::npos ? filePath : filePath.substr(pos + 1);
		}

		void FileCommunication::SendHeader(SOCKET& sendSocket, Packet_FileHeader& packet_fileHeader)
		{
			const char* buffer = reinterpret_cast<const char*>(&packet_fileHeader);
			unsigned int totalSendSize = 0;

			while (totalSendSize < sizeof(Packet_FileHeader))
			{
				int sendSize = send(sendSocket, buffer + totalSendSize, static_cast<int>(sizeof(Packet_FileHeader) - totalSendSize), 0);

				if (sendSize <= 0)
				{
					throw "error : client error Send";
				}

				totalSendSize += static_cast<unsigned int>(sendSize);
			}
		}

		void FileCommunication::SendBody(SOCKET& sendSocket, const char* buffer, unsigned int packetSize)
		{
			unsigned int totalSendSize = 0;

			while (totalSendSize < packetSize)
			{
				int sendSize = send(sendSocket, buffer + totalSendSize, static_cast<int>(packetSize - totalSendSize), 0);

				if (sendSize <= 0)
				{
					throw "error : client error Send";
				}

				totalSendSize += static_cast<unsigned int>(sendSize);
			}
		}

		void FileCommunication::SendFile(const u_short port, const std::string sendFilePath)
		{
			// Implement Multi accept via Thread to send multiple files at the same time.
			SOCKET serverSocket = OpenServer(port);

			std::ifstream file(sendFilePath, std::ios::binary);
			Packet_FileHeader fileHeader;

			if (file.is_open())
			{
				file.seekg(0, std::ios::end);
				std::streampos endPos = file.tellg();
				std::streamoff fileSize = static_cast<std::streamoff>(endPos);
				if (fileSize < 0)
				{
					throw "error : Unable to read file size.";
				}

				fileHeader.fileSize = static_cast<unsigned long long>(fileSize);
				file.seekg(0, std::ios::beg);
				
				std::string fileName = GetFileName(sendFilePath);
				size_t fileNameSize = std::min(fileName.length(), sizeof(fileHeader.fileName) - 1);
				memcpy(fileHeader.fileName, fileName.c_str(), fileNameSize);
				fileHeader.fileName[fileNameSize] = '\0';

				unsigned long long loopCount_full = fileHeader.fileSize / MB2;
				unsigned long long loopCount_present = 0;
				unsigned int remainSize = static_cast<unsigned int>(fileHeader.fileSize - (loopCount_full * MB2));

				SendHeader(serverSocket, fileHeader);

				std::vector<char> buffer(MB2);

				if (fileHeader.fileSize >= MB2)
				{
					// For memory limit, large files are sent in 2MB units 
					while (loopCount_present < loopCount_full)
					{
						file.read(buffer.data(), MB2);
						if (file.gcount() != MB2)
						{
							throw "error : Unable to read file.";
						}

						SendBody(serverSocket, buffer.data(), MB2);
						++loopCount_present;
					}
				}

				if (remainSize > 0)
				{
					file.read(buffer.data(), remainSize);
					if (file.gcount() != static_cast<std::streamsize>(remainSize))
					{
						throw "error : Unable to read file.";
					}

					SendBody(serverSocket, buffer.data(), remainSize);
				}
			}
			else
			{
				throw "error : Unable to open file.";
			}
		}

		void FileCommunication::ReceiveFile(const std::string ip, const u_short port, const std::string saveFilePath, double& progress)
		{
			if (ExistFile(saveFilePath) == true)
			{
				throw "error : file already exist.";
			}

			SOCKET clientSocket = OpenClient(ip, port);

			Packet_FileHeader fileHeader;
			
			char recvIdentifier[4];
			memcpy(recvIdentifier, fileHeader.identifier, 4);
			memset(fileHeader.identifier, '\0', 4);

			char* headerBuffer = reinterpret_cast<char*>(&fileHeader);
			unsigned int headerRecvSize = 0;
			while (headerRecvSize < sizeof(Packet_FileHeader))
			{
				int receiveSize = recv(clientSocket, headerBuffer + headerRecvSize, static_cast<int>(sizeof(Packet_FileHeader) - headerRecvSize), 0);

				if (receiveSize <= 0)
				{
					throw "error : FileCommunication::ReceiveFile() failed to receive header";
				}

				headerRecvSize += static_cast<unsigned int>(receiveSize);
			}

			unsigned long long totalRecvSize = 0;

			// check Header.
			if (memcmp(fileHeader.identifier, recvIdentifier, 4) == 0)
			{
				std::vector<char> buffer(MB2);
				std::ofstream file(saveFilePath, std::ios::binary);

				if (file.is_open())
				{
					while (fileHeader.fileSize > totalRecvSize)
					{
						unsigned long long remainSize = fileHeader.fileSize - totalRecvSize;
						int requestSize = static_cast<int>(std::min<unsigned long long>(MB2, remainSize));
						int receiveSize = recv(clientSocket, buffer.data(), requestSize, 0);
						
						if (receiveSize <= 0)
						{
							if (receiveSize == 0)
							{
								throw "error : FileCommunication::ReceiveFile() shutdown FileSender";
							}
							else
							{
								throw "error : FileCommunication::ReceiveFile()";
							}

						}
						
						file.write(buffer.data(), receiveSize);
						if (!file)
						{
							throw "error : FileCommunication::ReceiveFile() failed to write file";
						}

						totalRecvSize += receiveSize;
					
						progress = std::round((double)totalRecvSize / (double)fileHeader.fileSize * 100000) / 1000;
					}

					if (fileHeader.fileSize == 0)
					{
						progress = 100.0;
					}
				}
				else
				{
					throw "error : Unable to open file.";
				}
			}
			else
			{
				throw "error : FileCommunication::ReceiveFile() invalid file header";
			}
		}
	}
}
