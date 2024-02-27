/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     PauseManager.h
 \date     15-01-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Keep Tracks whether the game is paused or needs to be paused.
	Contains the function to unpause the game as well.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

#include "Singleton.h"

namespace PE
{
	class PauseManager : public Singleton<PauseManager>
	{
		friend class Singleton<PauseManager>;

		public:
			/*!***********************************************************************************
			 \brief Constructor for the PauseManager
			*************************************************************************************/
			PauseManager() = default;
			/*!***********************************************************************************
			 \brief Destructor for the PauseManager
			*************************************************************************************/
			~PauseManager() = default;

			/*!***********************************************************************************
			\brief     Returns whether the game is paused or not.
			\return    bool    True if the game is paused, false if the game is not paused.
			*************************************************************************************/
			bool IsPaused() const { return m_paused; }

			/*!***********************************************************************************
			 \brief     Sets the game to paused or unpaused.
			\param     paused    True if the game is to be paused, false if the game is to be unpaused.
			*************************************************************************************/
			void SetPaused(bool paused) { m_paused = paused; }

		private:
			bool m_paused = false;
	};
}

