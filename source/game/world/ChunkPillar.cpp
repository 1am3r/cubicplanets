#include "stdafx.h"

#include "game/types.h"
#include "ChunkPillar.h"
#include "terrain/TerrainGenerator.h"


ChunkPillar* ChunkPillar::loadFromStream(WorldRegion& region, wCoord x, wCoord z)
{
	return 0;
}


ChunkPillar::ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos)
	: mWRegion(wRegion), x(xPos), z(zPos), heightMapSet(false)
{
	std::fill(mChunks, mChunks + ChunksInPillar, static_cast<Chunk*>(0));
}

Chunk* ChunkPillar::getChunkFromStream(wCoord y)
{
	return 0;
}

void ChunkPillar::unloadChunks()
{
	for (int i = 0; i < ChunksInPillar; i++) {
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
		curChunk = getChunkFromStream(y);
		if (curChunk == 0) {
			curChunk = mWRegion.getTerraGen().generateChunk(this, x, y, z);
		}
		mChunks[index] = curChunk;
	}	

	return curChunk;
};
