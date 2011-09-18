
#include <cstdint>

#include "game/types.h"

#include "Chunk.h"
#include "WorldRegion.h"

#ifndef _CHUNKSTORAGE_H_
#define _CHUNKSTORAGE_H_

class World;
class TerrainGenerator;
class ChunkPillar;


class ChunkStorage
{
public:
	static const uint16_t ActivePillars = 64;
	static const uint16_t ActiveRegions = (ActivePillars / WorldRegion::ChunkPillars) + 1;

	ChunkStorage(World& level);
	~ChunkStorage();

	// chunk retrival
	Chunk* getChunk(wCoord x, wCoord y, wCoord z);

	void unloadChunk(Chunk* chunk)
	{
		mChunksToUnload.push_back(chunk);
	};

	// save all Chunks
	void saveAllChunks();

	void update(const Ogre::FrameEvent& evt);

	TerrainGenerator& getTerraGen() { return *mTerraGen; };
private:
	ChunkPillar* getPillar(wCoord x, wCoord z);
	ChunkPillar* loadPillar(wCoord x, wCoord z);
	ChunkPillar* getPillarFromDisk(wCoord x, wCoord z);
	void addPillarToMap(ChunkPillar* pillar, wCoord x, wCoord z);

	WorldRegion& getRegion(wCoord x, wCoord z);

private:
	World& mLevel;
	TerrainGenerator* mTerraGen;

	boost::ptr_map<Point3, Chunk> mLoadedChunks;
	ChunkPillar* mPillarMap[ActivePillars * ActivePillars];
	WorldRegion* mRegionMap[ActivePillars * ActivePillars];

	boost::ptr_vector<Chunk> mChunksToUnload;

	uint32_t getMapCoordHash(wCoord x, wCoord z)
	{
		uint16_t xMap = x & (ActivePillars - 1);
		uint16_t zMap = z & (ActivePillars - 1);

		return xMap + (zMap * ActivePillars);
	};
};

#endif // _CHUNKSTORAGE_H_
