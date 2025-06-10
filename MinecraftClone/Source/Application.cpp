#include "Application.hpp"

Application::Application()
    : chunk(Chunk({}, 0, 0, 0, 0, 0))
{
}

void Application::init()
{
    renderer = Renderer();
    renderer.init();
}

int Application::intPow(int base, int exp)
{
    return static_cast<int>(std::powf(base, exp));
}

void Application::prepare()
{
    uint32_t diffuseTextureAtlasId;
    renderer.generateTexture(diffuseTextureAtlasId, "Resources/DiffuseAtlas.png", GL_RGBA);
    uint32_t specularTextureAtlasId;
    renderer.generateTexture(specularTextureAtlasId, "Resources/SpecularAtlas.png", GL_RGBA);
    textureIds.push_back(diffuseTextureAtlasId);
    textureIds.push_back(specularTextureAtlasId);

    Texture diffuseTextureAtlas = Texture("", GL_RGBA);
    renderer.getTexture(diffuseTextureAtlasId, diffuseTextureAtlas);

    std::cout << "Image width: " << diffuseTextureAtlas.getWidth() << std::endl;
    std::cout << "Image height: " << diffuseTextureAtlas.getHeight() << std::endl;

    chunk = Chunk(renderedBlockData, chunkLength, chunkRadius, vertexFloatCount, diffuseTextureAtlas.getWidth(), diffuseTextureAtlas.getHeight());
    chunk.generateBlocks();

    vertexBufferIds.resize(64);
    vertexArrayIds.resize(64);
    vertexCounts.resize(64);

    for (uint32_t i = 0; i < intPow(chunkRadius, 3); i++)
    {
        int32_t chunkX = (i % intPow(chunkRadius, 1)) / intPow(chunkRadius, 0);
        int32_t chunkY = (i % intPow(chunkRadius, 2)) / intPow(chunkRadius, 1);
        int32_t chunkZ = (i % intPow(chunkRadius, 3)) / intPow(chunkRadius, 2);
        std::tuple<int32_t, int32_t, int32_t> chunkCoords = std::tuple<int32_t, int32_t, int32_t>(chunkX, chunkY, chunkZ);

        renderedVertexData.insert({ chunkCoords, {} });
        std::vector<float>& vertexData = renderedVertexData.at(chunkCoords);
        chunk.generateMesh(vertexData, chunkCoords);

        uint32_t vertexCount = vertexData.size() / vertexFloatCount;
        // std::cout << vertexCount << std::endl;

        renderer.generateVertexBuffer(vertexBufferIds.at(i), vertexData);
        vertexCounts.at(i) = vertexCount;
    }

    AttributeLayout posAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout normAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout texAttrib = AttributeLayout(2, GL_FLOAT);

    std::vector<AttributeLayout> attribs = std::vector<AttributeLayout>();
    attribs.push_back(posAttrib);
    attribs.push_back(normAttrib);
    attribs.push_back(texAttrib);

    for (uint32_t i = 0; i < intPow(chunkRadius, 3); i++)
    {
        renderer.generateVertexArray(vertexArrayIds.at(i), vertexBufferIds.at(i), attribs);
    }

    renderer.generateProgram(programId, "Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");

    renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());

    renderer.setUniform1i(programId, "materialAtlas.diffuse", 0);
    renderer.setUniform1i(programId, "materialAtlas.specular", 1);
    renderer.setUniform1f(programId, "materialAtlas.shininess", 32.0f);

    renderer.setUniform3f(programId, "directionalLight.direction", glm::vec3(-0.2f, 0.5f, 1.0f));
    renderer.setUniform3f(programId, "directionalLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    renderer.setUniform3f(programId, "directionalLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    renderer.setUniform3f(programId, "directionalLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    prepareForRun();
}

void Application::run()
{
    //runMultipleLighting();

    //std::cout << "----------------------------------------------" << std::endl;

    // PerformanceTimer runTimer = PerformanceTimer("Run");

    renderer.prepareForRender();
    renderer.calculateCameraTransform();

    glm::mat4 model = glm::mat4(1.0f);

    for (uint32_t i = 0; i < intPow(chunkRadius, 3); i++)
    {
        renderer.prepareForDraw(programId, textureIds, vertexArrayIds.at(i));
        renderer.setUniformMatrix4fv(programId, "normalMatrix", glm::transpose(glm::inverse(model)));
        renderer.applyMvp(programId, "model", "view", "projection");
        renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());
        renderer.draw(vertexCounts.at(i));
        renderer.unprepareForDraw(programId, textureIds);
    }
    

    renderer.calculateFps();
    renderer.updateGLFW();
}

void Application::prepareForRun()
{
    renderer.prepareForRun();
}

void Application::terminate()
{
    renderer.terminateGLFW();
}

bool Application::shouldEnd()
{
    return renderer.getWindowShouldClose();
}