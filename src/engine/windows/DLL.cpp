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
	WIN32_FILE_ATTRIBUTE_DATA data;
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
		"ok"
	};
	return str[(uint)error];
}


DLLError LoadDLL(DLL& dll, const char* fileName)
{
	dll.module = nullptr;
	dll.fileName = fileName;
	dll.writeTime = GetLastWriteTime(fileName);

	// Copy DLL to a temporary file so that we can recompile it while the process is running
	const char* toLoad = fileName;
	char tmpFileName[MAX_PATH];
	if (dll.version % 2 == 0)
	{
		sprintf_s(tmpFileName, "%s_temp", fileName);
		const BOOL copyRes = CopyFileA(fileName, tmpFileName, false); // false: overwrite
		if (copyRes == FALSE)
		{
			// FIXME Make the copy a policy? only needed for runtime recompilation
			return DLLError::copyFailed;
		}
		toLoad = tmpFileName;
	}

	HMODULE module = LoadLibraryA(toLoad);
	if (! module)
	{
		//DWORD err = GetLastError();
		return DLLError::loadLibraryFailed;
	}
	dll.module = module;
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
	DLLError err = DLLError::unchanged;
	//WIN32_FILE_ATTRIBUTE_DATA unused;
	//if ( !GetFileAttributesExA( "lock.tmp", GetFileExInfoStandard, &unused ) )
	{	
		const FileTime newTime = GetLastWriteTime(dll.fileName);
		if (newTime.lowDateTime != dll.writeTime.lowDateTime || newTime.highDateTime != dll.writeTime.highDateTime)
		{
//			std::cout << "Reloading dll " << dll.fileName << std::endl;
			DLL tmp_dll { dll };
			err = LoadDLL(tmp_dll, dll.fileName);
			if (err == DLLError::ok)
			{
				// Replace DLL on success
				FreeDLL(dll);
				dll = tmp_dll;
				// FIXME Callback?
			}
			else
			{
	//			std::cout << GetErrorMessage(err) << std::endl;
			}
		}
	}
	return err;
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
