#include "stdafx.h"

#include "WorldRegion.h"

#include <cstdint>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "game/types.h"
#include "game/world/World.h"
#include "game/world/ChunkPillar.h"

WorldRegion::WorldRegion(World& world, wCoord x, wCoord z) 
	: mWorld(world), xPos(x), zPos(z), isLoaded(false)
{
	std::ostringstream name;
	getFileNameFromCoords(x, z, name);
	mFileName.assign(name.str());

	boost::filesystem::path regions = mWorld.getWorldDirectory() / "regions";
	if (!boost::filesystem::exists(regions)) {
		boost::filesystem::create_directories(regions);
	}
	mFile = regions / mFileName;
};

ChunkPillar& WorldRegion::getChunkPillar(wCoord x, wCoord z)
{

}

void WorldRegion::save()
{
	
}

