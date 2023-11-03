/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ScriptingEngine.h
 \date     10-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    This file contains the ScriptEngine class along with its support classes
           ScriptClass and ScriptInstance. It manages the scripting lifecycle, allowing
           for interaction with C# scripts using the Mono runtime. Functionality
           includes class instantiation, method invocation, and handling script
           assembly loading and unloading.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "Scripting/ScriptingEngine.h"
#include "Scripting/ScriptGlue.h"
#include "Scripting/Base.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include <stdio.h>
#include "glm/glm.hpp"
#include <memory>
#define FMT_HEADER_ONLY
#include <fmt/format.h>

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
            std::streamsize size = end - stream.tellg();


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

            if (status != MONO_IMAGE_OK) {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                std::cerr << "Error loading Mono image: " << errorMessage << std::endl;
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
        std::unordered_map<std::string, MonoObject*> MonoObjectMap;

    };

    static ScriptEngineData* s_Data = nullptr;



    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();
        LoadAssembly("../Purring_Engine/Resources/Scripts/PE-ScriptCore.dll");
        LoadAssemblyClasses(s_Data->CoreAssembly);
        ScriptGlue::RegisterFunctions();



        //Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
        // Retrieve and instaitiate class w/ constructor
        s_Data->EntityClass = ScriptClass("PE", "Entity");

        MonoObject* instance = s_Data->EntityClass.Instantiate();

        // 2. call method
        MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
        s_Data->EntityClass.InvokeMethod(instance, printMessageFunc, nullptr);

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
        if (!s_Data->AppDomain) {
            std::cout << "myappdoman test";
            return;
        }
        mono_domain_set(s_Data->AppDomain, true);

        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        if (!s_Data->CoreAssembly) {
            std::cout << "LoadMonoAssembly filepath test";
            return;
        }
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

        // Check if entityClass is nullptr before proceeding.
        if (!entityClass) {
            // Log the error or handle it as needed.
            std::cerr << "Failed to find PE.Entity class." << std::endl;
            return; // Exit the function as we can't proceed without this class.
        }

        for (int32_t i = 0; i < numTypes; ++i)
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

            // Check if monoClass is nullptr before using it.
            if (!monoClass) {
                // Optionally, log the full name of the class that couldn't be loaded.
                std::cerr << "Failed to find class: " << fullName << std::endl;
                continue; // Skip this iteration as the class was not found.
            }

            if (monoClass == entityClass)
                continue;

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
            if (isEntity) {
                // Make sure to use fullName in the map, not the string "fullName".
                s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
            }

            // Debug print can be enabled if needed to check class names being loaded.
            // std::cout << fullName << std::endl;
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
    //MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
    //{
    //    return mono_runtime_invoke(method, instance, params, nullptr);
    //}

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass)
        : m_ScriptClass(scriptClass)
    {
        m_Instance = scriptClass->Instantiate();
        m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
        m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
    }

    //void ScriptInstance::InvokeOnCreate()
    //{
    //    m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod);
    //}

    //void ScriptInstance::InvokeOnUpdate(float ts)
    //{
    //    void* param = &ts;
    //    m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
    //}

    std::string CreateCompositeKey(const std::string& objectID, const std::string& key)
    {
        return objectID + "_" + key;
    }

    MonoClass* ScriptEngine::GetMonoClassByKey(const std::string& key)
    {
        auto it = s_Data->EntityClasses.find(key);
        if (it != s_Data->EntityClasses.end())
        {
            return it->second->GetMonoClass();
        }
        return nullptr;
    }

    MonoObject* ScriptEngine::GetOrCreateMonoObject(const EntityID& objectID, const std::string& key)
    {
        std::string compositeKey = CreateCompositeKey(std::to_string(objectID), key);

        // First, attempt to find the MonoObject using the compositeKey
        auto it = s_Data->MonoObjectMap.find(compositeKey);
        if (it != s_Data->MonoObjectMap.end())
        {
            // If found, return the existing MonoObject
            return it->second;
        }

        // If not found, create a new MonoObject
        MonoClass* monoClass = GetMonoClassByKey(key);
        if (monoClass == nullptr)
        {
            // Handle error: Class not found
            return nullptr;
        }

        MonoObject* newMonoObject = InstantiateClass(monoClass);

        // Add the new MonoObject to the map and return it
        s_Data->MonoObjectMap[compositeKey] = newMonoObject;

        return newMonoObject;
    }

    MonoMethod* PE::ScriptEngine::GetMethodFromMonoObject(MonoObject* obj, const char* methodName, int paramCount)
    {
        if (obj == nullptr) {
            return nullptr;
        }

        // Get the MonoClass from the MonoObject
        MonoClass* klass = mono_object_get_class(obj);

        // Search for the method
        MonoMethod* method = mono_class_get_method_from_name(klass, methodName, paramCount);

        return method;
    }
}
