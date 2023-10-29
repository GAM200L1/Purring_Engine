#pragma once
typedef unsigned long long EntityID;

namespace PE 
{
enum class ScriptState {INIT, UPDATE, EXIT};
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
	 \brief					Destructor for Scripts
	*************************************************************************************/
	virtual ~Script() {}
};

}