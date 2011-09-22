#include "stdafx.h"

#include "game/types.h"
#include "game/world/WorldRegion.h"
#include "ChunkStorage.h"
#include "ChunkPillar.h"
#include "terrain/TerrainGenerator.h"


ChunkStorage::ChunkStorage(World& level) :
	mLevel(level), mTerraGen(0)
{
	std::memset(mRegionMap, 0, sizeof(mRegionMap));
	
	mTerraGen = new TerrainGenerator(mLevel);
}


ChunkStorage::~ChunkStorage()
{
	if (mTerraGen) {
		delete mTerraGen;
	}
}

Chunk* ChunkStorage::getChunk(wCoord x, wCoord y, wCoord z)
{
	return getPillar(x, z).getChunk(y);
}

ChunkPillar& ChunkStorage::getPillar(wCoord x, wCoord z)
{
	return getRegion(x, z).getPillar(x, z);
}

WorldRegion& ChunkStorage::getRegion(wCoord x, wCoord z)
{
	x = getChunkCoord(x);
	z = getChunkCoord(z);
	size_t index = getRegionIndex(x, z);

	WorldRegion* curRegion = mRegionMap[index];
	if (curRegion == 0 || !curRegion->checkCoords(x, z)) {
		if (curRegion != 0) {
			delete curRegion;
		}
		curRegion = new WorldRegion(mLevel, *this, x, z);
		mRegionMap[index] = curRegion;
	}

	return *curRegion;
}
