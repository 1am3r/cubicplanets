
#include <cstdint>

#include "game/randomGen.h"
#include "game/world/World.h"
#include "terrain/noise/Noise2D.h"

#ifndef _TERRAINGENERATOR_H_
#define _TERRAINGENERATOR_H_

class GameWorld::Chunk;
class GameWorld::ChunkPillar;

class TerrainGenerator
{
public:
	TerrainGenerator(GameWorld::World& world);
	~TerrainGenerator();

	void setHeightMap(GameWorld::ChunkPillar& pillar, wCoord xPos, wCoord zPos);
	GameWorld::Chunk* generateChunk(GameWorld::ChunkPillar& pillar, wCoord xPos, wCoord yPos, wCoord zPos);

private:
	void generateHeightMap(GameWorld::ChunkPillar& pillar, wCoord xPos, wCoord zPos);
	GameWorld::Chunk* fillChunk(GameWorld::ChunkPillar& pillar, wCoord yPos);

private:
	GameWorld::World& mWorld;
	Noise2D* mMountainsNoise;
	Noise2D* mFlatNoise;
	Noise2D* mSelectorNoise;
};

#endif // #ifndef _TERRAINGENERATOR_H_
