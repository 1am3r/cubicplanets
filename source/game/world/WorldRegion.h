
#include <string>
#include <iostream>
#include <cstdint>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "game/types.h"

#ifndef _WORLDREGION_H_
#define _WORLDREGION_H_

class World;
class ChunkPillar;
class ChunkStorage;

class WorldRegion
{
public:
	static const wCoord ChunkPillars = 32;
	static void getFileNameFromCoords(wCoord x, wCoord z, std::ostream& out)
	{
		out << "wr_" << boost::lexical_cast<std::string>(x) << "_" << boost::lexical_cast<std::string>(z) << ".wrc";
	};


public:
	WorldRegion(World& world, ChunkStorage& store, wCoord x, wCoord z);
	~WorldRegion() {};

	wCoord getXPos() const { return xPos; };
	wCoord getZPos() const { return zPos; };
	bool checkPosition(wCoord x, wCoord z) { return (x == xPos && z == zPos); };

	ChunkPillar& getChunkPillar(wCoord x, wCoord z);

	void save();
	void load();
private:
	World& mWorld;
	ChunkStorage& mStorage;
	wCoord xPos;
	wCoord zPos;

	std::string mFileName;
	boost::filesystem::path mFile;
	bool isLoaded;

private:
	struct WorldRegionFile
	{
		wCoord xPos;
		wCoord zPos;

		uint32_t pillarOffset[ChunkPillars * ChunkPillars];
	};
};


#endif // _WORLDREGION_H_
