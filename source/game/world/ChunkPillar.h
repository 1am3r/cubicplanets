
#include <cstdint>

#include "game/types.h"
#include "Chunk.h"
#include "ChunkStorage.h"

#ifndef _CHUNKPILLAR_H_
#define _CHUNKPILLAR_H_

class ChunkPillar
{
public:
	static const wCoord ChunksInPillar = 128;

	static ChunkPillar* loadFromStream(WorldRegion& region, wCoord x, wCoord z);

public:
	wCoord x, z;

	wCoord maxY, minY;
	bool heightMapSet;

	wCoord heightMap[Chunk::ChunkSizeX][Chunk::ChunkSizeZ];
	Chunk* mChunks[ChunksInPillar];

public:
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos);
	~ChunkPillar() { unloadChunks(); };

	void unloadChunks();
	Chunk* getChunk(wCoord y);

private:
	static size_t getChunkIndex(wCoord y) { return positiveMod(y, ChunksInPillar); };
	

	WorldRegion& mWRegion;
	
	Chunk* getChunkFromStream(wCoord y);

	struct ChunkPillarFile {
		// uint8_t here, as it is a relative position in the WorldRegion, so no need for wCoord
		uint8_t xPos;
		uint8_t zPos;

		uint32_t chunkOffsets[ChunksInPillar];
	};
};

#endif // _CHUNKPILLAR_H_
