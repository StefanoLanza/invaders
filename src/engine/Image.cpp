#include "Image.h"
#include <cassert>
#include <vector>

namespace {
	std::vector<Image> imageMap;
}

void SetImage(ImageId imageId, const Image& image) {
	assert(imageId != nullImageId);
	const int idx = static_cast<int>(imageId) - 1;
	imageMap.resize(idx + 1);
	imageMap[idx] = image;
}

void SetImages(ImageId firstImageId, int imageCount, const Image* images) {
	assert(firstImageId != nullImageId);
	assert(imageCount > 0);
	const int idx = static_cast<int>(firstImageId) - 1;
	imageMap.resize(idx + imageCount);
	for (int i = 0; i < imageCount; ++i) {
		imageMap[idx + i] = images[i];
	}
}

const Image& GetImage(ImageId imageId) {
	const int idx = static_cast<int>(imageId) - 1;
	return imageMap[idx];
}

Vector2D GetImageSize(ImageId imageId) {
	const Image& image = GetImage(imageId);
	return { (float)image.width, (float)image.height };
}
