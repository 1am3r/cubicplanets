#include "stdafx.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_stream.hpp>
// Workaround for cast warning
#include "boost/gzip.hpp"


#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/WorldRegion.h"
#include "game/world/World.h"
#include "game/world/ChunkStorage.h"
#include "game/world/ChunkPillar.h"


namespace bio = boost::iostreams;
namespace bfs = boost::filesystem;

namespace GameWorld {

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
	} else {
		mRegionFileSize = 0;
		mRegionFile.open(regionFilePath, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);
	}
	
	if (bfs::exists(chunkFilePath)) {
		mChunkFileSize = static_cast<uint32_t>(bfs::file_size(chunkFilePath));
		mChunkFile.open(chunkFilePath, std::ios::binary | std::ios::out | std::ios::in);
	} else {
		mChunkFileSize = 0;
		mChunkFile.open(chunkFilePath, std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);
	}

	if (mRegionFileSize <  RegionFileSectorSize) {
		createRegionFile();
		createChunksFile();
	} else {
		// Load the regions file
		loadRegionFile();

		// Test if there is even a gzip header
		if (mChunkFileSize < 16) {
			createChunksFile();
		} else {
			// Try loading the chunks file
			try {
				loadChunksFile();
			}
			catch (bio::gzip_error& e)
			{
				std::cout << "WorldRegion::loadChunksFile: gzip exception: " << e.what() << std::endl;
				createChunksFile();
			}
		}
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
	mFreeRegionSectors.clear();
	mFreeRegionSectors.resize(PillarsPerRegion + 1, true);
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
	mRegionFile.seekg(0);
	mRegionFile.read(reinterpret_cast<char*>(mPillarOffsets.data()), mPillarOffsets.size() * sizeof(mPillarOffsets[0]));

	for (size_t index = 0; index < mPillarOffsets.size(); ++index) {
		uint32_t offset = mPillarOffsets[index];
		uint32_t sector = getSectorFromOffset(offset);
		uint8_t numSectors = getSectorSizeFromOffset(offset);
		for(uint32_t index = sector; index < sector + numSectors; index++) {
			mFreeRegionSectors[index] = false;
		}
	}
}

void WorldRegion::createChunksFile()
{
	mFreeChunkSectors.clear();
	mFreeChunkSectors.resize(ChunksPerRegion + 16, true);
	mChunkOffsets.clear();
	mChunkFile.seekp(0);

	uint32_t zero = 0;
	for (uint32_t i = 0; i < ChunksPerRegion; ++i) {
		mChunkFile.write(reinterpret_cast<char*>(&zero), sizeof(zero));
	}

	// Mark header sectors as used
	uint32_t headerSize = static_cast<uint32_t>(mChunkFile.tellp());
	mChunkFileHeaderSectors = ((headerSize - 1) / ChunkFileSectorSize) + 1;
	for (uint8_t i = 0; i < mChunkFileHeaderSectors; ++i) {
		mFreeChunkSectors[i] = false;
	}
	mChunkFile.sync();
}

void WorldRegion::loadChunksFile()
{
	uint32_t chunkSectors = getChunkSectorCount();
	mFreeChunkSectors.clear();
	mFreeChunkSectors.resize(chunkSectors, true);
	mChunkFile.seekg(0);
	
	for (uint32_t i = 0; i < ChunksPerRegion; ++i) {
		uint32_t offset;
		mChunkFile.read(reinterpret_cast<char*>(&offset), sizeof(offset));
		if (offset != 0) {
			// Insert offset in map
			mChunkOffsets[i] = offset;
			
			// Mark sectors as used
			uint32_t sector = getSectorFromOffset(offset);
			uint8_t numSectors = getSectorSizeFromOffset(offset); 
			for (uint32_t j = sector; j < sector + numSectors; ++j) {
				mFreeChunkSectors[j] = false;
			}
		}
	}
	
	// Mark header sectors as used
	uint32_t headerSize = static_cast<uint32_t>(mChunkFile.tellg());
	mChunkFileHeaderSectors = ((headerSize - 1) / ChunkFileSectorSize) + 1;
	for (uint8_t i = 0; i < mChunkFileHeaderSectors; ++i) {
		mFreeChunkSectors[i] = false;
	}
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
			(*pillarIt)->saveToStream(gzOut);
			gzOut.pop();

			uint32_t pillarDataSize = static_cast<uint32_t>(pillarData.str().size());
			uint32_t sectorOffset = findFreeRegionSectorOffset(*pillarIt, pillarDataSize);
			if (sectorOffset == 0) {
				throw std::exception("Couldn't find big enough free sector in RegionFile!");
			}

			uint32_t offset = getSectorFromOffset(sectorOffset) * RegionFileSectorSize;
			regionData.seekp(offset);
			regionData.write(pillarData.str().c_str(), pillarDataSize);
		}
	}

	// Save RegionFile Header
	regionData.seekp(0);
	regionData.write(reinterpret_cast<char*>(mPillarOffsets.data()), mPillarOffsets.size() * sizeof(mPillarOffsets[0]));

	// Now save the chunks
	saveChunksToStream(chunkData);
}

uint32_t WorldRegion::findFreeRegionSectorOffset(ChunkPillar* pillar, uint32_t neededSize)
{
	size_t index = getPillarIndex(pillar->mX, pillar->mZ);
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

std::fstream log("logging.log", std::ios::trunc | std::ios::out);

void WorldRegion::saveChunksToStream(std::ostream& chunksDataStream)
{
	// Save the chunks
	bio::gzip_compressor gzComp;
	bio::filtering_ostream gzOut;
	gzOut.push(gzComp);
	std::vector<Chunk*> chunksToSave;
	
	for (auto pillarIt = mPillars.begin(); pillarIt != mPillars.end(); ++pillarIt) {
		
		if (*pillarIt == 0) {
			continue;
		}

		// Get all chunks to save from the pillar
		(*pillarIt)->getChunksToSave(chunksToSave);

		for (size_t numChunks = 0; numChunks < chunksToSave.size(); ++numChunks) {
			Chunk* curChunk = chunksToSave[numChunks];
			std::ostringstream chunkData;
			gzOut.push(chunkData);
			if (!curChunk->saveToStream(gzOut)) {
				// No data or error occured, skip chunk
				gzOut.pop();
				continue;
			}
			gzOut.pop();

			uint32_t chunkDataSize = static_cast<uint32_t>(chunkData.str().size());
			log << "Chunk x:" << (int) curChunk->mX << " y:" << (int) curChunk->mY << " z:" << (int) curChunk->mZ << " Size: " << (unsigned int) chunkDataSize << std::endl;
			uint32_t sectorOffset = findFreeChunkSectorOffset(curChunk, chunkDataSize);
			if (sectorOffset == 0) {
				throw std::exception("Couldn't find big enough free sector in ChunkFile!");
			}

			uint32_t offset = getSectorFromOffset(sectorOffset) * ChunkFileSectorSize;
			chunksDataStream.seekp(offset);
			chunksDataStream.write(chunkData.str().c_str(), chunkDataSize);
		}
		chunksToSave.clear();
	}


	// Save ChunkFile Header
	for (auto chunkIt = mChunkOffsets.begin(); chunkIt != mChunkOffsets.end(); ++chunkIt) {
		if (chunkIt->second > 0) {
			chunksDataStream.seekp(chunkIt->first * sizeof(chunkIt->second));
			chunksDataStream.write(reinterpret_cast<char*>(&chunkIt->second), sizeof(chunkIt->second));
		}
	}
}

uint32_t WorldRegion::findFreeChunkSectorOffset(Chunk* chunk, uint32_t neededSize)
{
	size_t index = getChunkIndex(chunk->mX, chunk->mY, chunk->mZ);
	uint8_t neededSectors = (neededSize / ChunkFileSectorSize) + 1;

	uint32_t offset = mChunkOffsets[index];
	if (getSectorSizeFromOffset(offset) < neededSectors) {
		// Need more space, search a bigger place

		// At first, mark old sectors as free
		for (uint32_t i = getSectorFromOffset(offset); i < getSectorSizeFromOffset(offset); ++i) {
			mFreeChunkSectors[i] = true;
		}

		// Set result to not found
		offset = 0;

		// Now search for a big enough space
		auto startIt = mFreeChunkSectors.begin();
		auto endIt = mFreeChunkSectors.end();
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
				uint32_t sector = std::distance(mFreeChunkSectors.begin(), startIt);
				offset = makeOffset(sector, neededSectors);
				
				if (sector + neededSectors > mFreeChunkSectors.size()) {
					mFreeChunkSectors.resize(mFreeChunkSectors.size() * 2, true);
				}

				// Mark found sectors as used
				mChunkOffsets[index] = offset;
				for (uint32_t i = sector; i < sector + neededSectors; ++i) {
					mFreeChunkSectors[i] = false;
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
	for (uint8_t x = 0; x < RegionPillarsXZ; x++) {
		for (uint8_t z = 0; z < RegionPillarsXZ; z++) {
			size_t index = getPillarIndexLocal(x, z);
			delete mPillars[index];
			mPillars[index] = 0;
		}
	}
}

TerrainGenerator& WorldRegion::getTerraGen()
{
	return mStorage.getTerraGen();
}

ChunkPillar& WorldRegion::getPillar(uint8_t x, uint8_t z)
{
	ChunkPillar* curPillar;
	size_t index = getPillarIndexLocal(x, z);

	curPillar = mPillars[index];
	if (curPillar == 0) {
		// Load the pillar from file
		curPillar = loadChunkPillar(x, z);
		if (curPillar == 0) {
			// Saved pillar was corrupted or not generated, create a new one
			curPillar = createChunkPillar(x, z);
		}
		mPillars[index] = curPillar;
	}
	
	return *curPillar;
}

ChunkPillar* WorldRegion::createChunkPillar(uint8_t x, uint8_t z)
{
	wCoord xAbs = xPos * RegionPillarsXZ + x;
	wCoord zAbs = zPos * RegionPillarsXZ + z;
	return new ChunkPillar(mWorld, *this, xAbs, zAbs);
}

ChunkPillar* WorldRegion::loadChunkPillar(uint8_t x, uint8_t z)
{
	uint32_t index = getPillarIndexLocal(x, z);
	uint32_t offset = mPillarOffsets[index];
	if (offset != 0) {
		try {
			// Pillar is saved, load it
			uint32_t sector = getSectorFromOffset(offset);
			uint8_t sectorSize = getSectorSizeFromOffset(offset);
			bio::gzip_decompressor gzDecomp;
			bio::filtering_istream gzIn;
			gzIn.push(gzDecomp);
			mRegionFile.seekg(sector * RegionFileSectorSize);
			gzIn.push(mRegionFile);

			wCoord xAbs = xPos * RegionPillarsXZ + x;
			wCoord zAbs = zPos * RegionPillarsXZ + z;
			return new ChunkPillar(mWorld, *this, xAbs, zAbs, gzIn);
		}
		catch (bio::gzip_error& e)
		{
			// Something was wrong with this pillar, ignore it
			std::cout << "WorldRegion::loadChunkPillar: gzip exception: " << e.what() << std::endl;
			return 0;
		}
	}
	
	// Pillar is not generated yet
	return 0;
}

Chunk* WorldRegion::loadChunk(ChunkPillar& parent, wCoord x, wCoord y, wCoord z)
{
	size_t index = getChunkIndex(x, y, z);
	uint32_t offset = mChunkOffsets[index];
	if (offset != 0) {
		try {
			// Chunk is saved, load it
			uint32_t sector = getSectorFromOffset(offset);
			uint8_t sectorSize = getSectorSizeFromOffset(offset);
			bio::gzip_decompressor gzDecomp;
			bio::filtering_istream gzIn;
			gzIn.push(gzDecomp);
			mChunkFile.seekg(sector * ChunkFileSectorSize);
			gzIn.push(mChunkFile);
			return parent.createChunkFromStream(y, gzIn);
		}
		catch (bio::gzip_error& e)
		{
			// Something was wrong with this chunk, ignore it
			std::cout << "WorldRegion::loadChunk: gzip exception: " << e.what() << std::endl;
			return 0;
		}
	}

	// Chunk not yet generated
	return 0;
}

};
