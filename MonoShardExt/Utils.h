#include "Structures/Documented/YYRValue/YYRValue.hpp"
#include "Structures/Documented/APIVars/APIVars.hpp"
#include "Structures/Documented/CCode/CCode.hpp"
#include "Structures/Undocumented/CScript/CScript.hpp"
#include "SDK/Structures/Undocumented/YYObjectBase/YYObjectBase.hpp"
#include "SDK\Structures\Undocumented\VMExec\VMExec.hpp"
#include "GMInterface.h"
#include "MonoLoader.h"

class Utils
{
	public:
		static void LoadInnerFunctions();
		static RValue GetInstanceProperty(const char* name, CInstance* self, CInstance* other);
		static RValue SetInstanceProperty(const char* name, YYRValue value, CInstance* self, CInstance* other);
		static RValue GetValueFromList(int index, YYRValue map, CInstance* self, CInstance* other);
		static RValue SetValueInList(int index, YYRValue map, YYRValue value, CInstance* self, CInstance* other);
		static RValue GetValueFromMap(const char* name, YYRValue map, CInstance* self, CInstance* other);
		static RValue SetValueInMap(const char* name, YYRValue map, YYRValue value, CInstance* self, CInstance* other);
		static int AddSprite(const char* spritePath);
		static int AddSprite(char* spriteData, const char* spriteName);
		static int GetRealSize(char* data);
		static const char* GetFileName(const char* filePath);
		static const char* GetFileNameWithoutExtension(const char* filePath);
		static void* (*AllocMemory)(size_t, int64, int64, int64);
};