#include "prpch.h"
#include "CSharpLogicSystem.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Editor/Editor.h"
#include "Scripting/ScriptingEngine.h"
#include <vector>
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "Scripting/ScriptingEngine.h"

std::map<std::string, PE::ScriptInstance> PE::CSharpLogicSystem::m_cSharpScriptContainer;

PE::CSharpLogicSystem::CSharpLogicSystem()
{
    LoadScripts();

}

PE::CSharpLogicSystem::~CSharpLogicSystem()
{
}

void PE::CSharpLogicSystem::InitializeSystem()
{
    ScriptEngine::Init();
}

void PE::CSharpLogicSystem::UpdateSystem(float deltaTime)
{
    for (EntityID objectID : SceneView<CSharpScriptComponent>())
    {
        CSharpScriptComponent& sc = EntityManager::GetInstance().Get<CSharpScriptComponent>(objectID);

        for (auto& [key, state] : sc.m_cSharpScriptKeys)
        {
            // Create or retrieve MonoObject instance by objectID and key
            MonoObject* scriptInstance = ScriptEngine::GetInstance().GetOrCreateMonoObject(objectID, key);

            switch (state)
            {
            case CSharpScriptState::INIT:
            {
                // Call script's Init method
                MonoMethod* initMethod = ScriptEngine::GetMethodFromMonoObject(scriptInstance, "Init", 0);
                if (initMethod != nullptr) {
                    mono_runtime_invoke(initMethod, scriptInstance, nullptr, nullptr);
                }
            }
                break;

            case CSharpScriptState::UPDATE:
            {
                // Call script's Update method and pass deltaTime
                MonoMethod* updateMethod = ScriptEngine::GetMethodFromMonoObject(scriptInstance, "Update", 1);
                if (updateMethod != nullptr) {
                    void* args[1];
                    args[0] = &deltaTime;
                    mono_runtime_invoke(updateMethod, scriptInstance, args, nullptr);
                }
            }
                break;

            case CSharpScriptState::EXIT:
            {
                // Call script's Destroy or Exit method
                MonoMethod* exitMethod = ScriptEngine::GetMethodFromMonoObject(scriptInstance, "Destroy", 0);
                if (exitMethod != nullptr) {
                    mono_runtime_invoke(exitMethod, scriptInstance, nullptr, nullptr);
                }
            }
                break;
            }
        }
    }
}

void PE::CSharpLogicSystem::LoadScripts() {
    std::cout << "LoadScripts is being called" << std::endl;

    // Get all the script classes from the scripting engine
    auto scriptClasses = PE::ScriptEngine::GetEntityClasses();

    // Iterate over all the script classes
    for (const auto& [className, scriptClass] : scriptClasses) {
        // Instantiate the script class
        PE::ScriptInstance scriptInstance(scriptClass);

        // Store the script instance in the container
        m_cSharpScriptContainer[className] = scriptInstance;
    }
}



void PE::CSharpLogicSystem::DestroySystem()
{
    ScriptEngine::Shutdown();
}

std::string PE::CSharpLogicSystem::GetName()
{
    return "CSharpLogicSystem";
}