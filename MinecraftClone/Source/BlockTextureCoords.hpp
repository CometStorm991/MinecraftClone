#pragma once

#include <cstdint>

#include "CubeFace.hpp"
#include "TexturerBlockType.hpp"

class BlockTextureCoords
{
public:
	static void get(TexturerBlockType type, CubeFace face, uint32_t& startX, uint32_t& startY, uint32_t& endX, uint32_t& endY);
};