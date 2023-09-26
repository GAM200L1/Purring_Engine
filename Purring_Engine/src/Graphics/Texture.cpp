/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Texture.h
 \date     16-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the Texture class, which contains methods that encapsulates
						a texture object.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "GLHeaders.h"
#include "Texture.h"
#include "stb_image.h"

namespace PE
{
	namespace Graphics
	{
		Texture::Texture() : m_textureID{}, m_width{}, m_height{}
		{
				/* Empty by design */
		}

		Texture::~Texture()
		{
			glDeleteTextures(1, &m_textureID);
		}

		void Texture::Bind(unsigned int textureUnit) const
		{
			// Bind the texture
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, m_textureID);
		}

		void Texture::Unbind() const
		{
			// Retrieve current texture
			GLint currentTexture{};
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

			// Unbind if currently bound
			if (static_cast<GLuint>(currentTexture) == m_textureID)
			{
					glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		bool Texture::CreateTexture(std::string const& path)
		{
			int width, height, channels, internalFormat, imageFormat;
			stbi_set_flip_vertically_on_load(true);
			unsigned char* textureData = stbi_load(path.c_str(), &width, &height, &channels, 0);

			// if texture data is loaded
			if (textureData)
			{
				m_width = width;
				m_height = height;

				internalFormat = channels == 4 ? GL_RGBA8 : GL_RGB8;
				imageFormat = channels == 4 ? GL_RGBA : GL_RGB;

				glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
				glTextureStorage2D(m_textureID, 1, internalFormat, m_width, m_height);

				glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, imageFormat, GL_UNSIGNED_BYTE, textureData);
			
				stbi_image_free(textureData);
				return true;
			}
			// texture fail to load
			else
			{
				return false;
			}
		}
	}
}