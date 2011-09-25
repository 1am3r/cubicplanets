#include <cstdint>
#include <iostream>
#include <array>

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"
#include "game/world/ChunkBase.h"

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

	std::array<wCoord, ChunkSizeZ * ChunkSizeX> heightMap;

public:
	ChunkPillar(World& world, WorldRegion& wRegion, wCoord xPos, wCoord zPos);
	ChunkPillar(World& world, WorldRegion& wRegion, wCoord xPos, wCoord zPos, std::istream& data);
	~ChunkPillar() { unloadChunks(); };

	void saveToStream(std::ostream& pillarData);
	void getChunksToSave(std::vector<Chunk*>& chunksToSave);

	void unloadChunks();

	bool isModified() { return mModified; };

	Chunk* getChunkAbs(wCoord y);
	Chunk* getChunkLocal(wCoord y);

	void setCubeTypeAbs(wCoord x, wCoord y, wCoord z, uint8_t cubeType)
	{
		Chunk* chunk = getChunkAbs(y);
		if (chunk->isEmptyChunk()) {
			convertEmptyChunk(&chunk);
		}
		static_cast<ChunkBase*>(chunk)->setCubeTypeLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z), cubeType); 
	};
	void setCubeDataAbs(wCoord x, wCoord y, wCoord z, uint8_t cubeData)
	{
		Chunk* chunk = getChunkAbs(y);
		if (chunk->isEmptyChunk()) {
			convertEmptyChunk(&chunk);
		}
		static_cast<ChunkBase*>(chunk)->setCubeDataLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z), cubeData);
	};
	uint8_t getCubeTypeAbs(wCoord x, wCoord y, wCoord z) { return getChunkAbs(y)->getCubeTypeLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z)); };
	uint8_t getCubeDataAbs(wCoord x, wCoord y, wCoord z) { return getChunkAbs(y)->getCubeDataLocal(getCubeIndexXZ(x), getCubeIndexY(y), getCubeIndexXZ(z)); };

	Chunk* createChunkFromStream(wCoord y, std::istream& data) { return new ChunkBase(mWorld, mX, y, mZ, data); };
	Chunk* createChunk(wCoord y) { return new ChunkBase(mWorld, mX, y, mZ); };
	Chunk* createChunkEmpty(wCoord y) { return new Chunk(mX, y, mZ); };
	void convertEmptyChunk(Chunk** chunk)
	{
		size_t index = getChunkIndex(getChunkIndexY((*chunk)->mY));
		ChunkBase* convChunk = new ChunkBase(mWorld, (*chunk)->mX, (*chunk)->mY, (*chunk)->mZ);
		mChunks[index] = convChunk;
		delete (*chunk);
		*chunk = convChunk;
	};

private:
	static size_t getChunkIndex(uint8_t y) { return y; };
	ChunkBase* getChunkBaseAbs(wCoord y);

private:
	std::array<Chunk*, ChunksPerPillar> mChunks;
	WorldRegion& mWRegion;
	World& mWorld;
	bool mModified;
};

};

#endif // _CHUNKPILLAR_H_
