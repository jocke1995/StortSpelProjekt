#include "stdafx.h"
#include "EncryptTask.h"
#include "../Cryptor.h"

EncryptTask::EncryptTask(unsigned int key, const std::string path): MultiThreadedTask(FLAG_THREAD::ENCRYPT),
	m_Key(key), m_Path(path)
{

}

void EncryptTask::Execute()
{
	Cryptor::Encrypt(m_Key, m_Path.c_str());
	Log::Print("Encrypted %s with key %d\n", m_Path.c_str(), m_Key);
}
