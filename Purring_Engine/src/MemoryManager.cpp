/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     MemoryManager.cpp
 \date     9/10/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	cpp file containing the definitions of MemoryManager

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#include "prpch.h"
#include "MemoryManager.h"
#include "cstdio"
#include "Imgui/ImGuiWindow.h"

namespace PE
{
	std::unique_ptr<MemoryManager> MemoryManager::s_Instance = nullptr;

	MemoryManager::MemoryManager()
	{
	}

	MemoryManager::~MemoryManager()
	{
		//m_stackAllocator.~StackAllocator();
	}

	MemoryManager* MemoryManager::GetInstance()
	{
		if (!s_Instance)
			s_Instance = std::make_unique<MemoryManager>();


		return s_Instance.get();
	}

	void* MemoryManager::AllocateMemory(std::string name, int size)
	{
		try
		{
			std::stringstream ss1;
			ss1 << "currently allocated: " << m_stackAllocator.getStackTop();
			ImGuiWindow::GetInstance()->addConsole(ss1.str());

			ImGuiWindow::GetInstance()->addConsole("trying to allocate more memory now");

			//determine buffer size incase of writing over
			int buffer = (size + 1) / 2;
			//attempt to allocate memory
			void* newptr = m_stackAllocator.Allocate(size + buffer);

			//save data of allocated memory
			m_memoryAllocationData.push_back(MemoryData(name, size, buffer));

			//print to console
			std::stringstream ss;
			ss << "memory allocated of size: " << size << " to: " << name << " along with buffer of: " << buffer;
			ImGuiWindow::GetInstance()->addConsole(ss.str());

			return newptr;
		}
		catch (int i) {
			//sending error to logs
			std::stringstream ss;
			ss << "memory cannot be allocated to " << name;
			ImGuiWindow::GetInstance()->addError(ss.str());
			return nullptr;
		}
	}

	void MemoryManager::Pop_BackMemory()
	{
		try
		{
			m_stackAllocator.Free(m_memoryAllocationData[m_memoryAllocationData.size()-1].s_size + m_memoryAllocationData[m_memoryAllocationData.size() - 1].s_bufferSize);
			m_memoryAllocationData.pop_back();
		}
		catch (int i) {
			throw; //error message
			//maybe dont need throw here if we catch outside but if we throw we send error here
		}
	}

	void MemoryManager::CheckMemoryOver()
	{
		//loop through the entire memory stack from 0
		int totalMemory = 0;
		
		for (int i = 0; i < m_memoryAllocationData.size(); i++) {
			if (*(m_stackAllocator.getStack() + m_memoryAllocationData[i].s_size + totalMemory) != '\0')
			{
				std::stringstream ss;
				ss << m_memoryAllocationData[i].s_name << " Writing into more than allocated spaces!" << std::endl;
				ImGuiWindow::GetInstance()->addError(ss.str());
			}
			totalMemory += m_memoryAllocationData[i].s_size + m_memoryAllocationData[i].s_bufferSize;
		}
	}

	void MemoryManager::printData()
	{
		std::stringstream ss;
		ss << "currently allocated: " << m_stackAllocator.getStackTop();
		ImGuiWindow::GetInstance()->addConsole(ss.str());

		for (int i = 0; i < m_memoryAllocationData.size(); i++) {
			ImGuiWindow::GetInstance()->addConsole(m_memoryAllocationData[i].ToString());
		}
	}


	StackAllocator::StackAllocator(int size) : m_totalSize(size), m_stackTop(0)
	{
		static int test = 1;
		std::cout << "Stack Allocator launched: " << test++ << " size: " << size << std::endl;
		m_stack = new char[size]();
		memset(m_stack, '\0', size);
	}

	void* StackAllocator::Allocate(int size)
	{
		if (m_stackTop + size <= m_totalSize)
		{
			void* newPtr = (m_stack + m_stackTop);
			m_stackTop += size;
			return newPtr;
		}
		else 
		{
			throw 0;
			return nullptr;
		}
	}

	void StackAllocator::Free(int size)
	{
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

	char* StackAllocator::getStack()
	{
		return m_stack;
	}

	int StackAllocator::getStackTop()
	{
		return m_stackTop;
	}


	std::string MemoryData::ToString() const
	{
		std::stringstream ss;
		ss << s_name << " has been allocated: " << s_size << " with a buffer of " << s_bufferSize;
		return ss.str();
	}

}