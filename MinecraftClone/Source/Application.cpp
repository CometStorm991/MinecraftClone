#include "Application.hpp"

Application::Application()
    : attribs(std::vector<AttributeLayout>()), pool(ThreadPool(6)), diffuseTexturer(Texturer(0, 0))
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
    diffuseTexturer = Texturer(diffuseTextureAtlas.getWidth(), diffuseTextureAtlas.getHeight());

    std::cout << "Image width: " << diffuseTextureAtlas.getWidth() << std::endl;
    std::cout << "Image height: " << diffuseTextureAtlas.getHeight() << std::endl;

    AttributeLayout posAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout normAttrib = AttributeLayout(3, GL_FLOAT);
    AttributeLayout texAttrib = AttributeLayout(2, GL_FLOAT);

    attribs.push_back(posAttrib);
    attribs.push_back(normAttrib);
    attribs.push_back(texAttrib);

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

    glm::vec3 currentPos = renderer.getCameraPos();
    if (currentPos != lastCameraPos)
    {
        desiredMeshes.clear();

        int32_t currChunkX = std::floor(currentPos.x / chunkLength);
        int32_t currChunkY = std::floor(currentPos.y / chunkLength);
        int32_t currChunkZ = std::floor(currentPos.z / chunkLength);

        for (uint32_t i = 0; i < intPow(renderChunkLength, 3); i++)
        {
            int32_t deltaChunkX = (i % intPow(renderChunkLength, 1)) / intPow(renderChunkLength, 0) - renderDistance;
            int32_t deltaChunkY = (i % intPow(renderChunkLength, 2)) / intPow(renderChunkLength, 1) - renderDistance;
            int32_t deltaChunkZ = (i % intPow(renderChunkLength, 3)) / intPow(renderChunkLength, 2) - renderDistance;

            int32_t desiredChunkX = currChunkX + deltaChunkX;
            int32_t desiredChunkY = currChunkY + deltaChunkY;
            int32_t desiredChunkZ = currChunkZ + deltaChunkZ;
            std::tuple<int32_t, int32_t, int32_t> desiredChunk = std::make_tuple(desiredChunkX, desiredChunkY, desiredChunkZ);

            desiredMeshes.push_back(desiredChunk);
        }

        lastCameraPos = currentPos;
    }

    // std::cout << "Desired meshes size: " << desiredMeshes.size() << std::endl;
    for (const std::tuple<int32_t, int32_t, int32_t>& desiredChunk : desiredMeshes)
    {
        if (completedMeshesMutex.try_lock())
        {
            if (completedMeshes.find(desiredChunk) == completedMeshes.end())
            {
                // Set mesh completion to false
                completedMeshes.insert({ desiredChunk, false });

                pool.enqueue([this, desiredChunk]() {
                    const std::tuple<int32_t, int32_t, int32_t>& chunkCoords = desiredChunk;

                    // Allocate space for block and mesh data
                    std::vector<BlockType> chunkData;
                    chunkData.resize(intPow(chunkGenLength, 3));

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
                    std::unique_lock<std::mutex> completedMeshesMutexLock(completedMeshesMutex);
                    completedMeshes.at(chunkCoords) = true;
                    completedMeshesMutexLock.unlock();

                    {
                        std::lock_guard<std::mutex> ioMutexLock(ioMutex);
                        std::cout << "Set mesh completion of chunk " << std::get<0>(chunkCoords) << " " << std::get<1>(chunkCoords) << " " << std::get<2>(chunkCoords) << " to true" << std::endl;
                    }
                    
                });
                completedMeshesMutex.unlock();
                continue;
            }

            if (completedMeshes.at(desiredChunk) && vertexArrayIds.find(desiredChunk) == vertexArrayIds.end())
            {
                if (meshesMutex.try_lock())
                {
                    const std::tuple<int32_t, int32_t, int32_t>& chunkCoords = desiredChunk;

                    const std::vector<float>& vertexData = meshes.at(chunkCoords);

                    uint32_t vertexCount = vertexData.size() / vertexFloatCount;
                    vertexCounts.insert({ chunkCoords, vertexCount });

                    vertexBufferIds.insert({ chunkCoords, 0 });
                    uint32_t& vertexBufferId = vertexBufferIds.at(chunkCoords);
                    renderer.generateVertexBuffer(vertexBufferId, vertexData);

                    vertexArrayIds.insert({ chunkCoords, 0 });
                    uint32_t& vertexArrayId = vertexArrayIds.at(chunkCoords);
                    renderer.generateVertexArray(vertexArrayId, vertexBufferId, attribs);
                    

                    meshesMutex.unlock();
                }
            }
            completedMeshesMutex.unlock();
        }
    }

    renderer.prepareForRender();
    renderer.calculateCameraTransform();

    glm::mat4 model = glm::mat4(1.0f);

    // std::cout << "Vertex Arrays Size: " << vertexArrayIds.size() << std::endl;
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