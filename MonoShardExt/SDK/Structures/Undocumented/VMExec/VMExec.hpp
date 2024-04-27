#pragma once
#include "../../Documented/CCode/CCode.hpp"
#include "../../Documented/YYRValue/YYRValue.hpp"
#include "../../Documented/VMBuffer/VMBuffer.hpp"

struct VMExec
{
	VMExec* pPrev; //0
	VMExec* pNext; //8
	char* pStack; //16
	int localCount; //24
	YYObjectBase* pLocals; //32
	YYObjectBase* pSelf; //40
	YYObjectBase* pOther; //48
	CCode* pCCode; //56
	YYRValue* pArgs; //64
	int argumentCount; //72
	const char* pCode; //80
	char* pBP; //88
	VMBuffer* pBuffer; //96
	int line; //104
	const char* pName; //16
	VMBuffer* pDebugInfo; //16
	const char* pScript; //16
	int stackSize; //16
	int offs;
	int boffs;
	int retCount;
	int bufferSize;
	int prevoffs;
	void** buff;
	int* jt;
};
