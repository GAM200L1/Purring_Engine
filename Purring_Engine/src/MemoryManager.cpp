/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MemoryManager.cpp
 \date     09-10-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	cpp file containing the definitions of MemoryManager

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "MemoryManager.h"
#include "cstdio"
#include "Editor/Editor.h"

namespace PE
{
	MemoryManager::MemoryManager(){}
	MemoryManager::~MemoryManager(){}


	void* MemoryManager::AllocateMemory(std::string name, int size)
	{
		try
		{
			//printing total allocated memory
			std::string s("Currently allocated: ");
			s += std::to_string(m_stackAllocator.GetStackTop());
			Editor::GetInstance().AddInfoLog(s);
			Editor::GetInstance().AddInfoLog("trying to allocate more memory now");

			//determine buffer size incase of writing over
			int buffer = (size + 1) / 2;
			//attempt to allocate memory
			void* p_newptr = m_stackAllocator.Allocate(size + buffer);

			//save data of allocated memory
			m_memoryAllocationData.push_back(MemoryData(name, size, buffer));

			//print to console how much was allocated
			std::stringstream ss;
			ss << "memory allocated of size: " << size << " to: " << name << " along with buffer of: " << buffer;
			Editor::GetInstance().AddInfoLog(ss.str());

			return p_newptr;
		}
		catch (int i) {
			i; //stop unreference
			std::string ss("memory cannot be allocated to ");
			ss += name;
			Editor::GetInstance().AddErrorLog(ss);
			return nullptr;
		}
	}

	void MemoryManager::Pop_BackMemory()
	{
		try
		{
			//freeing the latest allocated memory, have to give the size + buffer
			m_stackAllocator.Free(m_memoryAllocationData[m_memoryAllocationData.size()-1].size + m_memoryAllocationData[m_memoryAllocationData.size() - 1].bufferSize);
			m_memoryAllocationData.pop_back();
		}
		catch (int i) {
			throw i;
		}
	}

	void MemoryManager::CheckMemoryOver()
	{
		//loop through the entire memory stack from 0
		int totalMemory = 0;
		
		//loop through all allocated objects
		for (int i = 0; i < m_memoryAllocationData.size(); i++) 
		{
			//if written over buffer
			if (*(m_stackAllocator.GetStack() + m_memoryAllocationData[i].size + totalMemory) != '\0')
			{
				std::string s(m_memoryAllocationData[i].name);
				s += " Writing into more than allocated spaces!";
				Editor::GetInstance().AddErrorLog(s);
			}

			//go to next object
			totalMemory += m_memoryAllocationData[i].size + m_memoryAllocationData[i].bufferSize;
		}
	}

	void MemoryManager::PrintData()
	{
		//print total allocated memory
		std::string s("Currently allocated: ");
		s += std::to_string(m_stackAllocator.GetStackTop());
		Editor::GetInstance().AddInfoLog(s);

		//print structure data
		for (int i = 0; i < m_memoryAllocationData.size(); i++) {
			Editor::GetInstance().AddInfoLog(m_memoryAllocationData[i].ToString());
		}
	}


	StackAllocator::StackAllocator(int size) : m_totalSize(size), m_stackTop(0)
	{
		//allocate memory based on given size
		p_stack = new char[size]();
		//initialize values just incase
		memset(p_stack, '\0', size);
	}

	void* StackAllocator::Allocate(int size)
	{
		//if trying to allocate over max size
		if (m_stackTop + size <= m_totalSize)
		{
			void* p_newPtr = (p_stack + m_stackTop);
			m_stackTop += size;
			return p_newPtr;
		}
		else 
		{
			throw 0;
		}
	}

	void StackAllocator::Free(int size)
	{
		//if trying to free over 0
		if (m_stackTop != 0 && size < m_stackTop)
		{
			m_stackTop -= size;
		}
		else
		{
			throw 0;
			//error, we cannot go negative
		}
	}

	char* StackAllocator::GetStack()
	{
		return p_stack;
	}

	int StackAllocator::GetStackTop()
	{
		return m_stackTop;
	}


	std::string MemoryData::ToString() const
	{
		std::stringstream ss;
		ss << name << " has been allocated: " << size << " with a buffer of " << bufferSize;
		return ss.str();
	}

}