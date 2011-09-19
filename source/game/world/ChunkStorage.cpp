#include "stdafx.h"

#include "game/types.h"
#include "game/world/WorldRegion.h"
#include "ChunkStorage.h"
#include "ChunkPillar.h"
#include "terrain/TerrainGenerator.h"


ChunkStorage::ChunkStorage(World& level) :
	mLevel(level), mTerraGen(0)
{
	std::memset(mPillarMap, 0, sizeof(mPillarMap));
	std::memset(mRegionMap, 0, sizeof(mRegionMap));

	mChunksToUnload.reserve(ActivePillars * ActivePillars * ActivePillars);

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
	ChunkPillar* curPillar = mPillarMap[getCoordHash(x, z, ActivePillars)];
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
	uint32_t coordHash = getCoordHash(x, z, ActivePillars);

	ChunkPillar* oldPillar = mPillarMap[coordHash];
	if (oldPillar != 0 && !oldPillar->checkPosition(x, z)) {
		oldPillar->unloadChunks();
		delete oldPillar;
	}

	mPillarMap[coordHash] = pillar;
}


WorldRegion& ChunkStorage::getRegion(wCoord x, wCoord z)
{
	x /= WorldRegion::ChunkPillars;
	z /= WorldRegion::ChunkPillars;
	uint32_t coordHash = getCoordHash(x, z, WorldRegion::ChunkPillars);

	WorldRegion* curRegion = mRegionMap[coordHash];
	if (curRegion == 0 || !curRegion->checkPosition(x, z)) {
		curRegion = loadRegion(x, z);
	}

	return *curRegion;
}

WorldRegion* ChunkStorage::loadRegion(wCoord x, wCoord z)
{
	return 0;
}
