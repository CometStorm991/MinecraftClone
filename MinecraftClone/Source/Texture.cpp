#include "Texture.hpp"

Texture::Texture(const std::string& imagePath, GLenum pixelFormat)
	: imagePath(imagePath), pixelFormat(pixelFormat), loaded(false)
{

}

void Texture::load()
{
    int imageWidth, imageHeight, channelCount;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &channelCount, 0);
    if (!data)
    {
        std::cout << "[Error] Failed to load texture" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }

    /*std::cout << imagePath << ":" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << static_cast<int>(data[(255 * 256) * 4 + i]) << std::endl;
    }*/

    width = imageWidth;
    height = imageHeight;

    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    loaded = true;
}

void Texture::use(GLenum textureUnit)
{
    if (!loaded)
    {
        std::cout << "[Error] Texture was not loaded" << std::endl;
    }

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unuse(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t Texture::getId()
{
    return id;
}

bool Texture::getLoaded()
{
    return loaded;
}

uint32_t Texture::getWidth()
{
    return width;
}

uint32_t Texture::getHeight()
{
    return height;
}