#pragma once

#include <mutex>
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
	std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> vertexBufferIds;
	std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> vertexArrayIds;
	std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> vertexCounts;
	std::vector<uint32_t> textureIds;
	uint32_t vertexFloatCount = 8;

	Chunk chunk;
	uint32_t chunkLength = 16;
	std::tuple<uint32_t, uint32_t, uint32_t> chunkCounts = std::make_tuple(8, 4, 8);
	uint32_t totalChunkCount = std::get<0>(chunkCounts) * std::get<1>(chunkCounts) * std::get<2>(chunkCounts);
	std::map<std::tuple<int32_t, int32_t, int32_t>, std::vector<float>> meshes;
	std::mutex meshesMutex;

	ThreadPool pool;

	int intPow(int base, int exp);
	
	void prepareForRun();
public:
	Application();

	void init();
	void prepare();
	void run();
	void terminate();

	bool shouldEnd();
};