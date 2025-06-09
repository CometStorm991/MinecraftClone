#pragma once

#include <random>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AttributeLayout.hpp"
#include "BlockType.hpp"
#include "Chunk.hpp"
#include "Cube.hpp"
#include "PerformanceTimer.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

class Application
{
private:
	Renderer renderer;
	

	uint32_t programId;
	std::vector<uint32_t> vertexBufferIds;
	std::vector<uint32_t> vertexArrayIds;
	std::vector<uint32_t> textureIds;

	Chunk chunk;
	uint32_t chunkLength = 16;
	std::map<std::tuple<int32_t, int32_t>, std::vector<BlockType>> renderedBlockData;
	std::map<std::tuple<int32_t, int32_t>, std::vector<float>> renderedVertexData;
	std::vector<uint32_t> vertexCounts;

	void generateChunkData();
	
	void prepareForRun();
public:
	Application();

	void init();
	void prepare();
	void run();
	void terminate();

	bool shouldEnd();
};