
#include <string>
#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>

// Workaround for cast warning
#include "boost/gzip.hpp"

#include "game/types.h"
#include "game/world/Chunk.h"

#ifndef _WORLDREGION_H_
#define _WORLDREGION_H_

class World;
class ChunkPillar;
class ChunkStorage;
class TerrainGenerator;

namespace bfs = boost::filesystem;
namespace bio = boost::iostreams;

class WorldRegion
{
public:
	static const wCoord ChunkPillars = 32;
	static const size_t ChunkPillarsSize = ChunkPillars * ChunkPillars;
	static const uint32_t RegionFileSectorSize = 4 * 1024;
	static const uint32_t ChunkFileSectorSize = 8 * 1024;

	static void getFileNameFromCoords(wCoord x, wCoord z, std::ostream& out) { out << "wr_" << boost::lexical_cast<std::string>(x) << "_" << boost::lexical_cast<std::string>(z); };

public:
	WorldRegion(World& world, ChunkStorage& store, wCoord x, wCoord z);
	~WorldRegion();

	wCoord getXPos() const { return xPos; };
	wCoord getZPos() const { return zPos; };
	bool checkCoords(wCoord x, wCoord z) { return (x == xPos && z == zPos); };

	void unloadPillars();
	ChunkPillar& getPillar(wCoord x, wCoord z);

	TerrainGenerator& getTerraGen();

private:
	World& mWorld;
	ChunkStorage& mStorage;
	wCoord xPos;
	wCoord zPos;

	std::array<ChunkPillar*, ChunkPillarsSize> mPillars;

	
	// File stuff
	bfs::fstream mRegionFile;
	bfs::fstream mChunkFile;
	uint32_t mRegionFileSize;
	uint32_t mChunkFileSize;

	std::vector<bool> mFreeRegionSectors;
	std::array<uint32_t, ChunkPillarsSize> mPillarOffsets;
	std::vector<bool> mFreeChunkSectors;
	std::map<uint32_t, uint32_t> mChunkOffsets;
	
	void createRegionFile();
	void loadRegionFile();
	void saveToStream(std::ostream& regionData, std::ostream& chunkData);

	ChunkPillar* createChunkPillar(wCoord x, wCoord z);
	ChunkPillar* loadChunkPillar(wCoord x, wCoord z);

	uint32_t findFreeRegionSectorOffset(ChunkPillar* pillar, uint32_t neededSize);
	uint32_t findFreeChunkSectorOffset(Chunk* pillar, uint32_t neededSize);

	inline uint32_t getRegionSectorCount() { return (mRegionFileSize / RegionFileSectorSize) + 1; };
	static inline uint32_t getSectorFromOffset(uint32_t offset) { return (offset >> 8); };
	static inline uint8_t  getSectorSizeFromOffset(uint32_t offset) { return (static_cast<uint8_t>(offset & 0xFF)); };
	static inline uint32_t getChunkPosIndex(uint8_t x, uint8_t y, uint8_t z) { return ((x * Chunk::ChunkSizeX + z) * Chunk::ChunkSizeZ + y); };
	static inline uint32_t makeOffset(uint32_t sector, uint8_t sectorSize) { return ((sector << 8) | sectorSize); };
	static inline uint32_t getPillarIndex(wCoord x, wCoord z) { return ((positiveMod(x, ChunkPillars) * ChunkPillars) + positiveMod(z, ChunkPillars)); };
	static inline uint32_t getChunkIndex(wCoord x, wCoord y, wCoord z);
};


#endif // _WORLDREGION_H_
