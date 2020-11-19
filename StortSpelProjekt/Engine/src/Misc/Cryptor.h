#ifndef CRYPTOR_H
#define CRYPTOR_H
#include <sstream>
class Cryptor
{
public:
	static bool Encrypt(int key, const char* source, const char* destination);
	static bool Encrypt(int key, const char* source);
	static bool EncryptDirectory(int key, const char* path);
	static bool Decrypt(int key, const char* source, const char* destination);
	static bool Decrypt(int key, const char* source, std::stringstream* ss);
	static bool Decrypt(int key, const wchar_t* source, std::wstringstream* wss);
private:
	static void encryptDirectoryHelper(int key, const char* path);
};

#endif