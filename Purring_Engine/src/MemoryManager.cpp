#include "prpch.h"
#include "MemoryManager.h"
#include "cstdio"
namespace PE
{
	std::unique_ptr<MemoryManager> MemoryManager::s_Instance = nullptr;

	MemoryManager::MemoryManager()
	{
	}

	MemoryManager::~MemoryManager()
	{
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
			std::cout << "trying to alloccate now" << std::endl;
			int buffer = (size + 1) / 2;
			void* newptr = m_stackAllocator.Allocate(size + buffer);
			m_memoryAllocationData.push_back(MemoryData(name, size));
			std::cout << "memoryAllocated of size: " << size << " to: " << name << std::endl;
			std::cout << "allocated buffer of " << buffer << std::endl;
			return newptr;
		}
		catch (int i) {
			std::cout << "error allocating" << std::endl;
			throw; //error message
			//maybe dont need throw here if we catch outside but if we throw we send error here
		}
	}

	void MemoryManager::Pop_BackMemory()
	{
		try
		{
			m_stackAllocator.Free(m_memoryAllocationData[m_memoryAllocationData.size()].s_size);
		}
		catch (int i) {
			throw; //error message
			//maybe dont need throw here if we catch outside but if we throw we send error here
		}
	}

	void MemoryManager::CheckMemoryOver()
	{
		//loop through the entire memory stack from 0
		for (int i = 0; i < m_memoryAllocationData.size(); i++) {
			if (*(m_stackAllocator.getStack() + m_memoryAllocationData[i].s_size) != '\0')
			{
				std::cout<< m_memoryAllocationData[i].s_name << " Writing into more than allocated spaces!" << std::endl;
			}
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
			//whatever debug code for out of memory
			//we cannot go over max
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


}