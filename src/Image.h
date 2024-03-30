#pragma once
#include <vector>
#include <ImageInfo.h>

struct Image
{
 std::vector<std::vector<uint16_t>> img;
 ImageInfo info;
};
