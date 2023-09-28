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
	//! Forward declaration of structs
	struct vec2;
	struct vec3;
	struct mat3x3;
	struct mat4x4;

	// Vector 2D struct. Contains 2 floats denoted as x and y.
	struct vec2
	{
		// ----- Public Variables ----- //
		public:
		float x, y;

		// ----- Constructors ----- //
		public:
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

		// ----- Public Methods ----- //
		
		public:
		// ----- Access Operators ----- //
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

		// ----- Addition ----- //
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

		// ----- Subtraction ----- //
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

		// ----- Scale ----- //
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

		/*!***********************************************************************************
		 \brief Negative operator overload. Negates the vec2 and returns the result.
		 
		 \return vec2 Negated version of the vec2
		*************************************************************************************/
		vec2 operator-() const;

		/*!***********************************************************************************
		 \brief Zeroes out the x and y values of the vec2
		 
		*************************************************************************************/
		void Zero();


		// ----- Cross Product ----- //
		/*!***********************************************************************************
		 \brief Calculates the cross product of two vec2 (cross product magnitude/z-axis)
		 
		 \param[in] r_rhs 	The vec2 to cross
		 \return float 		The cross product magnitude
		*************************************************************************************/
		float Cross(vec2 const& r_rhs) const;

		// ----- Dot Product ----- //
		/*!***********************************************************************************
		 \brief Calculates the dot product of two vec2
		 
		 \param[in] r_rhs  	The vec2 to dot
		 \return float 		The dot product
		*************************************************************************************/
		float Dot(vec2 const& r_rhs) const;

		// ----- Distance ----- //
		/*!***********************************************************************************
		 \brief Calculates the distance squared between two vec2 points
		 
		 \param[in] r_rhs 	The vec2 to get the distance squared from
		 \return float 		The distance squared
		*************************************************************************************/
		float DistanceSquared(vec2 const& r_rhs) const;
		
		/*!***********************************************************************************
		 \brief Calculates the distance between two vec2 points
		 
		 \param[in] r_rhs 	The vec2 to get the distance from
		 \return float 		The distance
		*************************************************************************************/
		float Distance(vec2 const& r_rhs) const;
		
		// ----- Normalize ----- //
		/*!***********************************************************************************
		 \brief Calculates the normalized vector of this vec2, and returns a copy
		 
		 \return vec2 	The copy of this vec2's normalized vector
		*************************************************************************************/
		vec2 GetNormalized() const;

		/*!***********************************************************************************
		 \brief Normalizes this vec2
		 
		*************************************************************************************/
		void Normalize();

		// ----- Length ----- //
		/*!***********************************************************************************
		 \brief Caclulates the magnitude squared of this vec2 vector
		 
		 \return float 	The magnitude squared
		*************************************************************************************/
		float LengthSquared() const;
		
		/*!***********************************************************************************
		 \brief Caclulates the magnitude of this vec2 vector
		 
		 \return float 	The magnitude
		*************************************************************************************/
		float Length() const;
	};

	// Vector 3 struct, contains x, y and z coordinates
	struct vec3
	{
		// ----- Public Variables ----- //
		public:
		float x, y, z;

		// ----- Constructors ----- //
		public:
		/*!***********************************************************************************
		 \brief Default construct a new vec3 object
		 
		*************************************************************************************/
		vec3() : x{ 0.f }, y{ 0.f }, z{ 0.f } {}

		/*!***********************************************************************************
		 \brief Conversion construct a new vec3 object from 3 float values.
		 
		 \param[in] ux 	X coordinate
		 \param[in] uy 	Y coordinate
		 \param[in] uz  Z coordinate
		*************************************************************************************/
		explicit vec3(float ux, float uy, float uz) : x{ ux }, y{ uy }, z{ uz } {}

		/*!***********************************************************************************
		 \brief Conversion construct a new vec3 object from a vec2 and a float value.
		 
		 \param[in] r_v 	vec2 containing x and y coordinates
		 \param[in] uz 		float value containing the z coordinates
		*************************************************************************************/
		explicit vec3(vec2 const& r_v, float uz) : x{ r_v.x }, y{ r_v.y }, z{ uz } {}

		/*!***********************************************************************************
		 \brief Copy construct a new vec3 object.
		 
		 \param[in] r_cpy 	vec3 to copy from
		*************************************************************************************/
		vec3(vec3 const& r_cpy) : x{ r_cpy.x }, y{ r_cpy.y }, z{ r_cpy.z } {}
		

		// ----- Public Methods ----- //
		public:
		/*!***********************************************************************************
		 \brief Assignment operator overload, allows the copy assignment of a vec3 from a 
		 		vec3.
		 
		 \param[in] r_cpy 	vec3 to copy from
		 \return vec3& 		Reference to this vec3
		*************************************************************************************/
		vec3& operator=(vec3 const& r_cpy);

		// ----- Access Operators ----- //
		/*!***********************************************************************************
		 \brief Array subscript operator overload, allows the reading of x, y and z values 
		 		using array subscript syntax.
		 
		 \param[in] index 	The index to read (x == 0, y == 1, z == 2)
		 \return float 		A copy to the accessed index
		*************************************************************************************/
		float operator[](unsigned int index) const;

		/*!***********************************************************************************
		 \brief Array subscript operator overload, allows the access of x, y and z values
		 		using array subcript syntax, allowing for modification of values.
		 
		 \param[in] index 	The index to access (x == 0, y == 1, z == 2)
		 \return float& 	A reference to the accessed index
		*************************************************************************************/
		float& operator[](unsigned int index);

		// ----- Addition ----- //
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

		// ----- Subtraction ----- //
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

		// ----- Scale ----- //
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

		// Zeroes the vector
		/*!***********************************************************************************
		 \brief Zeroes out this vec3
		 
		*************************************************************************************/
		void Zero();

		// ----- Cross Product ----- //
		/*!***********************************************************************************
		 \brief Calulates the cross product of two vec3, and returns a copy of the result.
		 
		 \param[in] r_rhs 	The vec3 to cross
		 \return vec3 		The cross product
		*************************************************************************************/
		vec3 Cross(vec3 const& r_rhs) const;

		// ----- Dot Product ----- //
		/*!***********************************************************************************
		 \brief Caclulates the dot product of the two vec3, and returns a copy of the result.
		 
		 \param[in] r_rhs 	The vec3 to dot
		 \return float 		The dot product
		*************************************************************************************/
		float Dot(vec3 const& r_rhs) const;

		// ----- Distance ----- //
		/*!***********************************************************************************
		 \brief Calculates the distance squared between the two vec3 points, and returns the result.
		 
		 \param[in] r_rhs 	The vec3 to get the distance to
		 \return float 		The distance squared between this and r_rhs
		*************************************************************************************/
		float DistanceSquared(vec3 const& r_rhs) const;
		// Distance between two vectors

		/*!***********************************************************************************
		 \brief Calculates the distance between the two vec3 points, and returns the result.
		 
		 \param[in] r_rhs 	The vec3 to get the distance to
		 \return float 		The distance between this and r_rhs
		*************************************************************************************/
		float Distance(vec3 const& r_rhs) const;
		
		// ----- Normalize ----- //
		/*!***********************************************************************************
		 \brief Caclulates the normalized vector of this vec3, returns a copy of the result.
		 
		 \return vec3 	The normalized vector of this vec3
		*************************************************************************************/
		vec3 GetNormalized() const;

		/*!***********************************************************************************
		 \brief Normalizes this vec3
		 
		*************************************************************************************/
		void Normalize();

		// ----- Length ----- //
		/*!***********************************************************************************
		 \brief Calculates the magnitude squared of this vec3
		 
		 \return float 	The magnitude squared of this vec3
		*************************************************************************************/
		float LengthSquared() const;

		/*!***********************************************************************************
		 \brief Calculates the magnitude of this vec3
		 
		 \return float 	The magnitude of this vec3
		*************************************************************************************/
		float Length() const;
		
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

	//! Matrix 3 column 3 row struct
	struct mat3x3
	{
		float m[9];
		// column major
		float& _m00 = m[0], & _m01 = m[1], & _m02 = m[2],
			 & _m10 = m[3], & _m11 = m[4], & _m12 = m[5],
			 & _m20 = m[6], & _m21 = m[7], & _m22 = m[8];

		// ----- Constructors ----- //
		/*!***********************************************************************************
		 \brief Constructs a new mat3x3 object where all elements are 0.
		 
		*************************************************************************************/
		mat3x3();
		/*!***********************************************************************************
		 \brief Constructs a new mat3x3 object based on user's input. Note that input has to be
		 		column major.
		 
		 \param[in,out] c0_r0 - element at column 0 row 0
		 \param[in,out] c0_r1 - element at column 0 row 1
		 \param[in,out] c0_r2 - element at column 0 row 2
		 \param[in,out] c1_r0 - element at column 1 row 0
		 \param[in,out] c1_r1 - element at column 1 row 1
		 \param[in,out] c1_r2 - element at column 1 row 2
		 \param[in,out] c2_r0 - element at column 2 row 0
		 \param[in,out] c2_r1 - element at column 2 row 1
		 \param[in,out] c2_r2 - element at column 2 row 2
		*************************************************************************************/
		explicit mat3x3(float c0_r0, float c0_r1, float c0_r2,
			float c1_r0, float c1_r1, float c1_r2,
			float c2_r0, float c2_r1, float c2_r2);
		
		/*!***********************************************************************************
		 \brief Copy constructs a new mat3x3 object 
		 
		 \param[in,out] r_cpy - mat3x3 object to make copy of
		*************************************************************************************/
		mat3x3(mat3x3 const& r_cpy);
		
		/*!***********************************************************************************
		 \brief Construct a new mat3x3 object using 3 vec3s. vec3s are to represent each column
		 		of the mat3x3.
		 
		 \param[in,out] r_col0 - vec3 that represents the values in the first column of the mat3x3
		 \param[in,out] r_col1 - vec3 that represents the values in the second column of the mat3x3
		 \param[in,out] r_col2 - vec3 that represents the values in the third column of the mat3x3
		*************************************************************************************/
		mat3x3(vec3 const& r_col0, vec3 const& r_col1, vec3 const& r_col2);
		
		/*!***********************************************************************************
		 \brief Copy assignment operator overload for mat3x3. mat3x3 elements will become a copy
		 		of the mat3x3 input.
		 
		 \param[in,out] r_cpy - the mat3x3 to copy
		 \return mat3x3& - reference to this mat3x3
		*************************************************************************************/
		mat3x3& operator=(mat3x3 const& r_cpy);

		// ----- Access Operators ----- //
		/*!***********************************************************************************
		 \brief Subscript Operator for mat3x3 to access and read the element at the index.
		 		mat3x3 indexing: 0 3 6
								 1 4 7
								 2 5 8
		 
		 \param[in,out] index - The index of the element to be accessed
		 \return float - element in mat3x3 at 'index'
		*************************************************************************************/
		float operator[](unsigned int index) const; // 0-8
		// float operator[][](unsigned int index1, unsigned int index2) const; // [col][row]

		/*!***********************************************************************************
		 \brief Subscript Operator for mat3x3 to access, read and modify the element at the index
		 		mat3x3 indexing: 0 3 6
								 1 4 7
								 2 5 8
		 \param[in,out] index - The index of the element to be accessed.
		 \return float& - reference to the element at 'index' which can be modified
		*************************************************************************************/
		float& operator[](unsigned int index); // 0-8
		// float& operator[][](unsigned int index1, unsigned int index2); // [col][row]


		// ----- Addition ----- //
		/*!***********************************************************************************
		 \brief Addition operator overload to add two mat3x3 objects together. 
		 		Returns the result of the addition.
	
		 \param[in,out] r_rhs - The mat3x3 to add on to this mat3x3
		 \return mat3x3 - the sum of the two mat3x3s
		*************************************************************************************/
		mat3x3 operator+(mat3x3 const& r_rhs) const;

		/*!***********************************************************************************
		 \brief Addition assignment operator to add to the mat3x3, thereby modifying it.

		 \param[in,out] r_rhs - The mat3x3 to add on to this mat3x3
		 \return mat3x3& - this mat3x3 modified from adding r_rhs
		*************************************************************************************/
		mat3x3& operator+=(mat3x3 const& r_rhs);

		// ------ Subtraction ----- //
		/*!***********************************************************************************
		 \brief Subtraction operator to subtract a mat3x3 from another mat3x3
		 		and return the result of that subtraction
		 
		 \param[in,out] r_rhs - The mat3x3 to subtract from the left-side mat3x3
		 \return mat3x3 - The result of the subtraction
		*************************************************************************************/
		mat3x3 operator-(mat3x3 const& r_rhs) const;

		/*!***********************************************************************************
		 \brief Subtract assignment operator overload to subtract 'r_rhs' from reference mat3x3,
		 		directly modifying it.
		 
		 \param[in,out] r_rhs - mat3x3 to subtract from reference mat3x3
		 \return mat3x3& - the modified mat3x3 post subtraction
		*************************************************************************************/
		mat3x3& operator-=(mat3x3 const& r_rhs);

		// ----- Scaling ----- //
		/*!***********************************************************************************
		 \brief Multiply operator to scale matrix by a float.
		 
		 \param[in,out] scale - amount to multiply each element by
		 \return mat3x3 - the resultant mat3x3 from scaling
		*************************************************************************************/
		mat3x3 operator*(float scale) const;

		/*!***********************************************************************************
		 \brief Division operator to divide matrix by a float.
		 
		 \param[in,out] scale - amount to divide each element by
		 \return mat3x3 - the resultant mat3x3 from scaling
		*************************************************************************************/
		mat3x3 operator/(float scale) const;

		/*!***********************************************************************************
		 \brief Multiply assignment operator to directly modify and scale a mat3x3 by a float.
		 
		 \param[in,out] scale - amount to multiply each element by
		 \return mat3x3& - the referenced mat3x3 post scaling
		*************************************************************************************/
		mat3x3& operator*=(float scale);

		/*!***********************************************************************************
		 \brief Division assignment operator to directly modify and divide a mat3x3 by a float.
		 
		 \param[in,out] scale - amount to divide each element by
		 \return mat3x3& - the referenced mat3x3 post division
		*************************************************************************************/
		mat3x3& operator/=(float scale);

		// ----- Matrix Multiplication ----- //
		// vec2 operator*(vec2 const& r_rhs) const;
		/*!***********************************************************************************
		 \brief Perform matrix multiplication with a vec3, multiplying row of mat3x3 with vec3 column
		 
		 \param[in,out] r_rhs - vec3 to do matrix multiplication with
		 \return vec3 - the result of the matrix multiplication
		*************************************************************************************/
		vec3 operator*(vec3 const& r_rhs) const;
		/*!***********************************************************************************
		 \brief Perform matrix multiplication with a mat3x3, multiplying row of LHS mat3x3 with
		 		column of RHS mat3x3.
		 
		 \param[in,out] r_rhs - mat3x3 to do matrix multiplication wtih
		 \return mat3x3 - result of matrix multiplication
		*************************************************************************************/
		mat3x3 operator*(mat3x3 const& r_rhs) const;


		/*!***********************************************************************************
		 \brief Zeroes the mat3x3
		 
		*************************************************************************************/
		void Zero();

		/*!***********************************************************************************
		 \brief Set mat3x3 to identity
		 		1 0 0
				0 1 0
				0 0 1
		*************************************************************************************/
		void Identity();

		/*!***********************************************************************************
		 \brief Transposes matrix, row and columns are swapped
		 		Note that it is still in column major
		 		0 3 6		0 1 2
				1 4 7  -->  3 4 5
				2 5 8		6 7 8 
		*************************************************************************************/
		void Transpose();
		
		/*!***********************************************************************************
		 \brief Creates a translation matrix. Will change matrix to identity matrix before setting
		 		t_x and t_y
		 		1 0 t_x
				0 1 t_y
				0 0  1
		 
		 \param[in,out] t_x - x coordinate to translate by 
		 \param[in,out] t_y - y cooridnate to translate by
		*************************************************************************************/
		void Translate(float t_x, float t_y);

		/*!***********************************************************************************
		 \brief Creates a scaling matrix
		 		s_x   0   0
				 0   s_y  0
				 0    0   1
		 
		 \param[in,out] s_x - scaling by x axis
		 \param[in,out] s_y - scaling by y axis
		*************************************************************************************/
		void Scale(float s_x, float s_y);

		/*!***********************************************************************************
		 \brief Creates a rotation matrix from 'angle' which is in radians.
		 
		 \param[in,out] angle - angle of rotation in radians
		*************************************************************************************/
		void RotateRad(float angle);
		
		/*!***********************************************************************************
		 \brief Creates a rotation matrix from 'angle' which is in degrees
		 
		 \param[in,out] angle - angle of rotation in degrees
		*************************************************************************************/
		void RotateDeg(float angle);

		// equations
		/*!***********************************************************************************
		 \brief Get the dot product of two matrices.
		 
		 \param[in,out] r_rhs - mat3x3 to perform dot operation with
		 \return float - dot product of two matrices
		*************************************************************************************/
		float Dot(mat3x3 const& r_rhs) const;
		
		/*!***********************************************************************************
		 \brief Find determinant of mat3x3
		 
		 \return float - Determinant of mat3x3
		*************************************************************************************/
		float Determinant() const;
		
		/*!***********************************************************************************
		 \brief Find adjoint of mat3x3
		 
		 \return mat3x3 - adjoint version of mat3x3
		*************************************************************************************/
		mat3x3 Adjoint() const;
		
		/*!***********************************************************************************
		 \brief Find inverse of mat3x3 by using determinant and adjoint
		 
		 \return mat3x3 - Inverse of this matrix
		*************************************************************************************/
		mat3x3 Inverse() const;
	};

	//! Matrix column 4 row 4 struct
	struct mat4x4
	{
		// ----- Public Variables ----- //
		public:
		float m[16];

		// In column major
		float& _m00 = m[0], & _m01 = m[1], & _m02 = m[2], & _m03 = m[3],
			 & _m10 = m[4], & _m11 = m[5], & _m12 = m[6], & _m13 = m[7],
			 & _m20 = m[8], & _m21 = m[9], & _m22 = m[10], & _m23 = m[11],
			 & _m30 = m[12], & _m31 = m[13], & _m32 = m[14], & _m33 = m[15];

		// ----- Constructors ----- //
		/*!***********************************************************************************
		 \brief Construct a new mat4x4 object with each element initialized to 0.f
		 
		*************************************************************************************/
		mat4x4();
		/*!***********************************************************************************
		 \brief Construct a new mat4x4 object based on input parameters
		 
		 \param[in,out] c0_r0 - element at column 0 row 0
		 \param[in,out] c0_r1 - element at column 0 row 1
		 \param[in,out] c0_r2 - element at column 0 row 2
		 \param[in,out] c0_r3 - element at column 0 row 3
		 \param[in,out] c1_r0 - element at column 1 row 0
		 \param[in,out] c1_r1 - element at column 1 row 1
		 \param[in,out] c1_r2 - element at column 1 row 2
		 \param[in,out] c1_r3 - element at column 1 row 3
		 \param[in,out] c2_r0 - element at column 2 row 0
		 \param[in,out] c2_r1 - element at column 2 row 1
		 \param[in,out] c2_r2 - element at column 2 row 2
		 \param[in,out] c2_r3 - element at column 2 row 3
		 \param[in,out] c3_r0 - element at column 3 row 0
		 \param[in,out] c3_r1 - element at column 3 row 1
		 \param[in,out] c3_r2 - element at column 3 row 2
		 \param[in,out] c3_r3 - element at column 3 row 3
		*************************************************************************************/
		explicit mat4x4(float c0_r0, float c0_r1, float c0_r2, float c0_r3,
			float c1_r0, float c1_r1, float c1_r2, float c1_r3,
			float c2_r0, float c2_r1, float c2_r2, float c2_r3,
			float c3_r0, float c3_r1, float c3_r2, float c3_r3);
		
		/*!***********************************************************************************
		 \brief Copy constructor for mat4x4.
		 
		 \param[in,out] r_cpy - mat4x4 to construct copy of
		*************************************************************************************/
		mat4x4(mat4x4 const& r_cpy);

		// potentially unneeded
		//mat4x4(vec4 const& r_col0, vec4 const& r_col1, vec4 const& r_col2, vec4 const& r_col3);

		// ----- Public Methods ----- //
		public:

		/*!***********************************************************************************
		 \brief Copy assignment for mat4x4
		 
		 \param[in,out] r_cpy - mat4x4 to copy assign
		 \return mat4x4& - reference to this mat4x4, is directly modifiable
		*************************************************************************************/
		mat4x4& operator=(mat4x4 const& r_cpy);

		// ----- Access Operators ----- //
		/*!***********************************************************************************
		 \brief Subscript operator overload to access element in matrix at index
		 
		 \param[in,out] index - the index of the element to read
		 \return float - element at the index
		*************************************************************************************/
		float operator[](unsigned int index) const; // 0-15
		// float operator[][](unsigned int index1, unsigned int index2) const; // [col][row]

		/*!***********************************************************************************
		 \brief Subscript operator overload to access and modify element in matrix at index
		 
		 \param[in,out] index - the index of the element to read
		 \return float& - element at the index which can be directly modified
		*************************************************************************************/
		float& operator[](unsigned int index); // 0-15
		// float& operator[][](unsigned int index1, unsigned int index2); // [col][row]

		// ----- Addition ----- //
		/*!***********************************************************************************
		 \brief Addition operator overload. Sums two mat4x4 together and returns the result
		 
		 \param[in,out] r_rhs - mat4x4 to add to this mat4x4
		 \return mat4x4 - summation of two mat4x4
		*************************************************************************************/
		mat4x4 operator+(mat4x4 const& r_rhs) const;
		/*!***********************************************************************************
		 \brief Addition assignment operator overload. Sums two mat4x4 together and directly modifies
		 		referenced matrix
		 
		 \param[in,out] r_rhs - mat4x4 to add to this mat4x4
		 \return mat4x4& - reference to this mat4x4 post addition
		*************************************************************************************/
		mat4x4& operator+=(mat4x4 const& r_rhs);

		// ----- Subtraction ----- //
		/*!***********************************************************************************
		 \brief Subtraction operator overload. Subtracts mat4x4 from LHS mat4x4
		 
		 \param[in,out] r_rhs - mat4x4 to subtract from this mat4x4
		 \return mat4x4 - mat4x4 after subtracting r_rhs from it
		*************************************************************************************/
		mat4x4 operator-(mat4x4 const& r_rhs) const;
		/*!***********************************************************************************
		 \brief Subtraction assignment operator overload. Subtracts r_rhs from this matrix,
		 		directly modifying it.
		 
		 \param[in,out] r_rhs - mat4x4 to subtract from this mat4x4
		 \return mat4x4& - modified mat4x4 post subtraction
		*************************************************************************************/
		mat4x4& operator-=(mat4x4 const& r_rhs);

		// ----- Scale ----- //
		/*!***********************************************************************************
		 \brief Multiply operator to scale matrix by a float.
		 
		 \param[in,out] scale - amount to multiply each element by
		 \return mat4x4 - the resultant mat4x4 from scaling
		*************************************************************************************/
		mat4x4 operator*(float scale) const;

		/*!***********************************************************************************
		 \brief Division operator to divide matrix by a float.
		 
		 \param[in,out] scale - amount to divide each element by
		 \return mat4x4 - the resultant mat4x4 from scaling
		*************************************************************************************/
		mat4x4 operator/(float scale) const;

		/*!***********************************************************************************
		 \brief Multiply assignment operator to directly modify and scale a mat4x4 by a float.
		 
		 \param[in,out] scale - amount to multiply each element by
		 \return mat4x4& - the referenced mat4x4 post scaling
		*************************************************************************************/
		mat4x4& operator*=(float scale);

		/*!***********************************************************************************
		 \brief Division assignment operator to directly modify and divide a mat4x4 by a float.
		 
		 \param[in,out] scale - amount to divide each element by
		 \return mat4x4& - the referenced mat4x4 post division
		*************************************************************************************/
		mat4x4& operator/=(float scale);

		/*!***********************************************************************************
		 \brief Zeros this mat4x4
		 
		*************************************************************************************/
		void Zero();
		/*!***********************************************************************************
		 \brief Sets this mat4x4 to be identity matrix
		 		1 0 0 0
				0 1 0 0
				0 0 1 0
				0 0 0 1		 
		*************************************************************************************/
		void Identity();
		
		/*!***********************************************************************************
		 \brief Transposes this matrix. Swaps row and column
		 
		*************************************************************************************/
		void Transpose();
	};

	// ----- Non-Member functions ----- //

	// ----- Vec2 Functions ----- //
	/*!***********************************************************************************
	 \brief Calculates the dot product of two vec2
	
	 \param[in] r_lhs  	The vec2 to dot product with r_rhs
	 \param[in] r_rhs  	The vec2 to dot product with r_lhs
	 \return float 		The dot product
	*************************************************************************************/
	float Dot(vec2 const& r_lhs, vec2 const& r_rhs);

	/*!***********************************************************************************
	 \brief Calculates the cross product of two vec2 (cross product magnitude/z-axis)
	
	 \param[in] r_lhs 	The lhs vec2 to cross product with r_rhs
	 \param[in] r_rhs 	rhs vec2 to cross product with r_lhs
	 \return float 		The cross product magnitude
	*************************************************************************************/
	float Cross(vec2 const& r_lhs, vec2 const& r_rhs);

	// ----- Vec3 functions ----- //
	
	/*!***********************************************************************************
	 \brief Caclulates the dot product of the two vec3, and returns a copy of the result.
	
	 \param[in] r_lhs 	The vec3 to dot with r_rhs
	 \param[in] r_rhs 	The vec3 to dot with r_lhs
	 \return float 		The dot product
	*************************************************************************************/
	float Dot(vec3 const& r_lhs, vec3 const& r_rhs);

	/*!***********************************************************************************
	 \brief Calulates the cross product of two vec3, and returns a copy of the result.
	
	 \param[in] r_lhs 	The vec3 to cross with r_rhs
	 \param[in] r_rhs 	The vec3 to cross with r_lhs
	 \return vec3 		The cross product
	*************************************************************************************/
	vec3 Cross(vec3 const& r_lhs, vec3 const& r_rhs);

	//// vec4 functions
	//float Dot(vec4 const& r_lhs, vec4 const& r_rhs);
	//vec4 Cross(vec4 const& r_lhs, vec4 const& r_rhs);

	// ----- Mat3x3 functions ----- //
	/*!***********************************************************************************
	 \brief Get the dot product of two matrices.
	
	 \param[in,out] r_lhs - first mat3x3 in dot operation
	 \param[in,out] r_rhs - second mat3x3 in dot operation
	 \return float - dot product of two matrices
	*************************************************************************************/
	float Dot(mat3x3 const& r_lhs, mat3x3 const& r_rhs);

	/*!***********************************************************************************
	 \brief Converts input angle to degrees
	 
	 \param[in,out] radAngle - angle in radians
	 \return float - radAngle converted to degrees
	*************************************************************************************/
	float ConvertRadToDeg(float radAngle);

	/*!***********************************************************************************
	 \brief Converts input angle to radians
	 
	 \param[in,out] degAngle - angle in degreed
	 \return float - degAngle converted to radians
	*************************************************************************************/
	float ConvertDegToRad(float degAngle);

	/*!***********************************************************************************
	 \brief Clamps a variable within a range
	 
	 \param[in,out] r_varToClamp - variable to clamp
	 \param[in,out] min - the smallest value variable can be
	 \param[in,out] max - the larget value the variable can be
	*************************************************************************************/
	void Clamp(float& r_varToClamp, float min, float max);
	
	/*!***********************************************************************************
	 \brief Wraps a variable within a range. 
	 		ie. when variable is less than min, it becomes max and vice versa
	 
	 \param[in,out] r_varToWrap - variable to wrap
	 \param[in,out] min - minimum value variable becomes when it becomes larger than max
	 \param[in,out] max - maximum value variable becomes when it becomes smaller than min
	*************************************************************************************/
	void Wrap(float& r_varToWrap, float min, float max);
}


