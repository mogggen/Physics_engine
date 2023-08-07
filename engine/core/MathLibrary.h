//#include "config.h"
#pragma once
#include <cmath>
#include <vector>
#include <stdio.h>
#ifndef M_PI
#define M_PI 3.141592553584
#endif


#pragma region Vector2
//	Vector operations: +, -, *, length, normalize

/// <summary>
/// V2 is a 2 dimensional vector with coordinates.
/// </summary>
struct V2
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

	inline V2();
	inline V2(float x, float y);

	void operator+=(V2 right);
	void operator-=(V2 right);
	void operator*=(V2 right);
	void operator*=(float right);
	void operator/=(V2 right);
	void operator/=(float right);
	float &operator[](size_t index);

	float Dot(V2 right);

	float Length();
	float Length2();
	void Normalize();
};

// Vector Constructors
V2::V2() : x(0), y(0) {}

V2::V2(float x, float y) : x(x), y(y) {}

//	operator methods
inline void V2::operator+=(V2 right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] += right[i];
}

inline void V2::operator-=(V2 right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] -= right[i];
}

inline void V2::operator*=(V2 right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] *= right[i];
}

inline void V2::operator*=(float right)
{
	for (size_t i = 0; i < 2; i++)
		data[i] *= right;
}

inline void V2::operator/=(V2 right)
{
	for (size_t i = 0; i < 2; i++)
	{
		data[i] /= right[i];
	}
}

inline void V2::operator/=(float right)
{
	for (size_t i = 0; i < 2; i++)
	{
		data[i] /= right;
	}
}

inline float V2::Dot(V2 right)
{
	return x * right.x + y * right.y;
}

//	utilitiy methods
inline float &V2::operator[](size_t index)
{
	return data[index];
}

inline float V2::Length()
{
	return sqrtf(x * x + y * y);
}

inline float V2::Length2()
{
	return x * x + y * y;
}
	

inline void V2::Normalize()
{
	if (Length() == 0)
		return;
	float length = Length();
	for (size_t i = 0; i < 2; i++)
		data[i] /= length;
}

// operator functions
inline V2 operator+(V2 left, V2 right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] += right[i];
	return left;
}

inline V2 operator-(V2 left, V2 right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] -= right[i];
	return left;
}

inline V2 operator*(V2 left, V2 right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] *= right[i];
	return left;
}

inline V2 operator*(V2 left, float right)
{
	for (size_t i = 0; i < 2; i++)
		left[i] *= right;
	return left;
}

inline V2 operator*(float left, V2 right)
{
	for (size_t i = 0; i < 2; i++)
		right[i] *= left;
	return right;
}

inline V2 operator/(V2 left, V2 right)
{
	for (size_t i = 0; i < 2; i++)
	{
		left[i] /= right[i];
	}
	return left;
}

inline V2 operator/(V2 left, float right)
{
	for (size_t i = 0; i < 2; i++)
	{
		left[i] /= right;
	}
	return left;
}

inline V2 operator/(float left, V2 right)
{
	for (size_t i = 0; i < 2; i++)
	{
		right[i] /= left;
	}
	return right;
}

inline float Dot(V2 left, V2 right)
{
	float temp = 0;
	for (size_t i = 0; i < 2; i++)
		temp += left[i] * right[i];
	return temp;
}

inline float Length(V2 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

inline float Length2(V2 vector)
{
	return vector.x * vector.x + vector.y * vector.y;
}

/// <summary>
/// takes a V2 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline V2 Normalize(V2 vector)
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
/// V3 is a 3 dimensional vector with coordinates. or a rgb-value
/// </summary>
struct V3
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

	inline V3();
	inline V3(float x, float y, float z);
	inline V3(V2, float z);

	void operator+=(V3 right);
	void operator-=(V3 right);
	void operator*=(V3 right);
	void operator*=(float right);
	void operator/=(V3 right);
	void operator/=(float right);
	float &operator[](size_t index);

	bool V3::operator==(V3 rhs);
	bool V3::operator!=(V3 rhs);

	float Dot(V3 right);
	void Cross(V3 right);

	float Length();
	float Length2();
	void Normalize();

	bool operator!=(V3 const& rhs) const {
		for (size_t i = 0; i < 3; ++i) {
			if (data[i] + FLT_EPSILON <= rhs.data[i] &&
				data[i] - FLT_EPSILON >= rhs.data[i]
				) return false;
		}
		return true;
	}

	bool operator==(V3 const& rhs) const {
		for (size_t i = 0; i < 3; ++i) {
			if (data[i] + FLT_EPSILON > rhs.data[i] &&
				data[i] - FLT_EPSILON < rhs.data[i]
				) return false;
		}
		return true;
	}

	bool operator<(V3 const& rhs) const {
		for (size_t i = 0; i < 3; ++i) {
			if (data[i] - FLT_EPSILON >= rhs.data[i])
				return false;
		}
		return true;
	}

	bool operator>(V3 const& rhs) const {
		for (size_t i = 0; i < 3; ++i) {
			if (data[i] + FLT_EPSILON <= rhs.data[i])
				return false;
		}
		return true;
	}

	bool operator<=(V3 const& rhs) const {
		for (size_t i = 0; i < 3; ++i) {
			if (data[i] + FLT_EPSILON > rhs.data[i])
				return false;
		}
		return true;
	}

	bool operator>=(V3 const& rhs) const {
		for (size_t i = 0; i < 3; ++i) {
			if (data[i] - FLT_EPSILON < rhs.data[i])
				return false;
		}
		return true;
	}

};

// Vector Constructors
V3::V3() : x(0), y(0), z(0) {}

V3::V3(float x, float y, float z) : x(x), y(y), z(z) {}

V3::V3(V2 vec, float z) : x(vec.x), y(vec.y), z(z) {}

//	operator methods
inline void V3::operator+=(V3 right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] += right[i];
}

inline void V3::operator-=(V3 right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] -= right[i];
}

inline void V3::operator*=(V3 right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] *= right[i];
}

inline void V3::operator*=(float right)
{
	for (size_t i = 0; i < 3; i++)
		data[i] *= right;
}

inline void V3::operator/=(V3 right)
{
	for (size_t i = 0; i < 3; i++)
	{
		data[i] /= right[i];
	}
}

inline void V3::operator/=(float right)
{
	for (size_t i = 0; i < 3; i++)
	{
		data[i] /= right;
	}
}

inline float V3::Dot(V3 right)
{
	return x * right.x + y * right.y + z * right.z;
}

inline void V3::Cross(V3 right)
{
	x = y * right.z - z * right.y;
	y = z * right.x - x * right.z;
	z = x * right.y - y * right.x;
}

//	utilitiy methods
inline float &V3::operator[](size_t index)
{
	return data[index];
}

inline float V3::Length()
{
	return sqrtf(x * x + y * y + z * z);
}

inline float V3::Length2()
{
	return x * x + y * y + z * z;
}

inline void V3::Normalize()
{
	float length = Length();
	if (length)
	for (size_t i = 0; i < 3; i++)
		data[i] /= length;
}

inline bool V3::operator==(V3 rhs)
{
	if ((isnan(x) &&
		isnan(y) &&
		isnan(z)) &&

		(isnan(rhs.x) &&
			isnan(rhs.y) &&
			isnan(rhs.z))
		) return true;
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

inline bool V3::operator!=(V3 rhs)
{
	return !operator==(rhs);
}

//	operator functions
inline V3 operator+(V3 left, V3 right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] += right[i];
	return left;
}

inline V3 operator-(V3 left, V3 right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] -= right[i];
	return left;
}

inline V3 operator*(V3 left, V3 right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] *= right[i];
	return left;
}

inline V3 operator*(V3 left, float right)
{
	for (size_t i = 0; i < 3; i++)
		left[i] *= right;
	return left;
}

inline V3 operator*(float left, V3 right)
{
	for (size_t i = 0; i < 3; i++)
		right[i] *= left;
	return right;
}

inline V3 operator/(V3 left, V3 right)
{
	for (size_t i = 0; i < 3; i++)
	{
		left[i] /= right[i];
	}
	return left;
}

inline V3 operator/(V3 left, float right)
{
	for (size_t i = 0; i < 3; i++)
	{
		left[i] /= right;
	}
	return left;
}

inline V3 operator/(float left, V3 right)
{
	for (size_t i = 0; i < 3; i++)
	{
		right[i] /= left;
	}
	return right;
}

inline float Dot(V3 left, V3 right)
{
	float temp = 0;
	for (size_t i = 0; i < 3; i++)
		temp += left[i] * right[i];
	return temp;
}

inline V3 Cross(V3 left, V3 right)
{
	return V3(
		left.y * right.z -
			left.z * right.y,

		left.z * right.x -
			left.x * right.z,

		left.x * right.y -
			left.y * right.x);
}

inline float Length(V3 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

inline float Length2(V3 vector)
{
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/// <summary>
/// takes a V3 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline V3 Normalize(V3 vector)
{
	float length = Length(vector);
	if (length)
		for (size_t i = 0; i < 3; i++)
			vector[i] /= length;
	return vector;
}

inline bool point_in_triangle_3D(
	const V3& p,
	const V3& a,
	const V3& b,
	const V3& c
)
{
	const V3 x1 = Cross(b - a, p - a);
	const V3 x2 = Cross(c - b, p - b);
	const V3 x3 = Cross(a - c, p - c);
	return
		(Normalize(x1) == Normalize(x2) && Normalize(x2) == Normalize(x3))
		&&
		(Dot(x1, x2) > 0.f && Dot(x2, x3) > 0.f && Dot(x3, x1) > 0.f);
}

// Calculate the signed volume of a tetrahedron formed by four points
inline const float signedVolume(const V3& a, const V3& b, const V3& c, const V3& d) {
	return (1.0 / 6.0) * (
		(d.x - a.x) * ((b.y - a.y) * (c.z - a.z) - (b.z - a.z) * (c.y - a.y)) +
		(d.y - a.y) * ((b.z - a.z) * (c.x - a.x) - (b.x - a.x) * (c.z - a.z)) +
		(d.z - a.z) * ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x))
		);
}

// Calculate barycentric coordinates of point P with respect to the tetrahedron defined by a, b, c, and d
inline void barycentricCoordinates(const V3& a, const V3& b, const V3& c, const V3& d, const V3& p,
	float& w0, float& w1, float& w2, float& w3) {
	// Calculate the signed volumes of four tetrahedra
	float volPABC = signedVolume(p, a, b, c);
	float volPABD = signedVolume(p, a, b, d);
	float volPACD = signedVolume(p, a, c, d);
	float volPBCD = signedVolume(p, b, c, d);

	// Calculate the sum of the absolute values of the signed volumes
	float sumVolumes = std::abs(volPABC) + std::abs(volPABD) + std::abs(volPACD) + std::abs(volPBCD);



	// Calculate the normalized barycentric coordinates
	w0 = std::abs(volPABD) / sumVolumes;
	w1 = std::abs(volPACD) / sumVolumes;
	w2 = std::abs(volPBCD) / sumVolumes;
	w3 = std::abs(volPABC) / sumVolumes;
}

inline const V3 supportFunction(const std::vector<V3>& shapeVertices, const V3& direction)
{
	V3 farthestPoint = shapeVertices[0];
	float maxDotProduct = farthestPoint.Dot(direction);

	// Loop through the remaining vertices to find the farthest point
	for (size_t i = 1; i < shapeVertices.size(); ++i)
	{
		const V3& currentPoint = shapeVertices[i];
		float dotProduct = Dot(currentPoint, direction);

		if (dotProduct > maxDotProduct)
		{
			maxDotProduct = dotProduct;
			farthestPoint = currentPoint;

		}
	}
	return farthestPoint;
}


#pragma endregion // Vector3

#pragma region Vector4
//	Vector operations: +, -, *, length, normalize, dot product, cross product

/// <summary>
/// V4 is a 4 dimensional vector with homogeneous coordinates, or a rgba-value.
/// </summary>
struct V4
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

	inline V4();
	inline V4(float x, float y, float z, float w);
	inline V4(V3 vec, float w);

	void operator+=(V4 right);
	void operator-=(V4 right);
	void operator*=(V4 right);
	void operator*=(float right);
	void operator/=(V4 right);
	void operator/=(float right);
	float &operator[](size_t index);

	float Dot(V4 right);
	void Cross(V4 right);

	float Length();
	float Length2();
	void Normalize();
	V3 toV3();
};

// Vector Constructors
V4::V4() : x(0), y(0), z(0), w(0) {}

V4::V4(float x, float y, float z, float w = 0) : x(x), y(y), z(z), w(w) {}

V4::V4(V3 vec, float w = 0) : x(vec.x), y(vec.y), z(vec.z), w(w) {}

inline V3 V4::toV3()
{
	return V3(this->x, this->y, this->z);
}

//	operator methods
inline void V4::operator+=(V4 right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] += right[i];
}

inline void V4::operator-=(V4 right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] -= right[i];
}

inline void V4::operator*=(V4 right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] *= right[i];
}

inline void V4::operator*=(float right)
{
	for (size_t i = 0; i < 4; i++)
		data[i] *= right;
}

inline void V4::operator/=(V4 right)
{
	for (size_t i = 0; i < 4; i++)
	{
		data[i] /= right[i];
	}
}

inline void V4::operator/=(float right)
{
	for (size_t i = 0; i < 4; i++)
	{
		data[i] /= right;
	}
}

inline float V4::Dot(V4 right)
{
	return x * right.x + y * right.y + z * right.z + w * right.w;
}

inline void V4::Cross(V4 right)
{
	x = y * right.z - z * right.y;
	y = z * right.x - x * right.z;
	z = x * right.y - y * right.x;
	// w stays the same
}

//	utilitiy methods
inline float &V4::operator[](size_t index)
{
	return data[index];
}

inline float V4::Length()
{
	return sqrtf(x * x + y * y + z * z);
}

inline float V4::Length2()
{
	return x * x + y * y + z * z;
}

inline void V4::Normalize()
{
	float length = Length();
	for (size_t i = 0; i < 4; i++)
		data[i] /= length;
}

//	operator functions
inline V4 operator+(V4 left, V4 right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] += right[i];
	return left;
}

inline V4 operator-(V4 left, V4 right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] -= right[i];
	return left;
}

inline V4 operator*(V4 left, V4 right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] *= right[i];
	return left;
}

inline V4 operator*(V4 left, float right)
{
	for (size_t i = 0; i < 4; i++)
		left[i] *= right;
	return left;
}

inline V4 operator*(float left, V4 right)
{
	for (size_t i = 0; i < 4; i++)
		right[i] *= left;
	return right;
}

inline float Dot(V4 left, V4 right)
{
	float temp = 0;
	for (size_t i = 0; i < 4; i++)
		temp += left[i] * right[i];
	return temp;
}

inline V4 Cross(V4 left, V4 right)
{
	return V4(
		left.y * right.z -
			left.z * right.y,

		left.z * right.x -
			left.x * right.z,

		left.x * right.y -
			left.y * right.x);
}

inline float Length(V4 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

inline float Length2(V4 vector)
{
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/// <summary>
/// takes a V4 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline V4 Normalize(V4 vector)
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
struct M4
{
	V4 data[4];
	V4 operator[](size_t index) const;
	V4 &operator[](size_t index);

	inline M4();
	inline M4(V4 v[4]);
	V3 toV3();

	void Transpose();
};

M4::M4()
{
	for (size_t i = 0; i < 4; i++)
	{
		data[i] = V4(0, 0, 0, 0);
	}
}

M4::M4(V4 v[4])
{
	for (size_t i = 0; i < 4; i++)
		(*this)[i] = v[i];
}

inline V3 M4::toV3()
{
	V3 temp;
	for (size_t i = 0; i < 3; i++)
	{
		temp[i] = (*this)[i][3];
	}
	return temp;
}

inline V3 M4toV3(const M4& matrix)
{
	V3 temp;
	for (size_t i = 0; i < 3; i++)
	{
		temp[i] = (matrix)[i][3];
	}
	return temp;
}

inline V4 M4::operator[](size_t index) const
{
	return data[index];
}

inline V4 &M4::operator[](size_t index)
{
	return data[index];
}

inline M4 operator*(M4 left, M4 right)
{
	M4 temp;
	for (size_t i = 0; i < 64; i++)
		temp[(i / 16) % 4][(i / 4) % 4] += left[(i / 16) % 4][i % 4] * right[i % 4][(i / 4) % 4];
	return temp;
}

inline V4 operator*(M4 left, V4 right)
{
	V4 temp;
	for (size_t i = 0; i < 16; i++)
		temp[(i / 4) % 4] += left[(i / 4) % 4][i % 4] * right[i % 4];

	return temp;
}

inline void M4::Transpose()
{
	M4 temp = *this;
	for (size_t i = 0; i < 16; i++)
	{
		if (i / 4 == i % 4)
			continue;
		(*this)[i % 4][i / 4] = temp[i / 4][i % 4];
	}
}

inline M4 Transpose(M4 matrix)
{
	M4 temp = matrix;
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
inline M4 Inverse(M4 matrix)
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
		matrix = M4();

	else
		for (size_t i = 0; i < 16; i++)
		{
			matrix[i / 4][i % 4] /= det;
		}
	return matrix;
}

inline void Inverse(M4 &matrix)
{
	// to hold the matrix values
	float m[16];

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
		matrix = M4();

	else
		for (size_t i = 0; i < 16; i++)
		{
			matrix[i / 4][i % 4] /= det;
		}
}

// Line with direction of line and rotation around axis by theta radians
inline M4 Rotation(V4 line, float theta)
{
	line.Normalize();
	M4 temp;
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
inline M4 Translate(V4 pos)
{
	M4 temp;
	temp[0] = V4(1, 0, 0, pos[0]);
	temp[1] = V4(0, 1, 0, pos[1]);
	temp[2] = V4(0, 0, 1, pos[2]);
	temp[3] = V4(0, 0, 0, 1);
	return temp;
}

inline M4 Scalar(float s)
{
	M4 temp;
	temp[0] = V4(s, 0, 0, 0);
	temp[1] = V4(0, s, 0, 0);
	temp[2] = V4(0, 0, s, 0);
	temp[3] = V4(0, 0, 0, 1);
	return temp;
}

inline M4 Scalar(V4 v)
{
	M4 temp;
	temp[0] = V4(v[0], 0, 0, 0);
	temp[1] = V4(0, v[1], 0, 0);
	temp[2] = V4(0, 0, v[2], 0);
	temp[3] = V4(0, 0, 0, 1);
	return temp;
}

/// <param name="fov:">field of view, in degrees</param>
/// <param name="aspect:">aspectRatio = width / heigth</param>
/// <param name="n:">nearplane</param>
/// <param name="f:">farplane</param>
inline M4 projection(float fov, float aspect, float n, float f)
{
	M4 temp;
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

class Quaternion
{
private:
	float w, x, y, z; // Components of the quaternion: w + xi + yj + zk

public:
	// Constructor
	Quaternion(float w = 1.0, float x = 0.0, float y = 0.0, float z = 0.0)
		: w(w), x(x), y(y), z(z) {}

	// Getters for components
	float getW() const { return w; }
	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }

	// Quaternion operations
	float norm() const
	{
		return std::sqrt(w * w + x * x + y * y + z * z);
	}

	Quaternion normalized() const
	{
		float magnitude = norm();
		return Quaternion(w / magnitude, x / magnitude, y / magnitude, z / magnitude);
	}

	Quaternion conjugate() const
	{
		return Quaternion(w, -x, -y, -z);
	}


	// Quaternion arithmetic
	Quaternion operator+(const Quaternion& other) const
	{
		return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
	}

	Quaternion operator-(const Quaternion& other) const
	{
		return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
	}

	Quaternion operator*(const Quaternion& other) const
	{
		double newW = w * other.w - x * other.x - y * other.y - z * other.z;
		double newX = w * other.x + x * other.w + y * other.z - z * other.y;
		double newY = w * other.y - x * other.z + y * other.w + z * other.x;
		double newZ = w * other.z + x * other.y - y * other.x + z * other.w;
		return Quaternion(newW, newX, newY, newZ);
	}

	// Rotate a 3D vector using the quaternion
	// The input vector must be a 3D vector represented as a Quaternion with w = 0
	Quaternion rotate(const Quaternion& vector) const
	{
		Quaternion conjugateQ = this->conjugate();
		return (*this) * vector * conjugateQ;
	}

	Quaternion QuaternionLookAt(const V3& from, const V3& to)
	{
		// Calculate the direction vector from the 'from' position to the 'to' position
		V3 direction = Normalize(to - from);

		// Create a rotation Quaternion that rotates the 'forward' vector (0, 0, 1) to the 'direction' vector
		V3 forward(0.0, 0.0, 1.0);
		V3 axis = Normalize(Cross(forward, direction));
		float angle = std::acos(Dot(forward, direction));
		Quaternion rotation(std::cos(angle / 2.0), axis[0] * std::sin(angle / 2.0),
			axis[1] * std::sin(angle / 2.0), axis[2] * std::sin(angle / 2.0));

		return rotation;
	}
};

// If the quaternion is not normalized, make sure to normalize it before converting it to a rotation matrix.

#pragma endregion // Quaternions


#pragma region Plane


struct Plane
{
	V3 normal;
	V3 point;
	
	inline Plane(V3 point, V3 normal);
	bool pointIsOnPlane(const V3& point, float epsilon=FLT_EPSILON);
};

Plane::Plane(V3 point, V3 normal) : point(point), normal(normal)
{

}

inline bool Plane::pointIsOnPlane(const V3& point, float epsion)
{
	float res = Dot(normal, point - this->point);
	return res <= epsion || res >= -epsion;
}

#pragma endregion // Plane

#pragma region Ray

struct Ray
{
#define NAN_V3 V3(NAN, NAN, NAN)
	V3 origin;
	V3 dir;
	Ray(V3 origin, V3 dir);
	const V3 Ray::minDist(const std::vector<V3>& others);
	const V3 intersect(const Plane& plane, float& epsilon);
};

inline Ray::Ray(V3 origin, V3 dir) : origin(origin), dir(dir)
{

}

inline const V3 Ray::intersect(const Plane& plane, float& epsilon)
{
	float dotProduct = Dot(plane.normal, dir);

	if (!dotProduct) {
		epsilon = 1337;
		return NAN_V3;
	}

	float t = Dot(plane.point - origin, plane.normal) / dotProduct;

	//if (t < 0) return NAN_V3;

	return origin + t * dir;
}

inline const V3 Ray::minDist(const std::vector<V3>& others)
{
	int min_index = -1;
	float min = FLT_MAX;
	for (int i = 0; i < others.size(); i++)
	{
		float length_squared = (origin - others[i]).Length2();
		if (length_squared < min)
		{
			min = length_squared;
			min_index = i;
		}
	}

	if (min_index < 0)
	{
		return NAN_V3;
	}

	return others[min_index];
}

#pragma endregion // Ray
