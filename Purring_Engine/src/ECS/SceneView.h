#pragma once
#include "prpch.h"
#include "Entity.h"
#include "Components.h"


namespace Engine
{
	template<typename... ComponentTypes>
	struct SceneView
	{
		SceneView() : p_entityManager(Engine::g_entityManager)
		{
			if constexpr (sizeof...(ComponentTypes))
			{
				std::initializer_list<ComponentID> componentIDs = { p_entityManager->GetComponentID<ComponentTypes>() ... };
				for (const ComponentID& c : componentIDs)
				{
					m_components.emplace_back(c);
				}
			}
			else
			{
				m_all = true;
			}
		}

		struct Iterator
		{
			Iterator(EntityID index, const std::vector<ComponentID>& components, bool all) :
				p_entityManager(Engine::g_entityManager), m_index(index), m_components(components), m_all(all)
			{
			}

			EntityID operator* () const
			{
				return m_index;
			}

			bool operator== (const Iterator& rhs) const
			{
				return (m_index == rhs.m_index) || (m_index == p_entityManager->Size());
			}

			bool operator!= (const Iterator& rhs) const
			{
				return (m_index != rhs.m_index) || (m_index != p_entityManager->Size());
			}

			bool HasComponents() const
			{
				for (const ComponentID& components : m_components)
				{
					if (!p_entityManager->GetComponentPoolPtr(components)->HasEntity(m_index))
						return false;
				}
				return true;
			}

			bool ValidIndex() const
			{
				return (p_entityManager->IsEntityValid(m_index)) &&
					(m_all || (HasComponents()));
			}

			Iterator& operator++()
			{
				do
				{
					++m_index;
				} while (m_index < p_entityManager->Size() && !ValidIndex());
				return *this;
			}

			Engine::EntityManager* p_entityManager;
			EntityID m_index;
			const std::vector<ComponentID>& m_components;
			bool m_all{ false };
		};

		bool HasComponents(size_t index) const
		{
			for (const ComponentID& components : m_components)
			{
				if (!p_entityManager->GetComponentPoolPtr(components)->HasEntity(index))
					return false;
			}
			return true;
		}


		const Iterator begin() const
		{
			if (!p_entityManager->Size())
				throw; // update to error log
			size_t firstIndex{};
			while (firstIndex < p_entityManager->Size() &&
				(!HasComponents(firstIndex)) ||
				(!p_entityManager->IsEntityValid(firstIndex))
				)
			{
				++firstIndex;
			}
			return Iterator(firstIndex, m_components, m_all);
		}

		const Iterator end() const
		{
			return Iterator(p_entityManager->Size(), m_components, m_all);
		}

		Engine::EntityManager* p_entityManager{ nullptr };
		std::vector<ComponentID> m_components;
		bool m_all{ false };
	};
}