#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "GMCore.h"
#include "GameAddress.h"
#include "Export.h"
#include "SDK/Structures/Undocumented/YYObjectBase/YYObjectBase.hpp"
#include "SDK/Structures/Documented/RefThing/RefThing.hpp"
#include "GMInterface.h"
#include "MinHook.h"

#pragma comment(lib, "libMinHook.lib")

#define GMArgs RValue& result, CInstance* self, CInstance* other, int argc, RValue* args
#define GMFunc(pointer) RValue* (*func)(GMArgs);\
	func = (RValue*(*)(GMArgs))pointer;\
	func(result, self, other, argc, args);


static YYRunnerInterface g_YYRunnerInterface{};
YYRunnerInterface* g_pYYRunnerInterface;
__declspec(dllexport) void YYExtensionInitialise(const struct YYRunnerInterface* _struct, size_t _size) {
	if (_size < sizeof(YYRunnerInterface)) {
		memcpy(&g_YYRunnerInterface, _struct, _size);
	}
	else {
		memcpy(&g_YYRunnerInterface, _struct, sizeof(YYRunnerInterface));
	}
	g_pYYRunnerInterface = &g_YYRunnerInterface;
}

using namespace std;

GMS2EXPORT double InitMonoShard()
{
	AttachC();

	MonoLoader::InitMono();

	GMCore::InitializeCore();

	if (MH_Initialize() != MH_OK)
	{
		cout << "Initialize MinHook failed." << endl;
		return MH_ERROR_ALREADY_INITIALIZED;
	}

	Hooks::InnerHooks();

	return 1;
}


struct GMFunction
{
	union {
		unsigned char padding[64];
		char* name;
	};
	void* functionPointer;
	int32_t argc;
	int32_t usageCount;
	void Invoke(GMArgs)
	{
		GMFunc(functionPointer);
	}
};

static map<string, GMFunction> BuiltinFunctions;

char* GMCore::GetString(int address, const int length)
{
	char* s = new char[length];
	for (int i = 0; i < length; i++)
		s[i] = *(char*)(address + i);
	return s;
}

void GMCore::InitializeCore()
{
	LoadBuiltins();

	RValue res = {};
	CallBuiltin("@@GlobalScope@@", res, nullptr, nullptr, vector<RValue>{});

	gAPIVars.Globals.g_pGlobalObject = res.Object;

	gAPIVars.Functions.Code_Execute = (FNCodeExecute)GameAddress::ExecAddr;
}

void GMCore::LoadBuiltins()
{
	long offset = 0;
	char firstCode = *(char*)GameAddress::FuncAddr;
	while ((firstCode >= 97 && firstCode <= 122) || (firstCode >= 64 && firstCode <= 90) || firstCode == 36)
	{
		GMFunction gf;
		gf.name = GetString(GameAddress::FuncAddr + offset, 64);
		offset += 64;
		gf.functionPointer = *(void**)(GameAddress::FuncAddr + offset);
		offset += 8;
		gf.argc = *(int*)(GameAddress::FuncAddr + offset);
		offset += 4;
		gf.usageCount = *(int*)(GameAddress::FuncAddr + offset);
		offset += 4;
		firstCode = *(char*)(GameAddress::FuncAddr + offset);
		BuiltinFunctions[gf.name] = gf;
	}
}

void GMCore::CallBuiltin(const char* name, RValue& result, CInstance* self, CInstance* other, const vector<RValue>& args)
{
	bool global = !self && !other;

	if (BuiltinFunctions.count(name) == 0)
	{
		cout << "Cannot find builtin function with the name of " << name << "." << endl;
		return;
	}

	if (global) BuiltinFunctions[name].Invoke(result, gAPIVars.Globals.g_pGlobalInstance, gAPIVars.Globals.g_pGlobalInstance, args.size(), (RValue*)args.data());
	else BuiltinFunctions[name].Invoke(result, self, other, args.size(), (RValue*)args.data());
}

void GMCore::CallBuiltin(const char* name, RValue& result, CInstance* self, CInstance* other, const vector<YYRValue>& args)
{
	bool global = !self && !other;

	if (BuiltinFunctions.count(name) == 0)
	{
		cout << "Cannot find builtin function with the name of " << name << "." << endl;
		return;
	}

	if (global) BuiltinFunctions[name].Invoke(result, gAPIVars.Globals.g_pGlobalInstance, gAPIVars.Globals.g_pGlobalInstance, args.size(), (RValue*)args.data());
	else BuiltinFunctions[name].Invoke(result, self, other, args.size(), (RValue*)args.data());
}


#define rei reinterpret_cast

template <typename T>
inline MH_STATUS Hook(LPVOID pTarget, LPVOID pHook, T * *ppOriginal, const char* name)
{
	cout << "Hooking." << endl;

	return MH_CreateHook(pTarget, pHook, reinterpret_cast<LPVOID*>(ppOriginal));
}

void Hooks::InnerHooks()
{
	Hook(
		rei<void*>(GMCore::gAPIVars.Functions.Code_Execute),
		rei<void*>(&hokExecuteCode),
		rei<void**>(&pfnExecCodeOriginal),
		"ExecuteCode"
	);

	MH_EnableHook(rei<void*>(GMCore::gAPIVars.Functions.Code_Execute));

	Hook(
		rei<void*>(GameAddress::CallAddr),
		rei<void*>(&hokCallScript),
		rei<void**>(&pfnCallScriptOriginal),
		"CallScript"
	);

	MH_EnableHook(rei<void*>(GameAddress::CallAddr));
}

bool Hooks::hokExecuteCode(CInstance* self, CInstance* other, CCode* code, RValue* res, int flags)
{
	if (string(code->i_pName) == "gml_Object_o_player_Step_0")
	{
		if (MonoLoader::Player == nullptr)
		{
			MonoClass* klass = MonoLoader::GetClassInAssembly(MonoLoader::s_AppAssembly, "MonoShardModLib.PlayerUtils", "Player");
			MonoLoader::Player = mono_object_new(MonoLoader::s_AppDomain, klass);
			void* args[2] = { &self, &other };
			MonoLoader::CallCtor(MonoLoader::Player, 2, args);
		}
		MonoLoader::CallMethod(MonoLoader::Game, "PreUpdatePlayer", 0, nullptr);
	}
	bool ret = pfnExecCodeOriginal(self, other, code, res, flags);
	if (string(code->i_pName) == "gml_Object_o_player_Step_0")
		MonoLoader::CallMethod(MonoLoader::Game, "PostUpdatePlayer", 0, nullptr);
	return ret;
}

char* Hooks::hokCallScript(CScript* script, int argc, char* pStack, VMExec* VM, YYObjectBase* locals, YYObjectBase* arguments)
{
	return pfnCallScriptOriginal(script, argc, pStack, VM, locals, arguments);
}


// Get a instance's property.
// 获取某个实例的某个属性
RValue Utils::GetInstanceProperty(const char* name, CInstance* self, CInstance* other)
{
	RValue ret;
	GMCore::CallBuiltin("variable_instance_get", ret, self, other, { float(self->i_id), name });
	return ret;
}

// Set a instance's property.
// 设置某个实例的某个属性
RValue Utils::SetInstanceProperty(const char* name, YYRValue value, CInstance* self, CInstance* other)
{
	RValue ret;
	GMCore::CallBuiltin("variable_instance_set", ret, self, other, { float(self->i_id), name, value });
	return ret;
}

