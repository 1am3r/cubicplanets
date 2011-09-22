#include <cstdint>
#include <iostream>
#include <array>

#include "game/types.h"
#include "Chunk.h"
#include "ChunkStorage.h"

#ifndef _CHUNKPILLAR_H_
#define _CHUNKPILLAR_H_

class ChunkPillar
{
public:
	static const wCoord ChunksInPillar = 64;
	
public:
	wCoord x, z;

	wCoord maxY, minY;
	bool heightMapSet;

	std::array<std::array<wCoord, Chunk::ChunkSizeZ>, Chunk::ChunkSizeX> heightMap;
	std::array<Chunk*, ChunksInPillar> mChunks;

public:
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos);
	ChunkPillar(WorldRegion& wRegion, wCoord xPos, wCoord zPos, std::istream& data);
	~ChunkPillar() { unloadChunks(); };

	void saveToStream(std::ostream& pillarData, std::ostream& chunkData);

	void unloadChunks();
	Chunk* getChunk(wCoord y);

	bool isModified() { return mModified; };

private:
	static size_t getChunkIndex(wCoord y) { return positiveMod(y, ChunksInPillar); };
	
private:
	WorldRegion& mWRegion;
	bool mModified;
};

#endif // _CHUNKPILLAR_H_
