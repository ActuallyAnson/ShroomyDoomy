/******************************************************************************/
/*!
\file    ScriptSystem.cpp
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the implementation of the ScriptSystem class,
		 which manages the integration of Mono scripting into the game engine.
		 It provides functionality for loading assemblies, managing script
		 instances, and facilitating behavior component integration.

		 The ScriptSystem also ensures the seamless execution of C# scripts
		 through a managed runtime environment, supporting entity behaviors
		 and other game logic.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "DebugMacros.h"
#include "ScriptSystem.h"
#include "ScriptGlue.h"
#include "ScriptComponent.h"

namespace FruitLoops {

	namespace Utils {

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
			uint64_t size = end - stream.tellg();

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

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				//const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string string = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, string.c_str(), &status, 0);
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
				printf("%s.%s\n", nameSpace, name);
			}
		}

		// Function to check if the method is static
		bool IsMethodStatic(MonoMethod* monoMethod)
		{
			if (!monoMethod) return false;

			MonoMethodSignature* signature = mono_method_signature(monoMethod);
			if (!signature) return false;

			// Static methods return false for `mono_signature_is_instance`
			return !mono_signature_is_instance(signature);
		}
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptInstance>>
								ScriptSystem::s_ManagedInstances;

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptSystem::behaviorMap;

	ScriptSystem* SCRIPTSYSTEM = NULL;

	struct ScriptSystemData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
	};

	static ScriptSystemData* s_Data = nullptr;

	ScriptSystem::ScriptSystem()
	{
		SCRIPTSYSTEM = this;
	}

	ScriptSystem::~ScriptSystem()
	{
		ClearManagedInstances();
		ShutdownMono();
		SCRIPTSYSTEM = nullptr;
	}

	void ScriptSystem::Initialize() {
		FL_DEBUG_INFO("ScriptSystem: Initializing...");

		s_Data = new ScriptSystemData();

		try {
			InitMono();
			FL_DEBUG_INFO("Mono runtime initialized successfully.");

			LoadAssembly("../Assets/Scripts/Fruitloops-ScriptCore.dll");
			FL_DEBUG_INFO("Core assembly loaded successfully.");

			LoadAssemblyClasses(s_Data->CoreAssembly);
			FL_DEBUG_INFO("Assembly classes loaded successfully.");

			ScriptGlue::RegisterFunctions();
			FL_DEBUG_INFO("Internal calls registered successfully.");

			// Register all valid behaviors on GameLogic side
			RegisterBehaviorComponents();
			FL_DEBUG_INFO("Retrieving GameLogicSystem singleton instance...");


			

			MonoClass* gameLogicClass = mono_class_from_name(s_Data->CoreAssemblyImage, "FruitLoops", "GameLogicSystem");
			if (!gameLogicClass) {
				FL_DEBUG_ERROR("Failed to find GameLogicSystem class in the assembly.");
				return;
			}

			MonoMethod* getInstanceMethod = mono_class_get_method_from_name(gameLogicClass, "get_Instance", 0);
			if (!getInstanceMethod) {
				FL_DEBUG_ERROR("Failed to find GameLogicSystem get_Instance method.");
				return;
			}

			MonoObject* gameLogicInstance = mono_runtime_invoke(getInstanceMethod, nullptr, nullptr, nullptr);
			if (!gameLogicInstance) {
				FL_DEBUG_ERROR("Failed to retrieve GameLogicSystem singleton instance.");
				return;
			}



			// Register the singleton instance
			auto scriptInstance = std::make_shared<ScriptInstance>(std::make_shared<ScriptClass>("FruitLoops", "GameLogicSystem"));
			scriptInstance->SetInstance(gameLogicInstance);
			RegisterManagedInstance("FruitLoops.GameLogicSystem", scriptInstance);
		


			FL_DEBUG_INFO("GameLogicSystem singleton instance retrieved and registered.");

			// For entities with <ScriptComponent>, if namefield corresponds to a BehaviourComponent, add behaviour to entity
			InitializeBehaviorComponents();
		}
		catch (const std::exception& ex) {
			FL_DEBUG_ERROR("ScriptSystem initialization failed: {}", ex.what());

			UNUSED_PARAM(ex);
		}
	}

	void ScriptSystem::Update(float deltaTime) {
		UNUSED_PARAM(deltaTime);
		
	}

	void ScriptSystem::InitMono()
	{
		mono_set_assemblies_path("../Dependancies/lib"); 

		MonoDomain* rootDomain = mono_jit_init("FruitloopsJITRuntime");
		if (rootDomain == nullptr) { return; }

		// Store the root domain pointer
		s_Data->RootDomain = rootDomain;
	}

	void ScriptSystem::ShutdownMono() {
		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;

		delete s_Data;
	}

	void ScriptSystem::LoadAssembly(const std::filesystem::path& filepath)
	{
		spdlog::info("ScriptSystem: Loading assembly from {}", filepath.string());
		try {
			s_Data->AppDomain = mono_domain_create_appdomain("FruitloopsDomain", nullptr);
			mono_domain_set(s_Data->AppDomain, true);

			spdlog::info("AppDomain created and set successfully.");

			s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
			if (!s_Data->CoreAssembly) {
				spdlog::error("Failed to load Mono assembly from {}", filepath.string());
				return;
			}

			s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
			spdlog::info("Core assembly image loaded successfully.");
		}
		catch (const std::exception& ex) {
			spdlog::error("Error loading assembly: {}", ex.what());
			UNUSED_PARAM(ex);
		}
	}

	void ScriptSystem::LoadAssemblyClasses(MonoAssembly* assembly) {
		s_Data->EntityClasses.clear();

		FL_DEBUG_INFO("ScriptSystem: Loading assembly classes...");

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		FL_DEBUG_INFO("Found {} types in the assembly.", numTypes);

		MonoClass* entityClass = mono_class_from_name(image, "FruitLoops", "Entity");
		MonoClass* behaviourClass = mono_class_from_name(image, "FruitLoops", "BehaviourComponent");

		if (!entityClass) FL_DEBUG_WARN("Failed to find base Entity class.");
		if (!behaviourClass) FL_DEBUG_WARN("Failed to find base BehaviourComponent class.");


		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string fullName = (strlen(nameSpace) != 0) ? fmt::format("{}.{}", nameSpace, name) : name;

			if (strstr(name, "<") != nullptr || strstr(name, ">") != nullptr) {
				FL_DEBUG_INFO("Skipping compiler-generated class: {}", name);
				continue;
			}

			FL_DEBUG_INFO("Loading class {} from namespace {}.", name, nameSpace);
			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);
			if (!monoClass) {
				FL_DEBUG_ERROR("Could not find class {} in namespace {}.", name, nameSpace);
				return;
			}
			else if (!entityClass) {
				FL_DEBUG_ERROR("Could not find entityClass {} in namespace {}.", name, nameSpace);
				return;
			}
			else {
				FL_DEBUG_INFO("Class {} loaded successfully.", name);
			}

			// Check if the class is a subclass of Entity
			if (entityClass && mono_class_is_subclass_of(monoClass, entityClass, false)) {
				FL_DEBUG_INFO("Found entity subclass: {}", fullName);

				s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
			}

			// Check if the class is a subclass of BehaviourComponent
			if (behaviourClass && mono_class_is_subclass_of(monoClass, behaviourClass, false)) {
				FL_DEBUG_INFO("Found behavior component: {}", fullName);

				behaviorMap[fullName] = CreateRef<ScriptClass>(nameSpace, name);
			}
		}

		FL_DEBUG_INFO("Assembly class loading complete. Found {} entity classes and {} behavior components.",
			s_Data->EntityClasses.size(), behaviorMap.size());
	}

	void ScriptSystem::RegisterManagedInstance(const std::string& name, std::shared_ptr<ScriptInstance> instance)
	{
		s_ManagedInstances[name] = instance;
	}

	void ScriptSystem::ClearManagedInstances()
	{
		s_ManagedInstances.clear();
	}

	void ScriptSystem::CallStaticMethod(const std::string& className, const std::string& methodName, void** params)
	{
		//spdlog::info("ScriptSystem: Calling static method '{}::{}'", className, methodName);

		if (!s_Data->CoreAssemblyImage) {
			spdlog::error("Core assembly image not loaded! Cannot call static method.");

			throw std::runtime_error("Core assembly image not loaded!");
		}

		try {
			size_t dotPos = className.find_last_of('.');
			std::string classNamespace = className.substr(0, dotPos);
			std::string classShortName = className.substr(dotPos + 1);

			MonoClass* monoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), classShortName.c_str());
			if (!monoClass) {
				spdlog::error("Class '{}' not found.", className);

				throw std::runtime_error("Class not found: " + className);
			}

			MonoMethod* monoMethod = mono_class_get_method_from_name(monoClass, methodName.c_str(), -1);
			if (!monoMethod) {
				spdlog::error("Method '{}' not found in class '{}'.", methodName, className);

				throw std::runtime_error("Method not found: " + methodName);
			}

			MonoObject* instance = nullptr;
			if (!Utils::IsMethodStatic(monoMethod)) {
				instance = GetManagedInstance(className);
				if (!instance) {
					spdlog::error("Instance for '{}' is null. Cannot invoke instance method '{}'.", className, methodName);

					return;
				}
			}

			//spdlog::info("Invoking method '{}::{}'...", className, methodName);
			mono_runtime_invoke(monoMethod, instance, params, nullptr);
			//spdlog::info("Successfully invoked method '{}::{}'.", className, methodName);
		}
		catch (const std::exception& ex) {
			spdlog::error("Error calling static method '{}::{}': {}", className, methodName, ex.what());

			UNUSED_PARAM(ex);
		}
	}

	MonoObject* ScriptSystem::GetManagedInstance(const std::string& name) {
		auto it = s_ManagedInstances.find(name);
		if (it == s_ManagedInstances.end()) {


			spdlog::warn("Managed instance not found for '{}'", name);

			return nullptr;
		}
		return it->second->GetInstance();
	}

	MonoObject* ScriptSystem::InstantiateClass(MonoClass* monoClass)
	{
		spdlog::info("ScriptSystem: Instantiating class '{}'", mono_class_get_name(monoClass));

		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		if (!instance) {
			spdlog::error("Failed to instantiate class '{}'.", mono_class_get_name(monoClass));

			throw std::runtime_error("Failed to instantiate class");
		}

		mono_runtime_object_init(instance);

		spdlog::info("Class '{}' instantiated successfully.", mono_class_get_name(monoClass));

		return instance;
	}

	MonoObject* ScriptSystem::GetOrCreateEntityMonoObject(Entity entity) {
		spdlog::info("GetOrCreateEntityMonoObject called for Entity {}.", static_cast<uint64_t>(entity));

		spdlog::info("Creating new MonoObject for Entity {}.", static_cast<uint64_t>(entity));


		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "FruitLoops", "Entity");
		if (!entityClass) {
			spdlog::error("Failed to find 'Entity' class in the assembly.");

			return nullptr;
		}


		MonoObject* entityObject = mono_object_new(s_Data->AppDomain, entityClass);
		if (!entityObject) {
			spdlog::error("Failed to create MonoObject for Entity {}.", static_cast<uint64_t>(entity));

			return nullptr;
		}

		mono_runtime_object_init(entityObject);

		spdlog::info("Successfully registered MonoObject for Entity {}.", static_cast<uint64_t>(entity));
		spdlog::info("Registering MonoObject for Entity {}.", static_cast<uint64_t>(entity));

		uint64_t entityID = static_cast<uint64_t>(entity);
		MonoClassField* idField = mono_class_get_field_from_name(entityClass, "ID");

		if (idField) {
			mono_field_set_value(entityObject, idField, &entityID);

			spdlog::info("Set Entity ID {} in MonoObject for Entity {}.", entityID, entityID);
		}
		else {
			spdlog::warn("Failed to find 'ID' field in Entity MonoClass.");

		}


		return entityObject;
	}

	MonoImage* ScriptSystem::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage; // Assuming you store the image here
	}


	/*---------------------------BEHAVIOUR FCT---------------------------*/

	void ScriptSystem::RegisterBehaviorComponents()
	{
		spdlog::info("ScriptSystem: Registering behavior components...");

		MonoClass* behaviourBaseClass = mono_class_from_name(s_Data->CoreAssemblyImage, "FruitLoops", "BehaviourComponent");
		if (!behaviourBaseClass)
		{
			spdlog::error("Failed to find BehaviourComponent base class.");
			return;
		}

		for (const auto& [name, scriptClass] : s_Data->EntityClasses)
		{
			MonoClass* monoClass = scriptClass->GetMonoClass();
			spdlog::info("Checking class: {}", name);

			if (mono_class_is_subclass_of(monoClass, behaviourBaseClass, false))
			{
				spdlog::info("Registered behavior component: {}", name);
				behaviorMap[name] = scriptClass;
			}
			else
				spdlog::info("Class '{}' is not a subclass of BehaviourComponent.", name);
		}

		spdlog::info("Behavior component registration complete. Registered behaviors:");
		for (const auto& [name, scriptClass] : behaviorMap)
		{
			spdlog::info(" - {}", name);
		}
	}

	void ScriptSystem::InitializeBehaviorComponents() {
		auto entitiesWithScripts = ecsHub->GetEntitiesWithComponents<ScriptComponent>();

		for (auto entity : entitiesWithScripts) {
			
			auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
			if (!scriptComponent.Name.empty() && IsValidBehavior(scriptComponent.Name) && scriptComponent.isActive) {
				spdlog::info("Adding behaviour : {}", scriptComponent.Name);
				AddBehaviorToEntity(entity, scriptComponent.Name);
				//scriptComponent.UpdateScriptInstance();
			}
		}
	}

	void ScriptSystem::AddBehaviorToEntity(Entity entity, const std::string& behaviorName) {
		spdlog::info("Attempting to add behavior '{}' to entity {}.", behaviorName, entity);

		if (!ecsHub->HasComponent<ScriptComponent>(entity)) {
			spdlog::warn("Entity {} does not have a ScriptComponent.", entity);

			return;
		}

		auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);

		// skip non active entities with script
		

		if (scriptComponent.Name != behaviorName) {
			spdlog::warn("Behavior '{}' does not match ScriptComponent name '{}'. Skipping.", behaviorName, scriptComponent.Name);
			return;
		}

		auto it = behaviorMap.find(behaviorName);
		if (it == behaviorMap.end()) {
			spdlog::error("Behavior '{}' not found in behavior map.", behaviorName);

			return;
		}

		Ref<ScriptClass> scriptClass = it->second;
		MonoClass* monoClass = scriptClass->GetMonoClass();


		spdlog::info("Retrieved MonoClass for '{}'. Attempting to call 'CreateBehaviour'...", behaviorName);


		MonoMethod* createBehaviourMethod = mono_class_get_method_from_name(monoClass, "CreateBehaviour", 1);
		if (!createBehaviourMethod) {
			spdlog::error("Failed to find 'CreateBehaviour' method in behavior '{}'.", behaviorName);

			return;
		}

		MonoObject* entityObject = GetOrCreateEntityMonoObject(entity);
		if (!entityObject) {
			spdlog::error("Failed to create or retrieve MonoObject for entity {}.", entity);

			return;
		}


		spdlog::info("Behavior '{}' found. Associated MonoClass retrieved successfully.", behaviorName);


		void* args[] = { entityObject };
		try {
			spdlog::info("Invoking 'CreateBehaviour' for '{}'.", behaviorName);

			if (!createBehaviourMethod) {
				FL_DEBUG_ERROR("CreateBehaviour method not found.");
				return;
			}

			if (!entityObject) {
				FL_DEBUG_ERROR("Entity object is null.");
				return;
			}

			MonoObject* behaviourInstance = mono_runtime_invoke(createBehaviourMethod, nullptr, args, nullptr);

			if (!behaviourInstance) {


				spdlog::error("Failed to create behaviour instance for '{}'.", behaviorName);

				return;
			}

			scriptComponent.Instance = CreateRef<ScriptInstance>(scriptClass);
			scriptComponent.Instance->SetInstance(behaviourInstance);


			spdlog::info("Successfully linked behavior '{}' to entity {} using ScriptComponent.", behaviorName, entity);

		}
		catch (const std::exception& ex) {
			spdlog::error("Exception during 'CreateBehaviour' invocation for '{}': {}", behaviorName, ex.what());

			UNUSED_PARAM(ex);
		}
	}

	bool ScriptSystem::IsValidBehavior(const std::string& behaviorName) {
		return behaviorMap.find(behaviorName) != behaviorMap.end();
	}

	const std::unordered_map<std::string, Ref<ScriptClass>>& ScriptSystem::GetBehaviorMap() {
		return behaviorMap;
	}

	/*---------------------------SCRIPT CLASS---------------------------*/

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
		if (!m_MonoClass)
		{
			throw std::runtime_error("Failed to find MonoClass for " + classNamespace + "::" + className);
		}
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptSystem::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

	/*---------------------------SCRIPT INSTANCE---------------------------*/

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();
	}

	MonoObject* ScriptInstance::GetInstance()
	{
		return m_Instance;
	}

	Ref<ScriptClass> ScriptInstance::GetClass() const
	{
		return Ref<ScriptClass>();
	}

	void ScriptInstance::SetInstance(MonoObject* instance) {
		m_Instance = instance;
	}
}