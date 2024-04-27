#include "Utils.h"
#include "GMCore.h"
#include "GameAddress.h"
#include <fstream>
#include <iostream>

using namespace std;

void* (*Utils::AllocMemory)(size_t, int64, int64, int64) = 0;

void Utils::LoadInnerFunctions()
{
	AllocMemory = (void* (*)(size_t, int64, int64, int64))0x1404CB950;
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

RValue Utils::GetValueFromList(int index, YYRValue list, CInstance* self, CInstance* other)
{
	RValue ret;
	GMCore::CallBuiltin("ds_list_find_value", ret, self, other, { list, (float)index });
	return ret;
}

RValue Utils::SetValueInList(int index, YYRValue list, YYRValue value, CInstance* self, CInstance* other)
{
	RValue ret;
	GMCore::CallBuiltin("ds_list_replace", ret, self, other, { list, (float)index, value });
	return ret;
}

RValue Utils::GetValueFromMap(const char* name, YYRValue map, CInstance* self, CInstance* other)
{
	RValue ret;
	GMCore::CallBuiltin("ds_map_find_value", ret, self, other, { map, name });
	return ret;
}

RValue Utils::SetValueInMap(const char* name, YYRValue map, YYRValue value, CInstance* self, CInstance* other)
{
	RValue ret;
	GMCore::CallBuiltin("ds_map_replace", ret, self, other, { map, name, value });
	return ret;
}

int Utils::AddSprite(const char* spritePath)
{
	if (!spritePath) return -1;

	ifstream sprite;

	sprite.open(spritePath, ios::in | ios::binary);
	if (sprite.is_open())
	{
		char* (*func)(const char*, int*);
		func = (char* (*)(const char*, int*))0x1400C2FC0;
		int fileSize;
		char* file = func(spritePath, &fileSize);

		return AddSprite(file, GetFileNameWithoutExtension(spritePath));
	}
	return -1;
}

int Utils::AddSprite(char* spriteData, const char* spriteName)
{
	if (!spriteData) return -1;

	char* buffer = new char[256];
	memset(buffer, 0, 256);

	*(int64*)0x140A24F00 = ((int64(*)(int64, size_t, int, int))0x1404CBCD0)(
		*(int64*)0x140A24F00,
		8i64 * ++ * (int*)GameAddress::SpritesCount,
		0,
		0
	);
	*(int*)0x140A24EF8 = *(int*)GameAddress::SpritesCount;
	*(int64*)0x140A24F08 = ((int64(*)(int64, size_t, int, int))0x1404CBCD0)(
		*(int64*)0x140A24F08,
		8i64 * *(int*)GameAddress::SpritesCount,
		0,
		0
	);

	sprintf_s(buffer, 100ui64, spriteName);
	int bufferSize = GetRealSize(buffer);

	void* memory = AllocMemory(bufferSize + 1i64, 0, 0, 0);
	memmove(memory, buffer, bufferSize + 1i64);
	int64 index = *(int*)GameAddress::SpritesCount - 1i64;
	(*(void***)0x140A24F08)[index] = memory;

	void* memory2 = AllocMemory(0xD8ui64, index, 0, 1);
	void* newSprite;
	if (memory2)
		newSprite = ((void* (*)(void*))0x140119510)(memory2);
	else
		newSprite = 0;
	(*(void***)0x140A24F00)[index] = newSprite;

	bool (*addSprite)(void*, const char*, int, int, int, bool, bool, int, int, bool);
	addSprite = (bool(*)(void*, const char*, int, int, int, bool, bool, int, int, bool))0x140116AD0;
	addSprite(newSprite, spriteData, GetRealSize(spriteData), 1, 0, false, false, 0, 0, true);

	*(int*)((int)(*(int64***)0x140A24F00)[index] + 180i64) = index;
	*(int64*)((int)(*(int64***)0x140A24F00)[index] + 80i64) = (int)(*(int64***)0x140A24F08)[index];

	((void(*)(int64, int64, int64))0x14006B6F0)(0x1407E7C90, (int64)memory, index);

	cout << *(int*)GameAddress::SpritesCount - 1 << endl;

	return *(int*)GameAddress::SpritesCount - 1;
}

int Utils::GetRealSize(char* data)
{
	int size = -1;
	do
		++size;
	while (data[size]);
	return size;
}

const char* Utils::GetFileName(const char* filePath)
{
	string str(filePath);
	str = str.substr(str.find_last_of("/\\") + 1);

	char* ret = new char[64];
	memset(ret, 0, 64);
	int i = 0;
	while (str[i])
	{
		ret[i] = str[i];
		i++;
	}
	return ret;
}

const char* Utils::GetFileNameWithoutExtension(const char* filePath)
{
	string str(filePath);
	str = str.substr(str.find_last_of("/\\") + 1);
	size_t size(str.find_last_of("."));
	str = (size > 0 && size != string::npos ? str.substr(0, size) : str);

	char* ret = new char[64];
	memset(ret, 0, 64);
	int i = 0;
	while (str[i])
	{
		ret[i] = str[i];
		i++;
	}
	return ret;
}