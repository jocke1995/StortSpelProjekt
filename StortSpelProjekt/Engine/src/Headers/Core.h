#ifndef VECTORFLOATS_H
#define VECTORFLOATS_H

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

#endif