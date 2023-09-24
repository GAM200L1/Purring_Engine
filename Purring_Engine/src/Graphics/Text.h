#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Text.h
 \date     23-09-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the declarations of functions and classes required for
		   rendering text.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "glm/glm.hpp"
#include "Graphics/Texture.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Camera.h"

namespace PE
{
	// Holds all state information relevant to a character as loaded using FreeType
	struct Character
	{
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2 Size;		// Size of glyph
		glm::ivec2 Bearing;		// Offset from baseline to left/top of glyph
		unsigned int Advance;	// Horizontal offset to next glyph
	};

	// A renderer class for rendering text displayed by a font loaded using the 
	// FreeType library. A single font is loaded, processed into a list of Character
	// items for later rendering.
	class Font
	{
	public:
		std::map<char, Character> Characters;

		Font() = default;
		void Init(std::shared_ptr<Graphics::ShaderProgram>); // load the path of the font we taking

		void Load(const std::string& fontpath, unsigned int fontsize = 50);

		void RenderText(const std::string& text, glm::vec2 position, float scale, Graphics::Camera& camera, glm::vec3 color = glm::vec3(1.0f));

	private:
		std::shared_ptr<Graphics::ShaderProgram> TextShader;
		unsigned int mVao{ 0 }, mVbo{ 0 };

	};
}