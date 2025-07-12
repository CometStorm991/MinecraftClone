#include "Application.hpp"

Application::Application()
    : attribs(std::vector<AttributeLayout>()), pool(ThreadPool(6))
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
    Texturer diffuseTexturer = Texturer(diffuseTextureAtlas.getWidth(), diffuseTextureAtlas.getHeight());

    std::cout << "Image width: " << diffuseTextureAtlas.getWidth() << std::endl;
    std::cout << "Image height: " << diffuseTextureAtlas.getHeight() << std::endl;

    AttributeLayout posAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout normAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout texAttrib = AttributeLayout(2, GL_FLOAT);

    attribs.push_back(posAttrib);
    attribs.push_back(normAttrib);
    attribs.push_back(texAttrib);

    for (uint32_t i = 0; i < totalChunkCount; i++)
    {
        uint32_t chunkCountX = std::get<0>(chunkCounts);
        uint32_t chunkCountY = std::get<1>(chunkCounts);
        uint32_t chunkCountZ = std::get<2>(chunkCounts);
        int32_t chunkX = (i % chunkCountX) / 1;
        int32_t chunkY = (i % (chunkCountX * chunkCountY)) / chunkCountX;
        int32_t chunkZ = (i % (chunkCountX * chunkCountY * chunkCountZ)) / (chunkCountX * chunkCountY);
        std::tuple<int32_t, int32_t, int32_t> chunkCoords = std::tuple<int32_t, int32_t, int32_t>(chunkX, chunkY, chunkZ);

        // Set mesh completion to false
        std::unique_lock<std::mutex> completedMeshesMutexLock(meshesMutex);
        completedMeshes.insert({ chunkCoords, false });
        completedMeshesMutexLock.unlock();

        pool.enqueue([this, chunkCoords, diffuseTexturer]() {
            // Allocate space for block and mesh data
            std::vector<BlockType> chunkData;
            chunkData.resize(intPow(chunkLength + 2, 3));
            
            std::unique_lock<std::mutex> meshesMutexLock(meshesMutex);
            meshes.insert({ chunkCoords, {} });
            std::vector<float>& vertexData = meshes.at(chunkCoords);
            meshesMutexLock.unlock();

            // Generate block and mesh data
            Chunk chunk = Chunk(chunkData, vertexData, chunkLength, vertexFloatCount, diffuseTexturer);
            chunk.generateBlocks(chunkCoords);
            if (chunk.getSolidBlockCount() == 0)
            {
                meshesMutexLock.lock();
                meshes.erase(chunkCoords);
                meshesMutexLock.unlock();
                return;
            }
            chunk.generateMesh(chunkCoords);

            // Set mesh completion to true
            std::unique_lock<std::mutex> completedMeshesMutexLock(meshesMutex);
            completedMeshes.at(chunkCoords) = true;
            completedMeshesMutexLock.unlock();
        });
    }

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
    //runMultipleLighting();

    //std::cout << "----------------------------------------------" << std::endl;

    // PerformanceTimer runTimer = PerformanceTimer("Run");

    renderer.prepareForRender();
    renderer.calculateCameraTransform();

    glm::mat4 model = glm::mat4(1.0f);

    if (meshesMutex.try_lock())
    {
        for (auto& [chunkCoords, completed] : completedMeshes)
        {
            if (!completed)
            {
                continue;
            }

            if (vertexArrayIds.find(chunkCoords) == vertexArrayIds.end())
            {
                std::vector<float>& vertexData = meshes.at(chunkCoords);

                uint32_t vertexCount = vertexData.size() / vertexFloatCount;
                vertexCounts.insert({ chunkCoords, vertexCount });

                vertexBufferIds.insert({ chunkCoords, 0 });
                uint32_t& vertexBufferId = vertexBufferIds.at(chunkCoords);
                renderer.generateVertexBuffer(vertexBufferId, vertexData);

                vertexArrayIds.insert({ chunkCoords, 0 });
                uint32_t& vertexArrayId = vertexArrayIds.at(chunkCoords);
                renderer.generateVertexArray(vertexArrayId, vertexBufferId, attribs);
            }
        }

        meshesMutex.unlock();
    }

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
    pool.stopAndWait();
}

bool Application::shouldEnd()
{
    return renderer.getWindowShouldClose();
}