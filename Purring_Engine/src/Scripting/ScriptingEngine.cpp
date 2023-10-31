#include "prpch.h"
#include "Scripting/ScriptingEngine.h"
#include "Scripting/ScriptGlue.h"
#include "glm/glm.hpp"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include <stdio.h>
#include "Scripting/Base.h"
#include <memory>
#define FMT_HEADER_ONLY
#include <fmt/format.h>

//#include "fmt/format.h"
//#include "fmt/core.h"
//#include "fmt/format-inl.h"

//#define FMT_HEADER_ONLY

namespace PE
{
    namespace Utils
    {
        // todo: move to file system class later
        static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
        {
            std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

            if (!stream)
            {
                // Failed to open the file
                return nullptr;
            }

            std::streampos end = stream.tellg();
            stream.seekg(0, std::ios::beg);
            uint32_t size = end - stream.tellg();

            if (size == 0)
            {
                // File is empty
                return nullptr;
            }

            char* buffer = new char[size];
            stream.read((char*)buffer, size);
            stream.close();

            *outSize = (uint32_t)size;
            return buffer;
        }


        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
        {
            uint32_t fileSize = 0;
            char* fileData = ReadBytes(assemblyPath, &fileSize);

            // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

            if (status != MONO_IMAGE_OK)
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }

            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
            mono_image_close(image);

            // Don't forget to free the file data
            delete[] fileData;

            return assembly;
        }

        void PrintAssemblyTypes(MonoAssembly* assembly)
        {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

            for (int32_t i = 0; i < numTypes; i++)
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
                
                MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);
                MonoClass* entityClass = mono_class_from_name(image, "PE2", "Entity");

                bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
                printf("Is Entity: %s\n", isEntity ? "true" : "false");
                printf("%s.%s\n", nameSpace, name);
            }
        }


    }

    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;

        MonoAssembly* CoreAssembly = nullptr;
        MonoImage* CoreAssemblyImage = nullptr;

        ScriptClass EntityClass;

        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
    };

    static ScriptEngineData* s_Data = nullptr;



    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();
        LoadAssembly("../Purring_Engine/Resources/Scripts/PE-ScriptCore.dll");
        LoadAssemblyClasses(s_Data->CoreAssembly);
        ScriptGlue::RegisterFunctions();


#if 0
        //Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
        // Retrieve and instaitiate class w/ constructor
        s_Data->EntityClass = ScriptClass("PE", "Entity");

        MonoObject* instance = s_Data->EntityClass.Instantiate();

        // 2. call method
        MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
        s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);

        // 3. call method w/ param 
        MonoMethod* printIntFunc = s_Data->EntityClass.GetMethod("PrintInt", 1);

        int value = 5;
        void* param = &value;
        s_Data->EntityClass.InvokeMethod(instance, printIntFunc, &param);

        MonoMethod* printIntsFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);
        int value2 = 208;
        void* params[2] =
        {
            &value,
            &value2
        };
        s_Data->EntityClass.InvokeMethod(instance, printIntsFunc, params);

        MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");

        MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
        void* stringParam = monoString;
        s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);
#endif
    }



    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete s_Data;
    }



    void ScriptEngine::InitMono()
    {
            mono_set_assemblies_path("../Purring_Engine/mono/lib");

            MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");

            // Store the root domain pointer
            s_Data->RootDomain = rootDomain;

    }



    void ScriptEngine::ShutdownMono()
    {
        //// Unload assemblies, free memory, etc.
        mono_domain_unload(s_Data->AppDomain);
        s_Data->AppDomain = nullptr;
        mono_jit_cleanup(s_Data->RootDomain);
        s_Data->RootDomain = nullptr;

        //delete s_Data;
        //s_Data = nullptr;  // Set to nullptr to avoid dangling pointer
    }
    


    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
    {
        // Create an App Domain
        s_Data->AppDomain = mono_domain_create_appdomain("MyAppDomain", nullptr);
        mono_domain_set(s_Data->AppDomain, true);

        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        // assemblyImage is to get classes by its name
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

        // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

    }

    std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
    {
        return s_Data->EntityClasses;
    } 


    void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
    {
        s_Data->EntityClasses.clear();

        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        MonoClass* entityClass = mono_class_from_name(image, "PE", "Entity");


        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
            std::string fullName;
            if (strlen(nameSpace) != 0)
                fullName = fmt::format("{}.{}", nameSpace, name);
            else
                fullName = name;

            MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);
            if (monoClass == entityClass)
                continue;

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
            if (isEntity)
                s_Data->EntityClasses["fullName"] = CreateRef<ScriptClass>(nameSpace, name);

            //printf("%s.%s\n", nameSpace, name);
        }
    }


    MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
    {
        // 1. create an object and call constructor
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        mono_runtime_object_init(instance);

        return instance;
    }


    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
        :m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
    }

    MonoObject* ScriptClass::Instantiate()
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
    {
        return mono_runtime_invoke(method, instance, params, nullptr);
    }

    //ScriptInstance::ScriptClass(Ref<ScriptClass> scriptClass)
    //    : m_ScriptClass(scriptClass)
    //{
    //    m_Instance = scriptClass->Instantiate();
    //    m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
    //    m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
    //}

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass)
        : m_ScriptClass(scriptClass)
    {
        m_Instance = scriptClass->Instantiate();
        m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
    }

    void ScriptInstance::InvokeOnCreate()
    {
        m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)
    {
        void* param = &ts;
        m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
    }


}
