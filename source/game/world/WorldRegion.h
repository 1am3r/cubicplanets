
#include <string>
#include <cstdint>

#include "game/types.h"

#ifndef _WORLDREGION_H_
#define _WORLDREGION_H_

class World;
class ChunkPillar;

class WorldRegion
{
public:
	static const uint8_t ChunkPillarsX = 16;
	static const uint8_t ChunkPillarsZ = 16;

public:
	WorldRegion(World& world) : mWorld(world) {};
	~WorldRegion();


	int32_t xPos;
	int32_t zPos;

	ChunkPillar& getChunkPillar(uint8_t x, uint8_t z);

	void save();

private:
	World& mWorld;

	std::string mFileName;

private:
	struct WorldRegionFile
	{

	};
};


#endif // _WORLDREGION_H_