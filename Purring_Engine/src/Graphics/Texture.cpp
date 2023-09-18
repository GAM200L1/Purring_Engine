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

		}

		Texture::~Texture()
		{
			glDeleteTextures(1, &m_textureID);
		}

		void Texture::Bind(unsigned int textureUnit) const
		{
			glBindTextureUnit(textureUnit, m_textureID);
		}

		void Texture::UnBind() const
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		bool Texture::CreateTexture(std::string const& path)
		{
			int width, height, channels, internalFormat, imageFormat;
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