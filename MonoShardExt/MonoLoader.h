#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include "GMInterface.h"
#include "Export.h"
using namespace std;

void AttachC();

class MonoLoader
{
	public:
		static MonoDomain* s_RootDomain;
		static MonoDomain* s_AppDomain;
		static MonoAssembly* s_AppAssembly;
		static MonoObject* Game;
		static MonoObject* Player;
		static void InitMono();
		static void InitInternalCall();
		static MonoAssembly* LoadAssembly(const std::string& assemblyPath);
		static void PrintAssemblyTypes(MonoAssembly* assembly);
		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static MonoObject* InstantiateClass(const char* namespaceName, const char* className);
		static MonoObject* InstantiateClassWithoutInit(const char* namespaceName, const char* className);
		static MonoObject* CallMethod(MonoObject* obj, const char* methodName, int argc, void** params);
		static MonoObject* CallCtor(MonoObject* obj, int argc, void** params);
};

class InternalCallings
{
	public:
		static MonoString* GetGenPath();
		static MonoString* TestFunc(MonoString* val);
		static float GetInstancePropertyAsInteger(MonoString* name, int self, int other);
		static void SetInstancePropertyAsInteger(MonoString* name, float value, int self, int other);
		static MonoString* GetInstancePropertyAsString(MonoString* name, int self, int other);
		static void SetInstancePropertyAsString(MonoString* name, MonoString* value, int self, int other);
		static float GetValueFromMapAsInteger(MonoString* name, int map, CInstance* self, CInstance* other);
		static void SetValueInMapAsInteger(MonoString* name, int map, float value, CInstance* self, CInstance* other);
};