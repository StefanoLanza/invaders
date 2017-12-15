#pragma once

#include "Base.h"
#include "Handle.h"
#include <vector>
#include <memory>



class HandleManager;
struct RenderItem;

class VisualManager
{
public:

	VisualManager();
	~VisualManager();

	std::pair<Handle, RenderItem*> Alloc();
	void Free(Handle);
	std::pair<RenderItem*, const uint*> GetVisualsRW();
	std::pair<const RenderItem*, size_t> GetVisualsRO() const;
	void Clear();

private:

	std::unique_ptr<HandleManager> handleManager;
	std::vector<RenderItem> visuals;
};
