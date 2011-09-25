#include <cstdint>
#include <array>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/ChunkPillar.h"

#ifndef _CHUNKSTORAGE_H_
#define _CHUNKSTORAGE_H_

class TerrainGenerator;

namespace GameWorld {

class World;
class WorldRegion;
class Chunk;

class ChunkStorage
{
public:
	ChunkStorage(World& level);
	~ChunkStorage();

	void shutdown();

	Chunk* getChunkAbs(wCoord x, wCoord y, wCoord z);
	Chunk* getChunkLocal(wCoord x, wCoord y, wCoord z);
	ChunkPillar& getPillarAbs(wCoord x, wCoord z);
	ChunkPillar& getPillarLocal(wCoord x, wCoord z);

	uint8_t getCubeType(wCoord x, wCoord y, wCoord z) { return getPillarAbs(x, z).getCubeTypeAbs(x, y, z); };
	void    setCubeType(wCoord x, wCoord y, wCoord z, uint8_t type) { getPillarAbs(x, z).setCubeTypeAbs(x, y, z, type); };
	uint8_t getCubeData(wCoord x, wCoord y, wCoord z) { return getPillarAbs(x, z).getCubeDataAbs(x, y, z); };
	void    setCubeData(wCoord x, wCoord y, wCoord z, uint8_t data) { getPillarAbs(x, z).setCubeDataAbs(x, y, z, data); };

	void cubeModifiedAbs(wCoord x, wCoord y, wCoord z);

	TerrainGenerator& getTerraGen() { return *mTerraGen; };

private:
	WorldRegion& getRegion(wCoord x, wCoord z);
	void updateChunkAbs(wCoord x, wCoord y, wCoord z);
	void updateChunkLocal(wCoord x, wCoord y, wCoord z);

private:
	World& mLevel;
	TerrainGenerator* mTerraGen;

	std::array<WorldRegion*, ActiveRegions * ActiveRegions> mRegionMap;
};

};

#endif // _CHUNKSTORAGE_H_
