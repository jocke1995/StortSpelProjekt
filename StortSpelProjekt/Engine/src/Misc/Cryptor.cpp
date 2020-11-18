#include "stdafx.h"
#include "Cryptor.h"
#include <filesystem>
#include <fstream>
#include "EngineRand.h"

bool Cryptor::Encrypt(int key, const char* source, const char* destination)
{
	EngineRand rand(key);
	std::ifstream inStream(source);
	std::ofstream outStream(destination);
	char character;

	if (inStream.is_open() && outStream.is_open())
	{
		while (inStream.get(character))
		{
			character = (character + rand.Rand(0, 127)) % 127;
			outStream << static_cast<unsigned int>(character) << " ";
		}
	}
	inStream.close();
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
	inStream.close();

	std::ofstream outStream(source);
	if (outStream.is_open())
	{
		outStream << ss.rdbuf();
	}

	outStream.close();
	return true;
}

bool Cryptor::EncryptDirectory(const char* path)
{
	std::stringstream keys;

	encryptDirectoryHelper(path, keys);

	std::ofstream output("Keys.txt");
	if (output.is_open())
	{
		output << keys.rdbuf();
	}

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

	std::ofstream outStream(destination);
	if (outStream.is_open())
	{
		outStream << ss.rdbuf();
	}
	else
	{
		outStream.close();
		return false;
	}

	outStream.close();
	return true;
}

std::stringstream Cryptor::Decrypt(int key, const char* source)
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
	return ss;
}

void Cryptor::encryptDirectoryHelper(const char* path, std::stringstream& keys)
{
	EngineRand rand(time(NULL));
	unsigned long key = rand.Rand();
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			encryptDirectoryHelper(entry.path().generic_string().c_str(), keys);
			std::cout << "Encrypted folder\t" << entry.path().generic_string().c_str() << std::endl;
		}
		else
		{
			Encrypt(key, entry.path().generic_string().c_str());
			std::cout << "Encrypted file\t\t" << entry.path().generic_string().c_str() << " With key: " << key << std::endl;
			keys << entry.path().generic_string().c_str() << " " << key << "\n";
		}
		key = rand.Rand();
	}
}
