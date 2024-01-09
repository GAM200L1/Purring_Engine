#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Text.h
 \date     23-09-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu
 \par      code %:     90% 
 \par      changes:    Majority of the code 

 \co-author            FOONG Jun Wei	 
 \par      email:      f.junwei\@digipen.edu 
 \par      code %:     10% 
 \par      changes:    ToJson() & Deserialize() 

 \brief    This file contains the declarations of functions and classes required for
		   rendering text.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "glm/glm.hpp"
#include "Graphics/Texture.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/EditorCamera.h"
#include "Data/json.hpp"
#include "rttr/registration.h"
#include "ECS/Entity.h"
#include <iostream>

namespace PE
{
	// Holds all state information relevant to a character as loaded using FreeType
	struct Character
	{
		unsigned int textureID; // ID handle of the glyph texture
		glm::ivec2 size;		// Size of glyph
		glm::ivec2 bearing;		// Offset from baseline to left/top of glyph
		unsigned int advance;	// Horizontal offset to next glyph
	};

	class Font
	{
	public:
		std::map<char, Character> characters;
		unsigned int vertexArrayObject{ 0 }, vertexBufferObject{ 0 };
		float lineHeight{ 0.f };

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

	enum class EnumTextAlignment { LEFT, RIGHT, TOP, BOTTOM, CENTER };
	enum class EnumTextOverflow { WRAP, OVERFLOW_, TRUNCATE };

	struct TextBox
	{
		vec2 position{ 0.f, 0.f };
		float width{ 200.f };
		float height{ 100.f };
	};

	class TextComponent
	{

	public:
		/*!***********************************************************************************
		 \brief Get the Font 
		 
		 \return std::shared_ptr<const Font> 
		*************************************************************************************/
		inline std::shared_ptr<Font> GetFont() const { return m_font; }

		/*!***********************************************************************************
		 \brief Get the Font Key 
		 
		 \return std::string const& 
		*************************************************************************************/
		inline std::string const& GetFontKey() const { return m_fontKey; }

		/*!***********************************************************************************
		 \brief Get the Text 
		 
		 \return std::string const& 
		*************************************************************************************/
		inline std::string const& GetText() const { return m_text; }

		/*!***********************************************************************************
		 \brief Get the Size 
		 
		 \return float 
		*************************************************************************************/
		inline float GetSize() const { return m_size; }

		/*!***********************************************************************************
		 \brief Get the Color
		 
		 \return glm::vec4 const& 
		*************************************************************************************/
		inline glm::vec4 const& GetColor() const { return m_color; }

		/*!***********************************************************************************
		 \brief Get the horizontal alignment of the text

		 \return EnumTextAlignment - the horizontal alignment of the text
		*************************************************************************************/
		inline EnumTextAlignment GetHAlignment() const { return m_hAlignment; }

		/*!***********************************************************************************
		 \brief Get the vertical alignment of the text

		 \return EnumTextAlignment - the vertical alignment of the text
		*************************************************************************************/
		inline EnumTextAlignment GetVAlignment() const { return m_vAlignment; }

		/*!***********************************************************************************
		 \brief Get the horizontal overflow of the text

		 \return EnumTextAlignment - the horizontal overflow of the text
		*************************************************************************************/
		inline EnumTextOverflow GetHOverflow()  const { return m_hOverflow; }

		/*!***********************************************************************************
		 \brief Get the vertical overflow of the text

		 \return EnumTextAlignment - the vertical overflow of the text
		*************************************************************************************/
		inline EnumTextOverflow GetVOverflow()  const { return m_vOverflow; }

		/*!***********************************************************************************
		 \brief Get the line spacing of the text

		 \return float - the line spacing of the text
		*************************************************************************************/
		inline float GetLineSpacing() const { return m_lineSpacing; }

		/*!***********************************************************************************
		 \brief Set the Font
		 
		 \param[in] fontKey 
		*************************************************************************************/
		void SetFont(std::string fontKey);

		/*!***********************************************************************************
		 \brief Set the Tex
		 
		 \param[in] text 
		*************************************************************************************/
		void SetText(std::string const& text);

		/*!***********************************************************************************
		 \brief Set the Size
		 
		 \param[in] size 
		*************************************************************************************/
		void SetSize(float size);

		/*!***********************************************************************************
		 \brief Set the Color
		 
		 \param[in] color 
		*************************************************************************************/
		void SetColor(float const r = 0.f, float const g = 0.f, float const b = 0.f, float const a = 1.f);
		
		/*!***********************************************************************************
		 \brief Set the transparency of the text color on a scale of 0 to 1
		 
		 \param[in] alpha - a float from 0 to 1
		*************************************************************************************/
		void SetAlpha(float const alpha);

		/*!***********************************************************************************
		 \brief Set the horizontal alignment of the text

		 \param[in] hAlignment - the horizontal alignment of the text
		*************************************************************************************/
		void SetHAlignment(EnumTextAlignment hAlignment);

		/*!***********************************************************************************
		 \brief Set the vertical alignment of the text

		 \param[in] vAlignment - the vertical alignment of the text
		*************************************************************************************/
		void SetVAlignment(EnumTextAlignment vAlignment);

		/*!***********************************************************************************
		 \brief Set the horizontal overflow of the text

		 \param[in] hOverflow - the horizontal overflow of the text
		*************************************************************************************/
		void SetHOverflow(EnumTextOverflow hOverflow);

		/*!***********************************************************************************
		 \brief Set the vertical overflow of the text

		 \param[in] vOverflow - the vertical overflow of the text
		*************************************************************************************/
		void SetVOverflow(EnumTextOverflow vOverflow);

		/*!***********************************************************************************
		 \brief Set the line spacing of the text

		 \param[in] lineSpacing - the line spacing of the text
		*************************************************************************************/
		void SetLineSpacing(float lineSpacing);

		/*!***********************************************************************************
		 \brief Serializes the specified entity's TextComponent into a json file, returns a
		 		copy
		 
		 \param[in] id 				The entity to serialize
		 \return nlohmann::json 	The copy of the json generated
		*************************************************************************************/
		nlohmann::json ToJson(EntityID id) const
		{
			nlohmann::json ret;

			rttr::type type = rttr::type::get<TextComponent>();
			rttr::instance inst(EntityManager::GetInstance().Get<TextComponent>(id));
			for (auto& prop : type.get_properties())
			{
				rttr::variant var = prop.get_value(inst);
				
				if (var.get_type().get_name() == "std::string")
				{
					ret[prop.get_name().to_string()] = var.get_value<std::string>();
				}
				else if (var.get_type().get_name() == "float")
				{
					ret[prop.get_name().to_string()] = var.get_value<float>();
				}
				else if (var.get_type().get_name() == "structglm::vec<4,float,0>")
				{
					ret[prop.get_name().to_string()]["x"] = var.get_value<glm::vec4>().x;
					ret[prop.get_name().to_string()]["y"] = var.get_value<glm::vec4>().y;
					ret[prop.get_name().to_string()]["z"] = var.get_value<glm::vec4>().z;
					ret[prop.get_name().to_string()]["w"] = var.get_value<glm::vec4>().w;
				}
				else if (var.get_type().get_name() == "enumPE::EnumTextAlignment")
				{
					ret[prop.get_name().to_string()] = var.get_value<EnumTextAlignment>();
				}
				else if (var.get_type().get_name() == "enumPE::EnumTextOverflow")
				{
					ret[prop.get_name().to_string()] = var.get_value<EnumTextOverflow>();
				}
				else
				{
					std::cout << var.get_type().get_name() << std::endl;
				}
			}


			return ret;
		}

		/*!***********************************************************************************
		 \brief Deserializes from json file into this TextComponent and returns a reference
		 		to this
		 
		 \param[in] j 				The json file to deserialize from
		 \return TextComponent& 	The reference to this TextComponent Obj
		*************************************************************************************/
		TextComponent& Deserialize(const nlohmann::json& j)
		{
			rttr::type type = rttr::type::get<TextComponent>();
			rttr::instance inst(*this);
			for (auto& meth : type.get_methods())
			{
				if (meth.get_name() == "Color")
				{
					meth.invoke(inst, j[meth.get_name().to_string()]["x"].get<float>(), j[meth.get_name().to_string()]["y"].get<float>(), j[meth.get_name().to_string()]["z"].get<float>(), j[meth.get_name().to_string()]["w"].get<float>() );
				}
				else if (meth.get_name() == "Text" || meth.get_name() == "Font")
				{
					meth.invoke(inst, j[meth.get_name().to_string()].get<std::string>());
				}
				else if (meth.get_name() == "Size")
				{
					meth.invoke(inst, j[meth.get_name().to_string()].get<float>());
				}
				else if (meth.get_name() == "HAlignment" || meth.get_name() == "VAlignment")
				{
					meth.invoke(inst, j[meth.get_name().to_string()].get<EnumTextAlignment>());
				}
				else if (meth.get_name() == "HOverflow" || meth.get_name() == "VOverflow")
				{
					meth.invoke(inst, j[meth.get_name().to_string()].get<EnumTextOverflow>());
				}
				else if (meth.get_name() == "LineSpacing")
				{
					meth.invoke(inst, j[meth.get_name().to_string()].get<float>());
				}

			}
			return *this;
		}

	private:
		std::string m_text;
		std::string m_fontKey;
		std::shared_ptr<Font> m_font;
		// font style
		float m_size{ 1.f };
		float m_lineSpacing{ 1.f }; // % of font size
		EnumTextAlignment m_hAlignment{ EnumTextAlignment::LEFT };
		EnumTextAlignment m_vAlignment{ EnumTextAlignment::TOP };
		EnumTextOverflow m_hOverflow { EnumTextOverflow::WRAP }; // wrap, overflow
		EnumTextOverflow m_vOverflow { EnumTextOverflow::TRUNCATE}; // truncate, overflow
		glm::vec4 m_color{ 0.f, 0.f, 0.f, 1.f};
		// material
	};

	/*!***********************************************************************************
	 \brief Splits the text into lines based on the width of the text box

	 \param[in] r_textComponent The text component to split
	 \param[in] textBox The text box to split the text into
	 \return std::vector<std::string> The vector of lines
	*************************************************************************************/
	std::vector<std::string> SplitTextIntoLines(TextComponent const& r_textComponent, TextBox textBox);

	/*!***********************************************************************************
	 \brief Horizontally aligns the text based on the horizontal alignment of the text

	 \param[in] r_textComponent The text component to align
	 \param[in] line The line to align
	 \param[in] textBox The text box to align the text into
	 \param[in/out] hAlignOffset The horizontal offset of the text
	*************************************************************************************/
	void HorizontalTextAlignment(TextComponent const& r_textComponent,std::string const& r_line, TextBox textBox, float& r_hAlignOffset);

	/*!***********************************************************************************
	 \brief Vertically aligns the text based on the vertical alignment of the text

	 \param[in] r_textComponent The text component to align
	 \param[in] lines The lines to align
	 \param[in] textBox The text box to align the text into
	 \param[in/out] vAlignOffset The vertical offset of the text
	*************************************************************************************/
	void VerticalTextAlignment(TextComponent const& r_textComponent, std::vector<std::string> const& r_lines, TextBox textBox, float& r_vAlignOffset);

	/*!***********************************************************************************
	 \brief Calculates the width of the line

	 \param[in] r_textComponent The text component to calculate the width
	 \param[in] line The line to calculate the width
	 \return float The width of the line
	*************************************************************************************/
	float CalculateLineWidth(TextComponent const& r_textComponent, std::string const& r_line);

}