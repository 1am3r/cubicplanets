
#include <cstdint>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"
#include "game/world/WorldRegion.h"

#ifndef _CHUNKSTORAGE_H_
#define _CHUNKSTORAGE_H_

class TerrainGenerator;

namespace GameWorld {

class World;
class ChunkPillar;

class ChunkStorage
{
public:
	ChunkStorage(World& level);
	~ChunkStorage();

	Chunk* getChunk(wCoord x, wCoord y, wCoord z);
	ChunkPillar& getPillar(wCoord x, wCoord z);
	WorldRegion& getRegion(wCoord x, wCoord z);

	TerrainGenerator& getTerraGen() { return *mTerraGen; };
private:
	static size_t getRegionIndex(wCoord x, wCoord z){ return ((positiveMod(x, ActiveRegions) * ActiveRegions) + positiveMod(z, ActiveRegions)); };
	static wCoord getChunkCoord(wCoord val) { return ((val < 0) ? (val - (RegionPillarsXZ - 1)) : val) / RegionPillarsXZ; };

private:
	World& mLevel;
	TerrainGenerator* mTerraGen;

	WorldRegion* mRegionMap[ActiveRegions * ActiveRegions];
};

};

#endif // _CHUNKSTORAGE_H_
