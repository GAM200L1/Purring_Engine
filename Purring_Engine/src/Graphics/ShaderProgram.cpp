/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ShaderProgram.cpp
 \date     20-08-2023
 
 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu
  \par      code %:     95%

 \co-author            Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par      code %:     5%
 \par      changes:    16-09-2023
                       Load shader from file
 
 \brief    This file contains the ShaderProgram class, which contains methods
           to compile and store a shader program.
  
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "prpch.h"
#include <iostream>
#include "ShaderProgram.h" // <string>, <glm/glm.hpp>
#include "../Logging/Logger.h" // ----- @TODO: Fix the include paths... ------

extern Logger engine_logger;

namespace PE
{
    namespace Graphics
    {
        ShaderProgram::ShaderProgram()
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "Creating shader program", __FUNCTION__);
        }

        ShaderProgram::~ShaderProgram()
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "Destroying shader program", __FUNCTION__);
            DeleteProgram();
        }

        bool ShaderProgram::LoadAndCompileShadersFromFile(std::string vertexFile, std::string fragmentFile)
        {
            std::ifstream vertexFileStream(vertexFile);
            std::ifstream fragmentFileStream(fragmentFile);

            if (!vertexFileStream)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, "Vertex shader file " + vertexFile + " could not be opened", __FUNCTION__);
                return false;
            }
            else if (!fragmentFileStream) 
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, "Fragment shader file " + fragmentFile + " could not be opened", __FUNCTION__);
                return false;
            }

            std::stringstream vertexStream;
            std::stringstream fragmentStream;

            vertexStream << vertexFileStream.rdbuf();
            fragmentStream << fragmentFileStream.rdbuf();

            vertexFileStream.close();
            fragmentFileStream.close();

            return CompileLinkValidateProgram(vertexStream.str(), fragmentStream.str());
        }


        bool ShaderProgram::CompileLinkValidateProgram(std::string vertexString, std::string fragmentString)
        {
            // Check if able to create a program
            if (!CheckProgramId())
            {
                return false;
            }

            // Compile and attach the shaders
            if (!CompileShaderSource(vertexString, GL_VERTEX_SHADER) ||
                !CompileShaderSource(fragmentString, GL_FRAGMENT_SHADER))
            {
                // Unable to compile shaders
                return false;
            }

            // Link the shaders to the program
            glLinkProgram(m_programId);
            GLint status{};
            glGetProgramiv(m_programId, GL_LINK_STATUS, &status);

            if (GL_FALSE == status)
            {
                // Check if there is an info log
                GLint actualLength{ 0 };
                glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &actualLength);

                if (actualLength > 0)
                {
                    // Character array to store the logs in 
                    GLsizei maxLogLength{ 1024 };
                    GLchar informationLog[1024];
                    glGetProgramInfoLog(m_programId, maxLogLength,
                        reinterpret_cast<GLsizei*>(&actualLength), &informationLog[0]);

                    std::string logString{ "Unable to link shader program: " };
                    logString += informationLog;

                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, logString, __FUNCTION__);
                }

                return false;
            }

            m_isLinked = true;

            // Validate the program
            if (!ValidateProgram())
            {
                return false;
            }
            return true;
        }


        bool ShaderProgram::ValidateProgram()
        {
            if (m_programId <= 0 || !m_isLinked)
            {
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "No program to check.", __FUNCTION__);
                return false;
            }

            // Character array to store the logs in 
            GLsizei maxLogLength{ 1024 };
            char informationLog[1024];

            // Check the validation status of the program
            GLint programStatus;
            glValidateProgram(m_programId);
            glGetProgramiv(m_programId, GL_VALIDATE_STATUS, &programStatus);

            // If the status is invalid,
            if (!programStatus)
            {
                // Check if the log contains information
                GLint infoLogLength;
                glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLength);

                if (infoLogLength > 0) {
                    GLsizei actualLogLength;
                    glGetProgramInfoLog(m_programId, maxLogLength, &actualLogLength, informationLog);

                    std::string logString{ "Shader program validation failed: " };
                    logString += informationLog;

                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(false, logString, __FUNCTION__);
                }
                return false;
            }

            return true;
        }


        void ShaderProgram::Use() const
        {
            if (m_programId > 0 && m_isLinked)
            {
                glUseProgram(m_programId);
            }
        }


        void ShaderProgram::UnUse() const
        {
            glUseProgram(0);
        }


        void ShaderProgram::DeleteProgram()
        {
            UnUse();
            if (m_programId > 0) {
                glDeleteProgram(m_programId);
            }
        }


        void ShaderProgram::SetUniform(std::string const& r_uniformName, glm::mat4 const& r_matrix) {

            GLint matrixLocation = glGetUniformLocation(m_programId,
                reinterpret_cast<GLchar const*>(r_uniformName.c_str()));

            // Check if the uniform variable can be found
            if (matrixLocation >= 0 &&
                !(GL_INVALID_OPERATION == matrixLocation ||
                    GL_INVALID_VALUE == matrixLocation)) {

                // Pass the matrix as a uniform variable
                glUniformMatrix4fv(matrixLocation, 1, GL_FALSE,
                    reinterpret_cast<const GLfloat*>(&r_matrix[0][0]));
            }
            else {
                std::string errorLog{ "Uniform variable " + r_uniformName + " doesn't exist.\n" };
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, errorLog, __FUNCTION__);
            }
        }


        void ShaderProgram::SetUniform(std::string const& r_uniformName, glm::vec4 const& r_vector4) {

            GLint uniformLocation = glGetUniformLocation(m_programId,
                reinterpret_cast<GLchar const*>(r_uniformName.c_str()));

            // Check if the uniform variable can be found
            if (uniformLocation >= 0 &&
                !(GL_INVALID_OPERATION == uniformLocation ||
                    GL_INVALID_VALUE == uniformLocation)) {

                // Pass the vector values as a uniform variable
                glUniform4f(uniformLocation, static_cast<GLfloat>(r_vector4.x),
                    static_cast<GLfloat>(r_vector4.y),
                    static_cast<GLfloat>(r_vector4.z),
                    static_cast<GLfloat>(r_vector4.w));
            }
            else {
                std::string errorLog{ "Uniform variable " + r_uniformName + " doesn't exist.\n" };
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, errorLog, __FUNCTION__);
            }
        }


        void ShaderProgram::SetUniform(std::string const& r_uniformName, bool const value) {

            GLint uniformLocation = glGetUniformLocation(m_programId,
                reinterpret_cast<GLchar const*>(r_uniformName.c_str()));

            // Check if the uniform variable can be found
            if (uniformLocation >= 0 &&
                !(GL_INVALID_OPERATION == uniformLocation ||
                    GL_INVALID_VALUE == uniformLocation)) {

                // Pass the boolean value as a uniform variable
                glUniform1i(uniformLocation, static_cast<GLint>(value));
            }
            else {
                std::string errorLog{ "Uniform variable " + r_uniformName + " doesn't exist.\n" };
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, errorLog, __FUNCTION__);
            }
        }


        void ShaderProgram::SetUniform(std::string const& r_uniformName, GLuint const value) {

            GLint uniformLocation = glGetUniformLocation(m_programId,
                reinterpret_cast<GLchar const*>(r_uniformName.c_str()));

            // Check if the uniform variable can be found
            if (uniformLocation >= 0 &&
                !(GL_INVALID_OPERATION == uniformLocation ||
                    GL_INVALID_VALUE == uniformLocation)) {

                // Pass the int value as a uniform variable
                glUniform1ui(uniformLocation, value);
            }
            else {
                std::string errorLog{ "Uniform variable " + r_uniformName + " doesn't exist.\n" };
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(true, errorLog, __FUNCTION__);
            }
        }


        bool ShaderProgram::CheckProgramId() {

            if (m_programId <= 0)
            {
                // No program ID, so create one
                m_programId = glCreateProgram();
                m_isLinked = false;

                if (0 == m_programId)
                {
                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, "Cannot create shader program handle.", __FUNCTION__);
                    return false;
                }
            }

            // Program ID created
            return true;
        }


        bool ShaderProgram::CompileShaderSource(std::string shaderString, GLenum shaderType)
        {
            // Check if able to create a program
            if (!CheckProgramId())
            {
                return false;
            }

            // Cast the shader code string into chars
            GLchar const* shaderChar[]{ reinterpret_cast<const GLchar*>(shaderString.c_str()) };

            GLuint shaderHandle = glCreateShader(shaderType);
            glShaderSource(shaderHandle, 1, shaderChar, NULL);
            glCompileShader(shaderHandle);

            // Check if compilation was successful
            GLint status{};
            glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

            if (GL_FALSE == status)
            {
                // Check if there is an info log
                GLint actualLength{};
                glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &actualLength);

                if (actualLength > 0)
                {
                    // Character array to store the logs in 
                    GLsizei maxLogLength{ 1024 };
                    GLchar informationLog[1024];
                    glGetShaderInfoLog(shaderHandle, maxLogLength,
                        NULL, &informationLog[0]);

                    std::string logString{ "Unable to compile shader: " };
                    logString += informationLog;

                    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                    engine_logger.SetTime();
                    engine_logger.AddLog(true, logString, __FUNCTION__);
                }

                // Delete the shader
                glDeleteShader(shaderHandle);

                return false;
            }

            glAttachShader(m_programId, shaderHandle);

            // Flag the compiled shaders for deletion
            // Note: they will only get deleted 
            // when the shader program ceases to exist
            glDeleteShader(shaderHandle);

            return true;
        }
    } // End of Graphics namespace 
} // End of PE namespace
