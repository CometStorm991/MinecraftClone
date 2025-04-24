#include "Cube.hpp"

void Cube::generatePN(std::vector<float>& vertices)
{
	vertices = {
		// Front face
		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

		// Back face
		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,

		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,

		// Left face
		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,

		// Right face
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,

		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,

		// Top face
		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

		// Bottom face
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f
	};
}

void Cube::generatePNLeft(std::vector<float>& vertices)
{
	vertices = {
		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
	};
}

void Cube::generatePNRight(std::vector<float>& vertices)
{
	vertices = {
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,

		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
	};
}

void Cube::generatePNBottom(std::vector<float>& vertices)
{
	vertices = {
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f
	};
}

void Cube::generatePNTop(std::vector<float>& vertices)
{
	vertices = {
		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
	};
}

void Cube::generatePNBack(std::vector<float>& vertices)
{
	vertices = {
		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,

		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
	};
}

void Cube::generatePNFront(std::vector<float>& vertices)
{
	vertices = {
		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
	};
}