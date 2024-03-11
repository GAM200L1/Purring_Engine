/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     Script.h 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan\@digipen.edu
 \par      code %:     95% 
 \par      changes:    Majority of the code 

 \co-author            FOONG Jun Wei 
 \par      email:      f.junwei\@digipen.edu 
 \par      code %:     5% 
 \par      changes:    rttr::instance GetScriptData() 


 \brief  This file contains the base class declaration of Scripts class/type

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/
#pragma once
#include <rttr/registration.h>
typedef unsigned long long EntityID;

#define	REGISTERANIMATIONFUNCTION(func, namespace) Script::AddFunction(#func, [this](EntityID x) { this->##func(x); });

namespace PE 
{
enum class ScriptState {INIT, UPDATE, EXIT, DEAD};
//all the scripts will inherit this
class Script
{
public:
	/*!***********************************************************************************
	 \brief					The Initialise Function for a script, to initialize any variables
	 \param [In] EntityID	The ID of the object currently running the script
	*************************************************************************************/
	virtual void Init(EntityID) = 0;
	/*!***********************************************************************************
	 \brief					The update function of the script
	 \param [In] EntityID	The ID of the object currently running the script
	*************************************************************************************/
	virtual void Update(EntityID, float) = 0;
	/*!***********************************************************************************
	 \brief					The function that is called on the end of the script's lifetime
	 \param [In] EntityID	The ID of the object currently running the script
	*************************************************************************************/
	virtual void Destroy(EntityID) = 0;
	/*!***********************************************************************************
	 \brief					The function that is called when the script is attached onto an object
	 \param [In] EntityID	The ID of the object currently running the script
	*************************************************************************************/
	virtual void OnAttach(EntityID) = 0;
	/*!***********************************************************************************
	 \brief					The function that is called when the script is detached from an object
	 \param [In] EntityID	The ID of the object currently running the script
	*************************************************************************************/
	virtual void OnDetach(EntityID) = 0;

	/*!***********************************************************************************
	 \brief Get the Script Data returning an rttr::instance obj, to use for reflection of
	 		Script data
	 
	 \param[in] id 
	 \return rttr::instance 
	*************************************************************************************/
	virtual rttr::instance GetScriptData(EntityID id) = 0;
	/*!***********************************************************************************
	 \brief					Destructor for Scripts
	*************************************************************************************/
	virtual ~Script() {}

	// documentation
	void AddFunction(std::string str, const std::function<void(EntityID)> function)
	{
		animationFunctions[str] = function;
	}

	std::map<std::string, std::function<void(EntityID)>> animationFunctions;
};

}