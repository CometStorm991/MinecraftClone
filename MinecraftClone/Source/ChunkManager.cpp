#include "ChunkManager.hpp"

ChunkManager::ChunkManager(
    uint32_t chunkLength,
    uint32_t renderDistance,
    uint32_t vertexFloatCount,
    Texturer diffuseTexturer,

    Renderer& renderer,
    const std::vector<AttributeLayout>& attribs
):
    chunkLength(chunkLength),
    chunkGenLength(chunkLength + 2),

    renderDistance(renderDistance),
    renderChunkLength(2 * renderDistance + 1),

    vertexFloatCount(vertexFloatCount),
    diffuseTexturer(diffuseTexturer),

    renderer(renderer),
    attribs(attribs),

    pool(ThreadPool(6))
{

}

void ChunkManager::updateDesiredMeshes(const glm::vec3& currentPos)
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
}

void ChunkManager::manageChunks()
{
    for (const std::tuple<int32_t, int32_t, int32_t>& desiredChunk : desiredMeshes)
    {
        std::unique_lock<std::mutex> completedMeshesMutexLock(completedMeshesMutex, std::try_to_lock);
        if (!completedMeshesMutexLock.owns_lock())
        {
            continue;
        }

        if (completedMeshes.find(desiredChunk) == completedMeshes.end())
        {
            generateChunk(desiredChunk);
            continue;
        }

        if (completedMeshes.at(desiredChunk) && vertexArrayIds.find(desiredChunk) == vertexArrayIds.end())
        {
            uploadChunkToGPU(desiredChunk);
            continue;
        }
    }

    std::vector<std::tuple<int32_t, int32_t, int32_t>> chunksToDelete;
    for (auto& [chunkCoords, vertexArrayId] : vertexArrayIds)
    {
        // Check if a loaded mesh doesn't exist in the "desired" list
        if (std::find(desiredMeshes.begin(), desiredMeshes.end(), chunkCoords) == desiredMeshes.end())
        {
            chunksToDelete.push_back(chunkCoords);
        }
    }
    for (const std::tuple<int32_t, int32_t, int32_t>& chunkCoords : chunksToDelete)
    {
        deleteChunkFromGPU(chunkCoords);
    }
}

void ChunkManager::stopGeneratingChunks()
{
    pool.stopAndWait();
}

const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& ChunkManager::getVertexBufferIds()
{
    return vertexBufferIds;
}

const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& ChunkManager::getVertexArrayIds()
{
    return vertexArrayIds;
}

const std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t>& ChunkManager::getVertexCounts()
{
    return vertexCounts;
}

void ChunkManager::generateChunk(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
    completedMeshes.insert({ chunkCoords, false });

    pool.enqueue([this, chunkCoords]() {
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

        /*{
            std::lock_guard<std::mutex> ioMutexLock(ioMutex);
            std::cout << "Set mesh completion of chunk " << std::get<0>(chunkCoords) << " " << std::get<1>(chunkCoords) << " " << std::get<2>(chunkCoords) << " to true" << std::endl;
        }*/
     });
}

void ChunkManager::uploadChunkToGPU(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
    std::unique_lock<std::mutex> meshesMutexLock(meshesMutex, std::try_to_lock);
    if (!meshesMutexLock.owns_lock())
    {
        return;
    }

    const std::vector<float>& vertexData = meshes.at(chunkCoords);

    uint32_t vertexCount = vertexData.size() / vertexFloatCount;
    vertexCounts.insert({ chunkCoords, vertexCount });

    vertexBufferIds.insert({ chunkCoords, 0 });
    uint32_t& vertexBufferId = vertexBufferIds.at(chunkCoords);
    renderer.generateVertexBuffer(vertexBufferId, vertexData);

    vertexArrayIds.insert({ chunkCoords, 0 });
    uint32_t& vertexArrayId = vertexArrayIds.at(chunkCoords);
    renderer.generateVertexArray(vertexArrayId, vertexBufferId, attribs);
}

void ChunkManager::deleteChunkFromGPU(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
    uint32_t vertexArrayId = vertexArrayIds.at(chunkCoords);
    renderer.deleteVertexArray(vertexArrayId);

    uint32_t vertexBufferId = vertexBufferIds.at(chunkCoords);
    renderer.deleteVertexBuffer(vertexBufferId);

    vertexCounts.erase(chunkCoords);
    vertexBufferIds.erase(chunkCoords);
    vertexArrayIds.erase(chunkCoords);
}

int ChunkManager::intPow(int base, int exp)
{
    return static_cast<int>(std::pow(base, exp));
}