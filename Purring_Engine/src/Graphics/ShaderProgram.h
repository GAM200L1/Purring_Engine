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

/*                                                                   includes
----------------------------------------------------------------------------- */
#define GLEW_STATIC
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>
#include <string>

namespace PE
{
    namespace Graphics
    {
        class ShaderProgram
        {
        public:
            ShaderProgram();
            ~ShaderProgram();

            // ----- Public methods ----- //
        public:

            /*!***********************************************************************************
             \brief Not implemented yet. Loads files specified by the arguments and calls
                    CompileLinkValidateProgram() to compile and link the shader program.

             \param[in,out] vertexFile Filepath of the vertex shader.
             \param[in,out] fragmentFile Filepath of the fragment shader.
             \return true - If the program was compiled successfully and is ready to be executed.
             \return false - If any part of the process was unsuccessful.
            *************************************************************************************/
            bool LoadAndCompileShadersFromFile(std::string vertexFile, std::string fragmentFile);

            /*!***********************************************************************************
             \brief Compiles and links the shaders to a shader program.

             \param[in,out] vertexString Vertex shader code stored in a string.
             \param[in,out] fragmentString Fragment shader code stored in a string.
             \return true - If the program was compiled successfully and is ready to be executed.
             \return false - If any part of the process was unsuccessful.
            *************************************************************************************/
            bool CompileLinkValidateProgram(std::string vertexString, std::string fragmentString);

            /*!***********************************************************************************
             \brief Validates if the shader program can be executed.

             \return true - If the program was validated successfully and is ready to be executed.
             \return false - If the program could not be validated and thus cannot be executed.
            *************************************************************************************/
            bool ValidateProgram();

            /*!***********************************************************************************
             \brief Use this program.
            *************************************************************************************/
            void Use();

            /*!***********************************************************************************
             \brief Stop using this program.
            *************************************************************************************/
            void UnUse();

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

            // ----- Private variables ----- //
        private:
            unsigned int m_programId{}; //! ID of the program created for OpenGL. Set to zero if not linked to a program
            bool m_isLinked{ false }; //! True if the program has been compiled and linked successfully, false otherwise.

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