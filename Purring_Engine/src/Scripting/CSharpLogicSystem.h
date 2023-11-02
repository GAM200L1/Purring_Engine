#pragma once
#include "System.h"
#include "CSharpScriptComponent.h"
#include <map>
#include "ScriptingEngine.h"
#define REGISTER_CSHARPSCRIPT(name) 	PE::CSharpLogicSystem::m_csharpScriptContainer[#name] = new name()

namespace PE
{
    class CSharpLogicSystem : public System
    {
    public:
        static std::map<std::string, ScriptInstance> m_cSharpScriptContainer;

        CSharpLogicSystem();
        virtual ~CSharpLogicSystem();

        void InitializeSystem() override;
        void UpdateSystem(float deltaTime) override;
        void DestroySystem() override;

        std::string GetName() override;

        void LoadScripts();

    };


    struct CSharpScriptComponent
    {
        CSharpScriptComponent() {}

        // A map to store script keys along with their state
        std::map<std::string, CSharpScriptState> m_cSharpScriptKeys;

        void addCSharpScript(std::string key)
        {
            auto itr = m_cSharpScriptKeys.find(key);
            if (itr == m_cSharpScriptKeys.end())
                m_cSharpScriptKeys[key] = CSharpScriptState::INIT;
        }

        void removeCSharpScript(std::string key)
        {
            auto itr = m_cSharpScriptKeys.find(key);
            if (itr != m_cSharpScriptKeys.end())
                m_cSharpScriptKeys.erase(itr);
        }

        ~CSharpScriptComponent() {}
    };
}

