#ifndef CRYPTOR_H
#define CRYPTOR_H
class Cryptor
{
public:
	static bool Encrypt(int key, const char* source, const char* destination, bool binary = false);
	static bool Encrypt(int key, const char* source, bool binary = false);
	static bool EncryptDirectory(int key, const char* path);
	static bool Decrypt(int key, const char* source, const char* destination, bool binary = false);
	static bool Decrypt(int key, const char* source, std::stringstream* ss);
	static bool Decrypt(int key, const wchar_t* source, std::wstringstream* wss);
	static unsigned int GetGlobalKey();
private:
	static void encryptDirectoryHelper(int key, const char* path);
};
#endif