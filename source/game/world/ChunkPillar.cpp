#include "stdafx.h"

#include <iostream>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/ChunkPillar.h"
#include "game/world/WorldRegion.h"
#include "terrain/TerrainGenerator.h"


namespace GameWorld {

ChunkPillar::ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos)
	: mWRegion(wRegion), x(xPos), z(zPos), heightMapSet(false), mModified(true)
{
	mChunks.fill(static_cast<Chunk*>(0));
	mWRegion.getTerraGen().setHeightMap(*this, x, z);
}

ChunkPillar::ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos, std::istream& data)
	: mWRegion(wRegion), x(xPos), z(zPos), heightMapSet(false), mModified(false)
{
	mChunks.fill(static_cast<Chunk*>(0));
}


void ChunkPillar::saveToStream(std::ostream& pillarData, std::ostream& chunkData)
{
	// Save the heightmap
	for (auto xIt = heightMap.begin(); xIt != heightMap.end(); ++xIt) {
		pillarData.write(reinterpret_cast<char*>((*xIt).data()), (*xIt).size() * sizeof(heightMap[0][0]));
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

Chunk* ChunkPillar::getChunk(wCoord y)
{
	size_t index = getChunkIndex(y);
	Chunk* curChunk = mChunks[index];
	if (curChunk == 0) {
		//TODO: load chunk if saved
		//curChunk = loadChunk();
		if (curChunk == 0) {
			curChunk = mWRegion.getTerraGen().generateChunk(*this, x, y, z);
		}
		mChunks[index] = curChunk;
	}	

	return curChunk;
};

};
