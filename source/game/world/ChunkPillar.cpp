#include "stdafx.h"

#include <iostream>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/ChunkPillar.h"
#include "game/world/WorldRegion.h"
#include "terrain/TerrainGenerator.h"


namespace GameWorld {

ChunkPillar::ChunkPillar(World& world, WorldRegion& wRegion, wCoord xPos, wCoord zPos)
	: mWorld(world), mWRegion(wRegion), mX(xPos), mZ(zPos), heightMapSet(false), mModified(true)
{
	mChunks.fill(static_cast<Chunk*>(0));
	mWRegion.getTerraGen().setHeightMap(*this, mX, mZ);
}

ChunkPillar::ChunkPillar(World& world, WorldRegion& wRegion, wCoord xPos, wCoord zPos, std::istream& data)
	: mWorld(world), mWRegion(wRegion), mX(xPos), mZ(zPos), heightMapSet(false), mModified(false)
{
	mChunks.fill(static_cast<Chunk*>(0));
	data.read(reinterpret_cast<char*>(heightMap.data()), heightMap.size() * sizeof(heightMap[0]));
}


void ChunkPillar::saveToStream(std::ostream& pillarData)
{
	// Save the heightmap
	pillarData.write(reinterpret_cast<char*>(heightMap.data()), heightMap.size() * sizeof(heightMap[0]));
}

void ChunkPillar::getChunksToSave(std::vector<Chunk*>& chunksToSave)
{
	for (auto chunkIt = mChunks.begin(); chunkIt != mChunks.end(); ++chunkIt) {
		if (*chunkIt != 0 && (*chunkIt)->needsSaving()) {
			chunksToSave.push_back(*chunkIt);
		}
	}
}

void ChunkPillar::unloadChunks()
{
	for (int i = 0; i < ChunksPerPillar; i++) {
		if (mChunks[i] != 0) {
			delete mChunks[i];
			mChunks[i] = 0;
		}
	}
}

Chunk* ChunkPillar::getChunkAbs(wCoord y)
{
	return getChunkLocal(getChunkIndexY(y));
}

Chunk* ChunkPillar::getChunkLocal(wCoord y)
{
	size_t index = getChunkIndex(y);
	Chunk* curChunk = mChunks[index];
	if (curChunk == 0) {
		wCoord yAbs = (y > ChunksAboveZero ? y - ChunksPerPillar : y);
		curChunk = mWRegion.loadChunk(*this, mX, yAbs, mZ);
		if (curChunk == 0) {
			if (maxY > yAbs * ChunkSizeY) {
				// Chunk has blocks
				curChunk = createChunk(yAbs);
				mWRegion.getTerraGen().fillChunk(*this, *static_cast<ChunkBase*>(curChunk));
			} else {
				// Chunk is empty
				curChunk = createChunkEmpty(yAbs);
			}
		}
		mChunks[index] = curChunk;
	}	

	return curChunk;
};

};
