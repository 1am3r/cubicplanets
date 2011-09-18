
#include <cstdint>

#include "game/types.h"
#include "Chunk.h"
#include "ChunkStorage.h"

#ifndef _CHUNKPILLAR_H_
#define _CHUNKPILLAR_H_

class ChunkPillar
{
public:
	wCoord x, z;

	wCoord maxY, minY;
	bool heightMapSet;

	wCoord heightMap[Chunk::ChunkSizeX][Chunk::ChunkSizeZ];
	Chunk* mChunks[128];

public:
	ChunkPillar(ChunkStorage& store) :
	  mStorage(store), heightMapSet(false)
	{
		memset(mChunks, 0, sizeof(mChunks));
	};
	ChunkPillar(ChunkStorage& store, wCoord xPos, wCoord zPos) :
	  mStorage(store), x(xPos), z(zPos), heightMapSet(false)
	{
		memset(heightMap, 0, sizeof(heightMap));
		memset(mChunks, 0, sizeof(mChunks));
	};
	~ChunkPillar() {};

	bool checkPosition(wCoord xT, wCoord zT) { return ((x == xT) && (z == zT)); };
	void unloadChunks()
	{
		for (int i = 0; i < ChunkStorage::ActivePillars; i++) {
			if (mChunks[i] != 0) {
				mStorage.unloadChunk(mChunks[i]);
				mChunks[i] = 0;
			}
		}
	};
	void addChunk(Chunk* newChunk)
	{
		uint32_t hash = getMapCoordHash(newChunk->y);

		if (mChunks[hash] != 0) {
			mStorage.unloadChunk(mChunks[hash]);
		}
		mChunks[hash] = newChunk;
	};
	Chunk* getChunk(wCoord y)
	{
		Chunk* curChunk = mChunks[getMapCoordHash(y)];
		if (curChunk == 0 || !curChunk->checkPosition(x, y, z)) {
			curChunk = loadChunk(y);
		}

		return curChunk;
	};

private:
	ChunkStorage& mStorage;

	uint32_t getMapCoordHash(wCoord y) { return (y & (ChunkStorage::ActivePillars - 1)); };
	Chunk* loadChunk(wCoord y);
	Chunk* getChunkFromDisk(wCoord y);
	void addChunkToMap(Chunk* chunk, wCoord y);
};

#endif // _CHUNKPILLAR_H_
