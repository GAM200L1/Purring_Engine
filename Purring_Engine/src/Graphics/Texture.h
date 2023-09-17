#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Texture.h
 \date     20-08-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the Texture class, which contains methods that encapsulates
		   a texture object.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

namespace PE
{
	namespace Graphics
	{
		class Texture
		{
		public:
			Texture();
			~Texture();

			void Bind(unsigned int textureUnit) const;
			void UnBind() const;
			
			bool CreateTexture(std::string const& path);

			inline unsigned int GetTextureID() { return m_textureID; }
			inline unsigned int GetWidth() { return m_width; }
			inline unsigned int GetHeight() { return m_height; }

			
		private:
			unsigned int m_textureID;
			unsigned int m_width; // width of image pixels
			unsigned int m_height; // height of image pixels
		};
	}
}
