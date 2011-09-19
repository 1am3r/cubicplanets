#include "stdafx.h"

#include "WorldRegion.h"

#include <cstdint>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>


#include "game/types.h"
#include "game/world/World.h"
#include "game/world/ChunkStorage.h"
#include "game/world/ChunkPillar.h"


namespace bio = boost::iostreams;
namespace bfs = boost::filesystem;

WorldRegion* WorldRegion::loadFromDisk(World& world, ChunkStorage& store, wCoord x, wCoord z)
{
	return 0;
}


WorldRegion::WorldRegion(World& world, ChunkStorage& store, wCoord x, wCoord z) 
	: mWorld(world), mStorage(store), xPos(x), zPos(z), isLoaded(false)
{
	std::fill(mPillars, mPillars + ChunkPillarsSize, static_cast<ChunkPillar*>(0));


	std::ostringstream name;
	getFileNameFromCoords(x, z, name);
	boost::filesystem::path regions = mWorld.getWorldDirectory() / "regions";
	if (!boost::filesystem::exists(regions)) {
		boost::filesystem::create_directories(regions);
	}
	mFilePath = regions / name.str();

	mFile.open(mFilePath, std::ios_base::binary | std::ios_base::out | std::ios_base::in | std::ios_base::trunc);
	
	
	bio::filtering_ostream bout;
	bout.push(mFile);

	bout << "Test:";

	WorldRegionFile testData;
	testData.xPos = 4567;
	testData.zPos = 9876;
	testData.pillarOffsets[0] = 0;
	testData.pillarOffsets[1] = 0;
	testData.pillarOffsets[2] = 0;
	testData.pillarOffsets[3] = 0;
	testData.pillarOffsets[4] = 12345;

	bout << testData;
	bout.pop();
	mFile.close();
};

WorldRegion::~WorldRegion()
{
	unloadPillars();
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
		curPillar = ChunkPillar::loadFromStream(*this, x, z);
	}
	if (curPillar == 0) {
		curPillar = new ChunkPillar(*this, x, z);
		mPillars[index] = curPillar;
	}
	
	return *curPillar;
}
