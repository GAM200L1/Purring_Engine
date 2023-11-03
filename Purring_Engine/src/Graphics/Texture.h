#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Texture.h
 \date     16-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the Texture class, which contains methods that creates 
						and stores an OpenGL texture object.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

namespace PE
{
		namespace Graphics
		{
				class Texture
				{
						// ----- Constructors ----- // 
				public:
						Texture(); // Default intializes the member variables
						~Texture(); // Deletes the texture object


						// ----- Public getters ----- // 
				public:

						/*!***********************************************************************************
						\brief  Gets the texture object ID.

						\return unsigned int - The texture object ID.
						*************************************************************************************/
						inline unsigned int GetTextureID() { return m_textureID; }

						/*!***********************************************************************************
						\brief  Gets the texture width.

						\return unsigned int - The width of the texture.
						*************************************************************************************/
						inline unsigned int GetWidth() { return m_width; }

						/*!***********************************************************************************
						\brief  Gets the texture height.

						\return unsigned int - The height of the texture.
						*************************************************************************************/
						inline unsigned int GetHeight() { return m_height; }


						// ----- Public methods ----- // 
				public:
						/*!***********************************************************************************
							 \brief Unbinds this texture object if it is bound.

							 \param[in] textureUnit Texture unit that this object was bound to
							*************************************************************************************/
						void Bind(unsigned int textureUnit) const;

						/*!***********************************************************************************
							 \brief Unbinds this texture object if it is bound.

							 \param[in] textureUnit Texture unit that this object was bound to
							*************************************************************************************/
						void Unbind() const;

						/*!***********************************************************************************
						\brief Creates a 2D texture object and stores the handle.

						\param[in] pathname Filepath of the texture to load and use as the texture object.
						*************************************************************************************/
						bool CreateTexture(std::string const& path);

						// ----- Private variables ----- // 
				private:
						unsigned int m_textureID{}; // Handle of the texture object
						unsigned int m_width{}; // Width of image pixels
						unsigned int m_height{}; // Height of image pixels
				};
		}
}
