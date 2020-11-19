#include "stdafx.h"
#include "Cryptor.h"
#include <filesystem>
#include <fstream>
#include <comdef.h>
#include "EngineRand.h"
#include "Multithreading/ThreadPool.h"
#include "Multithreading/EncryptTask.h"

bool Cryptor::Encrypt(int key, const char* source, const char* destination)
{
	EngineRand rand(key);
	std::ifstream inStream(source);
	std::stringstream ss;
	char character;

	if (inStream.is_open())
	{
		while (inStream.get(character))
		{
			character = (character + rand.Rand(0, 127)) % 127;
			ss << static_cast<unsigned int>(character) << " ";
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

bool Cryptor::Encrypt(int key, const char* source)
{
	EngineRand rand(key);
	std::ifstream inStream(source);
	std::stringstream ss;
	char character;
	if (inStream.is_open())
	{
		inStream.get(character);
		while (!inStream.eof())
		{
			int randNum = rand.Rand(0, 127);
			character = ((character + randNum) % 127);
			ss << static_cast<unsigned int>(character) << " ";

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

bool Cryptor::Decrypt(int key, const char* source, const char* destination)
{
	EngineRand rand(key);
	std::stringstream ss;

	FILE* file = fopen(source, "r");

	unsigned char actualChar = 0;
	unsigned int result = 0;
	char res;
	if (file != NULL)
	{
		while (true)
		{
			res = fscanf(file, "%d ", &result);
			actualChar = static_cast<unsigned char>(result);
			int randNum = rand.Rand(0, 127);
			actualChar = (actualChar - randNum + 127) % 127;
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

bool Cryptor::Decrypt(int key, const char* source, std::stringstream* ss)
{
	EngineRand rand(key);

	FILE* file = fopen(source, "r");

	unsigned char actualChar = 0;
	unsigned int result = 0;
	char res;
	if (file != NULL)
	{
		while (true)
		{
			res = fscanf(file, "%d ", &result);
			actualChar = static_cast<unsigned char>(result);
			int randNum = rand.Rand(0, 127);
			actualChar = (actualChar - randNum + 127) % 127;
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

	unsigned char actualChar = 0;
	unsigned int result = 0;
	char res;
	if (file != NULL)
	{
		while (true)
		{
			res = fscanf(file, "%d ", &result);
			actualChar = static_cast<unsigned char>(result);
			int randNum = rand.Rand(0, 127);
			actualChar = (actualChar - randNum + 127) % 127;
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
			EncryptTask task(key, entry.path().generic_string());
			task.Execute();
		}
	}
}
