/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MemoryManager.h
 \date     09-10-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declerations of the memorymanager

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Singleton.h"

// CONSTANT VARIABLES
constexpr size_t max_size = 1000000;

namespace PE {
	/*!***********************************************************************************
	 \brief				Structure to hold the name of the object borrowing memory and amount borrowed
	 \param				name
	 \param				size
	 \param				bufferSize
	*************************************************************************************/
	struct MemoryData
	{
		std::string name;
		int size; // total size;
		int bufferSize; //the amount of buffer added to ensure safety
		/*!***********************************************************************************
		 \brief							Constructor
		 \param[in]		Std::string		name of the object borrowing memory
		 \param[in]		int size		the amount of memory borrowed
		 \param[in]		int buffersize	the amount of buffer added to ensure safety
		*************************************************************************************/
		MemoryData(std::string name, int size, int buffersize) : name(name), size(size) ,bufferSize(buffersize){}

		std::string ToString() const;
	};

	//a stack allocating system
	class StackAllocator 
	{
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief					Constructor
		 \param[in]				takes in a size, the max size of the stack
		*************************************************************************************/
		StackAllocator(int size = max_size);
		/*!***********************************************************************************
		 \brief					Destructor, deletes the allocated memory
		*************************************************************************************/
		~StackAllocator() { delete[] p_stack;}
		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief					Allocated a given amount of memory, throws an error if allocate out of memory
		 \param[in]				int size the amount of memory to allocate
		 \return				void* pointer to the allocated memory
		*************************************************************************************/
		void* Allocate(int size);
		/*!***********************************************************************************
		 \brief					Free a given amount of memory
		 \param[in]				size the amount of memory to deallocate
		*************************************************************************************/
		void Free(int size);
		// ----- Public getters ----- // 
	public:
		/*!***********************************************************************************
		 \brief					get the stack
		 \return				char* returns a pointer to the bottom of the stack
		*************************************************************************************/
		char* GetStack();
		/*!***********************************************************************************
		 \brief					returns the top of the stack
		 \return				char* returns a pointer to the top of the stack
		*************************************************************************************/
		int GetStackTop();
	private:
		char* p_stack{ nullptr }; //where all the data in the stack will be stored
		int m_totalSize; // size of the stack
		int m_stackTop; // current top of the stack
		//hold a shared pointer to ECS pool allocator
	};

	class MemoryManager : public Singleton <MemoryManager>
	{
		friend class Singleton<MemoryManager>;
		// ----- Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief					Constructor
		*************************************************************************************/
		MemoryManager();
		/*!***********************************************************************************
		 \brief					Destructor
		*************************************************************************************/
		~MemoryManager();
		// ----- Public methods ----- //
	public:
		/*!***********************************************************************************
		 \brief					Allocated a given amount of memory, throws an error if allocate out of memory
		 \param[in]				std::string name the name of the object memory is allocated to
		 \param[in]				size the amount of memory to allocate
		 \return				void* pointer to the allocated memory
		*************************************************************************************/
		void* AllocateMemory(std::string name, int size);
		/*!***********************************************************************************
		 \brief					popback the latest allocated memory
		*************************************************************************************/
		void  Pop_BackMemory();
		/*!***********************************************************************************
		 \brief					check if any of the allocated memory wrote out of the given size
		*************************************************************************************/
		void CheckMemoryOver();
		/*!***********************************************************************************
		 \brief					print all the details of allocated memory
		*************************************************************************************/
		void PrintData();
		// ----- Public getters ----- // 
	private:
		std::vector<MemoryData> m_memoryAllocationData;		//for storing what memory allocated to where
		StackAllocator m_stackAllocator; //the stack allocator
	};



}