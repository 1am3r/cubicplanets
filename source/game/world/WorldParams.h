

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

	static const uint16_t ActiveRegions = 3;

	static const uint32_t RegionFileSectorSize = 4 * 1024;
	static const uint32_t ChunkFileSectorSize = 8 * 1024;

	static inline void getFileNameFromCoords(wCoord x, wCoord z, std::ostream& out) { out << "wr_" << boost::lexical_cast<std::string>(x) << "_" << boost::lexical_cast<std::string>(z); };
	static inline uint32_t getSectorFromOffset(uint32_t offset) { return (offset >> 8); };
	static inline uint8_t  getSectorSizeFromOffset(uint32_t offset) { return (static_cast<uint8_t>(offset & 0xFF)); };
	static inline uint32_t makeOffset(uint32_t sector, uint8_t sectorSize) { return ((sector << 8) | sectorSize); };

	static inline uint32_t getChunkPosIndex(uint8_t x, uint8_t y, uint8_t z) { return ((x * ChunkSizeX + z) * ChunkSizeZ + y); };
	static inline uint32_t getPillarIndex(wCoord x, wCoord z) { return ((positiveMod(x, RegionPillarsXZ) * RegionPillarsXZ) + positiveMod(z, RegionPillarsXZ)); };
	static inline uint32_t getChunkIndex(wCoord x, wCoord y, wCoord z)
	{
		return (static_cast<uint8_t>(positiveMod(x, RegionPillarsXZ)) << 24 | 
				static_cast<uint8_t>(positiveMod(z, RegionPillarsXZ)) << 16 |
				static_cast<uint8_t>(positiveMod(y, ChunksPerPillar)));
	};
};

#endif // _WORLDPARAMS_H_
