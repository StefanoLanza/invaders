#include "../DLL.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cassert>
#include <cstdio>


namespace
{

FileTime GetLastWriteTime(const char* path)
{
	FileTime time = {};
	WIN32_FILE_ATTRIBUTE_DATA data {};
	if ( GetFileAttributesExA(path, GetFileExInfoStandard, &data ) )
	{
		time = { data.ftLastWriteTime.dwLowDateTime, data.ftLastWriteTime.dwHighDateTime };
	}
	return time;
}

}


const char* GetErrorMessage(DLLError error)
{
	static const char* str[] =
	{
		"unchanged",
		"getProcAddressFailed",
		"loadLibraryFailed",
		"ok",
	};
	return str[(uint)error];
}


DLLError LoadDLL(DLL& dll, const char* fileName)
{
	assert(fileName);
	FreeDLL(dll);
	HMODULE module = LoadLibraryA(fileName);
	if (! module)
	{
		return DLLError::loadLibraryFailed;
	}
	dll.module = module;
	dll.fileName = fileName;
	dll.writeTime = GetLastWriteTime(fileName);
	dll.version++;
	return DLLError::ok;
}


void FreeDLL(DLL& dll)
{
	if (dll.module)
	{
		FreeLibrary((HMODULE)dll.module);
		dll.module = nullptr;
		dll.writeTime = {};
	}
}


DLLError ReloadDLL(DLL& dll)
{
	const FileTime newTime = GetLastWriteTime(dll.fileName);
	if (newTime.lowDateTime != dll.writeTime.lowDateTime || newTime.highDateTime != dll.writeTime.highDateTime)
	{
		FreeDLL(dll);
		return LoadDLL(dll, dll.fileName);
	}
	return DLLError::unchanged;
}


DLLProc GetDLLProcedure(const DLL& dll, const char* procName)
{
	assert(dll.module);
	assert(procName);
	DLLProc pPerModuleInterfaceProcAdd = (DLLProc) GetProcAddress((HMODULE)dll.module, procName);
	return pPerModuleInterfaceProcAdd;
}


bool IsValid(const DLL& dll)
{
	return dll.module != nullptr;
}
