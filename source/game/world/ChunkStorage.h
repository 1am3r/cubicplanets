#include "stdafx.h"

#include "Chunk.h"

#ifndef _CHUNKSTORAGE_H_
#define _CHUNKSTORAGE_H_

class World;
class TerrainGenerator;
class ChunkPillar;


class ChunkStorage
{
public:
	static const uint16_t ActiveChunks = 64;

	ChunkStorage(World& level);
	~ChunkStorage();

	// chunk retrival
	Chunk* getChunk(wCoord x, wCoord y, wCoord z);

	void unloadChunk(Chunk* chunk)
	{
		mChunksToUnload.push_back(chunk);
	};

	// save all Chunks
	void saveAllChunks();

	void update(const Ogre::FrameEvent& evt);

	TerrainGenerator& getTerraGen() { return *mTerraGen; };
private:
	ChunkPillar* getPillar(wCoord x, wCoord z);
	ChunkPillar* loadPillar(wCoord x, wCoord z);
	ChunkPillar* getPillarFromDisk(wCoord x, wCoord z);
	void addPillarToMap(ChunkPillar* pillar, wCoord x, wCoord z);

private:
	World& mLevel;
	TerrainGenerator* mTerraGen;

	boost::ptr_map<Point3, Chunk> mLoadedChunks;
	ChunkPillar* mChunkMap[ActiveChunks * ActiveChunks];

	boost::ptr_vector<Chunk> mChunksToUnload;

	uint32_t getMapCoordHash(wCoord x, wCoord z)
	{
		uint8_t xMap = x & (ActiveChunks - 1);
		uint8_t zMap = z & (ActiveChunks - 1);

		return xMap + (zMap * ActiveChunks);
	};
};

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
		for (int i = 0; i < ChunkStorage::ActiveChunks; i++) {
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

	uint32_t getMapCoordHash(wCoord y) { return (y & (ChunkStorage::ActiveChunks - 1)); };
	Chunk* loadChunk(wCoord y);
	Chunk* getChunkFromDisk(wCoord y);
	void addChunkToMap(Chunk* chunk, wCoord y);
};

#endif // _CHUNKSTORAGE_H_
