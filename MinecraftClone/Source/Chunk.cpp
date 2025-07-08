#include "Chunk.hpp"

Chunk::Chunk(const std::map<std::tuple<int32_t, int32_t, int32_t>, std::vector<BlockType>>& data, unsigned int chunkLength, const std::tuple<uint32_t, uint32_t, uint32_t>& chunkCounts, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight)
	: data(data), chunkLength(chunkLength), chunkCounts(chunkCounts), vertexFloatCount(vertexFloatCount), imageWidth(imageWidth), imageHeight(imageHeight),
	texturer(Texturer(imageWidth, imageHeight))
{
	
}

void Chunk::placeBlock(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, BlockType blockType)
{
	std::tuple<int32_t, int32_t, int32_t> chunkCoords;
	std::tuple<uint32_t, uint32_t, uint32_t> blockCoords;
	worldToChunkCoords(worldCoords, chunkCoords, blockCoords);

	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	uint32_t blockX = std::get<0>(blockCoords);
	uint32_t blockY = std::get<1>(blockCoords);
	uint32_t blockZ = std::get<2>(blockCoords);

	std::vector<BlockType>& chunkData = data.at(chunkCoords);
	chunkData.at(blockZ * intPow(chunkLength, 2) + blockY * intPow(chunkLength, 1) + blockX * intPow(chunkLength, 0)) = blockType;
}

void Chunk::generateBlocks()
{
	std::mt19937 gen(1);
	std::uniform_int_distribution<int> blockExists(0, 1);
	std::uniform_int_distribution<int> blockType(1, 4);

	uint32_t chunkCountX = std::get<0>(chunkCounts);
	uint32_t chunkCountY = std::get<1>(chunkCounts);
	uint32_t chunkCountZ = std::get<2>(chunkCounts);

	for (uint32_t i = 0; i < chunkCountX * chunkCountY * chunkCountZ; i++)
	{
		int32_t chunkX = (i % chunkCountX) / 1;
		int32_t chunkY = (i % (chunkCountX * chunkCountY)) / chunkCountX;
		int32_t chunkZ = (i % (chunkCountX * chunkCountY * chunkCountZ)) / (chunkCountX * chunkCountY);
		std::tuple<int32_t, int32_t, int32_t> chunkCoords = std::tuple<int32_t, int32_t, int32_t>(chunkX, chunkY, chunkZ);

		data.insert({ chunkCoords, {} });
		std::vector<BlockType>& chunkData = data.at(chunkCoords);
		chunkData.resize(intPow(chunkLength, 3));
	}

	PerformanceTimer timer = PerformanceTimer("hey");
	ThreadPool pool = ThreadPool(6);
	std::mutex mutex;

	for (uint32_t i = 0; i < chunkCountX * chunkCountY * chunkCountZ; i++)
	{
		pool.enqueue([this, i, chunkCountX, chunkCountY, chunkCountZ, &mutex]() {
			int32_t chunkX = (i % chunkCountX) / 1;
			int32_t chunkY = (i % (chunkCountX * chunkCountY)) / chunkCountX;
			int32_t chunkZ = (i % (chunkCountX * chunkCountY * chunkCountZ)) / (chunkCountX * chunkCountY);
			std::tuple<int32_t, int32_t, int32_t> chunkCoords = std::tuple<int32_t, int32_t, int32_t>(chunkX, chunkY, chunkZ);

			for (uint32_t j = 0; j < intPow(chunkLength, 2); j++)
			{
				uint32_t blockX = (j % intPow(chunkLength, 1)) / intPow(chunkLength, 0);
				uint32_t blockZ = (j % intPow(chunkLength, 2)) / intPow(chunkLength, 1);

				int32_t worldX = chunkX * chunkLength + blockX;
				int32_t worldZ = chunkZ * chunkLength + blockZ;

				uint32_t height = 8.0f * (std::sin(worldX / 8.0f) + std::sin(worldZ / 8.0f) + 2.0f);
				height = std::min(height, chunkLength * chunkCountY - 1);
				height = std::max(height, 1u);

				for (uint32_t k = 0; k < height; k++)
				{
					uint32_t worldY = k;
					std::tuple<uint32_t, uint32_t, uint32_t> worldCoords = std::make_tuple(worldX, worldY, worldZ);

					{
						std::lock_guard<std::mutex> lock(mutex);
						if (worldY < 8)
						{
							placeBlock(worldCoords, BlockType::Sand);
						}
						else
						{
							placeBlock(worldCoords, BlockType::Grass);
						}
					}
				}
			}
		});
	}

	pool.stopAndWait();
	timer.stop();
}

void Chunk::getFreeVertices(std::vector<float>& meshPart, const std::tuple<int32_t, int32_t, int32_t>& worldCoords, BlockType blockType)
{
	meshPart.clear();

	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	// Get left
	if (getFreeFace(worldCoords, CubeFace::Left))
	{
		std::vector<float> left;
		texturer.generateFace(left, CubeFace::Left, blockType);
		meshPart.insert(meshPart.end(), left.begin(), left.end());
	}

	// Get right
	if (getFreeFace(worldCoords, CubeFace::Right))
	{
		std::vector<float> right;
		texturer.generateFace(right, CubeFace::Right, blockType);
		meshPart.insert(meshPart.end(), right.begin(), right.end());
	}

	// Get bottom
	if (getFreeFace(worldCoords, CubeFace::Bottom))
	{
		std::vector<float> bottom;
		texturer.generateFace(bottom, CubeFace::Bottom, blockType);
		meshPart.insert(meshPart.end(), bottom.begin(), bottom.end());
	}

	// Get top
	if (getFreeFace(worldCoords, CubeFace::Top))
	{
		std::vector<float> top;
		texturer.generateFace(top, CubeFace::Top, blockType);
		meshPart.insert(meshPart.end(), top.begin(), top.end());
	}

	// Get back
	if (getFreeFace(worldCoords, CubeFace::Back))
	{
		std::vector<float> back;
		texturer.generateFace(back, CubeFace::Back, blockType);
		meshPart.insert(meshPart.end(), back.begin(), back.end());
	}

	// Get front
	if (getFreeFace(worldCoords, CubeFace::Front))
	{
		std::vector<float> front;
		texturer.generateFace(front, CubeFace::Front, blockType);
		meshPart.insert(meshPart.end(), front.begin(), front.end());
	}

	// Offset cube model by location within chunk
	for (unsigned int i = 0; i < meshPart.size() / vertexFloatCount; i++)
	{
		meshPart.at(i * vertexFloatCount + 0) += worldX;
		meshPart.at(i * vertexFloatCount + 1) += worldY;
		meshPart.at(i * vertexFloatCount + 2) += worldZ;
	}
}

bool Chunk::getFreeFace(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, CubeFace face)
{
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	switch (face)
	{
	case CubeFace::Left:
		return !getBlockExists(std::make_tuple(worldX - 1, worldY, worldZ));
	case CubeFace::Right:
		return !getBlockExists(std::make_tuple(worldX + 1, worldY, worldZ));
	case CubeFace::Bottom:
		return !getBlockExists(std::make_tuple(worldX, worldY - 1, worldZ));
	case CubeFace::Top:
		return !getBlockExists(std::make_tuple(worldX, worldY + 1, worldZ));
	case CubeFace::Back:
		return !getBlockExists(std::make_tuple(worldX, worldY, worldZ - 1));
	case CubeFace::Front:
		return !getBlockExists(std::make_tuple(worldX, worldY, worldZ + 1));
	}

	return false;
}

void Chunk::chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	uint32_t blockX = std::get<0>(blockCoords);
	uint32_t blockY = std::get<1>(blockCoords);
	uint32_t blockZ = std::get<2>(blockCoords);

	std::get<0>(worldCoords) = chunkX * chunkLength + blockX;
	std::get<1>(worldCoords) = chunkY * chunkLength + blockY;
	std::get<2>(worldCoords) = chunkZ * chunkLength + blockZ;
}

void Chunk::worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords)
{
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	std::get<0>(chunkCoords) = worldX / chunkLength;
	std::get<1>(chunkCoords) = worldY / chunkLength;
	std::get<2>(chunkCoords) = worldZ / chunkLength;

	std::get<0>(blockCoords) = (worldX % chunkLength + chunkLength) % chunkLength;
	std::get<1>(blockCoords) = (worldY % chunkLength + chunkLength) % chunkLength;
	std::get<2>(blockCoords) = (worldZ % chunkLength + chunkLength) % chunkLength;
}

bool Chunk::getBlockExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	std::tuple<int32_t, int32_t, int32_t> chunkCoords;
	std::tuple<uint32_t, uint32_t, uint32_t> blockCoords;
	worldToChunkCoords(worldCoords, chunkCoords, blockCoords);

	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	uint32_t blockX = std::get<0>(blockCoords);
	uint32_t blockY = std::get<1>(blockCoords);
	uint32_t blockZ = std::get<2>(blockCoords);

	if (data.count(chunkCoords) == 0)
	{
		return false;
	}

	return data.at(chunkCoords).at(blockZ * intPow(chunkLength, 2) + blockY * intPow(chunkLength, 1) + blockX * intPow(chunkLength, 0)) != BlockType::Air;
}

int Chunk::intPow(int base, int exp)
{
	return static_cast<int>(std::powf(base, exp));
}

void Chunk::generateMesh(std::vector<float>& mesh, std::tuple<int32_t, int32_t, int32_t> chunkCoords)
{
	mesh.clear();
	for (unsigned int i = 0; i < data.at(chunkCoords).size(); i++)
	{
		BlockType blockType = data.at(chunkCoords).at(i);

		if (blockType == BlockType::Air)
		{
			continue;
		}

		uint32_t worldX = (i % intPow(chunkLength, 1)) / intPow(chunkLength, 0);
		uint32_t worldY = (i % intPow(chunkLength, 2)) / intPow(chunkLength, 1);
		uint32_t worldZ = (i % intPow(chunkLength, 3)) / intPow(chunkLength, 2);
		std::tuple<uint32_t, uint32_t, uint32_t> blockCoords = std::make_tuple(worldX, worldY, worldZ);

		std::tuple<int32_t, int32_t, int32_t> worldCoords;
		chunkToWorldCoords(chunkCoords, blockCoords, worldCoords);

		std::vector<float> meshPart;
		getFreeVertices(meshPart, worldCoords, blockType);
		
		mesh.insert(mesh.end(), meshPart.begin(), meshPart.end());
	}
}