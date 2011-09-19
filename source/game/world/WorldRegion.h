
#include <string>
#include <iostream>
#include <cstdint>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/lexical_cast.hpp>

#include "game/types.h"

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

	static void getFileNameFromCoords(wCoord x, wCoord z, std::ostream& out) { out << "wr_" << boost::lexical_cast<std::string>(x) << "_" << boost::lexical_cast<std::string>(z) << ".wrc"; };
	static WorldRegion* loadFromDisk(World& world, ChunkStorage& store, wCoord x, wCoord z);

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
	static size_t getPillarIndex(wCoord x, wCoord z) { return ((positiveMod(x, ChunkPillars) * ChunkPillars) + positiveMod(z, ChunkPillars)); };

private:
	World& mWorld;
	ChunkStorage& mStorage;
	wCoord xPos;
	wCoord zPos;

	ChunkPillar* mPillars[ChunkPillarsSize];

	bfs::path mFilePath;
	bfs::fstream mFile;

	bool isLoaded;

public:
	struct WorldRegionFile
	{
		wCoord xPos;
		wCoord zPos;

		uint32_t pillarOffsets[ChunkPillarsSize];
	};
};

static inline bfs::fstream& operator<< (bfs::fstream& ofs, WorldRegion::WorldRegionFile& rhs)
{
	ofs.write(reinterpret_cast<char*>(&rhs.xPos), sizeof(rhs.xPos));
	ofs.write(reinterpret_cast<char*>(&rhs.zPos), sizeof(rhs.zPos));
	ofs.write(reinterpret_cast<char*>(rhs.pillarOffsets), sizeof(rhs.pillarOffsets));
	return ofs;
};

static inline bio::filtering_ostream& operator<< (bio::filtering_ostream& ofs, WorldRegion::WorldRegionFile& rhs)
{
	ofs.write(reinterpret_cast<char*>(&rhs.xPos), sizeof(rhs.xPos));
	ofs.write(reinterpret_cast<char*>(&rhs.zPos), sizeof(rhs.zPos));
	ofs.write(reinterpret_cast<char*>(rhs.pillarOffsets), sizeof(rhs.pillarOffsets));
	return ofs;
};

#endif // _WORLDREGION_H_
