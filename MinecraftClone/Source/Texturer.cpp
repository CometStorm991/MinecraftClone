#include "Texturer.hpp"

Texturer::Texturer(uint32_t imageWidth, uint32_t imageHeight)
	: imageWidth(imageWidth), imageHeight(imageHeight)
{

}

void Texturer::changeFace(std::vector<float>& faceMesh, uint32_t pixX, uint32_t pixY, TexturerBlockType blockType)
{
	

	
}

void Texturer::pixelToTextureCoords(uint32_t pixX, uint32_t pixY, float& texX, float& texY)
{
	float minTexX = 1 / static_cast<float>(2 * imageWidth);
	float maxTexX = (2 * (imageWidth - 1) + 1) / static_cast<float>(2 * imageWidth);

	float minTexY = (2 * (imageHeight - 1) + 1) / static_cast<float>(2 * imageHeight);
	float maxTexY = 1 / static_cast<float>(2 * imageHeight);

	texX = mapRange(0, imageWidth - 1, minTexX, maxTexX, pixX);
	texY = mapRange(0, imageHeight - 1, minTexY, maxTexY, pixY);
}

float Texturer::mapRange(float inputStart, float inputEnd, float outputStart, float outputEnd, float input)
{
	return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}

void Texturer::generateFace(std::vector<float>& faceMesh, CubeFace face, TexturerBlockType blockType)
{
	Cube::generatePNTTyFace(faceMesh, face);

	if (blockType == TexturerBlockType::Air)
	{
		return;
	}

	uint32_t startPixX;
	uint32_t startPixY;
	uint32_t endPixX;
	uint32_t endPixY;
	BlockTextureCoords::get(blockType, face, startPixX, startPixY, endPixX, endPixY);
	float startX;
	float startY;
	float endX;
	float endY;
	pixelToTextureCoords(startPixX, startPixY, startX, startY);
	pixelToTextureCoords(endPixX, endPixY, endX, endY);

	// std::cout << "StartX: " << startX << " StartY: " << startY << " EndX: " << endX << " EndY: " << endY << std::endl;

	faceMesh[0 * 9 + 6] = startX;
	faceMesh[0 * 9 + 7] = endY;
	faceMesh[1 * 9 + 6] = endX;
	faceMesh[1 * 9 + 7] = endY;
	faceMesh[2 * 9 + 6] = endX;
	faceMesh[2 * 9 + 7] = startY;

	faceMesh[3 * 9 + 6] = startX;
	faceMesh[3 * 9 + 7] = endY;
	faceMesh[4 * 9 + 6] = endX;
	faceMesh[4 * 9 + 7] = startY;
	faceMesh[5 * 9 + 6] = startX;
	faceMesh[5 * 9 + 7] = startY;

	for (int i = 0; i < 6; i++)
	{
		faceMesh[i * 9 + 8] = static_cast<float>(blockType);
	}
}