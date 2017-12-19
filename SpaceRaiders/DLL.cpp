#include "stdafx.h"
#include "DLL.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cassert>


namespace
{

FileTime GetLastWriteTime(const char* path)
{
	FileTime time = {};
	WIN32_FILE_ATTRIBUTE_DATA data;
	if ( GetFileAttributesExA(path, GetFileExInfoStandard, &data ) )
	{
		time = { data.ftLastWriteTime.dwLowDateTime, data.ftLastWriteTime.dwHighDateTime };
	}
	return time;
}

}


DLLError LoadDLL(DLL& dll, const char* fileName)
{
	dll.module = nullptr;
	dll.fileName = fileName;
	dll.writeTime = {};
#ifdef _WIN32
	HMODULE module = LoadLibraryA(fileName);
#endif
	if (! module)
	{
		//DWORD err = GetLastError();
		return DLLError::loadLibraryFailed;
	}
	dll.module = module;
	dll.writeTime = GetLastWriteTime(fileName);
	++dll.version;
	return DLLError::ok;
}


void FreeDLL(DLL& dll)
{
	if (dll.module)
	{
		FreeLibrary((HMODULE)dll.module);
		dll.module = nullptr;
		++dll.version;
	}
}


DLLError ReloadDLL(DLL& dll)
{
	DLLError err = DLLError::ok;
	const FileTime newTime = GetLastWriteTime(dll.fileName);
	if (newTime.lowDateTime != dll.writeTime.lowDateTime || newTime.highDateTime != dll.writeTime.highDateTime)
	{
		FreeDLL(dll);
		// FIXME Callback?
		err = LoadDLL(dll, dll.fileName);
	}
	return err;
}


ModuleInterfaceProc GetDLLProcedure(const DLL& dll, const char* procName)
{
	assert(dll.module);
	ModuleInterfaceProc pPerModuleInterfaceProcAdd = nullptr;
#ifdef _WIN32
	pPerModuleInterfaceProcAdd = (ModuleInterfaceProc) GetProcAddress((HMODULE)dll.module, procName);
#endif
	return pPerModuleInterfaceProcAdd;
}


bool IsValid(const DLL& dll)
{
	return dll.module != nullptr;
}
