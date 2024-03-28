#include "Structures/Documented/YYRValue/YYRValue.hpp"
#include "Structures/Documented/APIVars/APIVars.hpp"
#include "Structures/Documented/CCode/CCode.hpp"
#include "Structures/Undocumented/CScript/CScript.hpp"
#include "SDK/Structures/Undocumented/YYObjectBase/YYObjectBase.hpp"
#include "SDK\Structures\Undocumented\VMExec\VMExec.hpp"
#include "GMInterface.h"
#include "MonoLoader.h"
#include <vector>

using namespace std;

class GMCore
{
	public:
		inline static CAPIVars gAPIVars;

		static char* GetString(int address, const int length);
		static void LoadBuiltins();
		static void InitializeCore();
		static void CallBuiltin(const char* name, RValue& result, CInstance* self, CInstance* other, const vector<RValue>& args);
		static void CallBuiltin(const char* name, RValue& result, CInstance* self, CInstance* other, const vector<YYRValue>& args);
};

class Hooks
{
	public:
		static MonoObject* Game;

		static void InnerHooks();
		static bool hokExecuteCode(CInstance* self, CInstance* other, CCode* code, RValue* res, int flags);
		static char* hokCallScript(CScript* script, int argc, char* pStack, VMExec* VM, YYObjectBase* locals, YYObjectBase* arguments);
		static inline decltype(&hokExecuteCode) pfnExecCodeOriginal;
		static inline decltype(&hokCallScript) pfnCallScriptOriginal;
};

class Utils
{
	public:
		static RValue GetInstanceProperty(const char* name, CInstance* self, CInstance* other);
		static RValue SetInstanceProperty(const char* name, YYRValue value, CInstance* self, CInstance* other);
		static RValue GetValueFromMap(const char* name, YYRValue map, CInstance* self, CInstance* other);
};