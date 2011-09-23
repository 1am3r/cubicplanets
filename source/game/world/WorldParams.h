

#include <cstdint>

#include <boost/lexical_cast.hpp>

#include "game/types.h"

#ifndef _WORLDPARAMS_H_
#define _WORLDPARAMS_H_

namespace GameWorld {
	static const uint8_t ChunkSizeX = 16;
	static const uint8_t ChunkSizeY = 128;
	static const uint8_t ChunkSizeZ = 16;

	static const wCoord ChunksPerPillar = 64;
	static const wCoord RegionPillarsXZ = 32;
	static const size_t PillarsPerRegion = RegionPillarsXZ * RegionPillarsXZ;
	static const size_t ChunksPerRegion = PillarsPerRegion * ChunksPerPillar;
	static const uint8_t ChunksAboveZero = (ChunksPerPillar / 2) -1;

	static const uint16_t ActiveRegions = 3;

	static const uint32_t RegionFileSectorSize = 4 * 1024;
	static const uint32_t ChunkFileSectorSize = 8 * 1024;

	static inline void getFileNameFromCoords(wCoord x, wCoord z, std::ostream& out) { out << "wr_" << boost::lexical_cast<std::string>(x) << "_" << boost::lexical_cast<std::string>(z); };
	static inline uint32_t getSectorFromOffset(uint32_t offset) { return (offset >> 8); };
	static inline uint8_t  getSectorSizeFromOffset(uint32_t offset) { return (static_cast<uint8_t>(offset & 0xFF)); };
	static inline uint32_t makeOffset(uint32_t sector, uint8_t sectorSize) { return ((sector << 8) | sectorSize); };

	static inline size_t getRegionIndex(wCoord x, wCoord z){ return ((positiveMod(x, ActiveRegions) * ActiveRegions) + positiveMod(z, ActiveRegions)); };
	static inline wCoord getRegionCoord(wCoord val) { return ((val < 0) ? (val - (RegionPillarsXZ - 1)) : val) / RegionPillarsXZ; };
	static inline wCoord getChunkCoordXZ(wCoord val) { return (((val < 0) ? (val - (ChunkSizeX - 1)) : val) / ChunkSizeX); };
	static inline wCoord getChunkCoordY(wCoord val) { return (((val < 0) ? (val - (ChunkSizeY - 1)) : val) / ChunkSizeY); };

	static inline uint8_t getCubeIndexXZ(wCoord val) { return val & (ChunkSizeX - 1); };
	static inline uint8_t getCubeIndexY(wCoord val) { return val & (ChunkSizeY - 1); };
	static inline uint8_t getChunkIndexXZ(wCoord val) { return positiveMod(getChunkCoordXZ(val), RegionPillarsXZ); };
	static inline uint8_t getChunkIndexY(wCoord val) { return positiveMod(getChunkCoordY(val), ChunksPerPillar); };
	static inline uint8_t getChunkIndexXZLocal(wCoord val) { return positiveMod(val, RegionPillarsXZ); };
	static inline uint8_t getChunkIndexYLocal(wCoord val) { return positiveMod(val, ChunksPerPillar); };
	static inline uint32_t getChunkPosIndex(uint8_t x, uint8_t y, uint8_t z) { return ((x * ChunkSizeZ + z) * ChunkSizeY + y); };
	static inline uint32_t getPillarIndexLocal(uint8_t x, uint8_t z) { return ((x * RegionPillarsXZ) + z); };
	static inline uint32_t getPillarIndex(wCoord x, wCoord z) { return getPillarIndexLocal(positiveMod(x, RegionPillarsXZ), positiveMod(z, RegionPillarsXZ)); };
	static inline uint32_t getChunkIndexLocal(uint8_t x, uint8_t y, uint8_t z) { return (((x * RegionPillarsXZ) + z) * ChunksPerPillar) + y; };
	static inline uint32_t getChunkIndex(wCoord x, wCoord y, wCoord z)
	{
		return getChunkIndexLocal(positiveMod(x, RegionPillarsXZ), positiveMod(y, ChunksPerPillar), positiveMod(z, RegionPillarsXZ));
	};
	static inline uint32_t getChunkPosKey(wCoord x, wCoord y, wCoord z)
	{
		return (static_cast<uint8_t>(positiveMod(x, RegionPillarsXZ)) << 24 | 
				static_cast<uint8_t>(positiveMod(z, RegionPillarsXZ)) << 16 |
				static_cast<uint8_t>(positiveMod(y, ChunksPerPillar)));
	};
	static inline uint32_t convertChunkPosKeyToIndex(uint32_t key) { return getChunkIndexLocal(key >> 24, (key >> 16) & 0xFF, key & 0xFF); };

	static inline size_t getHeightMapIndex(wCoord x, wCoord z) { return (x * ChunkSizeZ + z); };
};

#endif // _WORLDPARAMS_H_
