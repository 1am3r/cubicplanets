#include "stdafx.h"

#include "game/types.h"
#include "ChunkStorage.h"
#include "ChunkPillar.h"
#include "terrain/TerrainGenerator.h"


ChunkStorage::ChunkStorage(World& level) :
	mLevel(level), mTerraGen(0)
{
	memset(mChunkMap, 0, sizeof(mChunkMap));
	mChunksToUnload.reserve(ActiveChunks * ActiveChunks * ActiveChunks);

	mTerraGen = new TerrainGenerator(mLevel);
}


ChunkStorage::~ChunkStorage()
{
	if (mTerraGen) {
		delete mTerraGen;
	}
}

void ChunkStorage::update(const Ogre::FrameEvent& evt)
{
	mChunksToUnload.clear();
}



Chunk* ChunkStorage::getChunk(wCoord x, wCoord y, wCoord z)
{
	return getPillar(x, z)->getChunk(y);
}

ChunkPillar* ChunkStorage::getPillar(wCoord x, wCoord z)
{
	ChunkPillar* curPillar = mChunkMap[getMapCoordHash(x, z)];
	if (curPillar == 0 || !curPillar->checkPosition(x, z)) {
		curPillar = loadPillar(x, z);
	}

	return curPillar;
}

ChunkPillar* ChunkStorage::loadPillar(wCoord x, wCoord z)
{
	ChunkPillar* curPillar = getPillarFromDisk(x, z);
	if (curPillar == 0) {
		curPillar = new ChunkPillar(*this, x, z);
	}

	addPillarToMap(curPillar, x, z);

	return curPillar;
}

ChunkPillar* ChunkStorage::getPillarFromDisk(wCoord x, wCoord z)
{
	return 0;
}

void ChunkStorage::addPillarToMap(ChunkPillar* pillar, wCoord x, wCoord z)
{
	uint32_t coordHash = getMapCoordHash(x, z);

	ChunkPillar* oldPillar = mChunkMap[coordHash];
	if (oldPillar != 0 && !oldPillar->checkPosition(x, z)) {
		oldPillar->unloadChunks();
		delete oldPillar;
	}

	mChunkMap[coordHash] = pillar;
}
