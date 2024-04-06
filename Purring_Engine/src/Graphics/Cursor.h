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
			std::shared_ptr<Graphics::Texture> GetAppropriateTexture() const;

			inline vec2 GetPosition() const { return m_position; }

			inline bool GetVisibility() const { return m_isVisible; }

			void SetVisibility(bool _isVisible);

		public:
			inline void SetCursorDefault() { ChangeState(EnumCursorState::DEFAULT);  }

			inline void CursorOnHoverEnter() { ChangeState(EnumCursorState::HOVER); };
			inline void CursorOnHoverExit() { SetCursorDefault(); };

			inline void CursorOnClickEnter() { ChangeState(EnumCursorState::CLICK); };
			inline void CursorOnClickExit() { SetCursorDefault(); };

			inline void ChangeState(EnumCursorState _state) { m_currentState = _state; }

			void Update();

		private:
			vec2 m_position{0.f, 0.f}; // position of the cursor
			EnumCursorState m_currentState{ EnumCursorState::DEFAULT };
			bool m_isVisible{ false };

		}; // End of Cursor class

	} // End of Graphics namespace
} // End of PE namespace