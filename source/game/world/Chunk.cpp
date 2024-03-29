#include "stdafx.h"

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"

namespace GameWorld {

std::array<uint8_t, ChunkSizeX * ChunkSizeY * ChunkSizeZ> Chunk::emptyBlocks;
bool Chunk::emptyBlocksFilled = false;

Chunk::Chunk(wCoord xPos, wCoord yPos, wCoord zPos)
	: mX(xPos), mY(yPos), mZ(zPos)
{
	std::ostringstream chunkNameStream;
	chunkNameStream << "chunk" << mX << "_" << mY << "_" << mZ;
	mChunkName.assign(chunkNameStream.str());

	if (!emptyBlocksFilled) {
		emptyBlocksFilled = true;
		emptyBlocks.fill(0);
	}
}

};
