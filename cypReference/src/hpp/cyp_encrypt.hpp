#pragma once

#include "cryptopp/cryptlib.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/modes.h"
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"
#include "cyp_common.hpp"

#pragma comment(lib, "cryptlib.lib")

#include <tuple>

namespace cyp
{
	namespace encrypt
	{

		class Aes
		{
		private:

		public:
			/*
			* AES-CBC IV size must match the AES block size.
			*/
			std::tuple<std::string, std::string, std::string> CbcEncrypt_hex256(const std::string& plainText);
			std::string CbcDecrypt_hex256(const std::string& key, const std::string& iv, const std::string& cipherText);
		
		};

		class Rsa
		{

		};
	}
}
