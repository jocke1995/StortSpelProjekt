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
#include <fstream>

// Threads
#include <mutex>
#include <process.h>	// _beginThreadex

// Data Structures
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>

typedef DirectX::XMMATRIX float4x4;

#include "Core.h"
#include "structs.h"

#endif