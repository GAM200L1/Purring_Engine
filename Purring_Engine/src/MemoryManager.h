/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     MemoryManager.h
 \date     9/10/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declerations of the memorymanager

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
// CONSTANT VARIABLES
constexpr size_t max_size = 1024;

namespace PE {

	struct MemoryData
	{
		std::string s_name;
		int s_size; // total size;
		int s_bufferSize; // (10 + 2 - 1) / 2 = 11/2 = 5
		MemoryData(std::string name, int size, int buffersize) : s_name(name), s_size(size) ,s_bufferSize(buffersize){}

		std::string ToString() const;
	};

	//Allocator
	class StackAllocator
	{
	public:
		StackAllocator(int size = max_size);

		~StackAllocator() { delete[] m_stack; }

		void* Allocate(int size);

		void Free(int size);

		char* getStack();

		int getStackTop();
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

		void printData();
	private:
		//for storing what memory allocated to where
		std::vector<MemoryData> m_memoryAllocationData;
		StackAllocator m_stackAllocator;

		//make single instance of memory manager to ensure 
		//to ensure single instance of stack allocator
		static std::unique_ptr<MemoryManager> s_Instance;
	};



}