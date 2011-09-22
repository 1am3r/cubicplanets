#include "stdafx.h"

#include "WorldRegion.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_stream.hpp>
// Workaround for cast warning
#include "boost/gzip.hpp"


#include "game/types.h"
#include "game/world/World.h"
#include "game/world/ChunkStorage.h"
#include "game/world/ChunkPillar.h"


namespace bio = boost::iostreams;
namespace bfs = boost::filesystem;

WorldRegion::WorldRegion(World& world, ChunkStorage& store, wCoord x, wCoord z) 
	: mWorld(world), mStorage(store), xPos(x), zPos(z)
{
	mPillars.fill(static_cast<ChunkPillar*>(0));

	std::ostringstream name;
	getFileNameFromCoords(x, z, name);
	bfs::path regions = mWorld.getWorldDirectory() / "regions";
	if (!bfs::exists(regions)) {
		bfs::create_directories(regions);
	}
	bfs::path regionFilePath = regions / (name.str() + ".wr");
	bfs::path chunkFilePath = regions / (name.str() + ".wrc");

	if (bfs::exists(regionFilePath)) {
		mRegionFileSize = static_cast<uint32_t>(bfs::file_size(regionFilePath));
		mRegionFile.open(regionFilePath, std::ios::binary | std::ios::out | std::ios::in);
		mChunkFileSize = static_cast<uint32_t>(bfs::file_size(chunkFilePath));
		mChunkFile.open(chunkFilePath, std::ios::binary | std::ios::out | std::ios::in);
	} else {
		mRegionFileSize = 0;
		mRegionFile.open(regionFilePath, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);
		mChunkFileSize = 0;
		mChunkFile.open(chunkFilePath, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);
	}
	
	if (mRegionFileSize <  sizeof(mPillarOffsets)) {
		createRegionFile();
	} else {
		loadRegionFile();
	}
};

WorldRegion::~WorldRegion()
{
	saveToStream(mRegionFile, mChunkFile);

	unloadPillars();

	mRegionFile.close();
	mChunkFile.close();
}


void WorldRegion::createRegionFile()
{
	mFreeRegionSectors.resize(ChunkPillarsSize + 1, true);
	mPillarOffsets.fill(0);

	mRegionFile.seekp(0);
	mRegionFile.write(reinterpret_cast<char*>(mPillarOffsets.data()), mPillarOffsets.size() * sizeof(mPillarOffsets[0]));
	mFreeRegionSectors[0] = false;
	mRegionFile.sync();
}

void WorldRegion::loadRegionFile()
{
	uint32_t regionSectors = getRegionSectorCount();
	mFreeRegionSectors.clear();
	mFreeRegionSectors.resize(regionSectors, true);
	mRegionFile.read(reinterpret_cast<char*>(mPillarOffsets.data()), mPillarOffsets.size() * sizeof(mPillarOffsets[0]));

	for (size_t index = 0; index < mPillarOffsets.size(); ++index) {
		uint32_t offset = mPillarOffsets[index];
		uint32_t sector = getSectorFromOffset(offset);
		uint8_t numSectors = getSectorSizeFromOffset(offset);
		for(uint32_t index = sector; index < sector + numSectors; index++) {
			mFreeRegionSectors[index] = false;
		}
	}

	//TODO: the same for the chunk file
}

void WorldRegion::saveToStream(std::ostream& regionData, std::ostream& chunkData)
{
	// Save the Pillars
	bio::gzip_compressor gzComp;
	bio::filtering_ostream gzOut;
	gzOut.push(gzComp);

	for (auto pillarIt = mPillars.begin(); pillarIt != mPillars.end(); ++pillarIt) {
		if ((*pillarIt != 0) && (*pillarIt)->isModified()) {
			std::ostringstream pillarData;
			gzOut.push(pillarData);
			(*pillarIt)->saveToStream(gzOut, chunkData);
			gzOut.pop();

			uint32_t pillarDataSize = static_cast<uint32_t>(pillarData.str().size());
			uint32_t sectorOffset = findFreeRegionSectorOffset(*pillarIt, pillarDataSize);
			if (sectorOffset == 0) {
				throw std::exception("Couldn't find free sector big enough!");
			}

			uint32_t offset = getSectorFromOffset(sectorOffset) * RegionFileSectorSize;
			regionData.seekp(offset);
			regionData.write(pillarData.str().c_str(), pillarDataSize);
		}
	}

	// Save region file
	regionData.seekp(0);
	regionData.write(reinterpret_cast<char*>(mPillarOffsets.data()), mPillarOffsets.size() * sizeof(mPillarOffsets[0]));
}

uint32_t WorldRegion::findFreeRegionSectorOffset(ChunkPillar* pillar, uint32_t neededSize)
{
	size_t index = getPillarIndex(pillar->x, pillar->z);
	uint8_t neededSectors = (neededSize / RegionFileSectorSize) + 1;

	uint32_t offset = mPillarOffsets[index];
	if (getSectorSizeFromOffset(offset) < neededSectors) {
		// Need more space, search a bigger place

		// At first, mark old sectors as free
		for (uint32_t i = getSectorFromOffset(offset); i < getSectorSizeFromOffset(offset); ++i) {
			mFreeRegionSectors[i] = true;
		}

		// Set result to not found
		offset = 0;

		// Now search for a big enough space
		auto startIt = mFreeRegionSectors.begin();
		auto endIt = mFreeRegionSectors.end();
		while (startIt != endIt) {
			// Find first free
			auto posIt = std::find(startIt, endIt, true);

			// Save first free for index calculation
			startIt = posIt;

			// Test if enough space is free
			uint8_t free;
			for (free = 1; free < neededSectors; ++free) {
				++posIt;
				if (*posIt == false || posIt == endIt) {
					break;
				}
			}
			if (free == neededSectors) {
				// Found big enough place
				uint32_t sector = std::distance(mFreeRegionSectors.begin(), startIt);
				offset = makeOffset(sector, neededSectors);
				
				if (sector + neededSectors > mFreeRegionSectors.size()) {
					mFreeRegionSectors.resize(mFreeRegionSectors.size() * 2, true);
				}

				// Mark found sectors as used
				mPillarOffsets[index] = offset;
				for (uint32_t i = sector; i < sector + neededSectors; ++i) {
					mFreeRegionSectors[i] = false;
				}

				break;
			} else {
				// not big enough, start over
				startIt = posIt;
			}
		}

	} else {
		// Old space is big enough, reuse it
	}

	return offset;
}

void WorldRegion::unloadPillars()
{
	for (wCoord x = 0; x < ChunkPillars; x++) {
		for (wCoord z = 0; z < ChunkPillars; z++) {
			size_t index = getPillarIndex(x, z);
			delete mPillars[index];
			mPillars[index] = 0;
		}
	}
}

TerrainGenerator& WorldRegion::getTerraGen()
{
	return mStorage.getTerraGen();
}

ChunkPillar& WorldRegion::getPillar(wCoord x, wCoord z)
{
	ChunkPillar* curPillar;
	size_t index = getPillarIndex(x, z);

	curPillar = mPillars[index];
	if (curPillar == 0) {
		if  (mPillarOffsets[index] != 0) {
			// Load the pillar from file
			curPillar = loadChunkPillar(x, z);
		} else {
			// No pillar there, make a new one
			curPillar = createChunkPillar(x, z);
		}
		mPillars[index] = curPillar;
	}
	
	return *curPillar;
}

ChunkPillar* WorldRegion::createChunkPillar(wCoord x, wCoord z)
{
	return new ChunkPillar(*this, x, z);
}

ChunkPillar* WorldRegion::loadChunkPillar(wCoord x, wCoord z)
{
	uint32_t index = getPillarIndex(x, z);
	uint32_t offset = mPillarOffsets[index];
	if (offset == 0) {
		// Pillar is not generated yet
		return 0;
	} else {
		// Pillar is saved, load it
		uint32_t sector = getSectorFromOffset(offset);
		uint8_t sectorSize = getSectorSizeFromOffset(offset);
		bio::gzip_decompressor gzDecomp;
		bio::filtering_istream gzIn;
		gzIn.push(gzDecomp);
		mRegionFile.seekg(sector * RegionFileSectorSize);
		gzIn.push(mRegionFile);
		return new ChunkPillar(*this, x, z, gzIn);
	}

}


uint32_t WorldRegion::getChunkIndex(wCoord x, wCoord y, wCoord z)
{
	return (static_cast<uint8_t>(positiveMod(x, ChunkPillars)) << 24 | 
			static_cast<uint8_t>(positiveMod(z, ChunkPillars)) << 16 |
			static_cast<uint8_t>(positiveMod(y, ChunkPillar::ChunksInPillar)));
}
