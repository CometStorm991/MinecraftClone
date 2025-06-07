#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <STB/stb_image.h>

class Texture
{
private:
	std::string imagePath;
	GLenum pixelFormat;

	uint32_t id;

	uint32_t width;
	uint32_t height;

	bool loaded = false;
public:
	Texture(const std::string& imagePath, GLenum pixelFormat);

	void load();
	
	void use(GLenum textureUnit);
	void unuse(GLenum textureUnit);

	uint32_t getId();
	bool getLoaded();
	uint32_t getWidth();
	uint32_t getHeight();
};