#include "stdafx.h"
#include "TerrainGenerator.h"

#include "game/world/World.h"
#include "game/world/Chunk.h"
#include "game/world/ChunkPillar.h"

#include "terrain/noise/SimplexNoise2D.h"
#include "terrain/noise/RidgedMultifractalNoise2D.h"
#include "terrain/noise/BillowNoise2D.h"

TerrainGenerator::TerrainGenerator(World& world)
	: mWorld(world),
	mSelectorNoise(0),
	mMountainsNoise(0),
	mFlatNoise(0)
{
	mSelectorNoise = new SimplexNoise2D(world.chunkRng);
	mMountainsNoise = new RidgedMultifractalNoise2D(world.chunkRng);
	mFlatNoise = new BillowNoise2D(world.chunkRng);
};

TerrainGenerator::~TerrainGenerator()
{
	if (mSelectorNoise) {
		delete mSelectorNoise;
	}

	if (mMountainsNoise) {
		delete mMountainsNoise;
	}

	if (mFlatNoise) {
		delete mFlatNoise;
	}
}


Chunk* TerrainGenerator::generateChunk(ChunkPillar* pillar, wCoord xPos, wCoord yPos, wCoord zPos)
{
	if (!pillar->heightMapSet) {
		generateHeightMap(pillar, xPos, zPos);
	}

	Chunk* newChunk = fillChunk(pillar, yPos);

	return newChunk;
}

void TerrainGenerator::generateHeightMap(ChunkPillar* pillar, wCoord xPos, wCoord zPos)
{
	//const uint16_t scale = (Chunk::ChunkSizeY * 6) / 13;
	const uint16_t scale = 128;

	wCoord maxY = WCOORD_MIN;
	wCoord minY = WCOORD_MAX;
	
	const double lowBorder  = -0.125;
	const double highBorder =  0.125;

	const double inputDiv = 300.0;
	const double flatDiv = 1.0;
	const double flatScale = 0.125;
	const double flatBias = 0.75;
	const double mountainDiv = 1.38;
	const double selectorDiv = 2.54;

	for (int xi = 0; xi < Chunk::ChunkSizeX; xi++)
	{
		for (int zi = 0; zi < Chunk::ChunkSizeZ; zi++)
		{
			double xin = ((xi + xPos * 16) / inputDiv);
			double zin = ((zi + zPos * 16) / inputDiv);

			double ground;
			double select = mSelectorNoise->noise(xin / selectorDiv, zin / selectorDiv, 0);

			if (select < lowBorder) {
				ground = mFlatNoise->noise(xin / flatDiv, zin / flatDiv, 0);
				ground = (ground * 0.125) - 0.75;
			} else if (select > highBorder) {
				ground = mMountainsNoise->noise(xin / mountainDiv, zin / mountainDiv, 0);
			} else {
				double flat = (mFlatNoise->noise(xin / flatDiv, zin / flatDiv, 0) * flatScale) - flatBias;
				double mountain = mMountainsNoise->noise(xin / mountainDiv, zin / mountainDiv, 0);

				double point = (select - lowBorder) / (highBorder - lowBorder);
				double curve = (point * point * (3.0 - 2.0 * point));
				ground = ((1.0 - curve) * flat) + (curve * mountain);
			}

			wCoord height = (wCoord) (ground * scale); // + (Chunk::ChunkSizeY / 2);
			
			if (height > maxY) maxY = height;
			if (height < minY) minY = height;

			pillar->heightMap[xi][zi] = height;
		}
	}

	pillar->maxY = maxY;
	pillar->minY = minY;
	pillar->heightMapSet = true;
}

Chunk* TerrainGenerator::fillChunk(ChunkPillar* pillar, wCoord yPos)
{
	Chunk* newChunk = new Chunk(mWorld, pillar->x, yPos, pillar->z);
	uint16_t heighestCube = 0;

	if (pillar->maxY < yPos * Chunk::ChunkSizeY) {
		memset(newChunk->blocks, 0, sizeof(newChunk->blocks));
	} else if (pillar->minY >= (yPos + 1) * Chunk::ChunkSizeY) {
		heighestCube = Chunk::ChunkSizeY - 1;
		memset(newChunk->blocks, 3, sizeof(newChunk->blocks));
	} else {
		heighestCube = pillar->maxY - (yPos * Chunk::ChunkSizeY);
		if (heighestCube < 0) heighestCube = 0;
		if (heighestCube >= Chunk::ChunkSizeY) heighestCube = Chunk::ChunkSizeY - 1;

		for (int xi = 0; xi < Chunk::ChunkSizeX; xi++) {
			for (int zi = 0; zi < Chunk::ChunkSizeZ; zi++) {
				wCoord height = pillar->heightMap[xi][zi];
				height -= yPos * Chunk::ChunkSizeY;

				if (height > 0) {
					if (height >= Chunk::ChunkSizeY) height = Chunk::ChunkSizeY - 1;

					for (int yi = 0; yi <= height; yi++) {
						newChunk->blocks[xi][zi][yi] = 3;
					}

					newChunk->blocks[xi][zi][height] = 2;
				}
			}
		}
	}

	newChunk->heighestCube = heighestCube;
	return newChunk;
}

