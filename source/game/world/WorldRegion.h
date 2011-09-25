
#include <string>
#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_stream.hpp>


// Workaround for cast warning
#include "boost/gzip.hpp"

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"

#ifndef _WORLDREGION_H_
#define _WORLDREGION_H_

class TerrainGenerator;

namespace bfs = boost::filesystem;
namespace bio = boost::iostreams;

namespace GameWorld {

class World;
class ChunkPillar;
class ChunkStorage;

class WorldRegion
{
	friend class ChunkStorage;
	friend class ChunkPillar;
public:
	WorldRegion(World& world, ChunkStorage& store, wCoord x, wCoord z);
	~WorldRegion();

	void saveToDisk() { saveToStream(mRegionFile, mChunkFile); };

	wCoord getXPos() const { return xPos; };
	wCoord getZPos() const { return zPos; };
	bool checkCoords(wCoord x, wCoord z) { return (x == xPos && z == zPos); };

	void unloadPillars();

	TerrainGenerator& getTerraGen();

private:
	World& mWorld;
	ChunkStorage& mStorage;
	wCoord xPos;
	wCoord zPos;

	std::array<ChunkPillar*, PillarsPerRegion> mPillars;

	
	// File stuff
	bfs::fstream mRegionFile;
	bfs::fstream mChunkFile;
	uint32_t mRegionFileSize;
	uint32_t mChunkFileSize;

	std::vector<bool> mFreeRegionSectors;
	std::array<uint32_t, PillarsPerRegion> mPillarOffsets;
	std::vector<bool> mFreeChunkSectors;
	std::map<uint32_t, uint32_t> mChunkOffsets;
	uint32_t mChunkFileHeaderSectors;

private:
	ChunkPillar& getPillar(uint8_t x, uint8_t z);

	void createRegionFile();
	void loadRegionFile();
	void createChunksFile();
	void loadChunksFile();
	void saveToStream(std::ostream& regionData, std::ostream& chunkData);
	void saveChunksToStream(std::ostream& chunkData);

	ChunkPillar* createChunkPillar(uint8_t x, uint8_t z);
	ChunkPillar* loadChunkPillar(uint8_t x, uint8_t z);

	uint32_t findFreeRegionSectorOffset(ChunkPillar* pillar, uint32_t neededSize);
	uint32_t findFreeChunkSectorOffset(Chunk* chunk, uint32_t neededSize);

	Chunk* loadChunk(ChunkPillar& parent, wCoord x, wCoord y, wCoord z);

	inline uint32_t getRegionSectorCount() { return (mRegionFileSize / RegionFileSectorSize) + 1; };
	inline uint32_t getChunkSectorCount()  { return (mChunkFileSize / ChunkFileSectorSize) + 1; };
};

};

#endif // _WORLDREGION_H_
