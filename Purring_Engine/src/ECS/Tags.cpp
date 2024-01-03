/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Tags.h
 \date     01-03-2024
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief     Contains the implementation of ComponentPool. 
 
 
 All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

// INCLUDES
#include "prpch.h"
#include "Tags.h"
#include "SceneView.h"
// CONSTANT VARIABLES

namespace PE
{
	std::vector<EntityID> TagManager::GetEntities(const Tag& r_tags) const 
	{
		std::vector<EntityID> ret;
		for (auto id : SceneView<EntityDescriptor>())
		{
			if (HasTag(id, r_tags))
			{
				ret.emplace_back(id);
			}
		}
		return ret;
	}

	const std::string& TagManager::GetTagName(const Tag& r_tag) const
	{
		if (!ValidTag(r_tag))
			return std::move(std::string());

		return m_tags.at(r_tag);
	}

	bool TagManager::HasTag(const EntityID& r_id, const Tag& r_tag)  const
	{
		size_t ret = 0;
		const Tag& tag = EntityManager::GetInstance().Get<EntityDescriptor>(r_id).tags;
		for (size_t i{}; i < MAX; ++i)
		{
			if (ret > r_tag.count())
				break;
			ret = (r_tag.test(i) == tag.test(i));
		}
		return ret == r_tag.count();
	}

	bool TagManager::ValidTag(const Tag& r_tag) const
	{
		return m_tags.count(r_tag);
	}

	bool TagManager::ValidTag(const std::string& r_name) const
	{
		for (size_t i{}; i < m_numTags; ++i)
		{
			if (m_tags.at(Tag().set(i)) == r_name)
				return true;
		}
		return false;
	}

	bool TagManager::AddTag(const std::string& r_name)
	{
		if (m_numTags == MAX)
			return false;
		m_tags[Tag().set(m_numTags++)] = r_name;
		return true;
	}

	bool TagManager::RenameTag(const Tag& r_tag, const std::string& r_name)
	{
		if (ValidTag(r_tag))
		{
			if (!ValidTag(r_name))
			{
				m_tags[r_tag] = r_name;
				return true;
			}
		}
		return false;
	}

	bool TagManager::RenameTag(const std::string& r_oldName, const std::string& r_newName)
	{
		if (ValidTag(r_oldName))
		{
			if (!ValidTag(r_newName))
			{
				m_tags[GetTag(r_oldName)] = r_newName;
				return true;
			}
		}
		return false;
	}

	Tag TagManager::GetTag(const std::string& r_name) const
	{
		for (auto [k, v] : m_tags)
		{
			if (v == r_name)
				return k;
		}
		return std::move(Tag().flip());
	}

	size_t TagManager::GetTagPos(const std::string& r_name) const
	{
		for (auto [k, v] : m_tags)
		{
			if (v == r_name)
			{
				for (size_t i{}; i < k.size(); ++i)
				{
					if (k.test(i))
						return i;
				}
			}
		}
		return ULLONG_MAX;
	}

	size_t TagManager::GetTagPos(const Tag& r_tag) const
	{
		for (size_t i{}; i < r_tag.size(); ++i)
		{
			if (r_tag.test(i))
				return i;
		}
		return ULLONG_MAX;
	}

	void TagManager::EraseTag(const Tag& r_tag)
	{
		if (ValidTag(r_tag))
		{
			Tag lastTag;
			lastTag.set(m_numTags - 1);
			m_tags[r_tag] = m_tags[lastTag];
			for (auto id : SceneView<EntityDescriptor>())
			{
				EntityDescriptor& desc = EntityManager::GetInstance().Get<EntityDescriptor>(id);
				size_t lastTagPos = GetTagPos(lastTag);
				desc.tags.set(GetTagPos(r_tag), desc.tags[lastTagPos]);
				desc.tags.set(lastTagPos, false);
			}
			m_tags.erase(lastTag);
			--m_numTags;
		}
	}

	void TagManager::EraseTag(const std::string& r_name)
	{
		if (ValidTag(r_name))
		{
			EraseTag(GetTag(r_name));
		}
	}

	void TagManager::AssignTag(EntityID id, const std::string& r_name)
	{
		if (!ValidTag(r_name))
		{
			if (!AddTag(r_name))
				throw;
		}
		EntityManager::GetInstance().Get<EntityDescriptor>(id).tags.set(GetTagPos(r_name));
	}

	void TagManager::RemoveTag(EntityID id, const std::string& r_name)
	{
		if (!ValidTag(r_name))
			return;

		EntityManager::GetInstance().Get<EntityDescriptor>(id).tags.set(GetTagPos(r_name), false);
	}
};
