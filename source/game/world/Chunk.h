#include <cstdint>
#include <iostream>
#include <array>

#include <OGRE/Ogre.h>

#include "game/types.h"
#include "game/world/WorldParams.h"

#ifndef _CHUNK_H_
#define _CHUNK_H_

namespace GameWorld {

class World;
class ChunkStorage;

class Chunk
{
	// Attributes
public:
	wCoord mX, mY, mZ;
	friend class ChunkBase;
	friend class ChunkPillar;
	
	// Methods
public:

	// Saving
	virtual bool needsSaving() { return false; };
	virtual bool saveToStream(std::ostream& data) { return false; };

	// update after block modification
	virtual void update(bool force) {};

	virtual bool isChunkActive() { return false; };
	virtual void activateChunk() {};
	virtual void deactivateChunk() {};

	virtual bool isEmptyChunk() { return true; };

	virtual uint8_t getCubeTypeLocal(uint8_t x, uint8_t y, uint8_t z) { return 0; };
	virtual uint8_t getCubeDataLocal(uint8_t x, uint8_t y, uint8_t z) { return 0; };

protected:
	Chunk(wCoord xPos, wCoord yPos, wCoord zPos);
	virtual ~Chunk() {};

	virtual uint8_t* getCubeArray() { return 0; };

	Ogre::String& getChunkName(){ return mChunkName; };
	uint32_t getHighestCube(uint8_t x, uint8_t z) { return 0; };

	void fillBlocks(uint8_t cubeType) {};
	void setEmpty() {};

	void setHighestCube(uint16_t height) {};

protected:
	Ogre::String				mChunkName;
};

};

#endif // #ifndef _CHUNK_H_
