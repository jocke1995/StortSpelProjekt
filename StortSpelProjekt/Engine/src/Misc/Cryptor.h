#ifndef CRYPTOR_H
#define CRYPTOR_H
class Cryptor
{
public:
	static bool Encrypt(int key, const char* source, const char* destination, bool binary = false);
	static bool Encrypt(int key, const char* source, bool binary = false);
	static bool EncryptDDS(int key, const char* src, const char* destination = nullptr);
	template <typename T>
	static inline void Encrypt(int key, T* data, unsigned int nrOfElements, int minRand, int maxRand);
	static bool EncryptDirectory(int key, const char* path);
	static bool Decrypt(int key, const char* source, const char* destination, bool binary = false);
	static bool Decrypt(int key, const char* source, std::stringstream* ss);
	static bool Decrypt(int key, const wchar_t* source, std::wstringstream* wss);
	static bool DecryptDDS(int key, const char* src, const char* destination = nullptr);
	template <typename T>
	static inline void Decrypt(int key, T* data, unsigned int nrOfElements, int minRand, int maxRand);
	static bool DecryptDirectory(int key, const char* path);
	static unsigned int GetGlobalKey();
private:
	static void encryptDirectoryHelper(int key, const char* path);
	static void decryptDirectoryHelper(int key, const char* path);
};
#endif

#ifndef CRYPTORFUNC_H
#define CRYPTORFUNC_H
template<typename T>
inline void Cryptor::Encrypt(int key, T* data, unsigned int nrOfElements, int minRand, int maxRand)
{
	EngineRand rand(key);
	for (int i = 0; i < nrOfElements; i++)
	{
		data[i] = data[i] + rand.Rand(minRand, maxRand);
	}
};

template<typename T>
inline void Cryptor::Decrypt(int key, T* data, unsigned int nrOfElements, int minRand, int maxRand)
{
	EngineRand rand(key);
	for (int i = 0; i < nrOfElements; i++)
	{
		data[i] = data[i] - rand.Rand(minRand, maxRand);
	}
};
#endif