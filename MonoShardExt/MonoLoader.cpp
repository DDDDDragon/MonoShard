#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include "MonoLoader.h"
#include "GMInterface.h"
#include "Export.h"
#include "GMCore.h"
#include "SDK/Structures/Undocumented/YYObjectBase/YYObjectBase.hpp"
#include "SDK/Structures/Documented/RefThing/RefThing.hpp"
#pragma comment(lib, "mono-2.0-sgen.lib")

using namespace std;

void AttachC()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::cout << "Attach output to command successfully." << endl;
}
MonoDomain* MonoLoader::s_AppDomain = nullptr;
MonoAssembly* MonoLoader::s_AppAssembly = nullptr;
MonoObject* MonoLoader::Game = nullptr;
MonoObject* MonoLoader::Player = nullptr;

void MonoLoader::InitMono()
{
	mono_set_assemblies_path("mono/lib/4.5");

	s_AppDomain = mono_jit_init_version("MyScriptRuntime", "v6.12.0.200");
	mono_domain_set(s_AppDomain, true);

	InitInternalCall();

	s_AppAssembly = LoadAssembly(filesystem::current_path().string() + "\\MonoShardModLib.dll");

	Game = InstantiateClass("MonoShardModLib", "Game");

	MonoClass* ModLoader = MonoLoader::GetClassInAssembly(s_AppAssembly, "MonoShardModLib", "ModLoader");
	MonoMethod* Initialize = mono_class_get_method_from_name(ModLoader, "Initialize", 0);

	mono_runtime_invoke(Initialize, NULL, nullptr, nullptr);
	return;
}

void MonoLoader::InitInternalCall()
{
	mono_add_internal_call("MonoShardModLib.Core::GetGenPath", InternalCallings::GetGenPath);
	mono_add_internal_call("MonoShardModLib.Core::TestFunc", InternalCallings::TestFunc);
	mono_add_internal_call("MonoShardModLib.Core::GetInstancePropertyAsInteger", InternalCallings::GetInstancePropertyAsInteger);
	mono_add_internal_call("MonoShardModLib.Core::SetInstancePropertyAsInteger", InternalCallings::SetInstancePropertyAsInteger);
	mono_add_internal_call("MonoShardModLib.Core::GetInstancePropertyAsString", InternalCallings::GetInstancePropertyAsString);
	mono_add_internal_call("MonoShardModLib.Core::SetInstancePropertyAsString", InternalCallings::SetInstancePropertyAsString);
	mono_add_internal_call("MonoShardModLib.Core::GetValueFromMapAsInteger", InternalCallings::GetValueFromMapAsInteger);
	mono_add_internal_call("MonoShardModLib.Core::SetValueInMapAsInteger", InternalCallings::SetValueInMapAsInteger);
}

char* ReadBytes(const std::string& filePath, uint32_t* outSize)
{
	std::ifstream stream(filePath, std::ios::binary | std::ios::ate);

	if (!stream) return nullptr;

	std::streampos end = stream.tellg();
	stream.seekg(0, std::ios::beg);
	uint32_t size = end - stream.tellg();

	if (size == 0) return nullptr;

	char* buffer = new char[size];
	stream.read((char*)buffer, size);
	stream.close();

	*outSize = size;
	return buffer;
}

MonoAssembly* MonoLoader::LoadAssembly(const std::string& assemblyPath)
{
	uint32_t size = 0;
	char* fileData = ReadBytes(assemblyPath, &size);

	MonoImageOpenStatus status;
	MonoImage* image = mono_image_open_from_data_full(fileData, size, 1, &status, 0);

	if (status != MONO_IMAGE_OK)
	{
		const char* errorMsg = mono_image_strerror(status);
		return nullptr;
	}

	MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
	mono_image_close(image);
	delete[] fileData;

	return assembly;
}

void MonoLoader::PrintAssemblyTypes(MonoAssembly* assembly)
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

MonoClass* MonoLoader::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
	MonoImage* image = mono_assembly_get_image(assembly);
	MonoClass* klass = mono_class_from_name(image, namespaceName, className);

	if (klass == nullptr)
	{
		// Log error here
		return nullptr;
	}

	return klass;
}

MonoObject* MonoLoader::InstantiateClass(const char* namespaceName, const char* className)
{
	MonoClass* testingClass = GetClassInAssembly(s_AppAssembly, namespaceName, className);

	MonoObject* classInstance = mono_object_new(s_AppDomain, testingClass);

	if (classInstance == nullptr)
	{
		return NULL;
	}

	mono_runtime_object_init(classInstance);
	return classInstance;
}

MonoObject* MonoLoader::InstantiateClassWithoutInit(const char* namespaceName, const char* className)
{
	MonoClass* testingClass = GetClassInAssembly(s_AppAssembly, namespaceName, className);

	MonoObject* classInstance = mono_object_new(s_AppDomain, testingClass);

	if (classInstance == nullptr)
	{
		return NULL;
	}
	return classInstance;
}

MonoObject* MonoLoader::CallMethod(MonoObject* obj, const char* methodName, int argc, void** params)
{
	MonoClass* klass = mono_object_get_class(obj);
	if (klass == nullptr) return nullptr;
	MonoMethod* method = mono_class_get_method_from_name(klass, methodName, argc);
	if (method == nullptr) return nullptr;
	return mono_runtime_invoke(method, obj, params, nullptr);
}

MonoObject* MonoLoader::CallCtor(MonoObject* obj, int argc, void** params)
{
	MonoClass* klass = mono_object_get_class(obj);
	if (klass == nullptr) return nullptr;
	MonoMethod* method = mono_class_get_method_from_name(klass, ".ctor", argc);
	if (method == nullptr) return nullptr;
	return mono_runtime_invoke(method, obj, params, nullptr);
}

MonoString* InternalCallings::GetGenPath()
{
	return mono_string_new(mono_domain_get(), (const char*)filesystem::current_path().string().c_str());
}

MonoString* InternalCallings::TestFunc(MonoString* val)
{
	return val;
}

float InternalCallings::GetInstancePropertyAsInteger(MonoString* name, int self, int other)
{
	RValue val = Utils::GetInstanceProperty(mono_string_to_utf8(name), (CInstance*)self, (CInstance*)other);
	return KIND_RValue(&val) == VALUE_REAL ? val.Real : 0;
}

void InternalCallings::SetInstancePropertyAsInteger(MonoString* name, float value, int self, int other)
{
	Utils::SetInstanceProperty(mono_string_to_utf8(name), value, (CInstance*)self, (CInstance*)other);
	return;
}

MonoString* InternalCallings::GetInstancePropertyAsString(MonoString* name, int self, int other)
{
	RValue val = Utils::GetInstanceProperty(mono_string_to_utf8(name), (CInstance*)self, (CInstance*)other);
	return KIND_RValue(&val) == VALUE_STRING ? mono_string_new(MonoLoader::s_AppDomain, val.String->m_Thing) : mono_string_new(MonoLoader::s_AppDomain, "");
}

void InternalCallings::SetInstancePropertyAsString(MonoString* name, MonoString* value, int self, int other)
{
	Utils::SetInstanceProperty(mono_string_to_utf8(name), mono_string_to_utf8(value), (CInstance*)self, (CInstance*)other);
	return;
}

float InternalCallings::GetValueFromMapAsInteger(MonoString* name, int map, CInstance* self, CInstance* other)
{
	RValue val = Utils::GetValueFromMap(mono_string_to_utf8(name), (float)map, self, other);
	return KIND_RValue(&val) == VALUE_REAL ? val.Real : 0;
}

void InternalCallings::SetValueInMapAsInteger(MonoString* name, int map, float value, CInstance* self, CInstance* other)
{
	Utils::SetValueInMap(mono_string_to_utf8(name), (float)map, value, self, other);
	return;
}
