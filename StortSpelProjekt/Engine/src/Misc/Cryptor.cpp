#include "stdafx.h"
#include "Cryptor.h"
#include <filesystem>
#include <sstream>
#include <fstream>
#include <comdef.h>
#include "EngineRand.h"
#include "Multithreading/ThreadPool.h"
#include "Multithreading/EncryptTask.h"

bool Cryptor::Encrypt(int key, const char* source, const char* destination, bool binary)
{
	EngineRand rand(key);
	std::ifstream inStream(source, std::ios::in | std::ios::binary);
	std::stringstream ss;
	char character;
	unsigned int value;
	if (inStream.is_open())
	{
		while (inStream.read(&character,1))
		{
			value = static_cast<int>(static_cast<unsigned char>(character));
			value = (value + rand.Rand(0, 256));
			ss << value << " ";
		}
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open input file %s with cryptor!", source);
		return false;
	}
	inStream.close();

	std::ofstream outStream(destination);
	if (outStream.is_open())
	{
		outStream << ss.rdbuf();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open output file %s with cryptor!", source);
		return false;
	}

	outStream.close();
	return true;
}

bool Cryptor::Encrypt(int key, const char* source, bool binary)
{
	EngineRand rand(key);
	std::ifstream inStream(source, std::ios::in | std::ios::binary * binary);
	std::stringstream ss;
	char character;
	unsigned int value;
	if (inStream.is_open())
	{
		inStream.get(character);
		while (!inStream.eof())
		{
			value = static_cast<int>(static_cast<unsigned char>(character));
			value = (value + rand.Rand(0, 256));
			ss << value << " ";

			inStream.get(character);
		}
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open input file %s with cryptor!", source);
		return false;
	}
	inStream.close();

	std::ofstream outStream(source);
	if (outStream.is_open())
	{
		outStream << ss.rdbuf();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open output file %s with cryptor!", source);
		return false;
	}

	outStream.close();
	return true;
}

bool Cryptor::EncryptDirectory(int key, const char* path)
{
	encryptDirectoryHelper(key, path);

	ThreadPool::GetInstance().WaitForThreads(FLAG_THREAD::ENCRYPT);
	return true;
}

bool Cryptor::Decrypt(int key, const char* source, const char* destination, bool binary)
{
	EngineRand rand(key);
	std::stringstream ss;

	FILE* file = fopen(source, "r");

	char actualChar = 0;
	int value = 0;
	unsigned int result = 0;
	char res;
	if (file != NULL)
	{
		while (true)
		{
			res = fscanf(file, "%d ", &result);
			actualChar = (result - rand.Rand(0, 256));
			if (res == EOF)
			{
				break;
			}
			ss << actualChar;
		}
		fclose(file);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open input file %s with cryptor!", source);
	}

	std::ofstream outStream(destination, std::ios::out | std::ios::binary * binary);
	if (outStream.is_open())
	{
		outStream.write(ss.rdbuf()->str().c_str(), ss.rdbuf()->str().size());
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open output file %s with cryptor!", source);
		return false;
	}

	outStream.close();
	return true;
}

bool Cryptor::Decrypt(int key, const char* source, std::stringstream* ss)
{
	EngineRand rand(key);

	FILE* file = fopen(source, "r");

	char actualChar = 0;
	unsigned int result = 0;
	char res;
	if (file != NULL)
	{
		while (true)
		{
			res = fscanf(file, "%d ", &result);
			actualChar = (result - rand.Rand(0, 256));
			if (res == EOF)
			{
				break;
			}
			*ss << actualChar;
		}
		fclose(file);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open file %s with cryptor!", source);
		return false;
	}
	return true;
}

bool Cryptor::Decrypt(int key, const wchar_t* source, std::wstringstream* wss)
{
	EngineRand rand(key);
	_bstr_t src(source);
	FILE* file = fopen(src, "r");

	wchar_t actualChar = 0;
	unsigned int result = 0;
	char res;
	if (file != NULL)
	{
		while (true)
		{
			res = fscanf(file, "%d ", &result);
			actualChar = (result - rand.Rand(0, 256));
			if (res == EOF)
			{
				break;
			}
			*wss << actualChar;
		}
		fclose(file);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Could not open file %S with cryptor!", source);
		return false;
	}
	return true;
}

unsigned int Cryptor::GetGlobalKey()
{
	return 11;
}

void Cryptor::encryptDirectoryHelper(int key, const char* path)
{
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			encryptDirectoryHelper(key, entry.path().generic_string().c_str());
		}
		else
		{
			std::string extension = entry.path().extension().generic_string().c_str();
			if (extension != ".mtl")
			{
				if (extension == ".DDS" || extension == ".dds" || extension == ".fbx" || extension == ".FBX" || extension == ".png" || extension == ".jpg" || extension == ".tga")
				{
					Encrypt(key, entry.path().generic_string().c_str(),true);
					//EncryptTask task(key, true, entry.path().generic_string());
					//task.Execute();
				}
				else
				{
					Encrypt(key, entry.path().generic_string().c_str());
					//EncryptTask task(key, false, entry.path().generic_string());
					//task.Execute();
				}
			}
		}
	}
}
