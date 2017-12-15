#include "stdafx.h"
#include "VisualManager.h"
#include "HandleManager.h"
#include "RenderItem.h"



VisualManager::VisualManager() :
	handleManager { std::make_unique<HandleManager>() }
{}


VisualManager::~VisualManager() = default;


std::pair<Handle, RenderItem*> VisualManager::Alloc()
{
	const Handle handle = handleManager->AcquireHandle();
	visuals.push_back( {} );
	return { handle, &visuals.back() };
}


void VisualManager::Free(Handle handle)
{
	uint index = handleManager->ReleaseElementByHandle(handle);
	visuals[index] = visuals.back();
	visuals.pop_back();
}


std::pair<RenderItem*, const uint*> VisualManager::GetVisualsRW()
{
	return { visuals.data(), handleManager->GetHandleToIndexTable() };
}


std::pair<const RenderItem*, size_t> VisualManager::GetVisualsRO() const
{
	return { visuals.data(), visuals.size() };
}


void VisualManager::Clear()
{
	visuals.clear();
	handleManager->ReleaseAll();
}
