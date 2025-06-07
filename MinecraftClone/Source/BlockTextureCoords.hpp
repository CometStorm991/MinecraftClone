#pragma once

#include <cstdint>

#include "BlockType.hpp"
#include "CubeFace.hpp"

class BlockTextureCoords
{
public:
	static void get(BlockType type, CubeFace face, uint32_t& startX, uint32_t& startY, uint32_t& endX, uint32_t& endY);
};