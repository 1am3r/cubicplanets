
#include <cstdint>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"

#ifndef _CHUNKSTORAGE_H_
#define _CHUNKSTORAGE_H_

class TerrainGenerator;

namespace GameWorld {

class World;
class WorldRegion;
class ChunkPillar;

class ChunkStorage
{
public:
	ChunkStorage(World& level);
	~ChunkStorage();


	Chunk* getChunkAbs(wCoord x, wCoord y, wCoord z);
	Chunk* getChunkLocal(wCoord x, wCoord y, wCoord z); //{ return getChunkAbs(x * ChunkSizeX, y * ChunkSizeY, z * ChunkSizeZ); };

	uint8_t getCubeType(wCoord x, wCoord y, wCoord z) { return getChunkAbs(x, y, z)->getCubeTypeLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z)); };
	void    setCubeType(wCoord x, wCoord y, wCoord z, uint8_t type) { getChunkAbs(x, y, z)->setCubeTypeLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z), type); };
	uint8_t getCubeData(wCoord x, wCoord y, wCoord z) { return getChunkAbs(x, y, z)->getCubeDataLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z)); };
	void    setCubeData(wCoord x, wCoord y, wCoord z, uint8_t data) { getChunkAbs(x, y, z)->setCubeDataLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z), data); };

	void cubeModifiedAbs(wCoord x, wCoord y, wCoord z);
	void cubeModifiedLocal(wCoord x, wCoord y, wCoord z) { return cubeModifiedAbs(x * ChunkSizeX, y * ChunkSizeY, z * ChunkSizeZ); };

	TerrainGenerator& getTerraGen() { return *mTerraGen; };

private:
	ChunkPillar& getPillarAbs(wCoord x, wCoord z);
	ChunkPillar& getPillarLocal(wCoord x, wCoord z);
	WorldRegion& getRegion(wCoord x, wCoord z);
	void updateChunkAbs(wCoord x, wCoord y, wCoord z);
	void updateChunkLocal(wCoord x, wCoord y, wCoord z);

private:
	World& mLevel;
	TerrainGenerator* mTerraGen;

	WorldRegion* mRegionMap[ActiveRegions * ActiveRegions];
};

};

#endif // _CHUNKSTORAGE_H_
