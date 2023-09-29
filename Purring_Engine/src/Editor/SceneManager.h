/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     SceneManager.h
 \date     19-09-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	cpp file containing the definition of the SceneManager class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once

struct SceneData
{
	//Object List
	//Scene Name
	//idk other data
};


class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();
	//store different scenes serialized from file

	//function to return current scene? or just store current scene which can be changed using a function

	//serialize object list base on the scene

	//function to return object list for editor based on given scene?


	//tell everyone else what the current scene is

	//update each scene here?

	//render each scene?
	//or is render by objects, and each scene js holds the object to be rendered

public:
	std::vector<SceneData> sceneList;
private:
	int currentScene;
	std::unique_ptr<SceneManager> s_Instance; // only 1 scene manager
};