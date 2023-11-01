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
// ... other includes ...

std::map<std::string, PE::CSharpScript*> PE::CSharpLogicSystem::m_cSharpScriptContainer;

PE::CSharpLogicSystem::CSharpLogicSystem()
{
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



void PE::CSharpLogicSystem::DestroySystem()
{
    ScriptEngine::Shutdown();
}

std::string PE::CSharpLogicSystem::GetName()
{
    return "CSharpLogicSystem";
}