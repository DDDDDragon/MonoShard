#include <cstdint>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "GMCore.h"
#include "GameAddress.h"
#include "Export.h"
#include "SDK/Structures/Undocumented/YYObjectBase/YYObjectBase.hpp"
#include "SDK/Structures/Documented/RefThing/RefThing.hpp"
#include "GMInterface.h"
#include "MinHook.h"
#include "Utils.h"

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
	Utils::LoadInnerFunctions();
	RValue res = {};
	CallBuiltin("@@GlobalScope@@", res, nullptr, nullptr, {});

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
	cout << "Hooking at " << name << "." << endl;

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

float testReal = -1;
CInstance* testSelf;
CInstance* testOther;

bool Hooks::hokExecuteCode(CInstance* self, CInstance* other, CCode* code, RValue* res, int flags)
{
	string name = string(code->i_pName);

	if (name == "gml_Object_o_player_Step_0")
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

	if (name == "gml_Object_o_player_Step_0")
		MonoLoader::CallMethod(MonoLoader::Game, "PostUpdatePlayer", 0, nullptr);

	if (name == "gml_Object_o_inv_slot_Other_17")
	{
		//Object的id 前8位是ref type 后8位是id
		//cout << (Utils::GetInstanceProperty("id", self, other).I64 & 0xFFFFFFFF) << endl;

		void* args[3] = { 
			mono_string_new(MonoLoader::s_AppDomain, Utils::GetInstanceProperty("idName", self, other).String->m_Thing),
			&self,
			&other
		};

		MonoClass* weaponKlass = MonoLoader::GetClassInAssembly(MonoLoader::s_AppAssembly, "MonoShardModLib.ItemUtils", "Weapon");
		MonoMethod* createInstance = mono_class_get_method_from_name(weaponKlass, "CreateInstance", 3);
		MonoObject* weapon = mono_runtime_invoke(createInstance, NULL, args, nullptr);

		void* args_[1] = { weapon };

		MonoLoader::CallMethod(MonoLoader::Game, "InitializeWeapon", 1, args_);
	}

	if (name == "gml_Object_o_dataLoader_Other_10")
	{
		cout << *(int*)GameAddress::SpritesCount << endl;

		Utils::AddSprite("C:\\Users\\ASUS\\AppData\\Local\\StoneShard\\tModLoader.png");

		RValue val;
		GMCore::CallBuiltin("sprite_get_name", val, self, other, { 12842.0 });

		cout << val.String->m_Thing << endl;
	}
	return ret;
}

char* Hooks::hokCallScript(CScript* script, int argc, char* pStack, VMExec* VM, YYObjectBase* locals, YYObjectBase* arguments)
{
	//script->s_script 函数名称
	//VM->pName 当前所处的GameObject的Event名称
	//(CInstance*)(VM->pSelf) 获取Self对象
	//(CInstance*)(VM->pOther) 获取Other对象
	//未知原因 获取不到self对应的local vars
	//可能遍历locals可以搞到
	string name = string(script->s_script);

	CInstance* self = (CInstance*)(VM->pSelf);
	CInstance* other = (CInstance*)(VM->pOther);

	char* ret = pfnCallScriptOriginal(script, argc, pStack, VM, locals, arguments);

	self = (CInstance*)(VM->pSelf);
	other = (CInstance*)(VM->pOther);

	return ret;
}

