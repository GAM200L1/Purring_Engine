/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MathCustom.h
 \date     30-08-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu
 \par      code %:     100%
 
 \co-author            Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 \par      code %:     0%
 \par      changes:    15-09-2023
 					   Added function header comments(up till vec3) & updated file 
					   header
 
 \brief 	This file contains declarations for class member functions and variables
			of structs vec2, vec3, mat3x3, and mat4x4.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

constexpr float PE_PI = 3.14159265358979323846f;
namespace PE
{
	// forward declaring these structs
	struct vec2;
	struct vec3;
	struct mat3x3;
	struct mat4x4;

	/*!***********************************************************************************
	 \brief Vector 2 struct, contains x and y coordinates
	 
	*************************************************************************************/
	struct vec2
	{
		// variables
		float x, y;

		/*!***********************************************************************************
		 \brief Default construct a new vec2 object
		 
		*************************************************************************************/
		vec2() : x{ 0.f }, y{ 0.f } {}

		/*!***********************************************************************************
		 \brief Conversion construct a new vec2 object using 2 float values for x and y 
		 		respectively. 
				Explicit specifier used.

		 \param[in] ux 	value to set x coordinate to
		 \param[in] uy 	value to set y coordinate to
		*************************************************************************************/
		explicit vec2(float ux, float uy) : x{ ux }, y{ uy } {}

		/*!***********************************************************************************
		 \brief Copy construct a new vec2 object using another vec2's values
		 
		 \param[in] r_cpy 	The vec2 to copy from
		*************************************************************************************/
		vec2(vec2 const& r_cpy) : x{ r_cpy.x }, y{ r_cpy.y } {}

		/*!***********************************************************************************
		 \brief Copy assignment (operator= overload)
		 
		 \param[in] r_cpy 	The vec2 to copy from
		 \return vec2& 		Reference to this vec2
		*************************************************************************************/
		vec2& operator=(vec2 const& r_cpy);

		// Array subscript
		/*!***********************************************************************************
		 \brief Array subscript overload(const) allow the reading of x & y values using
		 		array subscript syntax.
		 
		 \param[in] index 	The index to read (x == 0, y == 1)
		 \return float 		A copy of the x or y value accessed
		*************************************************************************************/
		float operator[](unsigned int index) const;

		/*!***********************************************************************************
		 \brief Array subscript overload allow the accessing of x & y values using array 
		 		subscript syntax.
				Returns a reference to the index accessed, allowing for modification.
		 
		 \param[in] index 	The index to access (x == 0, y == 1)
		 \return float& 	Reference tto the x or y value accessed
		*************************************************************************************/
		float& operator[](unsigned int index);

		// Addition
		/*!***********************************************************************************
		 \brief Addition operator overload, allows the addition of two vec2 and returns a 
		 		copy of the result.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec2 		A copy of the resulting addition
		*************************************************************************************/
		vec2 operator+(vec2 const& r_rhs) const;

		/*!***********************************************************************************
		 \brief Addition assignment operator overload, allows the addition of the right
		 		operand into the left operand.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec2& 		A reference to the left operand
		*************************************************************************************/
		vec2& operator+=(vec2 const& r_rhs);

		// subtraction
		/*!***********************************************************************************
		 \brief Subtraction operator overload, allows for the subtraction of two vec2 and
		 		returns a copy of the result.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec2 		A copy of the resulting subtraction
		*************************************************************************************/
		vec2 operator-(vec2 const& r_rhs) const;

		/*!***********************************************************************************
		 \brief Subtraction assignment operator overload, allows for the subtraction of the
		 		right operand into the left operand.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec2& 		A reference to the left operand
		*************************************************************************************/
		vec2& operator-=(vec2 const& r_rhs);

		// scale
		/*!***********************************************************************************
		 \brief Multiplication operator overload, allows for the scaling (by multiplication) 
		 		of vec2 by a float value and returns a copy of the result.
		 		
		 \param[in] scale 	The scalar value to scale the vec2 by
		 \return vec2 		A copy of the resulting scaling.
		*************************************************************************************/
		vec2 operator*(float scale) const;

		/*!***********************************************************************************
		 \brief Division operator overload, allows for the scaling (by division) of vec2 by
		 		a float value and returns a copy of the result.
		 
		 \param[in] scale 	The scalar value to scale the vec2 by
		 \return vec2 		A copy of the resulting scaling
		*************************************************************************************/
		vec2 operator/(float scale) const;

		/*!***********************************************************************************
		 \brief Multiplication assignment operator overload, allows for the scaling (by 
		 		multiplication) of the left operand by a float value (right operand).
		 
		 \param[in] scale 	The scalar value to scale the vec2 by
		 \return vec2& 		A reference to the left operand
		*************************************************************************************/
		vec2& operator*=(float scale);

		/*!***********************************************************************************
		 \brief Division assignment operator overload, allows for the scaling (by division)
		 		of the left operand by a float value (right operand).
		 
		 \param[in] scale 	The scalar value to scale the vec2 by
		 \return vec2& 		A reference to the left operand
		*************************************************************************************/
		vec2& operator/=(float scale);

		vec2 operator-() const;

		// zero the vector
		/*!***********************************************************************************
		 \brief Zeroes out the x and y values of the vec2
		 
		*************************************************************************************/
		void Zero();
		// cross product (zaxis)
		/*!***********************************************************************************
		 \brief Calculates the cross product of two vec2 (cross product magnitude/z-axis)
		 
		 \param[in] r_rhs 	The vec2 to cross
		 \return float 		The cross product magnitude
		*************************************************************************************/
		float Cross(vec2 const& r_rhs) const;
		// dot product

		/*!***********************************************************************************
		 \brief Calculates the dot product of two vec2
		 
		 \param[in] r_rhs  	The vec2 to dot
		 \return float 		The dot product
		*************************************************************************************/
		float Dot(vec2 const& r_rhs) const;

		// Squared Distance betweem two vectors
		/*!***********************************************************************************
		 \brief Calculates the distance squared between two vec2
		 
		 \param[in] r_rhs 	The vec2 to get the distance squared from
		 \return float 		The distance squared
		*************************************************************************************/
		float DistanceSquared(vec2 const& r_rhs) const;
		// Distance between two vectors
		/*!***********************************************************************************
		 \brief Calculates the distance between two vec2
		 
		 \param[in] r_rhs 	The vec2 to get the distance from
		 \return float 		The distance
		*************************************************************************************/
		float Distance(vec2 const& r_rhs) const;
		// get normal of vec2
		/*!***********************************************************************************
		 \brief Calculates the normalized vector of this vec2, and returns a copy
		 
		 \return vec2 	The copy of this vec2's normalized vector
		*************************************************************************************/
		vec2 GetNormalized() const;

		// Find length^2
		/*!***********************************************************************************
		 \brief Caclulates the magnitude squared of this vec2
		 
		 \return float 	The magnitude squared
		*************************************************************************************/
		float LengthSquared() const;
		// Find length/magnitude
		/*!***********************************************************************************
		 \brief Caclulates the magnitude of this vec2
		 
		 \return float 	The magnitude
		*************************************************************************************/
		float Length() const;
		// normalize
		/*!***********************************************************************************
		 \brief Normalizes this vec2
		 
		*************************************************************************************/
		void Normalize();
	};

	/*!***********************************************************************************
	 \brief Vector 3 struct, contains x, y and z coordinates
	 
	*************************************************************************************/
	struct vec3
	{
		float x, y, z;

		// default constructor
		/*!***********************************************************************************
		 \brief Default construct a new vec3 object
		 
		*************************************************************************************/
		vec3() : x{ 0.f }, y{ 0.f }, z{ 0.f } {}

		// 3 floats constructor
		/*!***********************************************************************************
		 \brief Conversion construct a new vec3 object from 3 float values.
		 
		 \param[in] ux 	X coordinate
		 \param[in] uy 	Y coordinate
		 \param[in] uz  Z coordinate
		*************************************************************************************/
		explicit vec3(float ux, float uy, float uz) : x{ ux }, y{ uy }, z{ uz } {}

		// vec2 + float constructor
		/*!***********************************************************************************
		 \brief Conversion construct a new vec3 object from a vec2 and a float value.
		 
		 \param[in] r_v 	vec2 containing x and y coordinates
		 \param[in] uz 		float value containing the z coordinates
		*************************************************************************************/
		explicit vec3(vec2 const& r_v, float uz) : x{ r_v.x }, y{ r_v.y }, z{ uz } {}

		// copy constructor
		/*!***********************************************************************************
		 \brief Copy construct a new vec3 object.
		 
		 \param[in] r_cpy 	vec3 to copy from
		*************************************************************************************/
		vec3(vec3 const& r_cpy) : x{ r_cpy.x }, y{ r_cpy.y }, z{ r_cpy.z } {}
		
		// copy assignment
		/*!***********************************************************************************
		 \brief Assignment operator overload, allows the copy assignment of a vec3 from a 
		 		vec3.
		 
		 \param[in] r_cpy 	vec3 to copy from
		 \return vec3& 		Reference to this vec3
		*************************************************************************************/
		vec3& operator=(vec3 const& r_cpy);

		// access operator
		/*!***********************************************************************************
		 \brief Array subscript operator overload, allows the reading of x, y and z values 
		 		using array subscript syntax.
		 
		 \param[in] index 	The index to read (x == 0, y == 1, z == 2)
		 \return float 		A copy to the accessed index
		*************************************************************************************/
		float operator[](unsigned int index) const;

		// allows modification
		/*!***********************************************************************************
		 \brief Array subscript operator overload, allows the access of x, y and z values
		 		using array subcript syntax, allowing for modification of values.
		 
		 \param[in] index 	The index to access (x == 0, y == 1, z == 2)
		 \return float& 	A reference to the accessed index
		*************************************************************************************/
		float& operator[](unsigned int index);

		// addition
		/*!***********************************************************************************
		 \brief Addition operator overload, allows the addition of two vec3, returns a copy
		 		of the result.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec3 		The resulting vec3
		*************************************************************************************/
		vec3 operator+(vec3 const& r_rhs) const;
		/*!***********************************************************************************
		 \brief Addition assignment operator overload, allows the addition of the right 
		 		operand into this vec3.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec3& 		A reference to this vec3
		*************************************************************************************/
		vec3& operator+=(vec3 const& r_rhs);

		// subtraction
		/*!***********************************************************************************
		 \brief Subtraction operator overload, allows the subtraction of two vec3, returns
		 		a copy of the result
		 
		 \param[in] r_rhs 	The right operand
		 \return vec3 		The resulting vec3
		*************************************************************************************/
		vec3 operator-(vec3 const& r_rhs) const;

		/*!***********************************************************************************
		 \brief Subtraction assignment operator, allows the subtraction of the right operand
		 		into this vec3.
		 
		 \param[in] r_rhs 	The right operand
		 \return vec3& 		A reference to this vec3
		*************************************************************************************/
		vec3& operator-=(vec3 const& r_rhs);

		// scale
		/*!***********************************************************************************
		 \brief Multiplication operator overload, allows for the scaling (by multiplication) 
		 		of vec3 by a float value and returns a copy of the result.
		 
		 \param[in] scale 	Scalar value to scale the vec3 by
		 \return vec3 		A copy of the result
		*************************************************************************************/
		vec3 operator*(float scale) const;

		/*!***********************************************************************************
		 \brief Division operator overload, allows for the scaling (by division) of vec3 by a 
				float value and returns a copy of the result.
		 
		 \param[in] scale 	Scalar value to scale the vec3 by
		 \return vec3 		A copy of the result
		*************************************************************************************/
		vec3 operator/(float scale) const;

		/*!***********************************************************************************
		 \brief Multiplication assignment operator overload, allows for the scaling (by 
		 		multiplication) of this vec3 by a float scalar value.
		 
		 \param[in] scale 	Scalar value to scale the vec3 by
		 \return vec3& 		Reference to this vec3
		*************************************************************************************/
		vec3& operator*=(float scale);

		/*!***********************************************************************************
		 \brief Division assignment operator overload, allows for the scaling (by division)
		 		of this vec3 by a float scalar value.
		 
		 \param[in] scale 	Scalar value to scale the vec3 by
		 \return vec3& 		Reference to this vec3
		*************************************************************************************/
		vec3& operator/=(float scale);

		// zero the vector
		/*!***********************************************************************************
		 \brief Zeroes out this vec3
		 
		*************************************************************************************/
		void Zero();

		// cross product
		/*!***********************************************************************************
		 \brief Calulates the cross product of two vec3, and returns a copy of the result.
		 
		 \param[in] r_rhs 	The vec3 to cross
		 \return vec3 		The cross product
		*************************************************************************************/
		vec3 Cross(vec3 const& r_rhs) const;

		// dot product
		/*!***********************************************************************************
		 \brief Caclulates the dot product of the two vec3, and returns a copy of the result.
		 
		 \param[in] r_rhs 	The vec3 to dot
		 \return float 		The dot product
		*************************************************************************************/
		float Dot(vec3 const& r_rhs) const;

		// Squared Distance betweem two vectors
		/*!***********************************************************************************
		 \brief Calculates the distance squared between the two vec3, and returns the result.
		 
		 \param[in] r_rhs 	The vec3 to get the distance to
		 \return float 		The distance squared between this and r_rhs
		*************************************************************************************/
		float DistanceSquared(vec3 const& r_rhs) const;
		// Distance between two vectors

		/*!***********************************************************************************
		 \brief Calculates the distance between the two vec3, and returns the result.
		 
		 \param[in] r_rhs 	The vec3 to get the distance to
		 \return float 		The distance between this and r_rhs
		*************************************************************************************/
		float Distance(vec3 const& r_rhs) const;
		// get normal of vec3

		/*!***********************************************************************************
		 \brief Caclulates the normalized vector of this vec3, returns a copy of the result.
		 
		 \return vec3 	The normalized vector of this vec3
		*************************************************************************************/
		vec3 GetNormalized() const;

		// Find length^2
		/*!***********************************************************************************
		 \brief Calculates the magnitude squared of this vec3
		 
		 \return float 	The magnitude squared of this vec3
		*************************************************************************************/
		float LengthSquared() const;

		// Find length/magnitude
		/*!***********************************************************************************
		 \brief Calculates the magnitude of this vec3
		 
		 \return float 	The magnitude of this vec3
		*************************************************************************************/
		float Length() const;

		// normalize
		/*!***********************************************************************************
		 \brief Normalizes this vec3
		 
		*************************************************************************************/
		void Normalize();
	};

	//struct vec4
	//{
	//	float x, y, z, w;
	//
	//	// default constructor
	//	vec4() : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f } {}
	//	// 3 floats constructor
	//	vec4(float ux, float uy, float uz, float uw) : x{ ux }, y{ uy }, z{ uz }, w{ uw } {}
	//	// vec3 + float constructor
	//	vec4(vec3 const& r_v, float uw) : x{ r_v.x }, y{ r_v.y }, z{ r_v.z }, w{ uw } {}
	//	// copy constructor
	//	vec4(vec4 const& r_cpy) : x{ r_cpy.x }, y{ r_cpy.y }, z{ r_cpy.z }, w{ r_cpy.w } {}
	//	// copy assignment
	//	vec4& operator=(vec4 const& r_cpy);
	//
	//	// access operator
	//	float operator[](unsigned int index) const;
	//
	//	// allows modification
	//	float& operator[](unsigned int index);
	//
	//	// addition
	//	vec4 operator+(vec4 const& r_rhs);
	//	vec4& operator+=(vec4 const& r_rhs);
	//
	//	// subtraction
	//	vec4 operator-(vec4 const& r_rhs);
	//	vec4& operator-=(vec4 const& r_rhs);
	//
	//	// scale
	//	vec4 operator*(float scale);
	//	vec4 operator/(float scale);
	//	vec4& operator*=(float scale);
	//	vec4& operator/=(float scale);
	//
	//	// zero the vector
	//	void Zero();
	//	// cross product
	//	vec4 Cross(vec4 const& r_rhs);
	//	// dot product
	//	float Dot(vec4 const& r_rhs);
	//};


	struct mat3x3
	{
		float m[9];
		// col row format
		float& m_00 = m[0], & m_01 = m[1], & m_02 = m[2],
			& m_10 = m[3], & m_11 = m[4], & m_12 = m[5],
			& m_20 = m[6], & m_21 = m[7], & m_22 = m[8];

		// default constructor
		mat3x3();
		// constructor
		explicit mat3x3(float c0_r0, float c0_r1, float c0_r2,
			float c1_r0, float c1_r1, float c1_r2,
			float c2_r0, float c2_r1, float c2_r2);
		// copy constructor
		mat3x3(mat3x3 const& r_cpy);
		// 3 vec3 constructor
		mat3x3(vec3 const& r_col0, vec3 const& r_col1, vec3 const& r_col2);
		// copy assignment
		mat3x3& operator=(mat3x3 const& r_cpy);

		// access operator
		float operator[](unsigned int index) const; // 0-8
		// float operator[][](unsigned int index1, unsigned int index2) const; // [col][row]

		// allows modification
		float& operator[](unsigned int index); // 0-8
		// float& operator[][](unsigned int index1, unsigned int index2); // [col][row]

		// addition
		mat3x3 operator+(mat3x3 const& r_rhs) const;
		mat3x3& operator+=(mat3x3 const& r_rhs);

		// subtraction
		mat3x3 operator-(mat3x3 const& r_rhs) const;
		mat3x3& operator-=(mat3x3 const& r_rhs);

		// scale
		mat3x3 operator*(float scale) const;
		mat3x3 operator/(float scale) const;
		mat3x3& operator*=(float scale);
		mat3x3& operator/=(float scale);

		// matrix multiplication
		vec2 operator*(vec2 const& r_rhs) const;
		vec3 operator*(vec3 const& r_rhs) const;
		mat3x3 operator*(mat3x3 const& r_rhs) const;

		// zero the vector
		void Zero();
		// set to identity matrix
		void Identity();
		// transpose the matrix
		void Transpose();
		// create translation matrix
		void Translate(float t_x, float t_y);
		// create scale matrix
		void Scale(float s_x, float s_y);
		// create rotation matrix from angle which is in radians
		void RotateRad(float angle);
		// create rotation matrix from angle which is in degrees
		void RotateDeg(float angle);

		// equations
		// dot product
		float Dot(mat3x3 const& r_rhs) const;
		// Determinant
		float Determinant() const;
		// Adjoint
		mat3x3 Adjoint() const;
		// Inverse
		mat3x3 Inverse() const;
		// conversion for glm
		mat4x4 ConvertTo4x4() const;
	};


	struct mat4x4
	{
		float m[16];

		// col row format
		float& m_00 = m[0], & m_01 = m[1], & m_02 = m[2], & m_03 = m[3],
			& m_10 = m[4], & m_11 = m[5], & m_12 = m[6], & m_13 = m[7],
			& m_20 = m[8], & m_21 = m[9], & m_22 = m[10], & m_23 = m[11],
			& m_30 = m[12], & m_31 = m[13], & m_32 = m[14], & m_33 = m[15];

		// default constructor
		mat4x4();
		// constructor
		explicit mat4x4(float c0_r0, float c0_r1, float c0_r2, float c0_r3,
			float c1_r0, float c1_r1, float c1_r2, float c1_r3,
			float c2_r0, float c2_r1, float c2_r2, float c2_r3,
			float c3_r0, float c3_r1, float c3_r2, float c3_r3);
		// copy constructor
		mat4x4(mat4x4 const& r_cpy);

		// potentially unneeded
		//mat4x4(vec4 const& r_col0, vec4 const& r_col1, vec4 const& r_col2, vec4 const& r_col3);

		// copy assignment
		mat4x4& operator=(mat4x4 const& r_cpy);

		// access operator
		float operator[](unsigned int index) const; // 0-15
		// float operator[][](unsigned int index1, unsigned int index2) const; // [col][row]

		// allows modification
		float& operator[](unsigned int index); // 0-15
		// float& operator[][](unsigned int index1, unsigned int index2); // [col][row]

		// addition
		mat4x4 operator+(mat4x4 const& r_rhs) const;
		mat4x4& operator+=(mat4x4 const& r_rhs);

		// subtraction
		mat4x4 operator-(mat4x4 const& r_rhs) const;
		mat4x4& operator-=(mat4x4 const& r_rhs);

		// scale
		mat4x4 operator*(float scale) const;
		mat4x4 operator/(float scale) const;
		mat4x4& operator*=(float scale);
		mat4x4& operator/=(float scale);

		// zero the vector
		void Zero();
		// set to identity matrix
		void Identity();
		// transpose the matrix
		void Transpose();
	};


	// vec2 functions
	float Dot(vec2 const& r_lhs, vec2 const& r_rhs);
	float Cross(vec2 const& r_lhs, vec2 const& r_rhs);

	// vec3 functions
	float Dot(vec3 const& r_lhs, vec3 const& r_rhs);
	vec3 Cross(vec3 const& r_lhs, vec3 const& r_rhs);

	//// vec4 functions
	//float Dot(vec4 const& r_lhs, vec4 const& r_rhs);
	//vec4 Cross(vec4 const& r_lhs, vec4 const& r_rhs);

	// mat3x3 functions
	float Dot(mat3x3 const& r_lhs, mat3x3 const& r_rhs);

	float ConvertRadToDeg(float radAngle);

	float ConvertDegToRad(float degAngle);

	void Clamp(float& r_varToClamp, float min, float max);

	void Wrap(float& r_varToWrap, float min, float max);
}


