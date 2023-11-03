#pragma once
#include "System.h"
#include "CSharpScriptComponent.h"
#include <map>
#include "ScriptingEngine.h"
#define REGISTER_CSHARPSCRIPT(name) 	PE::CSharpLogicSystem::m_csharpScriptContainer[#name] = new name()

namespace PE
{
    /*!***********************************************************************************
     \brief     Manages the lifecycle and execution of C# scripts within the engine.
    *************************************************************************************/
    class CSharpLogicSystem : public System
    {
    public:
        /*!***********************************************************************************
         \brief     A container that maps script names to their respective ScriptInstance.
        *************************************************************************************/
        static std::map<std::string, ScriptInstance> m_cSharpScriptContainer;

        /*!***********************************************************************************
         \brief     Constructs a CSharpLogicSystem object.
        *************************************************************************************/
        CSharpLogicSystem();

        /*!***********************************************************************************
         \brief     Destructs the CSharpLogicSystem object.
        *************************************************************************************/
        virtual ~CSharpLogicSystem();

        /*!***********************************************************************************
         \brief     Initializes the CSharpLogicSystem and loads scripts.
        *************************************************************************************/
        void InitializeSystem() override;

        /*!***********************************************************************************
         \brief     Updates all C# scripts within the system using the given delta time.
         \param     deltaTime The time passed since the last update call.
        *************************************************************************************/
        void UpdateSystem(float deltaTime) override;

        /*!***********************************************************************************
         \brief     Destroys the CSharpLogicSystem, unloading all scripts and releasing resources.
        *************************************************************************************/
        void DestroySystem() override;

        /*!***********************************************************************************
         \brief     Returns the name of the system.
         \return    std::string The name of the system.
        *************************************************************************************/
        std::string GetName() override;

        /*!***********************************************************************************
         \brief     Loads and initializes scripts from the specified paths or resources.
        *************************************************************************************/
        void LoadScripts();

    };


    /*!***********************************************************************************
     \brief     Represents a component that can hold and manage multiple C# scripts' states.
    *************************************************************************************/
    struct CSharpScriptComponent
    {
        /*!***********************************************************************************
         \brief     Constructs a CSharpScriptComponent object.
        *************************************************************************************/
        CSharpScriptComponent() {}

        /*!***********************************************************************************
         \brief     A map storing the state of C# scripts associated with unique keys.
        *************************************************************************************/
        std::map<std::string, CSharpScriptState> m_cSharpScriptKeys;

        /*!***********************************************************************************
         \brief     Adds a C# script to the component if it does not already exist.
         \param     key The unique key identifying the script to add.
        *************************************************************************************/
        void addCSharpScript(std::string key)
        {
            auto itr = m_cSharpScriptKeys.find(key);
            if (itr == m_cSharpScriptKeys.end())
                m_cSharpScriptKeys[key] = CSharpScriptState::INIT;
        }

        /*!***********************************************************************************
         \brief     Removes a C# script from the component if it exists.
         \param     key The unique key identifying the script to remove.
        *************************************************************************************/
        void removeCSharpScript(std::string key)
        {
            auto itr = m_cSharpScriptKeys.find(key);
            if (itr != m_cSharpScriptKeys.end())
                m_cSharpScriptKeys.erase(itr);
        }

        /*!***********************************************************************************
         \brief     Destructs the CSharpScriptComponent object.
        *************************************************************************************/
        ~CSharpScriptComponent() {}
    };
}

