#include "BlockTextureCoords.hpp"

void BlockTextureCoords::get(TexturerBlockType type, CubeFace face, uint32_t& startX, uint32_t& startY, uint32_t& endX, uint32_t& endY)
{
	switch (type)
	{
	case TexturerBlockType::Air:
		return;
	case TexturerBlockType::Stone:
		startX = 0;
		startY = 0;
		endX = 15;
		endY = 15;
		return;
	case TexturerBlockType::Dirt:
		startX = 16;
		startY = 0;
		endX = 31;
		endY = 15;
		return;
	case TexturerBlockType::Grass:
		switch (face)
		{
		case CubeFace::Top:
			startX = 48;
			startY = 0;
			endX = 63;
			endY = 15;
			return;
		case CubeFace::Bottom:
			startX = 16;
			startY = 0;
			endX = 31;
			endY = 15;
			return;
		default:
			startX = 32;
			startY = 0;
			endX = 47;
			endY = 15;
			return;
		}
		return;
	case TexturerBlockType::Sand:
		startX = 64;
		startY = 0;
		endX = 79;
		endY = 15;
		return;
	}
}