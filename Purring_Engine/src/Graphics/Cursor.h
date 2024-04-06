/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Cursor.h
 \date     06-04-2023

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief     This file contains the Cursor class, which is used to render a custom cursor.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include "Math/Transform.h"
#include "Singleton.h"
#include "WindowManager.h"
#include "Texture.h"

namespace PE
{
	namespace Graphics
	{
		class Cursor : public Singleton<Cursor>
		{
		public:
			enum class EnumCursorState : char
			{
				DEFAULT,
				HOVER,
				CLICK,
				STATE_COUNT
			};

		public:

			/*!***********************************************************************************
			\brief  Returns the pointer to the texture based on the cursor state.

			\return std::shared_ptr<Graphics::Texture> Returns the pointer to the texture based 
				on the cursor state.
			*************************************************************************************/
			std::shared_ptr<Graphics::Texture> GetAppropriateTexture() const;

			/*!***********************************************************************************
			\brief  Returns the position the cursor tip should be at.

			\return vec2 Position the cursor tip should be at.
			*************************************************************************************/
			inline vec2 GetPosition() const { return m_position; }

			/*!***********************************************************************************
			\brief  Returns true if the custom cursor is visible and should be rendered.

			\return bool Returns true if the custom cursor is visible and should be rendered.
			*************************************************************************************/
			inline bool GetVisibility() const { return m_isVisible; }

			/*!***********************************************************************************
			\brief  Set to true to enable the custom cursor and disable the default cursor.
				Set to false for vice versa.

			\param[in] Set to true to enable the custom cursor and disable the default cursor.
				Set to false for vice versa.
			*************************************************************************************/
			void SetVisibility(bool _isVisible);

		public:
			/*!***********************************************************************************
			\brief Set the cursor to the default state.
			*************************************************************************************/
			inline void SetCursorDefault() { ChangeState(EnumCursorState::DEFAULT);  }

			/*!***********************************************************************************
			\brief Set the cursor to the hover state. Called when the LMB is clicked.
			*************************************************************************************/
			inline void CursorOnHoverEnter() { ChangeState(EnumCursorState::HOVER); };

			/*!***********************************************************************************
			\brief Set the cursor back to the default state.
			*************************************************************************************/
			inline void CursorOnHoverExit() { SetCursorDefault(); };

			/*!***********************************************************************************
			\brief Set the cursor to the clicked state. 
			*************************************************************************************/
			inline void CursorOnClickEnter() { ChangeState(EnumCursorState::CLICK); };

			/*!***********************************************************************************
			\brief Set the cursor back to the default state.
			*************************************************************************************/
			inline void CursorOnClickExit() { SetCursorDefault(); };

			/*!***********************************************************************************
			\brief Set the state to the state passed in. The state affects the texture rendered
			*************************************************************************************/
			inline void ChangeState(EnumCursorState _state) { m_currentState = _state; }

			/*!***********************************************************************************
			\brief Updates the position of the cursor. Called every frame.
			*************************************************************************************/
			void Update();

		private:
			vec2 m_position{0.f, 0.f}; // position of the cursor
			EnumCursorState m_currentState{ EnumCursorState::DEFAULT }; // State of the cursor. Affects which texture renders.
			bool m_isVisible{ false }; // True if the custom cursor should be rendered and the default cursor should be hidden. False for vice versa.

		}; // End of Cursor class

	} // End of Graphics namespace
} // End of PE namespace