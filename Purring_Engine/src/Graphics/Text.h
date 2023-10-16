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
#include "Graphics/EditorCamera.h"

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

	class Font
	{
	public:
		std::map<char, Character> Characters;

		/*!***********************************************************************************
			\brief default constructor of the font class
		*************************************************************************************/
		Font() = default;

		/*!***********************************************************************************
			\brief Init font class with vbo and vao

			\param[in] shaderProgram Shader program for rendering text.
		*************************************************************************************/
		void Init(std::shared_ptr<Graphics::ShaderProgram> shaderProgram);

		/*!***********************************************************************************
			\brief Loads character glyph from font into map of characters.

			\param[in] r_fontPath File path of font.
			\param[in] fontSize Size of pixel to extract.

			return
		*************************************************************************************/
		bool Load(std::string const& r_fontPath, unsigned int fontSize = 45);

		/*!***********************************************************************************
			\brief Renders text from r_text parameter. Retrieves glyph information from map
				   and renders a quad with the data.

			\param[in] r_text String to render.
			\param[in] position Position of text to render onto the screen.
			\param[in] scale Amount to scale text size.
			\param[in] r_worldToNdc Projection matrix for transforming vertex coordinates of quad
			\param[in] r_color Color to render text as.
		*************************************************************************************/
		void RenderText(std::string const& r_text, glm::vec2 position, float scale, glm::mat4 const& r_worldToNdc, glm::vec3 const& r_color = glm::vec3(0.f));

	private:
		std::shared_ptr<Graphics::ShaderProgram> TextShader;
		unsigned int m_vertexArrayObject{ 0 }, m_vertexBufferObject{ 0 };

	};
}