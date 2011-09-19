
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
	static const uint16_t ActiveRegions = 3;

	ChunkStorage(World& level);
	~ChunkStorage();

	Chunk* getChunk(wCoord x, wCoord y, wCoord z);
	ChunkPillar& getPillar(wCoord x, wCoord z);
	WorldRegion& getRegion(wCoord x, wCoord z);

	TerrainGenerator& getTerraGen() { return *mTerraGen; };
private:
	static size_t getRegionIndex(wCoord x, wCoord z){ return ((positiveMod(x, ActiveRegions) * ActiveRegions) + positiveMod(z, ActiveRegions)); };
	static wCoord getChunkCoord(wCoord val) { return ((val < 0) ? (val - (WorldRegion::ChunkPillars - 1)) : val) / WorldRegion::ChunkPillars; };

private:
	World& mLevel;
	TerrainGenerator* mTerraGen;

	WorldRegion* mRegionMap[ActiveRegions * ActiveRegions];
};

#endif // _CHUNKSTORAGE_H_
