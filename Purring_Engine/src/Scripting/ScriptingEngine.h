#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include "Scripting/Base.h"
#include <map>
#include "Singleton.h"
#include "CSharpScriptComponent.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	//typedef struct _MonoImage MonoImage;
}

namespace PE
{
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		MonoClass* GetMonoClass() const { return m_MonoClass; }

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass);


		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
	private:
		Ref<ScriptClass> m_ScriptClass;


		MonoObject* m_Instance = nullptr;
		//MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

	};

	class ScriptEngine : public Singleton<ScriptEngine>
	{
	public:
		friend class Singleton<ScriptEngine>;

		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);

		MonoClass* GetMonoClassByKey(const std::string& key);
		MonoObject* GetOrCreateMonoObject(const EntityID& objectID, const std::string& key);
		static MonoMethod* GetMethodFromMonoObject(MonoObject* obj, const char* methodName, int paramCount);

		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();


	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses(MonoAssembly* assembly);

		friend class ScriptClass;
	};
}

