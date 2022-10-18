//#include "config.h"
#pragma once
#include <cmath>
#include <stdio.h>
#ifndef M_PI
#define M_PI 3.141592553584
#endif


#pragma region Vector2
//	Vector operations: +, -, *, length, normalize

/// <summary>
/// vec2 is a 2 dimensional vector with coordinates.
/// </summary>
struct vec2
{
	union
	{
		struct
		{
			float x, y;
		};
		struct
		{
			float u, v;
		};
		float data[2];
	};

	inline vec2();
	inline vec2(float x, float y);

	bool operator==(vec2 right);
	bool operator!=(vec2 right);
	void operator+=(vec2 right);
	void operator-=(vec2 right);
	void operator*=(vec2 right);
	void operator*=(float right);
	void operator/=(vec2 right);
	void operator/=(float right);
	float &operator[](size_t index);

	float Dot(vec2 right);

	float Length();
	float Length2();
	void Normalize();
};

// Vector Constructors
vec2::vec2() : x(0), y(0) {}

vec2::vec2(float x, float y) : x(x), y(y) {}

//	operator methods
inline bool vec2::operator==(vec2 right)
{
	return (*this).x == right.x && (*this).y == right.y;
}

inline bool vec2::operator!=(vec2 right)
{
	return (*this).x != right.x || (*this).y != right.y;
}

inline void vec2::operator+=(vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] += right[i];
}

inline void vec2::operator-=(vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] -= right[i];
}

inline void vec2::operator*=(vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] *= right[i];
}

inline void vec2::operator*=(float right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] *= right;
}

inline void vec2::operator/=(vec2 right)
{
	for (size_t i = 0; i < 2; i++)
	{
		data[i] /= right[i];
	}
}

inline void vec2::operator/=(float right)
{
	for (size_t i = 0; i < 2; i++)
	{
		data[i] /= right;
	}
}

inline float vec2::Dot(vec2 right)
{
	return x * right.x + y * right.y;
}

//	utilitiy methods
inline float &vec2::operator[](size_t index)
{
	return data[index];
}

inline float vec2::Length()
{
	return sqrtf(x * x + y * y);
}

inline float vec2::Length2()
{
	return x * x + y * y;
}
	

inline void vec2::Normalize()
{
	if (Length() == 0)
		return;
	float length = Length();
	for (size_t i = 0; i < 2; i++)
		data[i] /= length;
}

// operator functions
inline vec2 operator+(vec2 left, vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] += right[i];
	return left;
}

inline vec2 operator-(vec2 left, vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] -= right[i];
	return left;
}

inline vec2 operator*(vec2 left, vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] *= right[i];
	return left;
}

inline vec2 operator*(vec2 left, float right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] *= right;
	return left;
}

inline vec2 operator*(float left, vec2 right)
{
	for (size_t i = 0; i < 2; i++)
		right[i] *= left;
	return right;
}

inline vec2 operator/(vec2 left, vec2 right)
{
	for (size_t i = 0; i < 2; i++)
	{
		left[i] /= right[i];
	}
	return left;
}

inline vec2 operator/(vec2 left, float right)
{
	for (size_t i = 0; i < 2; i++)
	{
		left[i] /= right;
	}
	return left;
}

inline vec2 operator/(float left, vec2 right)
{
	for (size_t i = 0; i < 2; i++)
	{
		right[i] /= left;
	}
	return right;
}

inline float Dot(vec2 left, vec2 right)
{
	float temp = 0;
	for (size_t i = 0; i < 2; i++)
		temp += left[i] * right[i];
	return temp;
}

inline float Length(vec2 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

inline float Length2(vec2 vector)
{
	return vector.x * vector.x + vector.y * vector.y;
}

/// <summary>
/// takes a vec2 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline vec2 Normalize(vec2 vector)
{
	float length = Length(vector);
	for (size_t i = 0; i < 3; i++)
		vector[i] /= length;
	return vector;
}
#pragma endregion

#pragma region Vector3
//	Vector operations: +, -, *, length, normalize, dot product, cross product

/// <summary>
/// vec3 is a 3 dimensional vector with coordinates. or a rgb-value
/// </summary>
struct vec3
{
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			float r, g, b;
		};
		struct
		{
			float u, v, w;
		};
		float data[3];
	};

	inline vec3();
	inline vec3(float x, float y, float z);
	inline vec3(vec2, float z);

	bool operator==(vec3 right);
	bool operator!=(vec3 right);
	void operator+=(vec3 right);
	void operator-=(vec3 right);
	void operator*=(vec3 right);
	void operator*=(float right);
	void operator/=(vec3 right);
	void operator/=(float right);
	float &operator[](size_t index);

	float Dot(vec3 right);
	void Cross(vec3 right);

	float Length();
	float Length2();
	void Normalize();
};

// Vector Constructors
vec3::vec3() : x(0), y(0), z(0) {}

vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

vec3::vec3(vec2 vec, float z) : x(vec.x), y(vec.y), z(z) {}

//	operator methods
inline bool vec3::operator==(vec3 right)
{
	return (*this).x == right.x && (*this).y == right.y && (*this).z == right.z;
}

inline bool vec3::operator!=(vec3 right)
{
	return (*this).x != right.x || (*this).y != right.y || (*this).z != right.z;
}

inline void vec3::operator+=(vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] += right[i];
}

inline void vec3::operator-=(vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] -= right[i];
}

inline void vec3::operator*=(vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] *= right[i];
}

inline void vec3::operator*=(float right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] *= right;
}

inline void vec3::operator/=(vec3 right)
{
	for (size_t i = 0; i < 3; i++)
	{
		data[i] /= right[i];
	}
}

inline void vec3::operator/=(float right)
{
	for (size_t i = 0; i < 3; i++)
	{
		data[i] /= right;
	}
}

inline float vec3::Dot(vec3 right)
{
	return x * right.x + y * right.y + z * right.z;
}

inline void vec3::Cross(vec3 right)
{
	x = y * right.z - z * right.y;
	y = z * right.x - x * right.z;
	z = x * right.y - y * right.x;
}

//	utilitiy methods
inline float &vec3::operator[](size_t index)
{
	return data[index];
}

inline float vec3::Length()
{
	return sqrtf(x * x + y * y + z * z);
}

inline void vec3::Normalize()
{
	float length = Length();
	if (length)
	for (size_t i = 0; i < 3; i++)
		data[i] /= length;
}

//	operator functions
inline vec3 operator+(vec3 left, vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] += right[i];
	return left;
}

inline vec3 operator-(vec3 left, vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] -= right[i];
	return left;
}

inline vec3 operator*(vec3 left, vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] *= right[i];
	return left;
}

inline vec3 operator*(vec3 left, float right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] *= right;
	return left;
}

inline vec3 operator*(float left, vec3 right)
{
	for (size_t i = 0; i < 3; i++)
		right[i] *= left;
	return right;
}

inline vec3 operator/(vec3 left, vec3 right)
{
	for (size_t i = 0; i < 3; i++)
	{
		left[i] /= right[i];
	}
	return left;
}

inline vec3 operator/(vec3 left, float right)
{
	for (size_t i = 0; i < 3; i++)
	{
		left[i] /= right;
	}
	return left;
}

inline vec3 operator/(float left, vec3 right)
{
	for (size_t i = 0; i < 3; i++)
	{
		right[i] /= left;
	}
	return right;
}

inline float Dot(vec3 left, vec3 right)
{
	float temp = 0;
	for (size_t i = 0; i < 3; i++)
		temp += left[i] * right[i];
	return temp;
}

inline vec3 Cross(vec3 left, vec3 right)
{
	return vec3(
		left.y * right.z -
			left.z * right.y,

		left.z * right.x -
			left.x * right.z,

		left.x * right.y -
			left.y * right.x);
}

inline float Length(vec3 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

inline float Length2(vec3 vector)
{
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/// <summary>
/// takes a vec3 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline vec3 Normalize(vec3 vector)
{
	float length = Length(vector);
	if (length)
		for (size_t i = 0; i < 3; i++)
			vector[i] /= length;
	return vector;
}

#pragma endregion // Vector3

#pragma region Vector4
//	Vector operations: +, -, *, length, normalize, dot product, cross product

/// <summary>
/// vec4 is a 4 dimensional vector with homogeneous coordinates, or a rgba-value.
/// </summary>
struct vec4
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float r, g, b, a;
		};
		float data[4];
	};

	inline vec4();
	inline vec4(float x, float y, float z, float w);
	inline vec4(vec3 vec, float w);

	void operator+=(vec4 right);
	void operator-=(vec4 right);
	void operator*=(vec4 right);
	void operator*=(float right);
	void operator/=(vec4 right);
	void operator/=(float right);
	float &operator[](size_t index);

	float Dot(vec4 right);
	void Cross(vec4 right);

	float Length();
	float Length2();
	void Normalize();
	vec3 toV3();
};

// Vector Constructors
vec4::vec4() : x(0), y(0), z(0), w(0) {}

vec4::vec4(float x, float y, float z, float w = 0) : x(x), y(y), z(z), w(w) {}

vec4::vec4(vec3 vec, float w = 0) : x(vec.x), y(vec.y), z(vec.z), w(w) {}

inline vec3 vec4::toV3()
{
	return vec3(this->x, this->y, this->z);
}

//	operator methods
inline void vec4::operator+=(vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] += right[i];
}

inline void vec4::operator-=(vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] -= right[i];
}

inline void vec4::operator*=(vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] *= right[i];
}

inline void vec4::operator*=(float right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] *= right;
}

inline void vec4::operator/=(vec4 right)
{
	for (size_t i = 0; i < 4; i++)
	{
		data[i] /= right[i];
	}
}

inline void vec4::operator/=(float right)
{
	for (size_t i = 0; i < 4; i++)
	{
		data[i] /= right;
	}
}

inline float vec4::Dot(vec4 right)
{
	return x * right.x + y * right.y + z * right.z + w * right.w;
}

inline void vec4::Cross(vec4 right)
{
	x = y * right.z - z * right.y;
	y = z * right.x - x * right.z;
	z = x * right.y - y * right.x;
	// w stays the same
}

//	utilitiy methods
inline float &vec4::operator[](size_t index)
{
	return data[index];
}

inline float vec4::Length()
{
	return sqrtf(x * x + y * y + z * z);
}

inline float vec4::Length2()
{
	return x * x + y * y + z * z;
}

inline void vec4::Normalize()
{
	float length = Length();
	for (size_t i = 0; i < 4; i++)
		data[i] /= length;
}

//	operator functions
inline vec4 operator+(vec4 left, vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] += right[i];
	return left;
}

inline vec4 operator-(vec4 left, vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] -= right[i];
	return left;
}

inline vec4 operator*(vec4 left, vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] *= right[i];
	return left;
}

inline vec4 operator*(vec4 left, float right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] *= right;
	return left;
}

inline vec4 operator*(float left, vec4 right)
{
	for (size_t i = 0; i < 4; i++)
		right[i] *= left;
	return right;
}

inline float Dot(vec4 left, vec4 right)
{
	float temp = 0;
	for (size_t i = 0; i < 4; i++)
		temp += left[i] * right[i];
	return temp;
}

inline vec4 Cross(vec4 left, vec4 right)
{
	return vec4(
		left.y * right.z -
			left.z * right.y,

		left.z * right.x -
			left.x * right.z,

		left.x * right.y -
			left.y * right.x);
}

inline float Length(vec4 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

inline float Length2(vec4 vector)
{
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/// <summary>
/// takes a vec4 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline vec4 Normalize(vec4 vector)
{
	if (Length(vector) == 0)
		return vector;
	float length = Length(vector);
	for (size_t i = 0; i < 4; i++)
		vector[i] /= length;
	return vector;
}

#pragma endregion

#pragma region Matrix
/* 3. Matrix operations:
 *		Product, product with vector.
 *		Initialization functions for rotation matrices around x, y, z axis and an arbitrary vector.
 *		Transpose and inverse.
 */


// data[rows][columns]
struct mat4
{
	vec4 data[4];
	vec4 operator[](size_t index) const;
	vec4 &operator[](size_t index);

	inline mat4();
	inline mat4(vec4 v[4]);
	vec3 toV3();

	void Inverse();
	void Transpose();
};

mat4::mat4()
{
	for (size_t i = 0; i < 4; i++)
	{
		data[i] = vec4(0, 0, 0, 0);
	}
}

mat4::mat4(vec4 v[4])
{
	for (size_t i = 0; i < 4; i++)
		(*this)[i] = v[i];
}

inline vec3 mat4::toV3()
{
	vec3 temp;
	for (size_t i = 0; i < 3; i++)
	{
		temp[i] = (*this)[i][3];
	}
	return temp;
}

inline vec3 M4toV3(const mat4& matrix)
{
	vec3 temp;
	for (size_t i = 0; i < 3; i++)
	{
		temp[i] = (matrix)[i][3];
	}
	return temp;
}

inline vec4 mat4::operator[](size_t index) const
{
	return data[index];
}

inline vec4 &mat4::operator[](size_t index)
{
	return data[index];
}

inline mat4 operator*(const mat4& left, const mat4& right)
{
	mat4 temp;
	for (size_t i = 0; i < 64; i++)
		temp[(i / 16) % 4][(i / 4) % 4] += left[(i / 16) % 4][i % 4] * right[i % 4][(i / 4) % 4];
	return temp;
}

inline vec4 operator*(const mat4& left, vec4& right)
{
	vec4 temp;
	for (size_t i = 0; i < 16; i++)
		temp[(i / 4) % 4] += left[(i / 4) % 4][i % 4] * right[i % 4];

	return temp;
}

inline void mat4::Transpose()
{
	mat4 temp = *this;
	for (size_t i = 0; i < 16; i++)
	{
		if (i / 4 == i % 4)
			continue;
		(*this)[i % 4][i / 4] = temp[i / 4][i % 4];
	}
}

inline mat4 Transpose(const mat4& matrix)
{
	mat4 temp = matrix;
	for (size_t i = 0; i < 16; i++)
	{
		if (i / 4 == i % 4)
			continue;
		matrix[i / 4][i % 4] = temp[i / 4][i % 4];
	}
	return matrix;
}

/// <summary>
/// Inverse a 4-by-4 matrix.
/// </summary>
/// <param name="matrix">the matrix to inverse</param>
inline mat4 Inverse(mat4& matrix)
{
	// to hold the matrix values
	float m[16];

	// to hold the Inverse

	size_t k = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			m[k] = matrix[i][j];
			k++;
		}
	}
	float det;

	/*
	0:0	0:1	0:2	0:3
	1:0	1:1	1:2	1:3
	2:0	2:1	2:2	2:3
	3:0	3:1	3:2	3:3

	0 	1	2	3
	4	5	6	7
	8	9	10	11
	12	13	14	15
	*/

	// coloum 0/3
	matrix[0][0] =
		m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	matrix[1][0] =
		-m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	matrix[2][0] =
		m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	matrix[3][0] =
		-m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	// column 1/3
	matrix[0][1] =
		-m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	matrix[1][1] =
		m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	matrix[2][1] =
		-m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	matrix[3][1] =
		m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	// column 2/3
	matrix[0][2] =
		m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	matrix[1][2] =
		-m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	matrix[2][2] =
		m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	matrix[3][2] =
		-m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	// column 3/3
	matrix[0][3] =
		-m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	matrix[1][3] =
		m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	matrix[2][3] =
		-m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	matrix[3][3] =
		m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * matrix[0][0] + m[1] * matrix[1][0] + m[2] * matrix[2][0] + m[3] * matrix[3][0];

	if (det == 0)
		matrix = mat4();

	else
		for (size_t i = 0; i < 16; i++)
		{
			matrix[i / 4][i % 4] /= det;
		}
	return matrix;
}

inline void mat4::Inverse()
{
	// to hold the matrix values
	float m[16];

	size_t k = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			m[k] = (*this)[i][j];
			k++;
		}
	}
	float det;

	/*
	0:0	0:1	0:2	0:3
	1:0	1:1	1:2	1:3
	2:0	2:1	2:2	2:3
	3:0	3:1	3:2	3:3

	0 	1	2	3
	4	5	6	7
	8	9	10	11
	12	13	14	15
	*/

	// coloum 0/3
	(*this)[0][0] =
		m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	(*this)[1][0] =
		-m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	(*this)[2][0] =
		m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	(*this)[3][0] =
		-m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	// column 1/3
	(*this)[0][1] =
		-m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	(*this)[1][1] =
		m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	(*this)[2][1] =
		-m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	(*this)[3][1] =
		m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	// column 2/3
	(*this)[0][2] =
		m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	(*this)[1][2] =
		-m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	(*this)[2][2] =
		m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	(*this)[3][2] =
		-m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	// column 3/3
	(*this)[0][3] =
		-m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	(*this)[1][3] =
		m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	(*this)[2][3] =
		-m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	(*this)[3][3] =
		m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * (*this)[0][0] + m[1] * (*this)[1][0] + m[2] * (*this)[2][0] + m[3] * (*this)[3][0];

	if (det == 0)
		(*this) = mat4();

	else
		for (size_t i = 0; i < 16; i++)
		{
			(*this)[i / 4][i % 4] /= det;
		}
}

// Line with direction of line and rotation around axis by theta radians
inline mat4 Rotation(vec4 line, float theta)
{
	line.Normalize();
	mat4 temp;
	temp[3].w = 1;

	// v[] x y z w
	// [0] 0 1 2 0
	// [1] 3 4 5 0
	// [2] 6 7 8 0
	// [3] 0 0 0 1

	// [coloumns][rows]
	temp[0].x = cosf(theta) + (1 - cosf(theta)) * (line.x * line.x);		// 0
	temp[0].y = (1 - cosf(theta)) * line.x * line.y + line.z * sinf(theta); // 1
	temp[0].z = (1 - cosf(theta)) * line.x * line.z - line.y * sinf(theta); // 2

	temp[1].x = (1 - cosf(theta)) * line.x * line.y - line.z * sinf(theta); // 3
	temp[1].y = cosf(theta) + (1 - cosf(theta)) * (line.y * line.y);		// 4
	temp[1].z = (1 - cosf(theta)) * line.y * line.z + line.x * sinf(theta); // 5

	temp[2].x = (1 - cosf(theta)) * line.x * line.z + line.y * sinf(theta); // 6
	temp[2].y = (1 - cosf(theta)) * line.y * line.z - line.x * sinf(theta); // 7
	temp[2].z = cosf(theta) + (1 - cosf(theta)) * (line.z * line.z);		// 8

	return temp;
}

/// <summary>
/// returns the matrix to give the resulting product of a translation along the given vector
/// </summary>
/// <param name="pos">all 0s = identity matrix</param>
/// <returns></returns>
inline mat4 Translate(vec4 pos)
{
	mat4 temp;
	temp[0] = vec4(1, 0, 0, pos[0]);
	temp[1] = vec4(0, 1, 0, pos[1]);
	temp[2] = vec4(0, 0, 1, pos[2]);
	temp[3] = vec4(0, 0, 0, 1);
	return temp;
}

inline mat4 Scalar(float s)
{
	mat4 temp;
	temp[0] = vec4(s, 0, 0, 0);
	temp[1] = vec4(0, s, 0, 0);
	temp[2] = vec4(0, 0, s, 0);
	temp[3] = vec4(0, 0, 0, 1);
	return temp;
}

inline mat4 Scalar(vec4 v)
{
	mat4 temp;
	temp[0] = vec4(v[0], 0, 0, 0);
	temp[1] = vec4(0, v[1], 0, 0);
	temp[2] = vec4(0, 0, v[2], 0);
	temp[3] = vec4(0, 0, 0, 1);
	return temp;
}

/// <param name="fov:">field of view, in degrees</param>
/// <param name="aspect:">aspectRatio = width / heigth</param>
/// <param name="n:">nearplane</param>
/// <param name="f:">farplane</param>
inline mat4 projection(float fov, float aspect, float n, float f)
{
	mat4 temp;
	// solution
	float d = tanf(2 * M_PI - fov * (M_PI / 180) / 2);

	temp[0][0] = d / aspect;
	temp[1][1] = d;
	temp[2][2] = (f + n) / (n - f);
	temp[2][3] = 2 * f * n / (n - f);
	temp[3][2] = -1;

	return temp;
}

#pragma endregion // Matrix

#pragma region Quaternions

struct Quat
{
	float scalar;
	union
	{
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float data[4];
		};
	};

	inline Quat();
	inline Quat(float x, float y, float z, float w);

	// to Euler angles

	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
};

Quat::Quat()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Quat::Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{

}

#pragma endregion // Quaternions


#pragma region Plane


struct Plane
{
	vec3 normal;
	vec3 point;
	float MARGIN = 1.e-5f;
	
	inline Plane(vec3 point, vec3 normal);
	bool pointIsOnPlane(const vec3& point, float margin);
};

Plane::Plane(vec3 point, vec3 normal) : point(point), normal(normal)
{

}

inline bool Plane::pointIsOnPlane(const vec3& point, float margin)
{
	return Dot(normal, point - this->point) <= margin ||
	Dot(normal, point - this->point) >= -margin;
}

#pragma endregion // Plane

#pragma region Ray

struct Ray
{
	vec3 start;
	vec3 dir;
	inline Ray(vec3 start, vec3 dir);
	bool Intersect(vec3& res, const Plane plane);
};

Ray::Ray(vec3 start, vec3 dir) : start(start), dir(dir)
{

}

inline bool Ray::Intersect(vec3& res, const Plane plane)
{
    if (!Dot(plane.normal, dir)) return false;

    float d = Dot(plane.normal, start);
    float t = (d - Dot(plane.normal, start)) / Dot(plane.normal, dir);
    
    res = start + Normalize(dir) * t;
    return true;
}

#pragma endregion // Ray
