#include "stdafx.h"

#include "Chunk.h"
#include "ChunkStorage.h"

#ifndef _WORLD_H_
#define _WORLD_H_

class World
{
public:
	RNGType chunkRng;

public:
	World(uint32_t seed, Ogre::SceneManager* sceneMgr);
	~World();

	void update(const Ogre::FrameEvent& evt);

	void prepareSpawnRegion();

	Chunk* getChunk(wCoord xPos, wCoord yPos, wCoord zPos) { return mChunkStore->getChunk(xPos, yPos, zPos); };
	Chunk* loadChunk(int32_t xPos, int32_t yPos, int32_t zPos);
	void activateChunk(Chunk& curChunk);
	void activateChunk(int32_t xPos, int32_t yPos, int32_t zPos);
	void deactivateChunk(Chunk& curChunk);
	void deactivateChunk(int32_t xPos, int32_t yPos, int32_t zPos);
	void updateChunk(Chunk& curChunk);
	void updateChunk(int32_t xPos, int32_t yPos, int32_t zPos);


	Ogre::SceneNode&	getChunkScene()		{ return *mChunksScene; };
	Ogre::SceneManager& getSceneMgr()		{ return *mSceneMgr; };
	btDynamicsWorld&	getPhysicsWorld()	{ return *mWorld; };

	void getCurrentPosition(int32_t& xPos, int32_t& yPos, int32_t& zPos)
	{
		xPos = mCurX;
		yPos = mCurY;
		zPos = mCurZ;
	};
	uint16_t getDimension() { return mDimension; };
	void moveCurrentPosition(int16_t xDiff, int16_t yDiff, int16_t zDiff);
	void setCurrentPosition(int16_t x, int16_t y, int16_t z);
	void updatePlayerPosition(int16_t x, int16_t y, int16_t z);

	void setCubeType(Point3& position, uint8_t cubeType);
	void setCubeType(int32_t x, int32_t y, int32_t z, uint8_t cubeType);

	void makeBox();

private:
	void makeCubeCoords(int32_t x, uint8_t& xLocal, int32_t y, uint8_t& yLocal, int32_t z, uint8_t& zLocal)
	{
		xLocal = x & (Chunk::ChunkSizeX - 1);
		yLocal = y & (Chunk::ChunkSizeY - 1);
		zLocal = z & (Chunk::ChunkSizeZ - 1);
	};
	void makeChunkCoords(int32_t x, int32_t& xChunk, int32_t y, int32_t& yChunk, int32_t z, int32_t& zChunk)
	{
		xChunk = ((x < 0) ? (x - (Chunk::ChunkSizeX - 1)) : x) / Chunk::ChunkSizeX;
		yChunk = ((y < 0) ? (y - (Chunk::ChunkSizeY - 1)) : y) / Chunk::ChunkSizeY;
		zChunk = ((z < 0) ? (z - (Chunk::ChunkSizeZ - 1)) : z) / Chunk::ChunkSizeZ;
	};

	void registerChunkBody(Chunk& curChunk);
	void deregisterChunkBody(Chunk& curChunk);

protected:
	uint16_t mRange;
	uint16_t mDimension;
	int32_t mCurX;
	int32_t mCurY;
	int32_t mCurZ;

	Chunk** mChunks;

	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode* mChunksScene;

	ChunkStorage* mChunkStore;
public:
	// Bullet physics stuff
	btAxisSweep3*							mBroadphase;
	btDefaultCollisionConfiguration*		mCollisionConfig;
	btCollisionDispatcher*					mDispatcher;
	btSequentialImpulseConstraintSolver*	mSolver;
	btDynamicsWorld*						mWorld;
	BtOgre::DebugDrawer*					mDebugDrawer;
	
	std::deque<btRigidBody*>				mBodies;
	int mNumEntitiesInstanced;
};

#endif // #ifndef _WORLD_H_
