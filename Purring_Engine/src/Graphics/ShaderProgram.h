#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ShaderProgram.h
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
 
 \brief    This file contains the ShaderProgram class, which contains methods
           to compile and store a shader program.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "Graphics/GLHeaders.h"
#include <glm/glm.hpp>

namespace PE
{
    namespace Graphics
    {
        /*!***********************************************************************************
         \brief Contains methods to compile and store a shader program.
        *************************************************************************************/
        class ShaderProgram
        {
        public:
            ShaderProgram();    // Does nothing.
            ~ShaderProgram();   // Deletes the shader program.

            // ----- Public methods ----- //
        public:

            /*!***********************************************************************************
             \brief Loads files specified by the arguments and calls
                    CompileLinkValidateProgram() to compile and link the shader program.

             \param[in,out] vertexFile Filepath of the vertex shader.
             \param[in,out] fragmentFile Filepath of the fragment shader.
             \return true - If the program was compiled successfully and is ready to be executed.
             \return false - If any part of the process was unsuccessful.
            *************************************************************************************/
            bool LoadAndCompileShadersFromFile(std::string const& vertexFile, std::string const& fragmentFile);

            /*!***********************************************************************************
             \brief Compiles and links the shaders to a shader program.

             \param[in,out] vertexString Vertex shader code stored in a string.
             \param[in,out] fragmentString Fragment shader code stored in a string.
             \return true - If the program was compiled successfully and is ready to be executed.
             \return false - If any part of the process was unsuccessful.
            *************************************************************************************/
            bool CompileLinkValidateProgram(std::string const& vertexString, std::string const& fragmentString);

            /*!***********************************************************************************
             \brief Validates if the shader program can be executed.

             \return true - If the program was validated successfully and is ready to be executed.
             \return false - If the program could not be validated and thus cannot be executed.
            *************************************************************************************/
            bool ValidateProgram();

            /*!***********************************************************************************
             \brief Use this program.
            *************************************************************************************/
            void Use() const;

            /*!***********************************************************************************
             \brief Stop using this program.
            *************************************************************************************/
            void UnUse() const;

            /*!***********************************************************************************
             \brief Delete the shader program. Call this when cleaning up.
            *************************************************************************************/
            void DeleteProgram();

            /*!***********************************************************************************
             \brief Set the Uniform object with the name [r_uniformName] using [r_matrix].

             \param[in,out] r_uniformName Name of the uniform.
             \param[in,out] r_matrix Reference to the matrix.
            *************************************************************************************/
            void SetUniform(std::string const& r_uniformName, glm::mat4 const& r_matrix);

            /*!***********************************************************************************
             \brief Set the Uniform object with the name [r_uniformName] using [r_vector4].

             \param[in,out] r_uniformName Name of the uniform.
             \param[in,out] r_vector4 Reference to the vector 4 object to set the uniform variable to.
            *************************************************************************************/
            void SetUniform(std::string const& r_uniformName, glm::vec4 const& r_vector4);

            /*!***********************************************************************************
             \brief Set the Uniform object with the name [r_uniformName] using [r_vector3].

             \param[in,out] r_uniformName Name of the uniform.
             \param[in,out] r_vector3 Reference to the vector 3 to set the uniform variable to.
            *************************************************************************************/
            void SetUniform(std::string const& r_uniformName, glm::vec3 const& r_vector3);
            
            /*!***********************************************************************************
             \brief Set the Uniform object with the name [r_uniformName] using [value].

             \param[in,out] r_uniformName Name of the uniform.
             \param[in,out] value Boolean value to set the uniform variable to.
            *************************************************************************************/
            void SetUniform(std::string const& r_uniformName, bool const value);
            
            /*!***********************************************************************************
             \brief Set the Uniform object with the name [r_uniformName] using [value].

             \param[in,out] r_uniformName Name of the uniform.
             \param[in,out] value Int value to set the uniform variable to.
            *************************************************************************************/
            void SetUniform(std::string const& r_uniformName, GLint const value);

            // ----- Private variables ----- //
        private:
            unsigned int m_programId{}; // ID of the program created for OpenGL. Set to zero if not linked to a program
            bool m_isLinked{ false }; // True if the program has been compiled and linked successfully, false otherwise.

            // ----- Private methods ----- //
        private:

            /*!***********************************************************************************
             \brief Checks for a program ID and creates one if there is none.

             \return true - If able to create a program ID or an ID exists,
             \return false - If unable to create a program id.
            *************************************************************************************/
            bool CheckProgramId();

            /*!***********************************************************************************
             \brief Compiles and attaches the shader string to the existing program.

             \param[in,out] shaderString Full shader code to generate a shader object from.
             \param[in,out] shaderType Type of shader (e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
             \return true - If compilation was successful.
             \return false - If compilation failed.
            *************************************************************************************/
            bool CompileShaderSource(std::string shaderString, GLenum shaderType);
        };
    } // End of Graphics namespace
} // End of PE namespace