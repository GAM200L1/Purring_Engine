#include "prpch.h"
#include "MemoryManager.h"

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


		return nullptr;
	}

	void* MemoryManager::AllocateMemory(std::string name, int size)
	{
		try 
		{
			void* newptr = m_stackAllocator.Allocate(size);
			m_memoryAllocationData.push_back(MemoryData(name, size));
			return newptr;
		}
		catch (int i) {
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


	void* StackAllocator::Allocate(int size)
	{
		if (m_stackTop + size <= m_totalSize)
		{
			void* newPtr = m_stack + m_stackTop;
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


}