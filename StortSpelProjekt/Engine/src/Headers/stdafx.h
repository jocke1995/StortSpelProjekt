#ifndef STDAFX_H
#define STDAFX_H

#define NOMINMAX	// Assimp is using a function called "min"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include "EngineMath.h"
#include "d3dx12.h"

// Windows stuff
#include <Windows.h>

// Time
#include <chrono>
#include <ctime>

// Stuff
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

// Threads
#include <mutex>
#include <process.h>	// _beginThreadex

// Data Structures
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>

// For wstring convertion
#include <locale>
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter;
inline std::string to_string(std::wstring wstr)
{
	return strconverter.to_bytes(wstr);
}
inline std::wstring to_wstring(std::string str)
{
	return strconverter.from_bytes(str);
}

typedef DirectX::XMMATRIX float4x4;

#include "Core.h"
#include "structs.h"

enum FLAG_DRAW
{
	ForwardRendering = BIT(1),
	Blend = BIT(2),
	Shadow = BIT(3),
	// animation = BIT(4),
	// etc..
};

namespace Log
{
	enum class Severity
	{
		WARNING,
		CRITICAL,
		OTHER
	};

	template <typename... Args>
	inline void PrintSeverity(const Severity type, const std::string string, const Args&... args)
	{
		std::vector<char> inputBuffer;
		inputBuffer.resize(256);
		char typeBuffer[32] = {};

		sprintf(inputBuffer.data(), string.c_str(), args...);

		switch (type)
		{
		case Severity::CRITICAL:
			sprintf(typeBuffer, "CRITICAL ERROR: ");
			break;

		case Severity::WARNING:
			sprintf(typeBuffer, "WARNING: ");
			break;

		default:
			sprintf(typeBuffer, "");
			break;
		}

		std::string finalBuffer = std::string(typeBuffer) + inputBuffer.data();

		OutputDebugStringA(finalBuffer.c_str());
	}

	template <typename... Args>
	inline void Print(const std::string string, const Args&... args)
	{
		std::vector<char> inputBuffer;
		inputBuffer.resize(256);

		sprintf(inputBuffer.data(), string.c_str(), args...);

		OutputDebugStringA(inputBuffer.data());
	}
}

#endif