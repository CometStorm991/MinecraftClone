#include "Application.hpp"

void Application::init()
{
    renderer = Renderer();
    renderer.init();
}

void Application::generateChunk()
{
    for (unsigned int i = 0; i < chunkLength; i++)
    {
        for (unsigned int j = 0; j < chunkLength; j++)
        {
            for (unsigned int k = 0; k < chunkLength; k++)
            {
                chunk.push_back(i);
                chunk.push_back(j);
                chunk.push_back(k);
            }
        }
    }
}

void Application::prepare()
{
    std::vector<float> grassVertices;
    Cube::generatePN(grassVertices);

    generateChunk();

    uint32_t vertexBuffer;
    renderer.generateVertexBuffer(vertexBuffer, grassVertices);

    AttributeLayout posAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout normAttrib = AttributeLayout(3, GL_FLOAT);

    std::vector<AttributeLayout> attribs = std::vector<AttributeLayout>();
    attribs.push_back(posAttrib);
    attribs.push_back(normAttrib);

    renderer.generateVertexArray(vaoId, vertexBuffer, attribs);

    renderer.generateProgram(programId, "Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");

    renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());

    renderer.setUniform3f(programId, "grassBlockMaterial.grassDiffuse", glm::vec3((119.0f / 255.0f), (221.0f / 255.0f), (119.0f / 255.0f)));
    renderer.setUniform3f(programId, "grassBlockMaterial.dirtDiffuse", glm::vec3((131.0f / 255.0f), (105.0f / 255.0f), (83.0f / 255.0f)));
    renderer.setUniform1f(programId, "grassBlockMaterial.grassSpecular", 0.5f);
    renderer.setUniform1f(programId, "grassBlockMaterial.dirtSpecular", 0.0f);
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

    std::cout << "----------------------------------------------" << std::endl;

    PerformanceTimer runTimer = PerformanceTimer("Run");

    renderer.prepareForRender();
    renderer.calculateCameraTransform();

    glm::mat4 model;

    renderer.prepareForDraw(programId, textureIds, vaoId);

    PerformanceTimer renderLoopTimer = PerformanceTimer("Render Loop");
    for (unsigned int i = 0; i < std::pow(chunkLength, 3); i++)
    {
        int x = chunk.at(i * 3);
        int y = chunk.at(i * 3 + 1);
        int z = chunk.at(i * 3 + 2);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        renderer.updateModelMatrix(model);
        renderer.setUniformMatrix4fv(programId, "normalMatrix", glm::transpose(glm::inverse(model)));
        renderer.applyMvp(programId, "model", "view", "projection");
        renderer.setUniform3f(programId, "viewPos", renderer.getCameraPos());
        renderer.draw(36);
    }
    renderLoopTimer.stop();

    renderer.unprepareForDraw(programId, textureIds);

    // renderer.calculateFps();
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