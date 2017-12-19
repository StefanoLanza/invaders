#pragma once

#include "Base.h"


struct FileTime
{
	dword lowDateTime;
	dword highDateTime;
};


struct DLL
{
	void*       module = nullptr;
	const char* fileName = nullptr;
	FileTime    writeTime;
	int         version = -1;
};

using ModuleInterfaceProc = void (__cdecl *)(void);


enum class DLLError : int
{
	getProcAddressFailed = -2,
	loadLibraryFailed = -1,
	ok = 0
};

DLLError LoadDLL(DLL& dll, const char* fileName);
void FreeDLL(DLL& dll);
DLLError ReloadDLL(DLL& dll);
ModuleInterfaceProc GetDLLProcedure(const DLL& dll, const char* procName);
template <typename Proc>
DLLError GetDLLProcedure(Proc* outProc, const DLL& dll, const char* procName)
{
	ModuleInterfaceProc proc = GetDLLProcedure(dll, procName);
	*outProc = reinterpret_cast<Proc>(proc);
	return proc == nullptr ? DLLError::getProcAddressFailed : DLLError::ok;
}

bool IsValid(const DLL& dll);
