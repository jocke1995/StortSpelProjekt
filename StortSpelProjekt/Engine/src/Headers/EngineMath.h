#ifndef ENGINEMATH_H
#define ENGINEMATH_H

#include <DirectXMath.h>
#include <DirectXCollision.h>

# define PI           3.14159265358979323846  /* pi */
#define EPSILON		  0.000001

typedef DirectX::XMMATRIX float4x4;
typedef DirectX::XMFLOAT3X3 float3x3;

typedef union
{
	struct { float x; float y; float z; float w; };
	struct { float r; float g; float b; float a; };
} float4;

typedef union float3
{
	struct { float x; float y; float z; };
	struct { float r; float g; float b; };

	float length()
	{
		return sqrt(x * x + y * y + z * z);
	};

	void normalize()
	{
		float length = this->length();
		if (length > EPSILON)
		{
			x /= length;
			y /= length;
			z /= length;
		}
	};

	void operator /= (float denom)
	{
		x /= denom;
		y /= denom;
		z /= denom;
	};

	void operator *= (float factor)
	{
		x *= factor;
		y *= factor;
		z *= factor;
	};

	float3 operator - (float3 other)
	{
		return {
			x - other.x,
			y - other.y,
			z - other.z
		};
	};

	float3 operator + (const float3& other) const
	{
		return {x + other.x, y + other.y, z + other.z};
	};

	float3 operator - (const float3& other) const
	{
		return { x - other.x, y - other.y, z - other.z };
	};

	float3 operator * (float factor)
	{
		return { x * factor, y * factor, z * factor };
	};

	bool operator == (float3 other)
	{
		return (x == other.x && y == other.y && z == other.z);
	}

	bool operator != (float3 other)
	{
		return (x != other.x || y != other.y || z != other.z);
	}

	float3 cross(const float3& that) const
	{
		return
		{
			this->y * that.z - this->z * that.y,
			this->z * that.x - this->x * that.z,
			this->x * that.y - this->y * that.x,
		};
	};

} float3;

typedef union float2
{
	struct { float x; float y; };
	struct { float u; float v; };

	float2 operator +(const float2& other) const
	{
		return { x + other.x, y + other.y };
	}

	float2 operator -(const float2& other) const
	{
		return { x - other.x, y - other.y };
	}
} float2;

typedef union
{
	struct { double x; double y; double z; double w; };
	struct { double r; double g; double b; double a; };
} double4;

typedef union double3
{
	struct { double x; double y; double z; };
	struct { double r; double g; double b; };

	double length()
	{
		return sqrt(x * x + y * y + z * z);
	};
	void normalize()
	{
		double length = this->length();
		if (length > EPSILON)
		{
			x /= length;
			y /= length;
			z /= length;
		}
	};

	void operator /= (double denom)
	{
		x /= denom;
		y /= denom;
		z /= denom;
	};
	void operator *= (double factor)
	{
		x *= factor;
		y *= factor;
		z *= factor;
	};
	bool operator == (double3 other)
	{
		return (x == other.x && y == other.y && z == other.z);
	}
	double3 cross(double3* that)
	{
		return
		{
			this->y * that->z - this->z * that->y,
			this->z * that->x - this->x * that->z,
			this->x * that->y - this->y * that->x,
		};
	};
} double3;

typedef union
{
	struct { double x; double y; };
	struct { double u; double v; };
} double2;

class EngineMath
{
public:
	static inline float convertToRadians(float degrees)
	{
		return degrees * (PI / 180.0f);
	}

	static inline float convertToDegrees(float radians)
	{
		return radians * (180.0f / PI);
	}

	static inline int convertToWholeDegrees(float radians)
	{
		return static_cast<int>(radians * (180.0f / PI));
	}
private:
	EngineMath() {};
};

#endif