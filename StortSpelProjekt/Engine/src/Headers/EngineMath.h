#ifndef ENGINEMATH_H
#define ENGINEMATH_H

#include <DirectXMath.h>

typedef DirectX::XMMATRIX float4x4;

typedef union
{
	struct { float x; float y; float z; float w; };
	struct { float r; float g; float b; float a; };
} float4;

typedef union
{
	struct { float x; float y; float z; };
	struct { float r; float g; float b; };
} float3;

typedef union
{
	struct { float x; float y; };
	struct { float u; float v; };
} float2;

typedef union
{
	struct { double x; double y; double z; double w; };
	struct { double r; double g; double b; double a; };
} double4;

typedef union
{
	struct { double x; double y; double z; };
	struct { double r; double g; double b; };
} double3;

typedef union
{
	struct { double x; double y; };
	struct { double u; double v; };
} double2;

#endif