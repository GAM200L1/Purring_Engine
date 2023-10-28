/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MathCustom.cpp
 \date     30-08-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu
 \par      code %:     100%
 
 \co-author            Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 \par      code %:     0%
 \par      changes:    15-09-2023:
					   Updated file header
 
 \brief 	This file contains definitions for class member functions of structs vec2, 
			vec3, mat3x3 and mat4x4
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "MathCustom.h"

namespace PE
{
	/*																				   vec2 struct member implementations
	--------------------------------------------------------------------------------------------------------------------- */
	// copy assignment
	vec2& vec2::operator=(vec2 const& r_cpy)
	{
		x = r_cpy.x;
		y = r_cpy.y;
		return *this;
	}

	// get operator
	float vec2::operator[](unsigned int index) const
	{
		//if (index > 1)
			// print error
		return (index) ? y : x; // index 1 for y, index 0 for x
	}

	float& vec2::operator[](unsigned int index)
	{
		//return (*this)[index];
		return (index) ? y : x; // index 1 for y, index 0 for x
	}

	// addition
	vec2 vec2::operator+(vec2 const& r_rhs) const
	{
		return vec2{ x + r_rhs.x,
					 y + r_rhs.y };
	}
	vec2& vec2::operator+=(vec2 const& r_rhs)
	{
		x += r_rhs.x;
		y += r_rhs.y;
		return *this;
	}

	// subtraction
	vec2 vec2::operator-(vec2 const& r_rhs) const
	{
		return vec2{ x - r_rhs.x,
					 y - r_rhs.y };
	}
	vec2& vec2::operator-=(vec2 const& r_rhs)
	{
		x -= r_rhs.x;
		y -= r_rhs.y;
		return *this;
	}

	// scale
	vec2 vec2::operator*(float scale) const
	{
		return vec2{ x * scale,
					 y * scale };
	}

	vec2 vec2::operator/(float scale) const
	{
		return vec2{ x / scale,
					 y / scale };
	}

	vec2& vec2::operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}
	vec2& vec2::operator/=(float scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}

	vec2 vec2::operator-() const
	{
		return vec2{ -x, -y };
	}

	// zero the vector
	void vec2::Zero()
	{
		x = 0.f;
		y = 0.f;
	}
	// cross product
	float vec2::Cross(vec2 const& r_rhs) const
	{
		return (this->x * r_rhs.y) - (this->y * r_rhs.x);
	}
	// dot product
	float vec2::Dot(vec2 const& r_rhs) const
	{
		return ((x * r_rhs.x) + (y * r_rhs.y));
	}

	// for vectors
	float vec2::LengthSquared() const
	{
		return (x * x) + (y * y);
	}

	// for vectors
	float vec2::Length() const
	{
		return sqrtf(this->LengthSquared());
	}

	// for point to point
	float vec2::DistanceSquared(vec2 const& r_rhs) const
	{
		return (((this->x - r_rhs.x) * (this->x - r_rhs.x)) + ((this->y - r_rhs.y) * (this->y - r_rhs.y)));
	}

	// for point to point
	float vec2::Distance(vec2 const& r_rhs) const
	{
		return sqrtf(this->DistanceSquared(r_rhs));
	}

	// return the normalized form of the vector
	vec2 vec2::GetNormalized() const
	{
		return vec2{ *this / this->Length() };
	}

	// normalizes the vector itself
	void vec2::Normalize()
	{
		*this /= this->Length();
	}


	/*                                                                                 vec3 struct member implementations
	--------------------------------------------------------------------------------------------------------------------- */
	// copy assignment
	vec3& vec3::operator=(vec3 const& r_cpy)
	{
		x = r_cpy.x;
		y = r_cpy.y;
		z = r_cpy.z;
		return *this;
	}

	// access operator
	float vec3::operator[](unsigned int index) const
	{
		// if (index > 2)
			// return error
		return (index) ? ((index == 2) ? z : y) : x; // index 2 for z, index 1 for y, index 0 for x
	}

	// allows modification
	float& vec3::operator[](unsigned int index)
	{
		return (index) ? ((index == 2) ? z : y) : x; // index 2 for z, index 1 for y, index 0 for x
		//return (*this)[index];
	}

	// addition
	vec3 vec3::operator+(vec3 const& r_rhs) const
	{
		return vec3{ x + r_rhs.x,
					 y + r_rhs.y,
					 z + r_rhs.z };
	}
	vec3& vec3::operator+=(vec3 const& r_rhs)
	{
		x += r_rhs.x;
		y += r_rhs.y;
		z += r_rhs.z;
		return *this;
	}

	// subtraction
	vec3 vec3::operator-(vec3 const& r_rhs) const
	{
		return vec3{ x - r_rhs.x,
					 y - r_rhs.y,
					 z - r_rhs.z };
	}
	vec3& vec3::operator-=(vec3 const& r_rhs)
	{
		x -= r_rhs.x;
		y -= r_rhs.y;
		z -= r_rhs.z;
		return *this;
	}

	// scale
	vec3 vec3::operator*(float scale) const
	{
		return vec3{ x * scale,
					 y * scale,
					 z * scale };
	}
	vec3 vec3::operator/(float scale) const
	{
		return vec3{ x / scale,
					 y / scale,
					 z / scale };
	}
	vec3& vec3::operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}
	vec3& vec3::operator/=(float scale)
	{
		x /= scale;
		y /= scale;
		z /= scale;
		return *this;
	}

	// zero the vector
	void vec3::Zero()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}
	// cross product
	vec3 vec3::Cross(vec3 const& r_rhs) const
	{
		return vec3{ (this->y * r_rhs.z) - (this->z * r_rhs.y),
					 (this->z * r_rhs.x) - (this->x * r_rhs.z),
					 (this->x * r_rhs.y) - (this->y * r_rhs.x) };

	}
	// dot product
	float vec3::Dot(vec3 const& r_rhs) const
	{
		return ((x * r_rhs.x) + (y * r_rhs.y) + (z * r_rhs.z));
	}

	// Squared Distance betweem two vectors
	float vec3::DistanceSquared(vec3 const& r_rhs) const
	{
		return ((this->x - r_rhs.x) * (this->x - r_rhs.x)) + ((this->y - r_rhs.y) * (this->y - r_rhs.y)) + ((this->x - r_rhs.z) * (this->x - r_rhs.z));
	}
	// Distance between two vectors
	float vec3::Distance(vec3 const& r_rhs) const
	{
		return sqrtf(this->DistanceSquared(r_rhs));
	}

	// Find length^2
	float vec3::LengthSquared() const
	{
		return (x * x) + (y * y) + (z * z);
	}
	// Find length/magnitude
	float vec3::Length() const
	{
		return sqrtf(this->LengthSquared());
	}

	vec3 vec3::GetNormalized() const
	{
		return vec3{ *this / this->Length() };
	}
	// normalize
	void vec3::Normalize()
	{
		*this /= this->Length();
	}

	///*potentially dont need																				   vec4 struct member implementations
	//--------------------------------------------------------------------------------------------------------------------- */
	//// copy assignment
	//vec4& vec4::operator=(vec4 const& r_cpy)
	//{
	//	x = r_cpy.x;
	//	y = r_cpy.y;
	//	z = r_cpy.z;
	//	w = r_cpy.w;
	//}
	//
	//// access operator
	//float vec4::operator[](unsigned int index) const
	//{
	//	// if (index > 3)
	//		// return error
	//	return (index) ? ((index == 3) ? w : ((index == 2) ? z : y)) : x;
	//}
	//
	//// allows modification
	//float& vec4::operator[](unsigned int index)
	//{
	//	return (*this)[index];
	//}
	//
	//// addition
	//vec4 vec4::operator+(vec4 const& r_rhs)
	//{
	//	return vec4{ x + r_rhs.x,
	//				 y + r_rhs.y,
	//				 z + r_rhs.z,
	//				 w + r_rhs.w };
	//}
	//vec4& vec4::operator+=(vec4 const& r_rhs)
	//{
	//	x += r_rhs.x;
	//	y += r_rhs.y;
	//	z += r_rhs.z;
	//	w += r_rhs.w;
	//	return *this;
	//}
	//
	//// subtraction
	//vec4 vec4::operator-(vec4 const& r_rhs)
	//{
	//	return vec4{ x - r_rhs.x,
	//				 y - r_rhs.y,
	//				 z - r_rhs.z,
	//				 w - r_rhs.w };
	//}
	//vec4& vec4::operator-=(vec4 const& r_rhs)
	//{
	//	x -= r_rhs.x;
	//	y -= r_rhs.y;
	//	z -= r_rhs.z;
	//	w -= r_rhs.w;
	//	return *this;
	//}
	//
	//// scale
	//vec4 vec4::operator/(float scale)
	//{
	//	return vec4{ x / scale,
	//				 y / scale,
	//				 z / scale,
	//				 w / scale };
	//}
	//vec4 vec4::operator*(float scale)
	//{
	//	return vec4{ x * scale,
	//				 y * scale,
	//				 z * scale,
	//				 w * scale };
	//}
	//vec4& vec4::operator*=(float scale)
	//{
	//	x *= scale;
	//	y *= scale;
	//	z *= scale;
	//	w *= scale;
	//	return *this;
	//}
	//vec4& vec4::operator/=(float scale)
	//{
	//	x /= scale;
	//	y /= scale;
	//	z /= scale;
	//	w /= scale;
	//	return *this;
	//}
	//
	//// zero the vector
	//void vec4::Zero()
	//{
	//	x = 0.f;
	//	y = 0.f;
	//	z = 0.f;
	//	w = 0.f;
	//}
	//// cross product
	//vec4 vec4::Cross(vec4 const& r_rhs)
	//{
	//
	//}
	//// dot product
	//float vec4::Dot(vec4 const& r_rhs)
	//{
	//	return ((x * r_rhs.x) + (y * r_rhs.y) + (z * r_rhs.z) + (w * r_rhs.w));
	//}

	/*                                                                               mat3x3 struct member implementations
	--------------------------------------------------------------------------------------------------------------------- */
	// default constructor
	mat3x3::mat3x3()
	{
		for (size_t i{ 0 }; i < 9; ++i)
			m[i] = 0.f;
	}
	// constructor
	mat3x3::mat3x3(float c0_r0, float c0_r1, float c0_r2,
		float c1_r0, float c1_r1, float c1_r2,
		float c2_r0, float c2_r1, float c2_r2)
	{
		m[0] = c0_r0; m[1] = c0_r1; m[2] = c0_r2;
		m[3] = c1_r0; m[4] = c1_r1; m[5] = c1_r2;
		m[6] = c2_r0; m[7] = c2_r1; m[8] = c2_r2;
	}
	// copy constructor
	mat3x3::mat3x3(mat3x3 const& r_cpy)
	{
		m[0] = r_cpy.m[0]; m[1] = r_cpy.m[1]; m[2] = r_cpy.m[2];
		m[3] = r_cpy.m[3]; m[4] = r_cpy.m[4]; m[5] = r_cpy.m[5];
		m[6] = r_cpy.m[6]; m[7] = r_cpy.m[7]; m[8] = r_cpy.m[8];
	}
	// 3 vec3 constructor
	mat3x3::mat3x3(vec3 const& r_col0, vec3 const& r_col1, vec3 const& r_col2)
	{
		m[0] = r_col0.x; m[3] = r_col1.x; m[6] = r_col2.x;
		m[1] = r_col0.y; m[4] = r_col1.y; m[7] = r_col2.y;
		m[2] = r_col0.z; m[5] = r_col1.z; m[8] = r_col2.z;
	}
	// copy assignment
	mat3x3& mat3x3::operator=(mat3x3 const& r_cpy)
	{
		for (unsigned int i{ 0 }; i < 9; ++i)
			m[i] = r_cpy[i];
		return *this;
	}

	// access operator
	float mat3x3::operator[](unsigned int index) const // 0-8
	{
		return m[index];
	}
	// float operator[][](unsigned int index1, unsigned int index2) const; // [col][row]

	// allows modification
	float& mat3x3::operator[](unsigned int index) // 0-8
	{
		return m[index];
	}
	// float& operator[][](unsigned int index1, unsigned int index2); // [col][row]

	// addition
	mat3x3 mat3x3::operator+(mat3x3 const& r_rhs) const
	{
		return mat3x3{ m[0] + r_rhs.m[0], m[1] + r_rhs.m[1], m[2] + r_rhs.m[2],
					   m[3] + r_rhs.m[3], m[4] + r_rhs.m[4], m[5] + r_rhs.m[5],
					   m[6] + r_rhs.m[6], m[7] + r_rhs.m[7], m[8] + r_rhs.m[8] };
	}
	mat3x3& mat3x3::operator+=(mat3x3 const& r_rhs)
	{
		for (unsigned int i{ 0 }; i < 9; ++i)
			m[i] += r_rhs[i];
		return *this;
	}

	// subtraction
	mat3x3 mat3x3::operator-(mat3x3 const& r_rhs) const
	{
		return mat3x3{ m[0] - r_rhs.m[0], m[1] - r_rhs.m[1], m[2] - r_rhs.m[2],
					   m[3] - r_rhs.m[3], m[4] - r_rhs.m[4], m[5] - r_rhs.m[5],
					   m[6] - r_rhs.m[6], m[7] - r_rhs.m[7], m[8] - r_rhs.m[8] };
	}
	mat3x3& mat3x3::operator-=(mat3x3 const& r_rhs)
	{
		for (unsigned int i{ 0 }; i < 9; ++i)
			m[i] -= r_rhs[i];
		return *this;
	}

	// scale
	mat3x3 mat3x3::operator*(float scale) const
	{
		return mat3x3{ m[0] * scale, m[1] * scale, m[2] * scale,
					   m[3] * scale, m[4] * scale, m[5] * scale,
					   m[6] * scale, m[7] * scale, m[8] * scale };
	}
	mat3x3 mat3x3::operator/(float scale) const
	{
		return mat3x3{ m[0] / scale, m[1] / scale, m[2] / scale,
					   m[3] / scale, m[4] / scale, m[5] / scale,
					   m[6] / scale, m[7] / scale, m[8] / scale };
	}
	mat3x3& mat3x3::operator*=(float scale)
	{
		for (size_t i{ 0 }; i < 9; ++i)
			m[i] *= scale;
		return *this;
	}
	mat3x3& mat3x3::operator/=(float scale)
	{
		for (size_t i{ 0 }; i < 9; ++i)
			m[i] /= scale;
		return *this;
	}

	// matrix multiplication
	// vec2 mat3x3::operator*(vec2 const& r_rhs) const
	// {
	// 	vec3 ret = *this * vec3{ r_rhs, 1.f };
	// 	return vec2{ ret.x, ret.y };
	// }
	vec3 mat3x3::operator*(vec3 const& r_rhs) const
	{
		vec3 ret{};
		for (int i = 0; i < 3; ++i)
		{
			int matIndex = i;
			ret[i] = (r_rhs[0] * m[matIndex]) +
					 (r_rhs[1] * m[matIndex + 3]) +
					 (r_rhs[2] * m[matIndex + 6]);
		}
		return ret;
	}
	mat3x3 mat3x3::operator*(mat3x3 const& r_rhs) const
	{
		mat3x3 ret{};
		for (unsigned int j{ 0 }; j < 3; ++j)
		{
			for (unsigned int i{ 0 }; i < 9; i += 3)
			{
				ret[i + j] = m[j] * r_rhs[i]
						 + m[j + 3] * r_rhs[i + 1]
						 + m[j + 6] * r_rhs[i + 2];
			}
		}
		return ret;
	}

	// zero the vector
	void mat3x3::Zero()
	{
		for (size_t i{ 0 }; i < 9; ++i)
			m[i] = 0.f;
	}
	// set to identity matrix
	void mat3x3::Identity()
	{
		this->Zero();
		_m00 = 1.f;
		_m11 = 1.f;
		_m22 = 1.f;
	}
	// transpose the matrix
	void mat3x3::Transpose()
	{
		mat3x3 transposed{ _m00, _m10, _m20,
						   _m01, _m11, _m21,
						   _m02, _m12, _m22 };
		*this = transposed;
	}
	// create translation matrix
	void mat3x3::Translate(float t_x, float t_y)
	{
		this->Identity();
		_m20 = t_x;
		_m21 = t_y;
	}
	// create scale matrix
	void mat3x3::Scale(float s_x, float s_y)
	{
		this->Identity();
		_m00 = s_x;
		_m11 = s_y;
	}
	// create rotation matrix from angle which is in radians
	void mat3x3::RotateRad(float angle)
	{
		this->Identity();
		_m00 = cosf(angle);
		_m01 = sinf(angle);
		_m10 = -sinf(angle);
		_m11 = cosf(angle);
	}
	// create rotation matrix from angle which is in degrees
	void mat3x3::RotateDeg(float angle)
	{
		this->RotateRad(ConvertDegToRad(angle));
	}
	// find the determinant of a matrix
	float mat3x3::Determinant() const
	{
		return   (_m00 * _m11 * _m22) + (_m01 * _m12 * _m20) + (_m02 * _m10 * _m21)
			- (_m02 * _m11 * _m20) - (_m01 * _m10 * _m22) - (_m00 * _m12 * _m21);
	}
	// find the adjoint of a matrix
	mat3x3 mat3x3::Adjoint() const
	{
		mat3x3 adj = mat3x3{ ((_m11 * _m22) - (_m21 * _m12)), -((_m10 * _m22) - (_m20 * _m12)),  ((_m10 * _m21) - (_m20 * _m11)),
							-((_m01 * _m22) - (_m21 * _m02)),  ((_m00 * _m22) - (_m20 * _m02)), -((_m00 * _m21) - (_m20 * _m01)),
							 ((_m01 * _m12) - (_m11 * _m02)), -((_m00 * _m12) - (_m10 * _m02)),  ((_m00 * _m11) - (_m10 * _m01)) };
		adj.Transpose();
		return adj;
	}
	// find the inverse of a matrix
	mat3x3 mat3x3::Inverse() const
	{
		float determinant = this->Determinant();
		if (determinant == 0.f)
			throw;
		else
		{
			mat3x3 adj = this->Adjoint();
			adj /= determinant;
			return adj;
		}
	}

	// equations
	// dot product
	float mat3x3::Dot(mat3x3 const& r_rhs) const
	{
		float dotProduct{ 0.f };

		for (unsigned int i{ 0 }; i < 9; ++i)
			dotProduct += m[i] * r_rhs[i];

		return dotProduct;
	}

	/*                                                                               mat4x4 struct member implementations
	--------------------------------------------------------------------------------------------------------------------- */
	mat4x4::mat4x4()
	{
		for (size_t i{ 0 }; i < 16; ++i)
		{
			m[i] = 0.f;
		}
	}

	mat4x4::mat4x4(float c0_r0, float c0_r1, float c0_r2, float c0_r3,
		float c1_r0, float c1_r1, float c1_r2, float c1_r3,
		float c2_r0, float c2_r1, float c2_r2, float c2_r3,
		float c3_r0, float c3_r1, float c3_r2, float c3_r3)
	{
		m[0] = c0_r0; m[1] = c0_r1; m[2] = c0_r2; m[3] = c0_r3;
		m[4] = c1_r0; m[5] = c1_r1; m[6] = c1_r2; m[7] = c1_r3;
		m[8] = c2_r0; m[9] = c2_r1; m[10] = c2_r2; m[11] = c2_r3;
		m[12] = c3_r0; m[13] = c3_r1; m[14] = c3_r2; m[15] = c3_r3;
	}

	mat4x4::mat4x4(mat4x4 const& r_cpy)
	{
		for (size_t i{ 0 }; i < 16; ++i)
		{
			m[i] = r_cpy.m[i];
		}
	}

	//mat4x4::mat4x4(vec4 const& r_col0, vec4 const& r_col1, vec4 const& r_col2, vec4 const& r_col3)
	//{
	//	m[0] = r_col0.x; m[4] = r_col1.x; m[8] = r_col2.x; m[12] = r_col3.x;
	//	m[1] = r_col0.y; m[5] = r_col1.y; m[9] = r_col2.y; m[13] = r_col3.y;
	//	m[2] = r_col0.z; m[6] = r_col1.z; m[10] = r_col2.z; m[14] = r_col3.z;
	//	m[3] = r_col0.w; m[7] = r_col1.w; m[11] = r_col2.w; m[15] = r_col3.w;
	//}
	// potentially dont need
	//mat4x4::mat4x4(mat3x3 const& r_mat, vec4 const& r_vec)
	//{
	//	m[0] = r_mat[0], m[1] = r_mat[1], m[2] = r_mat[2],  m[3] = r_vec.x,
	//	m[4] = r_mat[3], m[5] = r_mat[4], m[6] = r_mat[5],  m[7] = r_vec.y,
	//	m[8] = r_mat[6], m[9] = r_mat[7], m[10] = r_mat[8], m[11] = r_vec.z,
	//	m[12] = 0.f,	 m[13] = 0.f,	  m[14] = 0.f,		m[15] = r_vec.w;
	//}

	mat4x4& mat4x4::operator=(mat4x4 const& r_cpy)
	{
		for (unsigned int i{ 0 }; i < 16; ++i)
			m[i] = r_cpy[i];
		return *this;
	}

	// access operator
	float mat4x4::operator[](unsigned int index) const // 0-8
	{
		return m[index];
	}

	// allows modification
	float& mat4x4::operator[](unsigned int index) // 0-8
	{
		return m[index];
	}

	// addition
	mat4x4 mat4x4::operator+(mat4x4 const& r_rhs) const
	{
		return mat4x4{ m[0] + r_rhs.m[0],   m[1] + r_rhs.m[1],   m[2] + r_rhs.m[2],   m[3] + r_rhs.m[3],
					   m[4] + r_rhs.m[4],   m[5] + r_rhs.m[5],   m[6] + r_rhs.m[6],   m[7] + r_rhs.m[7],
					   m[8] + r_rhs.m[8],   m[9] + r_rhs.m[9],   m[10] + r_rhs.m[10],  m[11] + r_rhs.m[11],
					   m[12] + r_rhs.m[12],  m[13] + r_rhs.m[13],  m[14] + r_rhs.m[14],  m[15] + r_rhs.m[15] };
	}
	mat4x4& mat4x4::operator+=(mat4x4 const& r_rhs)
	{
		for (unsigned int i{ 0 }; i < 16; ++i)
			m[i] += r_rhs[i];
		return *this;
	}

	// subtraction
	mat4x4 mat4x4::operator-(mat4x4 const& r_rhs) const
	{
		return mat4x4{ m[0] - r_rhs.m[0],   m[1] - r_rhs.m[1],   m[2] - r_rhs.m[2],   m[3] - r_rhs.m[3],
					   m[4] - r_rhs.m[4],   m[5] - r_rhs.m[5],   m[6] - r_rhs.m[6],   m[7] - r_rhs.m[7],
					   m[8] - r_rhs.m[8],   m[9] - r_rhs.m[9],   m[10] - r_rhs.m[10],  m[11] - r_rhs.m[11],
					   m[12] - r_rhs.m[12],  m[13] - r_rhs.m[13],  m[14] - r_rhs.m[14],  m[15] - r_rhs.m[15] };
	}
	mat4x4& mat4x4::operator-=(mat4x4 const& r_rhs)
	{
		for (unsigned int i{ 0 }; i < 16; ++i)
			m[i] -= r_rhs[i];
		return *this;
	}

	// scale
	mat4x4 mat4x4::operator*(float scale) const
	{
		return mat4x4{ m[0] * scale,   m[1] * scale,   m[2] * scale,  m[3] * scale,
					   m[4] * scale,   m[5] * scale,   m[6] * scale,  m[7] * scale,
					   m[8] * scale,   m[9] * scale,   m[10] * scale,  m[11] * scale,
					   m[12] * scale,   m[13] * scale,   m[14] * scale,  m[15] * scale };
	}
	mat4x4 mat4x4::operator/(float scale) const
	{
		return mat4x4{ m[0] / scale,   m[1] / scale,   m[2] / scale,  m[3] / scale,
					   m[4] / scale,   m[5] / scale,   m[6] / scale,  m[7] / scale,
					   m[8] / scale,   m[9] / scale,   m[10] / scale,  m[11] / scale,
					   m[12] / scale,   m[13] / scale,   m[14] / scale,  m[15] / scale };
	}
	mat4x4& mat4x4::operator*=(float scale)
	{
		for (size_t i{ 0 }; i < 16; ++i)
			m[i] *= scale;
		return *this;
	}
	mat4x4& mat4x4::operator/=(float scale)
	{
		for (size_t i{ 0 }; i < 16; ++i)
			m[i] /= scale;
		return *this;
	}

	// zero the vector
	void mat4x4::Zero()
	{
		for (size_t i{ 0 }; i < 16; ++i)
			m[i] = 0.f;
	}
	// set to identity matrix
	void mat4x4::Identity()
	{
		this->Zero();
		_m00 = 1.f;
		_m11 = 1.f;
		_m22 = 1.f;
		_m33 = 1.f;
	}
	// transpose the matrix
	void mat4x4::Transpose()
	{
		mat4x4 transposed{ _m00, _m10, _m20, _m30,
						   _m01, _m11, _m21, _m31,
						   _m02, _m12, _m22, _m32,
						   _m03, _m13, _m23, _m33 };
		*this = transposed;
	}


	// ----- Non-member functions ----- //
	// vec2 functions
	float Dot(vec2 const& r_lhs, vec2 const& r_rhs)
	{
		return r_lhs.Dot(r_rhs);
	}
	float Cross(vec2 const& r_lhs, vec2 const& r_rhs)
	{
		return r_lhs.Cross(r_rhs);
	}

	// vec3 functions
	float Dot(vec3 const& r_lhs, vec3 const& r_rhs)
	{
		return r_lhs.Dot(r_rhs);
	}
	vec3 Cross(vec3 const& r_lhs, vec3 const& r_rhs)
	{
		return r_lhs.Cross(r_rhs);
	}

	// mat3x3 functons
	float Dot(mat3x3 const& r_lhs, mat3x3 const& r_rhs)
	{
		return r_lhs.Dot(r_rhs);
	}

	float ConvertRadToDeg(float radAngle)
	{
		return radAngle * 180.f / static_cast<float>(PE_PI);
	}

	float ConvertDegToRad(float degAngle)
	{
		return degAngle * static_cast<float>(PE_PI) / 180.f;
	}

	void Clamp(float& r_varToClamp, float min, float max)
	{
		r_varToClamp = (r_varToClamp < min) ? min : ((r_varToClamp > max) ? max : r_varToClamp);
	}

	void Wrap(float& r_varToWrap, float min, float max)
	{
		if (min > max)
			std::swap(min, max);
		r_varToWrap = (r_varToWrap >= 0.f ? min : max) + fmodf(r_varToWrap, max - min);
	}
}