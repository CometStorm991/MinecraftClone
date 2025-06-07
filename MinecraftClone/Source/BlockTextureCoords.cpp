#include "BlockTextureCoords.hpp"

void BlockTextureCoords::get(BlockType type, CubeFace face, uint32_t& startX, uint32_t& startY, uint32_t& endX, uint32_t& endY)
{
	switch (type)
	{
	case BlockType::Air:
		return;
	case BlockType::Stone:
		startX = 0;
		startY = 0;
		endX = 16;
		endY = 16;
		return;
	case BlockType::Dirt:
		startX = 16;
		startY = 0;
		endX = 32;
		endY = 16;
		return;
	case BlockType::Grass:
		switch (face)
		{
		case CubeFace::Top:
			startX = 48;
			startY = 0;
			endX = 64;
			endY = 16;
			return;
		case CubeFace::Bottom:
			startX = 16;
			startY = 0;
			endX = 32;
			endY = 16;
			return;
		default:
			startX = 32;
			startY = 0;
			endX = 48;
			endY = 16;
			return;
		}
		return;
	case BlockType::Sand:
		startX = 64;
		startY = 0;
		endX = 80;
		endY = 16;
		return;
	}
}