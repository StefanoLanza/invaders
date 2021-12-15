#include "HandleManager.h"
#include <cassert>


namespace
{
	void IncGeneration(uint8& g)
	{
		++g;
		// Reserve g == 0
		if (g == 0) g = 1;
	}
}


HandleManager::HandleManager() :
	freeHandle { -1 }
{
}


HandleManager::~HandleManager() = default;


bool HandleManager::IsValid(Handle handle) const
{
	return (handle.index < generations.size()) && (generations[handle.index] == handle.generation);
}


Handle HandleManager::AcquireHandle() 
{
	const uint32 denseIndex = static_cast<uint32>(denseToSparse.size());

	Handle handle;
	if (freeHandle == -1)
	{
		// Create new handle
		handle.Set(static_cast<uint32_t>(sparseToDense.size()), 1);
		sparseToDense.push_back(denseIndex);
		generations.push_back(handle.generation);
	}
	else
	{
		// Recycle a handle that was released
		const uint nextFree = sparseToDense[freeHandle];
		const uint sparseIndex = freeHandle;
		handle.Set(sparseIndex, generations[sparseIndex]);
		sparseToDense[sparseIndex] = denseIndex;
		freeHandle = nextFree;
	}
	denseToSparse.push_back(handle.index);
	
	return handle;
}


uint HandleManager::ReleaseElementByHandle(Handle handle)
{
	assert(IsValid(handle));
	assert(! denseToSparse.empty());
	
	const uint32 denseIndex = sparseToDense[handle.index];

	// Invalidate handle
	IncGeneration(generations[handle.index]);
	
	// The last element is copied over the deleted one. Update its handle accordingly
	const size_t last = denseToSparse.size() - 1;
	if (denseIndex < last)
	{
		const uint32 handleOfLastElement = denseToSparse[last];
		sparseToDense[handleOfLastElement] = denseIndex;
		// Move the released handled to the back of the indexToHandle array, so that it can be recycled
		//std::swap(denseToSparse[elementIndex], denseToSparse[last]);
		denseToSparse[denseIndex] =  denseToSparse[last];
	}
	denseToSparse.pop_back();

	// Add released handle to linked list of free handles
	sparseToDense[handle.index] = freeHandle;
	freeHandle = handle.index;
	
	return denseIndex;
}


size_t HandleManager::GetSize() const
{
	return denseToSparse.size();
}


void HandleManager::ReleaseAll()
{
	sparseToDense.clear();
	denseToSparse.clear();
	generations.clear();
	freeHandle = -1;
}


const uint32* HandleManager::GetHandleToIndexTable() const
{ 
	return sparseToDense.data(); 
}
