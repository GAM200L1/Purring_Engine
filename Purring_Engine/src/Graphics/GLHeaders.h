#pragma once
/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GLHeaders.h
 \date     13-09-2023

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief     This file contains the include statements for glew.h and glfw.h to 
						prevent issues where gl.h is included before glew.h.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#define GLFW_INCLUDE_NONE
#define GLEW_STATIC

// suppress warnings for glm's anonymous structs in type_quat.hpp
#define GLM_FORCE_SILENT_WARNINGS 
# pragma warning(push)
# pragma warning(disable:4201) // warning C4201: nonstandard extension used : nameless struct/union
# pragma warning(disable:4819) // warning C4819: The file contains a character that cannot be represented in the current code page(949)

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>