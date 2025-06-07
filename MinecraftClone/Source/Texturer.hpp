#pragma once

#include <iostream>
#include <vector>

#include "BlockTextureCoords.hpp"
#include "BlockType.hpp"
#include "Cube.hpp"
#include "CubeFace.hpp"

class Texturer
{
private:
	uint32_t imageWidth;
	uint32_t imageHeight;

	void changeFace(std::vector<float>& faceMesh, uint32_t pixX, uint32_t pixY, BlockType blockType);
	void pixelToTextureCoords(uint32_t pixX, uint32_t pixY, float& texX, float& texY);
	float mapRange(float inputStart, float inputEnd, float outputStart, float outputEnd, float input);
public:
	Texturer(uint32_t imageWidth, uint32_t imageHeight);
	void generateFace(std::vector<float>& faceMesh, CubeFace face, BlockType blockType);
};