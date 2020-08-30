#ifndef VECTORFLOATS_H
#define VECTORFLOATS_H

#include <string>
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


template <typename T>
inline T Min(T a, T b)
{
	if (a < b)
	{
		return a;
	}
	return b;
}

template <typename T>
inline T Max(T a, T b)
{
	if (a > b)
	{
		return a;
	}
	return b;
}

enum class COLOR_TYPE
{
	LIGHT_AMBIENT,
	LIGHT_DIFFUSE,
	LIGHT_SPECULAR,
	NUM_COLOR_TYPES
};

// this will only call release if an object exists (prevents exceptions calling release on non existant objects)
#define SAFE_RELEASE(p)			\
{								\
	if ((*p))					\
	{							\
		(*p)->Release();		\
		(*p) = NULL;			\
	}							\
}

#define NUM_SWAP_BUFFERS 2
#define BIT(x) (1 << x)
#define MAXNUMBER 10000000.0f
#define DRAWBOUNDINGBOX false

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