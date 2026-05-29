#pragma once
#include "cyp_common.hpp"
#include <windows.h>

namespace cyp
{
	namespace sharedMemory
	{
		/* 
		* CAUTION
		* shared memory cannot expand its size dynamically.
		* avoid using dynamically assignable types like std::string.
		* even when allocating class pointer, do not use dynamically assignable types within a class.
		*
		* RaceCondition may occur when concurrently accessing shared memory.
		* Therefore, in order to access the same memory at the same time, 
		* it is necessary to write an appropriate management mechanism.
		*/
		template <typename T>
		class SharedMemory
		{
		private:
			HANDLE _hmapFile;
			T* _pBuf;
			DWORD _fileAccess;

		public:
			~SharedMemory()
			{
				if (_pBuf)
				{
					UnmapViewOfFile(_pBuf);
				}

				if (_hmapFile)
				{
					CloseHandle(_hmapFile);
				}
			}

			SharedMemory() 
			{
				_hmapFile = nullptr;
				_pBuf = nullptr;
				_fileAccess = 0;
			}

			SharedMemory(const SharedMemory&) = delete;
			SharedMemory& operator=(const SharedMemory&) = delete;

			bool Install(std::string key, DWORD fileAccess = FILE_MAP_ALL_ACCESS)
			{
				if (sizeof(T) > MAXDWORD)
				{
					return false;
				}

				if (_pBuf)
				{
					UnmapViewOfFile(_pBuf);
					_pBuf = nullptr;
				}

				if (_hmapFile)
				{
					CloseHandle(_hmapFile);
					_hmapFile = nullptr;
				}

				_fileAccess = 0;

				_hmapFile = CreateFileMappingA(
					INVALID_HANDLE_VALUE, NULL,
					PAGE_READWRITE, 0,
					static_cast<DWORD>(sizeof(T)), key.c_str());

				if (_hmapFile == NULL)
					return false;

				_pBuf = (T *)MapViewOfFile(_hmapFile, fileAccess, 0, 0, sizeof(T));

				if (!_pBuf)
				{
					if (_hmapFile)
					{
						CloseHandle(_hmapFile);
						_hmapFile = nullptr;
					}
					return false;
				}

				_fileAccess = fileAccess;
				return true;
			}

			void Update(T* instance)
			{
				if (!_pBuf || !instance)
				{
					throw std::runtime_error("SharedMemory::Update() invalid memory");
				}

				if ((_fileAccess & (FILE_MAP_WRITE | FILE_MAP_COPY | FILE_MAP_ALL_ACCESS)) == 0)
				{
					throw std::runtime_error("SharedMemory::Update() memory is not writable");
				}

				CopyMemory((PVOID)_pBuf, instance, sizeof(T));
			}

			T* GetMemory()
			{
				if (_pBuf == nullptr)
				{
					throw std::runtime_error("SharedMemory::GetMemory() invalid memory");
 				}

				return _pBuf;
			}
		};
	}
}
