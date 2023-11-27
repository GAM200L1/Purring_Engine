/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Singleton.h
 \date     24-09-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the implementation of a Singleton template class.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

namespace PE
{
	template <typename T>
	class Singleton
	{
	public: 

		/*!***********************************************************************************
		 \brief     Initializes a static instance of type T and returns it.

		 \tparam T     The type of singleton being initialized
		 \return T&    This static instance to be returned.
		*************************************************************************************/
		static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		//! Disable copy and assignment
		Singleton(const Singleton&) = delete;
		Singleton & operator= (const Singleton&) = delete;

	protected:
		/*!***********************************************************************************
		 \brief Constructor for the Singleton
		*************************************************************************************/
		Singleton() = default;
		/*!***********************************************************************************
		 \brief Destructor for the Singleton
		*************************************************************************************/
		~Singleton() = default;
	};
}