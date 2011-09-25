
#include <cstdint>

#include "game/randomGen.h"
#include "game/world/World.h"
#include "terrain/noise/Noise2D.h"

#ifndef _TERRAINGENERATOR_H_
#define _TERRAINGENERATOR_H_

namespace GameWorld {
	class ChunkBase;
	class ChunkPillar;
}

class TerrainGenerator
{
public:
	TerrainGenerator(GameWorld::World& world);
	~TerrainGenerator();

	void setHeightMap(GameWorld::ChunkPillar& pillar, wCoord xPos, wCoord zPos);
	void fillChunk(GameWorld::ChunkPillar& pillar, GameWorld::ChunkBase& chunk);

private:
	void generateHeightMap(GameWorld::ChunkPillar& pillar, wCoord xPos, wCoord zPos);
	
private:
	GameWorld::World& mWorld;
	Noise2D* mMountainsNoise;
	Noise2D* mFlatNoise;
	Noise2D* mSelectorNoise;
};

#endif // #ifndef _TERRAINGENERATOR_H_
