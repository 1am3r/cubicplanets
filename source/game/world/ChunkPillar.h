#include <cstdint>
#include <iostream>
#include <array>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"
#include "game/world/ChunkStorage.h"

#ifndef _CHUNKPILLAR_H_
#define _CHUNKPILLAR_H_

namespace GameWorld {

class ChunkPillar
{
public:
	wCoord x, z;

	wCoord maxY, minY;
	bool heightMapSet;

	std::array<std::array<wCoord, ChunkSizeZ>, ChunkSizeX> heightMap;
	std::array<Chunk*, ChunksPerPillar> mChunks;

public:
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos);
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos, std::istream& data);
	~ChunkPillar() { unloadChunks(); };

	void saveToStream(std::ostream& pillarData, std::ostream& chunkData);

	void unloadChunks();
	Chunk* getChunk(wCoord y);

	bool isModified() { return mModified; };

private:
	static size_t getChunkIndex(wCoord y) { return positiveMod(y, ChunksPerPillar); };
	
private:
	WorldRegion& mWRegion;
	bool mModified;
};

};

#endif // _CHUNKPILLAR_H_
