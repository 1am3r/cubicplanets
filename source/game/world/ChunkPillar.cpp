#include "stdafx.h"

#include "game/types.h"
#include "ChunkPillar.h"
#include "terrain/TerrainGenerator.h"

Chunk* ChunkPillar::loadChunk(wCoord y)
{
	Chunk* curChunk = getChunkFromDisk(y);
	if (curChunk == 0) {
		curChunk = mStorage.getTerraGen().generateChunk(this, x, y, z);
	}

	addChunkToMap(curChunk, y);

	return curChunk;
}

Chunk* ChunkPillar::getChunkFromDisk(wCoord y)
{
	return 0;
}

void ChunkPillar::addChunkToMap(Chunk* chunk, wCoord y)
{
	uint32_t coordHash = getMapCoordHash(y);

	Chunk* oldChunk = mChunks[coordHash];
	if (oldChunk != 0 && !oldChunk->checkPosition(x, y, z)) {
		mStorage.unloadChunk(oldChunk);
	}

	mChunks[coordHash] = chunk;
}