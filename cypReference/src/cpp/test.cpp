﻿#include "cyp_encrypt.hpp"
#include "cyp_communication.hpp"
#include "cyp_hash.hpp"

int main()
{
	cyp::communication::Tcp tcp;
	cyp::encrypt::Aes aes;

	auto a = aes.cbcEncrypt("KIM");
	aes.cbcDecrypt(std::get<0>(a), std::get<1>(a), std::get<2>(a));

	cyp::hash::Sha sha;
	std::cout << sha.fileToSHA2_256("C:\\Users\\Cyp\\Desktop\\openCV_고기동.cs");
}