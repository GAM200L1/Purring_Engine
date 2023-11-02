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
		unsigned int m_vertexArrayObject{ 0 }, m_vertexBufferObject{ 0 };

		/*!***********************************************************************************
			\brief default constructor of the font class
		*************************************************************************************/
		Font() = default;

		/*!***********************************************************************************
			\brief Init font class with vbo and vao

			\param[in] shaderProgram Shader program for rendering text.
		*************************************************************************************/
		bool Initialize(std::string const& r_fontPath, unsigned int fontSize = 45);

		/*!***********************************************************************************
			\brief Loads character glyph from font into map of characters.

			\param[in] r_fontPath File path of font.
			\param[in] fontSize Size of pixel to extract.

			return
		*************************************************************************************/
		bool Load(std::string const& r_fontPath, unsigned int fontSize = 45);
		};

	class TextComponent
	{

	public:
		inline std::shared_ptr<const Font> GetFont() const { return m_font; }

		inline std::string const& GetFontKey() const { return m_fontKey; }

		inline std::string const& GetText() const { return m_text; }

		inline float GetSize() const { return m_size; }

		inline glm::vec4 const& GetColor() const { return m_color; }

		void SetFont(std::string fontKey);

		void SetText(std::string const& text);

		void SetSize(float size);

		void SetColor(glm::vec4 const& color);

	private:
		std::string m_text;
		std::string m_fontKey;
		std::shared_ptr<Font> m_font;
		// font style
		float m_size{ 1.f };
		// line spacing
		// paragraph
		// alignment
		// h_overflow
		// v_overflow
		glm::vec4 m_color{ 0.f, 0.f, 0.f, 1.f};
		// material
	};
}