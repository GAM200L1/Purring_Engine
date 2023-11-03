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
		/*!***********************************************************************************
		 \brief Get the Font 
		 
		 \return std::shared_ptr<const Font> 
		*************************************************************************************/
		inline std::shared_ptr<const Font> GetFont() const { return m_font; }

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
		void SetColor(glm::vec4 const& color);

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
					meth.invoke(inst, glm::vec4{ j[meth.get_name().to_string()]["x"].get<float>(), j[meth.get_name().to_string()]["y"].get<float>(), j[meth.get_name().to_string()]["z"].get<float>(), j[meth.get_name().to_string()]["w"].get<float>() });
				}
				else if (meth.get_name() == "Text" || meth.get_name() == "Font")
				{
					meth.invoke(inst, j[meth.get_name().to_string()].get<std::string>());
				}
				else if (meth.get_name() == "Size")
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
		// line spacing
		// paragraph
		// alignment
		// h_overflow
		// v_overflow
		glm::vec4 m_color{ 0.f, 0.f, 0.f, 1.f};
		// material
	};
}