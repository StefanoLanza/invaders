#pragma once

#include "Base.h"
#include <cassert>


struct FileTime
{
	dword lowDateTime;
	dword highDateTime;
};


struct DLL
{
	void*       module = nullptr;
	const char* fileName = nullptr;
	FileTime    writeTime = {};
	uint        version = 0;
};


enum class DLLError
{
	copyFailed = 0,
	unchanged = 1,
	getProcAddressFailed = 2,
	loadLibraryFailed = 3,
	ok = 4
};

#if WINDOWS
using DLLProc = void (__cdecl *)(void);
#else
using DLLProc = void ( *)(void); // FIXME
#endif

const char* GetErrorMessage(DLLError error);
DLLError LoadDLL(DLL& dll, const char* fileName);
void FreeDLL(DLL& dll);
DLLError ReloadDLL(DLL& dll);
bool IsValid(const DLL& dll);
DLLProc GetDLLProcedure(const DLL& dll, const char* procName);

template <typename Proc>
DLLError GetDLLProcedure(Proc* outProc, const DLL& dll, const char* procName)
{
	assert(outProc);
	DLLProc proc = GetDLLProcedure(dll, procName);
	*outProc = reinterpret_cast<Proc>(proc);
	return proc == nullptr ? DLLError::getProcAddressFailed : DLLError::ok;
}
