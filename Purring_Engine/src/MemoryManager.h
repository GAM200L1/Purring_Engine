/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     MemoryManager.h
 \date     9/10/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
Header file containing the declaration and definition of the event and nmemory manager class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
// CONSTANT VARIABLESs
constexpr size_t max_size = 1000000;

namespace PE {
	//Need to create a memory manager system that calls the stack allocator
	//memory manager is to track amount of memory allocated and who was it allocated to
	//maybe should create some function to calculated amt of memory in the memory allocated is used as well?

	//stores an array of structure
	//contains string name
	//contains amount of memory allocated
	//because its an array, its already indexed? or do we still want to add an id

	//store the amount of memory allocated to a given name on the engine
	struct MemoryData
	{
		std::string s_name;
		int s_size; // total size;
		int s_bufferSize; // (10 + 2 - 1) / 2 = 11/2 = 5
		MemoryData(std::string name, int size) : s_name(name), s_size(size) ,s_bufferSize((size + 2 - 1)/2){}
	};

	//Allocator
	class StackAllocator
	{
	public:
		StackAllocator(int size = max_size) : m_totalSize(size), m_stackTop(0)
		{
			static int test = 1;
			std::cout << "Stack Allocator launched: "<<test++<<"size: "<<size << std::endl;
			m_stack = new char[size]();
			memset(m_stack, '\0', size);
		}

		~StackAllocator() { delete[] m_stack; }

		void* Allocate(int size);

		void Free(int size);

		char* getStack();
	private:
		char* m_stack{ nullptr }; //where all the data in the stack will be stored
		int m_totalSize; // size of the stack
		int m_stackTop; // current top of the stack
		//hold a shared pointer to ECS pool allocator
	};

	class MemoryManager
	{
	public:
		MemoryManager();
		~MemoryManager();


		static MemoryManager* GetInstance();

		//allocate a chunk of memory size in bytes, name for the system getting the memory
		void* AllocateMemory(std::string name, int size);

		//free the latest used chunk of memory
		void  Pop_BackMemory();

		void CheckMemoryOver();

	private:
		//for storing what memory allocated to where
		std::vector<MemoryData> m_memoryAllocationData;
		StackAllocator m_stackAllocator;

		//make single instance of memory manager to ensure 
		//to ensure single instance of stack allocator
		static std::unique_ptr<MemoryManager> s_Instance;
	};



}