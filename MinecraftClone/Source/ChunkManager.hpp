#pragma once

#include <cstdint>
#include <map>
#include <tuple>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BlockType.hpp"
#include "Chunk.hpp"
#include "Renderer.hpp"
#include "ThreadPool.hpp"

class ChunkManager {
public:
	ChunkManager(
		uint32_t chunkLength,
		uint32_t renderDistance,
		uint32_t vertexFloatCount,
		Texturer diffuseTexturer,

		Renderer& renderer,
		const std::vector<AttributeLayout>& attribs
	);

	void updateDesiredMeshes(const glm::vec3& currentPos);
	void manageChunks();
	void stopGeneratingChunks();

	const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& getVertexBufferIds();
	const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& getVertexArrayIds();
	const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& getVertexCounts();
private:
	uint32_t chunkLength;
	uint32_t chunkGenLength;

	uint32_t renderDistance;
	uint32_t renderChunkLength;

	uint32_t vertexFloatCount;
	Texturer diffuseTexturer;

	std::map<std::tuple<int32_t, int32_t, int32_t>, std::vector<float>> meshes;
	std::mutex meshesMutex;

	std::map<std::tuple<int32_t, int32_t, int32_t>, bool> completedMeshes;
	std::mutex completedMeshesMutex;

	std::vector<std::tuple<int32_t, int32_t, int32_t>> desiredMeshes;

	ThreadPool pool;

	std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> vertexBufferIds;
	std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> vertexArrayIds;
	std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> vertexCounts;

	Renderer& renderer;
	std::vector<AttributeLayout> attribs;

	void generateChunk(const std::tuple<int32_t, int32_t, int32_t>& desiredChunk);
	void uploadChunkToGPU(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords);
	// void deleteChunkFromGPU();

	int intPow(int base, int exp);
};