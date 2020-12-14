#ifndef VECTORFLOATS_H
#define VECTORFLOATS_H

#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <Windows.h>

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

inline std::string GetFileExtension(const std::string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
	{
		return FileName.substr(FileName.find_last_of(".") + 1);
	}
	return "";
}

enum class WINDOW_MODE
{
	WINDOWED,
	WINDOWED_FULLSCREEN,
	FULLSCREEN
};

enum class TEXTURE_TYPE
{
	UNKNOWN,
	TEXTURE2D,
	TEXTURE2DGUI,
	TEXTURECUBEMAP,
	NUM_TYPES
};

enum class TEXTURE2D_TYPE
{
	ALBEDO,
	ROUGHNESS,
	METALLIC,
	NORMAL,
	EMISSIVE,
	OPACITY,
	NUM_TYPES
};

enum LIGHT_TYPE
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
	NUM_LIGHT_TYPES
};

enum SHADOW_RESOLUTION
{
	LOW,
	MEDIUM,
	HIGH,
	NUM_SHADOW_RESOLUTIONS,
	UNDEFINED
};

enum class ShaderType
{
	VS = 0,
	PS = 1,
	CS = 2,
	UNSPECIFIED = 3
};

enum class CAMERA_TYPE
{
	PERSPECTIVE,
	ORTHOGRAPHIC,
	NUM_CAMERA_TYPES,
	UNDEFINED
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
#define DEVELOPERMODE_DRAWBOUNDINGBOX true
#define DEVELOPERMODE_GAMEINTERFACE true
#define DEVELOPERMODE_DEVINTERFACE true
#define DEVELOPERMODE_NETWORKLOG false

enum FLAG_DRAW
{
	NO_DEPTH = BIT(1),
	DRAW_OPAQUE = BIT(2),
	DRAW_ANIMATED = BIT(3),
	DRAW_TRANSPARENT_CONSTANT = BIT(4),
	DRAW_TRANSPARENT_TEXTURE = BIT(5),
	GIVE_SHADOW = BIT(6),
	NUM_FLAG_DRAWS = 6,
};

enum FLAG_THREAD
{
	RENDER = BIT(1),
	NETWORK = BIT(2),
	HEIGHTMAPLOAD = BIT(3),
	A_STAR = BIT(4),
	// CopyTextures,
	// PrepareNextScene ..
	// etc
	ALL = BIT(8)
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

	template <typename... Args>
	inline void PrintToFile(const std::string string, const Args&...args)
	{
		std::FILE* file = std::fopen("ERROR.txt", "a");
		if (file)
		{
			fprintf(file, string.c_str(), args...);
			std::fclose(file);
		}
	}

	template <typename... Args>
	inline void PrintSeverityToFile(const Severity type, const std::string string, const Args&...args)
	{
		std::FILE* file = std::fopen("ERROR.txt", "a");
		if (file)
		{
			switch (type)
			{
			case Severity::CRITICAL:
				fprintf(file, "CRITICAL ERROR: ");
				break;

			case Severity::WARNING:
				fprintf(file, "WARNING: ");
				break;

			default:
				fprintf(file, "");
				break;
			}

			fprintf(file, string.c_str(), args...);
			std::fclose(file);
		}
	}

	inline void ResetFile()
	{
		std::FILE* file = std::fopen("ERROR.txt", "w");

		std::fclose(file);
	}
}

#endif