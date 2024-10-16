/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatController_v2_0.h
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions that manage cats in a scene.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Logic/Script.h"
#include "CatScript_v2_0.h"

namespace PE
{
	enum EnumUndoType
	{
		UNDO_MOVEMENT,
		UNDO_ATTACK
	};

	struct CatController_v2_0Data{};

	class CatController_v2_0 : public Script
	{
	public:
		// ----- Public Variables ----- //
		EntityID mainInstance;
		
		std::map<EntityID, CatController_v2_0Data> m_scriptData; // data associated with each instance of the script

	public:
		// ----- Public Functions ----- //
		/*!***********************************************************************************
		 \brief					Constructor for CatController_v2_0
		*************************************************************************************/
		CatController_v2_0();
		// ----- Constructor ----- //
		/*!***********************************************************************************
		 \brief					Destructor for CatController_v2_0
		*************************************************************************************/
		virtual ~CatController_v2_0();

		/*!***********************************************************************************
		 \brief					The Initialise Function for a script, to initialize any variables
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief					The update function of the script
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		\brief					The function that is called on the end of the script's lifetime
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Destroy(EntityID id);

		/*!***********************************************************************************
		\brief					The function that is called when the script is attached onto an object
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		\brief					The function that is called when the script is detached from an object
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Sets cat to play death animation, effectively removing it from scene

		 \param[in] id - ID of cat to remove
		*************************************************************************************/
		void KillCat(EntityID id);

		/*!***********************************************************************************
		 \brief Removes cats from current cats in the scene

		 \param[in] id - ID of cat to remove
		*************************************************************************************/
		void RemoveCatFromCurrent(EntityID id);

		/*!***********************************************************************************
		 \brief Updates m_currentCats with all the cats in the scene, including the caged ones

		 \param[in] id - id of the main instance of the cat controller
		*************************************************************************************/
		void UpdateCurrentCats(EntityID id);

		/*!***********************************************************************************
		 \brief Updates m_cachedCats with cats in m_currentCats

		 \param[in] id - id of the main instance of the cat controller
		*************************************************************************************/
		void UpdateCachedCats(EntityID id) { m_cachedCats = m_currentCats; }


		/*!***********************************************************************************
		 \brief Gets Cats that have been collected and are deployable

		 \param[out] std::vector<EnumCatType> - Vector of Deployable Cats
		*************************************************************************************/
		void UpdateDeployableCats(EntityID mainInstanceID);

		/*!***********************************************************************************
		 \brief Checks if entity is a cat

		 \param[in] id - ID to check
		 \param[out] bool - true if is cat, false if not
		*************************************************************************************/
		inline bool IsCat(EntityID id)
		{
			for (auto const& [catID, type] : m_currentCats)
			{
				if (catID == id) // cat is in vector
					return true;
			}
			return false; // is not cat
		}

		/*!***********************************************************************************
		 \brief Checks if cat is caged.

		 \param[in] id - catID to check
		 \param[out] bool - true if cat is caged, false if not
		*************************************************************************************/
		inline bool IsCatCaged(EntityID id)
		{
			return (GETSCRIPTDATA(CatScript_v2_0, id))->isCaged;
		}

		/*!***********************************************************************************
		 \brief Checks if cat is a cat following the main cat

		 \param[in] id - catID to check
		 \param[out] bool - true if cat is following, false if not
		*************************************************************************************/
		bool IsFollowCat(EntityID catID);

		/*!***********************************************************************************
		 \brief Checks if entity is a cat and not caged

		 \param[in] id - catID to check
		 \param[out] bool - true if entity is a cat and is not caged
		*************************************************************************************/
		inline bool IsCatAndNotCaged(EntityID catID)
		{
			return (IsCat(catID) && !IsCatCaged(catID));
		}
		
		/*!***********************************************************************************
		 \brief Adds cat id and the undo to perform to the internal stack

		 \param[in] catID - catID to undo
		 \param[in] undoType - to undo movement or attack
		*************************************************************************************/
		void AddToUndoStack(EntityID catID, EnumUndoType undoType);

		/*!***********************************************************************************
		 \brief Wrapper function for undo call

		 \param[in] id - button for undoing
		*************************************************************************************/
		void UndoCatPlanButtonCall(EntityID id);

		/*!***********************************************************************************
		 \brief Undos the latest action
		*************************************************************************************/
		void UndoCatPlan();

		/*!***********************************************************************************
		 \brief Clears the undo stack
		*************************************************************************************/
		void ClearCatUndoStack();

		// getters
		/*!***********************************************************************************
		 \brief Gets current movement energy of cat

		 \param[in] id - catID to get energy of
		 \param[out] int - energy of the cat
		*************************************************************************************/
		int GetCurrentMovementEnergy(EntityID id);
		
		/*!***********************************************************************************
		 \brief Gets max movement energy of cat

		 \param[in] id - catID to get energy of
		 \param[out] int - energy of the cat
		*************************************************************************************/
		int GetMaxMovementEnergy(EntityID id);

		/*!***********************************************************************************
		 \brief Gets current vector of cat ids and types, including caged cats

		 \param[in] id - id of the cat controller
		 \param[out] std::vector<std::pair<EntityID, EnumCatType>> - current vector of cats
		*************************************************************************************/
		std::vector<std::pair<EntityID, EnumCatType>> GetCurrentCats(EntityID id) { return m_currentCats; }

		/*!***********************************************************************************
		 \brief Gets cached vector of cat ids and types, including caged cats
				(should only be updated at the start of a level)

		 \param[in] id - id of the cat controller
		 \param[out] std::vector<std::pair<EntityID, EnumCatType>> - current vector of cats
		*************************************************************************************/
		std::vector<std::pair<EntityID, EnumCatType>> GetCachedCats(EntityID id) { return m_cachedCats; }

		/*!***********************************************************************************
		 \brief Gets vector of cat types which are deployable 
				(should be saved at the end of the previous level using UpdateDeployment function)

		 \param[in] id - id of the cat controller
		 \param[out] std::vector<EnumCatType> - deplyable vector of cats
		*************************************************************************************/
		std::vector<EnumCatType> GetDeployableCats(EntityID id) { return m_deployableCats; }

		/*!**********************************************************************************
		 \brief Gets the main cat ID

		 \param[out] EntityID - m_mainCatID
		*************************************************************************************/
		EntityID GetMainCatID() { return m_mainCatID; }

		/*!***********************************************************************************
		 \brief Gets script data

		 \param[out] std::map<EntityID, CatController_v2_0Data>& Reference to script data variable
		*************************************************************************************/
		std::map<EntityID, CatController_v2_0Data>& GetScriptData() { return m_scriptData; }

		/*!***********************************************************************************
		 \brief Gets rttr instance of script data

		 \param[out] rttr::instance - rttr instance to script data variable
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

	private:
		EntityID m_mainCatID{ 0 };
		bool m_lostGame;
		std::vector<std::pair<EntityID, EnumCatType>> m_currentCats;
		std::vector<std::pair<EntityID, EnumCatType>> m_cachedCats;
		std::vector<EnumCatType> m_deployableCats;
		std::stack<std::pair<EntityID, EnumUndoType>> m_catUndoStack;
		int m_mouseEventListener{}, m_mouseReleaseEventListener{};
		bool m_mouseClick{ false }, m_mouseClickPrev{ false };

		/*!***********************************************************************************
		 \brief Function to handle mouse click events for Cat_v2_0PLAN

		 \param[in] r_ME - Mouse event data.
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Function to handle mouse release events for Cat_v2_0PLAN

		 \param[in] r_ME - Mouse event data.
		*************************************************************************************/
		void OnMouseRelease(const Event<MouseEvents>& r_ME);
	};
}