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

#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include "Scripting/Base.h"
#include <map>
#include "Singleton.h"
#include "CSharpScriptComponent.h"
#include <glm/glm.hpp>
#include "ScriptGlue.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
}

namespace PE
{
	/*!***********************************************************************************
	 \brief     ScriptClass holds metadata about C# classes and provides methods for
				instance creation and method invocation.
	*************************************************************************************/
	class ScriptClass
	{
	public:
		/*!***********************************************************************************
		 \brief     Default constructor for the ScriptClass that initializes member variables.
		*************************************************************************************/
		ScriptClass() = default;

		/*!***********************************************************************************
		 \brief     Constructor that initializes a script class with namespace and class name.
		 \param     classNamespace The namespace the C# class resides in.
		 \param     className The name of the C# class.
		*************************************************************************************/
		ScriptClass(const std::string& classNamespace, const std::string& className);

		/*!***********************************************************************************
		 \brief     Creates an instance of the script class.
		 \return    MonoObject* A pointer to the instantiated Mono object.
		*************************************************************************************/
		MonoObject* Instantiate();

		/*!***********************************************************************************
		 \brief     Retrieves a method from the script class.
		 \param     name The name of the method to retrieve.
		 \param     parameterCount The number of parameters the method accepts.
		 \return    MonoMethod* A pointer to the MonoMethod if found, nullptr otherwise.
		*************************************************************************************/
		MonoMethod* GetMethod(const std::string& name, int parameterCount);

		/*!***********************************************************************************
		 \brief     Invokes a method on a given instance of a script class.
		 \param     instance A pointer to the MonoObject instance to invoke the method on.
		 \param     method A pointer to the MonoMethod to invoke.
		 \param     params An array of pointers to the parameters to pass to the method.
		 \return    MonoObject* A pointer to the MonoObject that the method returns.
		*************************************************************************************/
		static MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params);

		/*!***********************************************************************************
		 \brief     Retrieves the MonoClass of the script class.
		 \return    MonoClass* A pointer to the MonoClass.
		*************************************************************************************/
		MonoClass* GetMonoClass() const { return m_MonoClass; }

	private:
		std::string m_ClassNamespace; ///< The namespace of the C# class.
		std::string m_ClassName;      ///< The name of the C# class.

		MonoClass* m_MonoClass;       ///< A pointer to the MonoClass representing the script class.
	};


	/*!***********************************************************************************
	 \brief     ScriptInstance represents an instance of a C# class and provides methods
				for invoking its lifecycle methods like OnCreate and OnUpdate.
	*************************************************************************************/
	class ScriptInstance
	{
	public:
		/*!***********************************************************************************
		 \brief     Default constructor for the ScriptInstance that initializes member variables.
		*************************************************************************************/
		ScriptInstance() = default;

		/*!***********************************************************************************
		 \brief     Constructor that binds a ScriptClass reference to this instance.
		 \param     scriptClass A shared pointer to the ScriptClass to associate with this instance.
		*************************************************************************************/
		ScriptInstance(Ref<ScriptClass> scriptClass);

		/*!***********************************************************************************
		 \brief     Invokes the 'OnCreate' method on the C# script instance.
		*************************************************************************************/
		void InvokeOnCreate();

		/*!***********************************************************************************
		 \brief     Invokes the 'OnUpdate' method on the C# script instance with the timestep.
		 \param     ts The timestep to pass to the 'OnUpdate' method.
		*************************************************************************************/
		void InvokeOnUpdate(float ts);

	private:
		Ref<ScriptClass> m_ScriptClass; ///< A shared pointer to the associated ScriptClass.

		MonoObject* m_Instance = nullptr;       ///< A pointer to the Mono object instance.
		MonoMethod* m_OnCreateMethod = nullptr; ///< A pointer to the 'OnCreate' MonoMethod.
		MonoMethod* m_OnUpdateMethod = nullptr; ///< A pointer to the 'OnUpdate' MonoMethod.

	};


	/*!***********************************************************************************
	 \brief     ScriptEngine is a Singleton class that initializes and shuts down the
				Mono environment, and manages the loading and unloading of script
				assemblies and class instances.
	*************************************************************************************/
	class ScriptEngine : public Singleton<ScriptEngine>
	{
	public:
		friend class Singleton<ScriptEngine>;

		/*!***********************************************************************************
		 \brief     Initializes the script engine and its underlying Mono environment.
		*************************************************************************************/
		static void Init();

		/*!***********************************************************************************
		 \brief     Shuts down the script engine and cleans up the Mono environment.
		*************************************************************************************/
		static void Shutdown();

		/*!***********************************************************************************
		 \brief     Loads a compiled C# assembly from a specified file path.
		 \param     filepath The filesystem path to the compiled assembly.
		*************************************************************************************/
		static void LoadAssembly(const std::filesystem::path& filepath);

		/*!***********************************************************************************
		 \brief     Retrieves a MonoClass from the engine's stored classes by key.
		 \param     key A string key associated with a MonoClass.
		 \return    MonoClass* A pointer to the MonoClass if found, nullptr otherwise.
		*************************************************************************************/
		MonoClass* GetMonoClassByKey(const std::string& key);

		/*!***********************************************************************************
		 \brief     Retrieves or creates a MonoObject associated with a given entity ID.
		 \param     objectID The unique entity ID to retrieve or create a MonoObject for.
		 \param     key The class key to associate the MonoObject with.
		 \return    MonoObject* A pointer to the existing or new MonoObject.
		*************************************************************************************/
		MonoObject* GetOrCreateMonoObject(const EntityID& objectID, const std::string& key);

		/*!***********************************************************************************
		 \brief     Retrieves a specific method from a MonoObject.
		 \param     obj A pointer to the MonoObject to retrieve the method from.
		 \param     methodName The name of the method to retrieve.
		 \param     paramCount The number of parameters that the method accepts.
		 \return    MonoMethod* A pointer to the MonoMethod if found, nullptr otherwise.
		*************************************************************************************/
		static MonoMethod* GetMethodFromMonoObject(MonoObject* obj, const char* methodName, int paramCount);

		/*!***********************************************************************************
		 \brief     Gets a map of all loaded entity classes.
		 \return    std::unordered_map<std::string, Ref<ScriptClass>> A map with keys as class names
					and values as shared pointers to the associated ScriptClass objects.
		*************************************************************************************/
		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();

	private:
		/*!***********************************************************************************
		 \brief     Initializes the Mono environment.
		*************************************************************************************/
		static void InitMono();

		/*!***********************************************************************************
		 \brief     Shuts down the Mono environment and cleans up resources.
		*************************************************************************************/
		static void ShutdownMono();

		/*!***********************************************************************************
		 \brief     Instantiates a class within the Mono environment.
		 \param     monoClass A pointer to the MonoClass to instantiate.
		 \return    MonoObject* A pointer to the instantiated Mono object.
		*************************************************************************************/
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		/*!***********************************************************************************
		 \brief     Loads and stores all the classes from a given Mono assembly.
		 \param     assembly A pointer to the MonoAssembly to load classes from.
		*************************************************************************************/
		static void LoadAssemblyClasses(MonoAssembly* assembly);

		friend class ScriptClass;
	};
}

