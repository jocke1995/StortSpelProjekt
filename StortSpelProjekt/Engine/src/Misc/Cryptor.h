#ifndef CRYPTOR_H
#define CRYPTOR_H
#include <sstream>
class Cryptor
{
public:
	static bool Encrypt(int key, const char* source, const char* destination);
	static bool Encrypt(int key, const char* source);
	static bool EncryptDirectory(const char* path);
	static bool Decrypt(int key, const char* source, const char* destination);
	static std::stringstream Decrypt(int key, const char* source);
private:
	static void encryptDirectoryHelper(const char* path, std::stringstream& keys);
};

#endif