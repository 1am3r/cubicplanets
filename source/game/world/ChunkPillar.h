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
	friend class ChunkStorage;
	friend class WorldRegion;
public:
	wCoord mX, mZ;

	wCoord maxY, minY;
	bool heightMapSet;

	std::array<std::array<wCoord, ChunkSizeZ>, ChunkSizeX> heightMap;

public:
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos);
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos, std::istream& data);
	~ChunkPillar() { unloadChunks(); };

	void saveToStream(std::ostream& pillarData);

	void unloadChunks();

	bool isModified() { return mModified; };

private:
	static size_t getChunkIndex(uint8_t y) { return y; };
	Chunk* getChunk(uint8_t y);

private:
	std::array<Chunk*, ChunksPerPillar> mChunks;
	WorldRegion& mWRegion;
	bool mModified;
};

};

#endif // _CHUNKPILLAR_H_
