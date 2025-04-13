#pragma once

#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AttributeLayout.hpp"
#include "Cube.hpp"
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

	uint32_t chunkLength = 8;
	std::vector<float> chunk;

	void generateChunk();
	
	void prepareForRun();
public:
	void init();
	void prepare();
	void run();
	void terminate();

	bool shouldEnd();
};