//#include "config.h"
#ifndef MathLib
#define Mathlib
#endif

#define m_pi 3.141592741f

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
		float data[3];
	};

	V3();
	V3(float x, float y, float z);

	void operator+=(V3 right);
	void operator-=(V3 right);
	void operator*=(V3 right);
	void operator*=(float right);
	void operator/=(V3 right);
	void operator/=(float right);
	float& operator[](char index);

	float Dot(V3 right);
	void Cross(V3 right);

	float Length();	
	void Normalize();
};

//Vector Constructors
V3::V3() : x(0), y(0), z(0) { }

V3::V3(float x, float y, float z) : x(x), y(y), z(z) { }


//	operator methods
inline void V3::operator+=(V3 right)
{
	for (char i = 0; i < 3; i++)
		data[i] += right[i];
}

inline void V3::operator-=(V3 right)
{
	for (char i = 0; i < 3; i++)
		data[i] -= right[i];
}

inline void V3::operator*=(V3 right)
{
	for (char i = 0; i < 3; i++)
		data[i] *= right[i];
}

inline void V3::operator*=(float right)
{
	for (char i = 0; i < 3; i++)
		data[i] *= right;
}

inline void V3::operator/=(V3 right)
{
	for (char i = 0; i < 3; i++)
	{
		data[i] /= right[i];
	}
}

inline void V3::operator/=(float right)
{
	for (char i = 0; i < 3; i++)
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
inline float& V3::operator[](char index)
{
	return data[index];
}

inline float V3::Length()
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

inline void V3::Normalize()
{
	float length = Length();
	for (char i = 0; i < 3; i++)
		data[i] /= length;
}

//	operator functions
inline V3 operator+(V3 left, V3 right)
{
	for (char i = 0; i < 3; i++)
		left[i] += right[i];
	return left;
}

inline V3 operator-(V3 left, V3 right)
{
	for (char i = 0; i < 3; i++)
		left[i] -= right[i];
	return left;
}

inline V3 operator*(V3 left, V3 right)
{
	for (char i = 0; i < 3; i++)
		left[i] *= right[i];
	return left;
}

inline V3 operator*(V3 left, float right)
{
	for (char i = 0; i < 3; i++)
		left[i] *= right;
	return left;
}

inline V3 operator*(float left, V3 right)
{
	for (char i = 0; i < 3; i++)
		right[i] *= left;
	return right;
}

inline V3 operator/(V3 left, V3 right)
{
	for (char i = 0; i < 3; i++)
	{
		left[i] /= right[i];
	}
}

inline V3 operator/(V3 left, float right)
{
	for (char i = 0; i < 3; i++)
	{
		left[i] /= right;
	}
}

inline V3 operator/(float left, V3 right)
{
	for (char i = 0; i < 3; i++)
	{
		right[i] /= left;
	}
}

inline float Dot(V3 left, V3 right)
{
	float temp = 0;
	for (char i = 0; i < 3; i++)
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
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

/// <summary>
/// takes a V3 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline V3 Normalize(V3 vector)
{
	float length = Length(vector);
	for (char i = 0; i < 3; i++)
		vector[i] /= length;
	return vector;
}

#pragma endregion

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
	
	V4();
	V4(float x, float y, float z, float w);

	void operator+=(V4 right);
	void operator-=(V4 right);
	void operator*=(V4 right);
	void operator*=(float right);
	void operator/=(V4 right);
	void operator/=(float right);
	float& operator[](char index);

	float Dot(V4 right);
	void Cross(V4 right);

	float Length();
	void Normalize();
};

//Vector Constructors
V4::V4() : x(0), y(0), z(0), w(0) { }

V4::V4(float x, float y, float z, float w=0) : x(x), y(y), z(z), w(w) { }


//	operator methods
inline void V4::operator+=(V4 right)
{
	for (char i = 0; i < 4; i++)
		data[i] += right[i];
}

inline void V4::operator-=(V4 right)
{
	for (char i = 0; i < 4; i++)
		data[i] -= right[i];
}

inline void V4::operator*=(V4 right)
{
	for (char i = 0; i < 4; i++)
		data[i] *= right[i];
}

inline void V4::operator*=(float right)
{
	for (char i = 0; i < 4; i++)
		data[i] *= right;
}

inline void V4::operator/=(V4 right)
{
	for (char i = 0; i < 4; i++)
	{
		data[i] /= right[i];
	}
}

inline void V4::operator/=(float right)
{
	for (char i = 0; i < 4; i++)
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
inline float& V4::operator[](char index)
{
	return data[index];
}

inline float V4::Length()
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
}

inline void V4::Normalize()
{
	float length = Length();
	for (char i = 0; i < 4; i++)
		data[i] /= length;
}


//	operator functions
inline V4 operator+(V4 left, V4 right)
{
	for (char i = 0; i < 4; i++)
		left[i] += right[i];
	return left;
}

inline V4 operator-(V4 left, V4 right)
{
	for (char i = 0; i < 4; i++)
		left[i] -= right[i];
	return left;
}

inline V4 operator*(V4 left, V4 right)
{
	for (char i = 0; i < 4; i++)
		left[i] *= right[i];
	return left;
}

inline V4 operator*(V4 left, float right)
{
	for (char i = 0; i < 4; i++)
		left[i] *= right;
	return left;
}

inline V4 operator*(float left, V4 right)
{
	for (char i = 0; i < 4; i++)
		right[i] *= left;
	return right;
}

inline float Dot(V4 left, V4 right)
{
	float temp = 0;
	for (char i = 0; i < 4; i++)
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
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

/// <summary>
/// takes a V4 and normalize it, giving it a length of one.
/// </summary>
/// <param name="vector">the vector to normalize.</param>
/// <returns></returns>
inline V4 Normalize(V4 vector)
{
	float length = Length(vector);
	for (char i = 0; i < 4; i++)
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

struct M4
{
	V4 vectors[4];
	V4 operator[](char index) const;
	V4& operator[](char index);

	M4();
	M4(V4 v[4]);

	void Transpose();
	
};

M4::M4()
{
	for (char i = 0; i < 3; i++)
		vectors[i] = V4(0, 0, 0);
	vectors[3] = V4();
}

M4::M4(V4 v[4])
{
	for (char i = 0; i < 4; i++)
		(*this)[i] = v[i];
}

inline V4 M4::operator[](char index) const
{
	return vectors[index];
}

inline V4& M4::operator[](char index)
{
	return vectors[index];
}

inline M4 operator*(M4 left, M4 right)
{
	M4 temp;
	for (char i = 0; i < 64; i++)
		temp[(i / 16) % 4][(i / 4) % 4] += left[(i / 16) % 4][i % 4] * right[i % 4][(i / 4) % 4];
	return temp;
}

inline V4 operator*(M4 left, V4 right)
{
	V4 temp;
	for (char i = 0; i < 16; i++)
		temp[(i / 4) % 4] += left[(i / 4) % 4][i % 4] * right[i % 4];

	return temp;
}

inline void M4::Transpose()
{
	M4 temp;
	for (char i = 0; i < 16; i++)
	{
		temp[i / 4][i % 4] = temp[i % 4][i / 4];
	}
	for (char i = 0; i < 16; i++)
	{
		temp[i % 4][i / 4] = temp[i % 4][i / 4];
	}
		*this = temp;
}

inline M4 Transpose(M4 matrix)
{
	M4 temp;
	for (char i = 0; i < 16; i++)
	{
		temp[i / 4][i % 4] = matrix[i % 4][i / 4];
	}
	return temp;
}

/// <summary>
/// Inverse a 4-by-4 matrix.
/// </summary>
/// <param name="matrix">the matrix to inverse</param>
inline void Inverse(M4& matrix)
{
	//to hold the matrix values
	float m[16];

	// to hold the Inverse
	matrix;
	char k = 0;
	for (char i = 0; i < 4; i++)
	{
		for (char j = 0; j < 4; j++)
		{
			m[k] = matrix[i][j] = matrix[i][j];
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

	//coloum 0/3
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


	//column 1/3
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


	//column 2/3
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


	//column 3/3
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
		for (char i = 0; i < 16; i++)
		{
			matrix[i / 4][i % 4] = matrix[i / 4][i % 4] / det;
		}
}


//line with direction of dir and rotation around axis by theta radians
inline M4 Rotation(V4 dir, float θ)
{
	M4 temp;
		temp[3].w = 1;

	// v[] x y z w
	// [0] 0 1 2 0
	// [1] 3 4 5 0
	// [2] 6 7 8 0
	// [3] 0 0 0 1

	// [coloumns][rows]
	temp[0].x = cos(θ) + (1 - cos(θ)) * pow(dir.x, 2); // 0
	temp[0].y = (1 - cos(θ)) * dir.x * dir.y + dir.z * sin(θ); // 1
	temp[0].z = (1 - cos(θ)) * dir.x * dir.z - dir.y * sin(θ); // 2

	temp[1].x = (1 - cos(θ)) * dir.x * dir.y - dir.z * sin(θ); // 3
	temp[1].y = cos(θ) + (1 - cos(θ)) * pow(dir.y, 2); // 4
	temp[1].z = (1 - cos(θ)) * dir.y * dir.z + dir.x * sin(θ); // 5

	temp[2].x = (1 - cos(θ)) * dir.x * dir.z + dir.y * sin(θ); // 6
	temp[2].y = (1 - cos(θ)) * dir.y * dir.z - dir.x * sin(θ); // 7
	temp[2].z = cos(θ) + (1 - cos(θ)) * pow(dir.z, 2); // 8

	return temp;
}
#pragma endregion
