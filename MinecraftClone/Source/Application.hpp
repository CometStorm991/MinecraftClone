#pragma once

#include <random>

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
	uint32_t vaoId;
	std::vector<uint32_t> textureIds;

	Chunk chunk;
	uint32_t chunkLength = 16;
	std::vector<BlockType> chunkData;
	uint32_t vertexCount;

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