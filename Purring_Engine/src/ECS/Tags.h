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
#include <Singleton.h>
#include <bitset>
#include <tuple>
#include <string>
#include <map>
#include <vector>
// CONSTANT VARIABLES

namespace
{
	constexpr int MAX = 32;
	using EntityID = size_t;
	struct Comparer 
	{
		/*!***********************************************************************************
		 \brief Compares the bitsets lhs < rhs value

		 \param[in] b1 bitset lhs
		 \param[in] b2 bitset rhs
		 \return true 	lhs < rhs
		 \return false  rhs > lhs
		*************************************************************************************/
		bool operator() (const std::bitset<MAX>& b1, const std::bitset<MAX>& b2) const {
			return b1.to_ulong() < b2.to_ulong();
		}
	};
};

namespace PE
{
	// up to 32 bits for tagging
	using Tag = std::bitset<MAX>;

	class TagManager : public Singleton<TagManager>
	{
	//----- Public Variables -----//
	public:
		friend class Singleton<TagManager>;
	//----- Public Getters -----//
	public:
		std::vector<EntityID> GetEntities(const Tag& r_tags) const;
		const std::string& GetTagName(const Tag& r_tag) const;

	//----- Public Methods -----//
	public:
		bool HasTag(const EntityID& r_id, const Tag& r_tags)  const;

		bool ValidTag(const Tag& r_tag) const;

		bool ValidTag(const std::string& r_name) const;

		bool AddTag(const std::string& r_name);
		bool TagManager::RenameTag(const Tag& r_oldName, const std::string& r_newName);
		bool TagManager::RenameTag(const std::string& r_oldName, const std::string& r_newName);


		Tag GetTag(const std::string& r_name) const;

		size_t GetTagPos(const std::string& r_name) const;
		size_t GetTagPos(const Tag& r_tag) const;


		void EraseTag(const Tag& r_tag);

		void EraseTag(const std::string& r_name);

		// out ward facing stuffs
		void AssignTag(EntityID id, const std::string& r_name);

		void RemoveTag(EntityID id, const std::string& r_name);
	//----- Private Methods -----//
	private:
		
	//----- Private Variables -----//
	private:
		std::map<Tag, std::string, Comparer> m_tags;
		size_t m_numTags{};
	};
};
