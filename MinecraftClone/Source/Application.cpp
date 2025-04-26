#include "Application.hpp"

void Application::init()
{
    renderer = Renderer();
    renderer.init();
}

void Application::generateChunk()
{
    std::mt19937 gen(1);
    std::uniform_int_distribution<int> dist(0, 1);

    for (unsigned int i = 0; i < std::powf(chunkLength, 3.0f); i++)
    {
        chunkData.push_back(dist(gen));
    }
}

void Application::prepare()
{
    generateChunk();
    chunk = Chunk(chunkData, chunkLength, 9);

    std::vector<float> grassVertices;
    chunk.generateMesh(grassVertices);
    vertexCount = grassVertices.size() / 9;
    std::cout << vertexCount << std::endl;

    uint32_t vertexBuffer;
    renderer.generateVertexBuffer(vertexBuffer, grassVertices);

    AttributeLayout posAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout normAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout texAttrib = AttributeLayout(2, GL_FLOAT);
    AttributeLayout typeAttrib = AttributeLayout(1, GL_FLOAT);

    std::vector<AttributeLayout> attribs = std::vector<AttributeLayout>();
    attribs.push_back(posAttrib);
    attribs.push_back(normAttrib);
    attribs.push_back(typeAttrib);

    renderer.generateVertexArray(vaoId, vertexBuffer, attribs);

    uint32_t grassBlockId;
    renderer.generateTexture(grassBlockId, "Resources/GrassBlock.png", GL_RGBA);

    renderer.generateProgram(programId, "Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");

    renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());

    /*renderer.setUniform3f(programId, "grassBlockMaterial.grassDiffuse", glm::vec3((119.0f / 255.0f), (221.0f / 255.0f), (119.0f / 255.0f)));
    renderer.setUniform3f(programId, "grassBlockMaterial.dirtDiffuse", glm::vec3((131.0f / 255.0f), (105.0f / 255.0f), (83.0f / 255.0f)));
    renderer.setUniform1f(programId, "grassBlockMaterial.grassSpecular", 0.5f);
    renderer.setUniform1f(programId, "grassBlockMaterial.dirtSpecular", 0.0f);*/
    renderer.setUniform1i(programId, "grassBlockMaterial.diffuse", 0);
    renderer.setUniform1f(programId, "grassBlockMaterial.grassShininess", 32.0f);
    renderer.setUniform1f(programId, "grassBlockMaterial.dirtShininess", 1.0f);

    renderer.setUniform3f(programId, "directionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
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

    renderer.prepareForDraw(programId, textureIds, vaoId);

    // PerformanceTimer renderLoopTimer = PerformanceTimer("Render Loop");
    /*int x = chunkData.at(i * 3);
        int y = chunkData.at(i * 3 + 1);
        int z = chunkData.at(i * 3 + 2);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        renderer.updateModelMatrix(model);*/
    renderer.setUniformMatrix4fv(programId, "normalMatrix", glm::transpose(glm::inverse(model)));
    renderer.applyMvp(programId, "model", "view", "projection");
    renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());
    renderer.draw(vertexCount);

    // renderLoopTimer.stop();

    renderer.unprepareForDraw(programId, textureIds);

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