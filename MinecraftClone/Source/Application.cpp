#include "Application.hpp"

Application::Application()
    :
    attribs(std::vector<AttributeLayout>()),
    diffuseTexturer(Texturer(0, 0))
{
}

void Application::init()
{
    renderer = Renderer();
    renderer.init();
}

int Application::intPow(int base, int exp)
{
    return static_cast<int>(std::pow(base, exp));
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
    diffuseTexturer = Texturer(diffuseTextureAtlas.getWidth(), diffuseTextureAtlas.getHeight());

    std::cout << "Image width: " << diffuseTextureAtlas.getWidth() << std::endl;
    std::cout << "Image height: " << diffuseTextureAtlas.getHeight() << std::endl;

    AttributeLayout posAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout normAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout texAttrib = AttributeLayout(2, GL_FLOAT);

    attribs.push_back(posAttrib);
    attribs.push_back(normAttrib);
    attribs.push_back(texAttrib);

    chunkManager.emplace(
        chunkLength,
        renderDistance,
        vertexFloatCount,
        diffuseTexturer,

        renderer,
        attribs
    );

    renderer.generateProgram(programId, "Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");

    renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());

    renderer.setUniform1i(programId, "materialAtlas.diffuse", 0);
    renderer.setUniform1i(programId, "materialAtlas.specular", 1);
    renderer.setUniform1f(programId, "materialAtlas.shininess", 32.0f);

    renderer.setUniform3f(programId, "directionalLight.direction", glm::vec3(-1.0f, -0.5f, -1.0f));
    renderer.setUniform3f(programId, "directionalLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    renderer.setUniform3f(programId, "directionalLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    renderer.setUniform3f(programId, "directionalLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    prepareForRun();
}

void Application::run()
{
    // PerformanceTimer runTimer = PerformanceTimer("Run");

    renderer.prepareForRender();
    renderer.calculateCameraTransform();

    glm::mat4 model = glm::mat4(1.0f);

    if (!chunkManager.has_value())
    {
        throw std::runtime_error("chunkManager was not initialized");
    }

    glm::vec3 currentPos = renderer.getCameraPos();
    if (currentPos != lastCameraPos)
    {
        chunkManager.value().updateDesiredMeshes(currentPos);
        lastCameraPos = currentPos;
    }
    chunkManager.value().manageChunks();

    const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& vertexArrayIds = chunkManager.value().getVertexArrayIds();
    const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& vertexCounts = chunkManager.value().getVertexCounts();

    // std::cout << "Vertex array id count: " << vertexArrayIds.size() << std::endl;
    for (auto& [chunkCoords, vertexArrayId] : vertexArrayIds)
    {
        renderer.prepareForDraw(programId, textureIds, vertexArrayId);
        renderer.setUniformMatrix4fv(programId, "normalMatrix", glm::transpose(glm::inverse(model)));
        renderer.applyMvp(programId, "model", "view", "projection");
        renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());
        renderer.draw(vertexCounts.at(chunkCoords));
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
    chunkManager.value().stopGeneratingChunks();
}

bool Application::shouldEnd()
{
    return renderer.getWindowShouldClose();
}