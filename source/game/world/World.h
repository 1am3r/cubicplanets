
#include <cstdint>

#include <boost/filesystem.hpp>

#include "game/types.h"
#include "game/randomGen.h"
#include "game/world/Chunk.h"
#include "game/world/ChunkStorage.h"
#include "physics/BtOgreExtras.h"
#include "physics/BtOgreGP.h"
#include "physics/BtOgrePG.h"

#ifndef _WORLD_H_
#define _WORLD_H_

namespace GameWorld {

class World
{
public:
	RNGType chunkRng;

public:
	World(uint32_t seed, Ogre::SceneManager* sceneMgr);
	~World();

	void update(const Ogre::FrameEvent& evt);
	void UpdateCachedChunks(wCoord xDiff, wCoord yDiff, wCoord zDiff);

	void World::prepareRegion(wCoord x, wCoord y, wCoord z);

	Chunk* getChunk(wCoord xPos, wCoord yPos, wCoord zPos) { return mChunkStore->getChunk(xPos, yPos, zPos); };
	Chunk* loadChunk(wCoord xPos, wCoord yPos, wCoord zPos);
	void activateChunk(Chunk& curChunk);
	void activateChunk(wCoord xPos, wCoord yPos, wCoord zPos);
	void deactivateChunk(Chunk& curChunk);
	void deactivateChunk(wCoord xPos, wCoord yPos, wCoord zPos);
	void updateChunk(Chunk& curChunk);
	void updateChunk(wCoord xPos, wCoord yPos, wCoord zPos);

	Chunk* getCachedChunk(wCoord x, wCoord y, wCoord z);
	void setCachedChunk(wCoord x, wCoord y, wCoord z, Chunk* chunk);
	void updateCachedChunk(wCoord x, wCoord y, wCoord z);

	Ogre::SceneNode&	getChunkScene()		{ return *mChunksScene; };
	Ogre::SceneManager& getSceneMgr()		{ return *mSceneMgr; };
	btDynamicsWorld&	getPhysicsWorld()	{ return *mWorld; };

	void getCurrentPosition(wCoord& xPos, wCoord& yPos, wCoord& zPos)
	{
		xPos = mCurX;
		yPos = mCurY;
		zPos = mCurZ;
	};
	wCoord getDimension() { return mDimension; };
	void moveCurrentPosition(wCoord xDiff, wCoord yDiff, wCoord zDiff);
	void setCurrentPosition(wCoord x, wCoord y, wCoord z);
	void updatePlayerPosition(wCoord x, wCoord y, wCoord z);

	void setCubeType(Point3& position, uint8_t cubeType);
	void setCubeType(wCoord x, wCoord y, wCoord z, uint8_t cubeType);

	boost::filesystem::path getWorldDirectory() { return mWorldDirectory; };


	//HACK: Bullet test boxes
	void makeBox();

private:
	void makeCubeCoords(wCoord x, uint8_t& xLocal, wCoord y, uint8_t& yLocal, wCoord z, uint8_t& zLocal)
	{
		xLocal = x & (ChunkSizeX - 1);
		yLocal = y & (ChunkSizeY - 1);
		zLocal = z & (ChunkSizeZ - 1);
	};
	void makeChunkCoords(wCoord x, wCoord& xChunk, wCoord y, wCoord& yChunk, wCoord z, wCoord& zChunk)
	{
		xChunk = ((x < 0) ? (x - (ChunkSizeX - 1)) : x) / ChunkSizeX;
		yChunk = ((y < 0) ? (y - (ChunkSizeY - 1)) : y) / ChunkSizeY;
		zChunk = ((z < 0) ? (z - (ChunkSizeZ - 1)) : z) / ChunkSizeZ;
	};

	void registerChunkBody(Chunk& curChunk);
	void deregisterChunkBody(Chunk& curChunk);

protected:
	wCoord mRange;
	wCoord mDimension;
	wCoord mCurX;
	wCoord mCurY;
	wCoord mCurZ;

	Chunk** mChunks;

	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode* mChunksScene;

	ChunkStorage* mChunkStore;

	boost::filesystem::path mWorldDirectory;
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

};

#endif // #ifndef _WORLD_H_
