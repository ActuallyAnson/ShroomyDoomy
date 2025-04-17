/******************************************************************************/
/*!
\file    ScriptSystem.h
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the declarations of the ScriptSystem, ScriptClass,
		 and ScriptInstance classes, which form the backbone of the scripting
		 integration in the game engine. These classes provide mechanisms to
		 manage C# scripts, load assemblies, and interface with Mono's runtime.

		 The ScriptSystem handles the initialization of the Mono environment,
		 the loading of assemblies, and the registration of script behaviors.
		 ScriptClass encapsulates Mono class interactions, while ScriptInstance
		 manages individual instances of scripts attached to entities.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Utility.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace FruitLoops {

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int paramCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		MonoClass* GetMonoClass() const { return m_MonoClass; } // Add this method

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass);
		MonoObject* GetInstance();
		Ref<ScriptClass> GetClass() const;

		void SetInstance(MonoObject* instance);
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

	};

	class ScriptSystem : public EngineSystems {
	public:
		ScriptSystem();
		~ScriptSystem();

		void Initialize() override;
		void Update(float deltaTime) override;

		static void LoadAssembly(const std::filesystem::path& filepath);

		static MonoObject* ScriptSystem::GetManagedInstance(const std::string& name);
		static MonoImage* GetCoreAssemblyImage();
		static void RegisterManagedInstance(const std::string& name, std::shared_ptr<ScriptInstance> instance);
		static void ClearManagedInstances();
		static void CallStaticMethod(const std::string& className, const std::string& methodName, void** params);
		static void RegisterBehaviorComponents();
		static void AddBehaviorToEntity(Entity entity, const std::string& behaviorName);
		static bool IsValidBehavior(const std::string& behaviorName);

		static const std::unordered_map<std::string, Ref<ScriptClass>>& GetBehaviorMap();
		void InitializeBehaviorComponents();

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static MonoObject* GetOrCreateEntityMonoObject(Entity entity);
		static void LoadAssemblyClasses(MonoAssembly* assembly);

		static std::unordered_map<std::string, std::shared_ptr<ScriptInstance>> s_ManagedInstances;
		static std::unordered_map<std::string, Ref<ScriptClass>> behaviorMap;

		friend class ScriptClass;
	};
	extern ScriptSystem* SCRIPTSYSTEM;
}