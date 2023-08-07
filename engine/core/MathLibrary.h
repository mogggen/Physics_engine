//#include "config.h"
#pragma once
#include <cmath>
#include <vector>
#include <initializer_list>
#include <iostream>
#include <functional>
#ifndef M_PI
#define M_PI 3.141592553584
#endif


template <typename T, size_t N>
struct Vector {
    T data[N];

    Vector() {
		static_assert(N > 0);
        for (size_t i = 0; i < N; i++) {
            data[i] = T();
        }
    }

    Vector(const T* values) {
		static_assert(N > 0);
        for (size_t i = 0; i < N; i++) {
            data[i] = values[i];
        }
    }

	Vector(std::initializer_list<T> args) {
		if (args.size() != N) {
			throw std::invalid_argument("Wrong number of arguments.");
		}
		size_t i = 0;
		for (const auto& arg : args) {
			data[i++] = arg;
		}
	}

	template <typename... Args, typename = std::enable_if_t<std::is_same_v<T, float>>>
	Vector(Args... args) {
		static_assert(sizeof...(Args) == N, "Wrong number of arguments.");
		setValues(args...);
	}

	template <typename... Args>
	void setValues(float arg, Args... args) {
		data[N - sizeof...(Args) - 1] = arg;
		setValues(args...);
	}

	void setValues() {}

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

	template <size_t M>
    auto operator+(Vector<T, M> const& rhs) const {
                using ResultType = typename std::conditional<(N < M), Vector<T, M>, Vector<T, N>>::type;
        ResultType result;
        for (size_t i = 0; i < std::min(N, M); ++i) {
			
			result.data[i] = data[i] + rhs.data[i];
		}
        if (N >= M) {
            for (size_t i = M; i < N; ++i) {
                result.data[i] = data[i]; // Copy remaining components
            }
        } else {
            for (size_t i = N; i < M; ++i) {
                result.data[i] = rhs.data[i]; // Copy remaining components
            }
        }
        return result;

    }
 
	template <size_t M>
	auto operator-(Vector<T, M> const& rhs) const {
		        using ResultType = typename std::conditional<(N < M), Vector<T, M>, Vector<T, N>>::type;
        ResultType result;
        for (size_t i = 0; i < std::min(N, M); ++i) {
			
			result.data[i] = data[i] - rhs.data[i];
		}
        if (N >= M) {
            for (size_t i = M; i < N; ++i) {
                result.data[i] = data[i]; // Copy remaining components
            }
        } else {
            for (size_t i = N; i < M; ++i) {
                result.data[i] = rhs.data[i]; // Copy remaining components
            }
        }
        return result;

    }

	template <size_t M>
	auto operator*(Vector<T, M> const& rhs) const {
		        using ResultType = typename std::conditional<(N < M), Vector<T, M>, Vector<T, N>>::type;
        ResultType result;
        for (size_t i = 0; i < std::min(N, M); ++i) {
			
			result.data[i] = data[i] * rhs.data[i];
		}
        if (N >= M) {
            for (size_t i = M; i < N; ++i) {
                result.data[i] = data[i]; // Copy remaining components
            }
        } else {
            for (size_t i = N; i < M; ++i) {
                result.data[i] = rhs.data[i]; // Copy remaining components
            }
        }
        return result;
    }

	Vector<T, N> operator*(float const& rhs) const {
		Vector<T, N> result;
		for (size_t i = 0; i < N; i++) {
			result = data[i] * rhs;
		}
		return result;
	}

	template <size_t M>
	auto operator/(Vector<T, M> const& rhs) const {
		        using ResultType = typename std::conditional<(N < M), Vector<T, M>, Vector<T, N>>::type;
        ResultType result;
        for (size_t i = 0; i < std::min(N, M); ++i) {
			if (rhs.data[i] != 0.f)
			result.data[i] = data[i] / rhs.data[i];
		}
        if (N >= M) {
            for (size_t i = M; i < N; ++i) {
                result.data[i] = data[i]; // Copy remaining components
            }
        } else {
            for (size_t i = N; i < M; ++i) {
                result.data[i] = rhs.data[i]; // Copy remaining components
            }
        }
        return result;

    }

	template <size_t M>
	bool operator!=(Vector<T, M> const& rhs) const {
        for (size_t i = 0; i < std::min(N, M); ++i) {
			if (data[i] + FLT_EPSILON <= rhs.data[i] &&
				data[i] - FLT_EPSILON >= rhs.data[i]
				) return false;
		}
        return true;
    }

	template <size_t M>
	bool operator==(Vector<T, M> const& rhs) const {
		for (size_t i = 0; i < std::min(N, M); ++i) {
			if (data[i] + FLT_EPSILON > rhs.data[i] &&
				data[i] - FLT_EPSILON < rhs.data[i]
				) return false;
		}
        return true;
    }

	template <size_t M>
	bool operator<(Vector<T, M> const& rhs) const {
		for (size_t i = 0; i < std::min(N, M); ++i) {
			if (data[i] - FLT_EPSILON >= rhs.data[i])
				return false;
		}
		return true;
    }

	template <size_t M>
	bool operator>(Vector<T, M> const& rhs) const {
		for (size_t i = 0; i < std::min(N, M); ++i) {
			if (data[i] + FLT_EPSILON <= rhs.data[i])
				return false;
		}
        return true;
    }

	template <size_t M>
	bool operator<=(Vector<T, M> const& rhs) const {
		for (size_t i = 0; i < std::min(N, M); ++i) {
			if (data[i] + FLT_EPSILON > rhs.data[i])
				return false;
		}
        return true;
    }

	template <size_t M>
	bool operator>=(Vector<T, M> const& rhs) const {
		for (size_t i = 0; i < std::min(N, M); ++i) {
			if (data[i] - FLT_EPSILON < rhs.data[i])
				return false;
		}
        return true;
    }

    T dot(const Vector<T, N>& other) const {
        T result = 0;
        for (size_t i = 0; i < N; i++) {
            result += data[i] * other[i];
        }
        return result;
    }

    Vector<T, 3> cross(const Vector<T, 3>& other) const {
        Vector<T, 3> result;
        result[0] = data[1] * other[2] - data[2] * other[1];
        result[1] = data[2] * other[0] - data[0] * other[2];
        result[2] = data[0] * other[1] - data[1] * other[0];
        return result;
    }

    Vector<T, N> normalize() const {
        T magnitude = length();
		if (magnitude == 0.f || magnitude == 1.f)
		return *this;
        Vector<T, N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = data[i] / magnitude;
        }
        return result;
    }

    T length() const {
        return std::sqrtf(length_2());
    }

	T length_2() const {
		T sum = 0;
        for (size_t i = 0; i < N; i++) {
            sum += data[i] * data[i];
        }
        return sum;
	}

	bool lib_isnan() const {
		bool sum = false;
		for (size_t i = 0; i < N || sum; i++)
		{
			sum = std::_Is_nan(data[i]);
		}
		return sum;
	}

	bool point_in_triangle_3D(
		const Vector<T, 3>& p,
		const Vector<T, 3>& a,
		const Vector<T, 3>& b,
		const Vector<T, 3>& c
	)
	{
		const Vector<T, 3> x1 = (b - a).cross(p - a);
		const Vector<T, 3> x2 = (c - b).cross(p - b);
		const Vector<T, 3> x3 = (a - c).cross(p - c);
		return 
		(x1.normalize() == x2.normalize() && x2.normalize() == x3.normalize())
		&&
		(x1.dot(x2) > 0.f && x2.dot(x3) > 0.f && x3.dot(x1) > 0.f)
	}

	// Calculate the signed volume of a tetrahedron formed by four points
	float signedVolume(const Vector<T, 3>& A, const Vector<T, 3>& B, const Vector<T, 3>& C, const Vector<T, 3>& D) {
		return (1.0 / 6.0) * (
			(D.x - A.x) * ((B.y - A.y) * (C.z - A.z) - (B.z - A.z) * (C.y - A.y)) +
			(D.y - A.y) * ((B.z - A.z) * (C.x - A.x) - (B.x - A.x) * (C.z - A.z)) +
			(D.z - A.z) * ((B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x))
		);
	}

	// Calculate barycentric coordinates of point P with respect to the tetrahedron defined by A, B, C, and D
	void barycentricCoordinates(const Vector<T, 3>& A, const Vector<T, 3>& B, const Vector<T, 3>& C, const Vector<T, 3>& D, const Vector<T, 3>& P,
								float& w0, float& w1, float& w2, float& w3) {
		// Calculate the signed volumes of four tetrahedra
		float volPABC = signedVolume(P, A, B, C);
		float volPABD = signedVolume(P, A, B, D);
		float volPACD = signedVolume(P, A, C, D);
		float volPBCD = signedVolume(P, B, C, D);

		// Calculate the sum of the absolute values of the signed volumes
		float sumVolumes = std::abs(volPABC) + std::abs(volPABD) + std::abs(volPACD) + std::abs(volPBCD);

		// Calculate the normalized barycentric coordinates
		w0 = std::abs(volPABD) / sumVolumes;
		w1 = std::abs(volPACD) / sumVolumes;
		w2 = std::abs(volPBCD) / sumVolumes;
		w3 = std::abs(volPABC) / sumVolumes;
	}

	Vector<T, 3> supportFunction(const std::vector<Vector<T, 3>>& shapeVertices, const Vector<T, 3>& direction)
	{
		Vector<T, 3> farthestPoint = shapeVertices[0];
		float maxDotProduct = farthestPoint.dot(direction);

		// Loop through the remaining vertices to find the farthest point
		for (size_t i = 1; i < shapeVertices.size(); ++i)
		{
			const Vector<T, 3>& currentPoint = shapeVertices[i];
			float dotProduct = currentPoint.dot(direction);

			if (dotProduct > maxDotProduct)
			{
				maxDotProduct = dotProduct;
				farthestPoint = currentPoint;
			}
		}

		return farthestPoint;
	}
};




typedef Vector<float, 2> V2;
typedef Vector<float, 3> V3;
typedef Vector<float, 4> V4;


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
		data[i] = V4{ 0, 0, 0, 0 };
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

inline V3 M4toV3(const M4 &matrix)
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
	line.normalize();
	M4 temp;
	temp[3][3] = 1;

	// row_major
	// v[] x y z w
	// [0] 0 1 2 0
	// [1] 3 4 5 0
	// [2] 6 7 8 0
	// [3] 0 0 0 1

	// [coloumns][rows]
	temp[0][0] = cosf(theta) + (1 - cosf(theta)) * (line[0] * line[0]);		// 0
	temp[0][1] = (1 - cosf(theta)) * line[0] * line[1] + line[2] * sinf(theta); // 1
	temp[0][2] = (1 - cosf(theta)) * line[0] * line[2] - line[1] * sinf(theta); // 2

	temp[1][0] = (1 - cosf(theta)) * line[0] * line[1] - line[2] * sinf(theta); // 3
	temp[1][1] = cosf(theta) + (1 - cosf(theta)) * (line[1] * line[1]);		// 4
	temp[1][2] = (1 - cosf(theta)) * line[1] * line[2] + line[0] * sinf(theta); // 5

	temp[2][0] = (1 - cosf(theta)) * line[0] * line[2] + line[1] * sinf(theta); // 6
	temp[2][1] = (1 - cosf(theta)) * line[1] * line[2] - line[0] * sinf(theta); // 7
	temp[2][2] = cosf(theta) + (1 - cosf(theta)) * (line[2] * line[2]);		// 8

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
	temp[0] = V4{ 1, 0, 0, pos[0] };
	temp[1] = V4{0, 1, 0, pos[1]};
	temp[2] = V4{0, 0, 1, pos[2]};
	temp[3] = V4{0, 0, 0, 1};
	return temp;
}

inline M4 Scalar(float s)
{
	M4 temp;
	temp[0] = V4{s, 0, 0, 0};
	temp[1] = V4{0, s, 0, 0};
	temp[2] = V4{0, 0, s, 0};
	temp[3] = V4{0, 0, 0, 1};
	return temp;
}

inline M4 Scalar(V4 v)
{
	M4 temp;
	temp[0] = V4{v[0], 0, 0, 0};
	temp[1] = V4{0, v[1], 0, 0};
	temp[2] = V4{0, 0, v[2], 0};
	temp[3] = V4{0, 0, 0, 1};
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

#pragma region Quaternions

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
	Quaternion operator+(const Quaternion &other) const
	{
		return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
	}

	Quaternion operator-(const Quaternion &other) const
	{
		return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
	}

	Quaternion operator*(const Quaternion &other) const
	{
		double newW = w * other.w - x * other.x - y * other.y - z * other.z;
		double newX = w * other.x + x * other.w + y * other.z - z * other.y;
		double newY = w * other.y - x * other.z + y * other.w + z * other.x;
		double newZ = w * other.z + x * other.y - y * other.x + z * other.w;
		return Quaternion(newW, newX, newY, newZ);
	}

	// Rotate a 3D vector using the quaternion
	// The input vector must be a 3D vector represented as a Quaternion with w = 0
	Quaternion rotate(const Quaternion &vector) const
	{
		Quaternion conjugateQ = this->conjugate();
		return (*this) * vector * conjugateQ;
	}

	Quaternion QuaternionLookAt(const V3& from, const V3& to)
	{
		// Calculate the direction vector from the 'from' position to the 'to' position
		V3 direction = (to - from).normalize();

		// Create a rotation Quaternion that rotates the 'forward' vector (0, 0, 1) to the 'direction' vector
		V3 forward(0.0, 0.0, 1.0);
		V3 axis = forward.cross(direction).normalize();
		float angle = std::acos(forward.dot(direction));
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
	float MARGIN = 1.e-5f;

	inline Plane(V3 point, V3 normal);
	bool pointIsOnPlane(const V3 &point, float margin);
};

Plane::Plane(V3 point, V3 normal) : point(point), normal(normal)
{
}

inline bool Plane::pointIsOnPlane(const V3 &point, float margin)
{
	float res = normal.dot(point - this->point);
	return res <= margin || res >= -margin;
}

#pragma endregion // Plane


#pragma region Ray

struct Ray
{
#define NAN_V3 V3{NAN, NAN, NAN}
	V3 origin;
	V3 dir;
	Ray(V3 origin, V3 dir);
	void Ray::minDist(V3& res, const std::vector<V3> &others);
	void intersect(V3& res, const Plane &plane, const float &epsilon);
};

inline Ray::Ray(V3 origin, V3 dir) : origin(origin), dir(dir)
{
}

inline void Ray::intersect(V3& res, const Plane &plane, const float &epsilon = 1e-10f)
{
	float dotProduct = plane.normal.dot(dir);

	if (!dotProduct)
	{
		return;
	}

	float t = plane.normal.dot(plane.point - origin) / dotProduct;

	if (t < 0.f)
	{
		std::cout << "backwards Ray intersection!" << std::endl;
		return;
	}

	res = origin + dir * Vector<float, 1>(t);
}

inline void Ray::minDist(V3& res, const std::vector<V3> &others)
{
	int min_index = -1;
	float min = FLT_MAX;
	for (int i = 0; i < others.size(); i++)
	{
		float length_squared = (origin - others[i]).length_2();
		if (length_squared < min)
		{
			min = length_squared;
			min_index = i;
		}
	}

	if (min_index < 0)
	{
		return;
	}

	res = others[min_index];
}

#pragma endregion // Ray
