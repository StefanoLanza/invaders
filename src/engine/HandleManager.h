#pragma once

#include "Base.h"
#include "Handle.h"
#include <cassert>
#include <cstddef>
#include <vector>


class HandleManager
{
public:

	HandleManager();
	~HandleManager();
	
	Handle AcquireHandle();
	// \return index of deleted element
	// Use as element[index] = elements.back(); elements.pop_back();
	uint ReleaseElementByHandle(Handle handle);
	size_t GetSize() const;
	void ReleaseAll();
	uint GetIndex(Handle handle) const;
	size_t GetCount() const;
	Handle GetHandle(size_t index) const;
	bool IsValid(Handle handle) const;
	const uint32* GetHandleToIndexTable() const;

private:

	std::vector<uint32>  sparseToDense;
	std::vector<uint32>  denseToSparse;
	std::vector<uint8>   generations;
	int                  freeHandle;
};


inline uint HandleManager::GetIndex(Handle handle) const
{
	assert(IsValid(handle));
	return sparseToDense[handle.index];
}


inline Handle HandleManager::GetHandle(size_t elementIndex) const
{
	Handle handle;
	handle.index = denseToSparse[elementIndex];
	handle.generation = generations[handle.index];
	return handle;
}
