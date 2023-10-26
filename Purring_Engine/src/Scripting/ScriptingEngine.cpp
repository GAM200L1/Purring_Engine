#include "prpch.h"
#include "Scripting/ScriptingEngine.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

namespace PE
{
    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;
    };

    static ScriptEngineData* s_Data;

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();
    }

    void ScriptEngine::Shutdown()
    {
        delete s_Data;
    }

    void ScriptEngine::InitMono()
    {
        mono_set_assemblies_path("mono/lib");

        MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");

        if (rootDomain == nullptr)
        {
            // Maybe log some error here
            return;
        }

        // Store the root domain pointer
        s_Data->RootDomain = rootDomain;

        // Create an App Domain
        s_Data->AppDomain = mono_domain_create_appdomain("MyAppDomain", nullptr);
        mono_domain_set(s_Data->AppDomain, true);
    }

    void ScriptEngine::ShutdownMono()
    {
        // ... implementation ...
    }
}
