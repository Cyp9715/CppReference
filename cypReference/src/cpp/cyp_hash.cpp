#include "cyp_hash.hpp"

namespace cyp
{
	namespace hash
	{
		template std::string Sha::StrToSha<CryptoPP::SHA1>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA224>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA256>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA384>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA512>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA3_224>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA3_256>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA3_384>(const std::string& str);
		template std::string Sha::StrToSha<CryptoPP::SHA3_512>(const std::string& str);

		template std::string Sha::FileToSha<CryptoPP::SHA1>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA224>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA256>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA384>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA512>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA3_224>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA3_256>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA3_384>(const std::string& str);
		template std::string Sha::FileToSha<CryptoPP::SHA3_512>(const std::string& str);


		/*
		* Add other hash algorithms if necessary.
		* If CryptoPP supports it, most algorithms can be added.
		* ex) MD5 : template std::string Hkdf::StrToHkdf<CryptoPP::Weak::MD5>(const std::string& str...
		* ex) BLAKE2 : template std::string Pbkdf2::StrToPbkdf2<CryptoPP::BLAKE2b>(const std::string& str...
		*/
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA1>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA224>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA256>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA384>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA512>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA3_224>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA3_256>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA3_384>(const std::string& str, const std::string& salt, unsigned int iterate, float time);
		template std::string Pbkdf2::StrToPbkdf2<CryptoPP::SHA3_512>(const std::string& str, const std::string& salt, unsigned int iterate, float time);

		template std::string Hkdf::StrToHkdf<CryptoPP::SHA1>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA224>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA256>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA384>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA512>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA3_224>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA3_256>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA3_384>(const std::string& str, const std::string& salt, const std::string& info);
		template std::string Hkdf::StrToHkdf<CryptoPP::SHA3_512>(const std::string& str, const std::string& salt, const std::string& info);

		template <typename T>
		std::string Sha::StrToSha(const std::string& str)
		{
			T hash;
			std::string output;

			CryptoPP::StringSource s(str, true, new CryptoPP::HashFilter(hash,
				new CryptoPP::HexEncoder(new CryptoPP::StringSink(output))));

			return output;
		}

		template <typename T>
		std::string Sha::FileToSha(const std::string& filePath)
		{
			T hash;
			std::string output;

			try
			{
				CryptoPP::FileSource f(filePath.c_str(), true,
					new CryptoPP::HashFilter(hash,
						new CryptoPP::HexEncoder(new
							CryptoPP::StringSink(output))));
			}
			catch (CryptoPP::FileStore::OpenErr const& e)
			{
				CryptoPP::Exception::ErrorType errorMsg = e.GetErrorType();
				output = "Error : FileToSha() = " + errorType(errorMsg);
				return output;
			}

			return output;
		}

		std::string Md::StrToMD5(const std::string& str)
		{
			CryptoPP::byte digest[CryptoPP::Weak::MD5::DIGESTSIZE];

			CryptoPP::Weak::MD5 hash;
			hash.CalculateDigest(digest, (const CryptoPP::byte*)str.c_str(), str.length());

			CryptoPP::HexEncoder encoder;
			std::string output;

			encoder.Attach(new CryptoPP::StringSink(output));
			encoder.Put(digest, sizeof(digest));
			encoder.MessageEnd();

			return output;
		}

		std::string Md::FileToMD5(const std::string filePath)
		{
			CryptoPP::Weak::MD5 md;
			std::string output;
			const size_t size = CryptoPP::Weak::MD5::DIGESTSIZE * 2;
			CryptoPP::byte buf[size] = { 0 };
			try
			{
				CryptoPP::FileSource filesource(
					filePath.c_str(), true,
					new CryptoPP::HashFilter(
						md, new CryptoPP::HexEncoder(new CryptoPP::ArraySink(buf, size))));
			}
			catch (CryptoPP::FileStore::OpenErr const& e)
			{
				CryptoPP::Exception::ErrorType errorMsg = e.GetErrorType();
				output = "Error : FileToMD5() = " + errorType(errorMsg);
				return output;
			}

			output = std::string(reinterpret_cast<const char*>(buf), size);

			return output;
		}

		template<typename T>
		std::string Pbkdf2::StrToPbkdf2(const std::string& str, const std::string& salt, unsigned int iterate, float time)
		{
			const CryptoPP::byte* password_ = reinterpret_cast<const CryptoPP::byte*>(str.data());
			const CryptoPP::byte* salt_ = reinterpret_cast<const CryptoPP::byte*>(salt.data());
			size_t passwordLen = str.size();
			size_t saltLen = salt.size();

			CryptoPP::byte derived[T::DIGESTSIZE];

			CryptoPP::PKCS5_PBKDF2_HMAC<T> pbkdf;

			CryptoPP::byte unused = 0;

			pbkdf.DeriveKey(derived, sizeof(derived), unused, password_, passwordLen, salt_, saltLen, iterate, time);

			std::string result;
			CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(result));

			encoder.Put(derived, sizeof(derived));
			encoder.MessageEnd();

			return result;
		}

		template<typename T>
		std::string Hkdf::StrToHkdf(const std::string& str, const std::string& salt, const std::string& info)
		{
			const CryptoPP::byte* password_ = reinterpret_cast<const CryptoPP::byte*>(str.data());
			const CryptoPP::byte* salt_ = reinterpret_cast<const CryptoPP::byte*>(salt.data());
			const CryptoPP::byte* info_ = reinterpret_cast<const CryptoPP::byte*>(info.data());
			size_t passwordLen = str.size();
			size_t saltLen = salt.size();
			size_t infoLen = info.size();

			CryptoPP::byte derived[T::DIGESTSIZE];

			CryptoPP::HKDF<T> hkdf;

			hkdf.DeriveKey(derived, sizeof(derived), password_, passwordLen, salt_, saltLen, info_, infoLen);

			std::string result;
			CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(result));

			encoder.Put(derived, sizeof(derived));
			encoder.MessageEnd();

			return result;
		}


		std::string errorType(CryptoPP::Exception::ErrorType exception)
		{
			std::string output;

			switch (exception)
			{
			case CryptoPP::Exception::ErrorType::NOT_IMPLEMENTED:
				output = "NOT_IMPLEMENTED";
				break;

			case CryptoPP::Exception::ErrorType::INVALID_ARGUMENT:
				output = "INVALID_ARGUMENT";
				break;

			case CryptoPP::Exception::ErrorType::CANNOT_FLUSH:
				output = "CANNOT_FLUSH";
				break;

			case CryptoPP::Exception::ErrorType::DATA_INTEGRITY_CHECK_FAILED:
				output = "DATA_INTEGRITY_CHECK_FAILED";
				break;

			case CryptoPP::Exception::ErrorType::INVALID_DATA_FORMAT:
				output = "INVALID_DATA_FORMAT";
				break;

			case CryptoPP::Exception::ErrorType::IO_ERROR:
				output = "IO_ERROR";
				break;

			default:
				output = "OTHER_ERROR";
				break;
			}

			return output;
		}
	}
}
