#include "cyp_convert.hpp"

namespace cyp
{
	namespace convert
	{
		std::string Hex::StrToHex(std::string& str)
		{
			std::string decoded;

			CryptoPP::HexDecoder decoder;
			decoder.Put((CryptoPP::byte*)str.data(), str.size());
			decoder.MessageEnd();

			CryptoPP::word64 size = decoder.MaxRetrievable();
			if (size && size <= SIZE_MAX)
			{
				decoded.resize(size);
				decoder.Get((CryptoPP::byte*)&decoded[0], decoded.size());
			}

			return decoded;
		}
		
		std::string Hex::HexToStr(unsigned char* decoded, size_t size)
		{
			if (decoded == nullptr && size > 0)
			{
				throw std::invalid_argument("HexToStr() input is null");
			}

			std::string encoded;

			CryptoPP::HexEncoder encoder;
			encoder.Put(decoded, size);
			encoder.MessageEnd();

			CryptoPP::word64 size_ = encoder.MaxRetrievable();
			if (size_)
			{
				encoded.resize(size_);
				encoder.Get((CryptoPP::byte*)&encoded[0], encoded.size());
			}

			return encoded;
		}
	}
}

