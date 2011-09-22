
#include <cstdint>

#include "game/randomGen.h"
#include "game/world/World.h"
#include "terrain/noise/Noise2D.h"

#ifndef _TERRAINGENERATOR_H_
#define _TERRAINGENERATOR_H_

class Chunk;
class ChunkPillar;

class TerrainGenerator
{
public:
	TerrainGenerator(World& world);
	~TerrainGenerator();

	void setHeightMap(ChunkPillar& pillar, wCoord xPos, wCoord zPos);
	Chunk* generateChunk(ChunkPillar& pillar, wCoord xPos, wCoord yPos, wCoord zPos);

private:
	void generateHeightMap(ChunkPillar& pillar, wCoord xPos, wCoord zPos);
	Chunk* fillChunk(ChunkPillar& pillar, wCoord yPos);

private:
	World& mWorld;
	Noise2D* mMountainsNoise;
	Noise2D* mFlatNoise;
	Noise2D* mSelectorNoise;
};

#endif // #ifndef _TERRAINGENERATOR_H_
