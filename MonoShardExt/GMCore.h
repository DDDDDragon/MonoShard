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

struct retVal
{
	int idk0;
	int idk1;
	int line;
	int idk2;
	//LODWORD(VM->pStack) + LODWORD(VM->stackSize) - v19
	int v20;
	int localCount;
	//LODWORD(VM->pStack) + LODWORD(VM->stackSize) - v11
	int idk3;
	int idk4;
	CInstance* pSelf;
	CInstance* pOther;
	CCode* pCCode;
	VMBuffer* pBuffer;
	VMBuffer* pDebugInfo;
	const char* pScript;
	char* pName;
	void* idk5;
	void* idk6;
	YYObjectBase* pLocals;
	void* idk7;
	void* v22;
};
