#include "cyp_encrypt.hpp"

namespace cyp
{
	namespace encrypt
	{
		std::tuple<std::string, std::string, std::string> Aes::CbcEncrypt_hex256(const std::string& plainText)
		{
			std::string cipherHex;
			std::string cipherText, ivText, keyText;

			CryptoPP::AutoSeededRandomPool prng;

			CryptoPP::byte key[CryptoPP::AES::MAX_KEYLENGTH];
			prng.GenerateBlock(key, sizeof(key));

			CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
			prng.GenerateBlock(iv, sizeof(iv));

			CryptoPP::StringSource ss_key(key, sizeof(key), true,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(keyText)
				)
			);

			CryptoPP::StringSource ss_iv(iv, sizeof(iv), true,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(ivText)
				)
			);

			try
			{
				CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
				e.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv));

				CryptoPP::StringSource s(plainText, true,
					new CryptoPP::StreamTransformationFilter(e,
						new CryptoPP::StringSink(cipherHex)
					)
				);
			}
			catch (const CryptoPP::Exception& e)
			{
				throw std::runtime_error(e.what());
			}


			CryptoPP::StringSource ss_cipher(cipherHex, true,
				new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(cipherText)
				)
			);
	
			std::tuple<std::string, std::string, std::string> output = std::make_tuple(keyText, ivText, cipherText);

			return output;
		}


		std::string Aes::CbcDecrypt_hex256(const std::string& keyText, const std::string& ivText, const std::string& cipherText)
		{
			std::string keyHex;
			std::string ivHex;
			std::string cipherHex;
			std::string plainText;

			try
			{
				CryptoPP::StringSource ss_key(keyText, true,
					new CryptoPP::HexDecoder(
						new CryptoPP::StringSink(keyHex)
					)
				);

				CryptoPP::StringSource ss_iv(ivText, true,
					new CryptoPP::HexDecoder(
						new CryptoPP::StringSink(ivHex)
					)
				);

				CryptoPP::StringSource ss_cipher(cipherText, true,
					new CryptoPP::HexDecoder(
						new CryptoPP::StringSink(cipherHex)
					)
				);

				if (keyHex.size() != CryptoPP::AES::MAX_KEYLENGTH)
				{
					throw std::invalid_argument("AES-256 key must be 32 bytes.");
				}

				if (ivHex.size() != CryptoPP::AES::BLOCKSIZE)
				{
					throw std::invalid_argument("AES-CBC IV must be 16 bytes.");
				}

				CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption d;
				d.SetKeyWithIV(
					reinterpret_cast<const CryptoPP::byte*>(keyHex.data()), keyHex.size(),
					reinterpret_cast<const CryptoPP::byte*>(ivHex.data()), ivHex.size());

				CryptoPP::StringSource s(cipherHex, true,
					new CryptoPP::StreamTransformationFilter(d,
						new CryptoPP::StringSink(plainText)
					)
				);
			}
			catch (const CryptoPP::Exception& e)
			{
				throw std::runtime_error(e.what());
			}
			return plainText;
		}
	}
}
