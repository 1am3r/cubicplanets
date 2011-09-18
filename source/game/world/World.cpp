#include "stdafx.h"

#include "physics/BtOgreExtras.h"
#include "physics/BtOgreGP.h"
#include "physics/BtOgrePG.h"

#include "game/world/World.h"
#include "game/world/Chunk.h"
#include "game/world/ChunkStorage.h"


World::World(uint32_t seed, Ogre::SceneManager* sceneMgr)
	: mChunks(0), mCurX(0), mCurY(0), mCurZ(0), mChunkStore(0),
	  mWorld(0), mWorldDirectory(boost::filesystem::initial_path()),
	  mDebugDrawer(0), mSolver(0), mDispatcher(0), mCollisionConfig(0), mBroadphase(0)
{
	mSceneMgr = sceneMgr;
	mChunksScene = sceneMgr->getRootSceneNode()->createChildSceneNode("chunksScene");

	chunkRng.seed(seed);

	mChunkStore = new ChunkStorage(*this);

	mRange = 4;
	mDimension = (mRange * 2 + 1);

	mChunks = new Chunk*[mDimension * mDimension * mDimension];
	memset(mChunks, 0, sizeof(Chunk*) * mDimension * mDimension * mDimension);

	

	//Bullet initialisation.
	mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 4096);
	mCollisionConfig = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfig);
	mSolver = new btSequentialImpulseConstraintSolver();
	
	// Start Bullet
	mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
	mWorld->setGravity(btVector3(0, (btScalar) -9.8f, 0));

	mDebugDrawer = new BtOgre::DebugDrawer(mSceneMgr->getRootSceneNode(), mWorld);
	mWorld->setDebugDrawer(mDebugDrawer);
	mDebugDrawer->setDebugMode(0);

	mNumEntitiesInstanced = 0; // how many shapes are created
}


World::~World(void)
{
	if (mChunkStore) {
		delete mChunkStore;
	}

	if (mChunks) {
		delete mChunks;
	}

	// OgreBullet physic delete - RigidBodies
	std::deque<btRigidBody *>::iterator itBody = mBodies.begin();
	while (mBodies.end() != itBody)
	{   
		mWorld->removeRigidBody(*itBody);
		delete *itBody;
		++itBody;
	}   
	mBodies.clear();

	if (mWorld) {
		mWorld->setDebugDrawer(0);
		delete mWorld;
	}

	if (mDebugDrawer) {
		delete mDebugDrawer;
	}
	if (mSolver) {
		delete mSolver;
	}
	if (mDispatcher) {
		delete mDispatcher;
	}
	if (mCollisionConfig) {
		delete mCollisionConfig;
	}
	if (mBroadphase) {
		delete mBroadphase;
	}
}


void World::prepareSpawnRegion()
{
	mCurX = 0;
	mCurY = 0;
	mCurZ = 0;

	for (int16_t x = 0; x < mDimension; x++)
	{
		for (int16_t z = 0; z < mDimension; z++)
		{
			for (int16_t y = 0; y < mDimension; y++)
			{
				Chunk* newChunk = getChunk(x + mCurX, y + mCurY, z + mCurZ);
				mChunks[(x * mDimension + z) * mDimension + y] = newChunk;
			}
		}
	}

	for (int32_t xIt = mCurX; xIt < mCurX + mDimension; xIt++)
	{
		for (int32_t zIt = mCurZ; zIt < mCurZ + mDimension; zIt++)
		{
			for (int32_t yIt = mCurY; yIt < mCurY + mDimension; yIt++)
			{
				Chunk* curChunk = getChunk(xIt, yIt, zIt);
				curChunk->activateEntity();
				curChunk->activatePhysicsBody();
			}
		}
	}
}


void World::updateChunk(Chunk& curChunk)
{
	curChunk.setModified();
	curChunk.update();
}

void World::updateChunk(int32_t xPos, int32_t yPos, int32_t zPos)
{
	updateChunk(*getChunk(xPos, yPos, zPos));
}


void World::setCubeType(Point3& position, uint8_t cubeType)
{
	setCubeType(position.x, position.y, position.z, cubeType);
}

void World::setCubeType(int32_t x, int32_t y, int32_t z, uint8_t cubeType)
{
	uint8_t xCube, yCube, zCube;
	makeCubeCoords(x, xCube, y, yCube, z, zCube);

	int32_t xChunk, yChunk, zChunk;
	makeChunkCoords(x, xChunk, y, yChunk, z, zChunk);

	Chunk * curChunk = getChunk(xChunk, yChunk, zChunk);
	curChunk->setCubeType(xCube, yCube, zCube, cubeType);

	if (xCube == Chunk::ChunkSizeX - 1) updateChunk(xChunk + 1, yChunk    , zChunk    );
	if (xCube == 0)						updateChunk(xChunk - 1, yChunk    , zChunk    );
	if (yCube == Chunk::ChunkSizeY - 1) updateChunk(xChunk    , yChunk + 1, zChunk    );
	if (yCube == 0)						updateChunk(xChunk    , yChunk - 1, zChunk    );
	if (zCube == Chunk::ChunkSizeZ - 1) updateChunk(xChunk    , yChunk    , zChunk + 1);
	if (zCube == 0)						updateChunk(xChunk    , yChunk    , zChunk - 1);
}


void World::moveCurrentPosition(int16_t xDiff, int16_t yDiff, int16_t zDiff)
{
	uint16_t range = mRange;
	
	Chunk** tempArray = new Chunk*[mDimension * mDimension * mDimension];

	int32_t xNew = mCurX + xDiff;
	int32_t yNew = mCurY + yDiff;
	int32_t zNew = mCurZ + zDiff;

	for (int16_t x = 0; x < mDimension; x++)
	{
		for (int16_t z = 0; z < mDimension; z++)
		{
			for (int16_t y = 0; y < mDimension; y++)
			{
				if (((x + mCurX - xNew) < 0) || ((x + mCurX - xNew) >= mDimension) ||
					((z + mCurZ - zNew) < 0) || ((z + mCurZ - zNew) >= mDimension) ||
					((y + mCurY - yNew) < 0) || ((y + mCurY - yNew) >= mDimension))
				{
					Chunk* curChunk = mChunks[(x * mDimension + z) * mDimension + y];
					if (curChunk != 0) curChunk->deactivateEntity();
					mChunks[(x * mDimension + z) * mDimension + y] = 0;
				}

				Chunk* newChunk = getChunk(x + xNew, y + yNew, z + zNew);
				tempArray[(x * mDimension + z) * mDimension + y] = newChunk;
			}
		}
	}

	memcpy(mChunks, tempArray, sizeof(Chunk*) * mDimension * mDimension * mDimension);
	delete tempArray;

	mCurX = xNew;
	mCurY = yNew;
	mCurZ = zNew;

	for (int16_t x = 0; x < mDimension; x++)
	{
		for (int16_t z = 0; z < mDimension; z++)
		{
			for (int16_t y = 0; y < mDimension; y++)
			{
				Chunk* newChunk = mChunks[(x * mDimension + z) * mDimension + y];
				if (!newChunk->isChunkActive()) {
					newChunk->activateEntity();
					newChunk->activatePhysicsBody();
				}
			}
		}
	}
}

void World::setCurrentPosition(int16_t x, int16_t y, int16_t z)
{
	mCurX = x;
	mCurY = y;
	mCurZ = z;

	moveCurrentPosition(-mRange, -mRange, -mRange);
}

void World::updatePlayerPosition(int16_t x, int16_t y, int16_t z)
{
	int32_t xNew, yNew, zNew;

	makeChunkCoords(x, xNew, y, yNew, z, zNew);

	xNew -= (mCurX + mRange);
	yNew -= (mCurY + mRange);
	zNew -= (mCurZ + mRange);

	if ((xNew != 0) || (yNew != 0) || (zNew != 0)) {
		moveCurrentPosition(xNew, yNew, zNew);
	}
}

void World::update(const Ogre::FrameEvent& evt)
{
	mWorld->stepSimulation(evt.timeSinceLastFrame);
	mChunkStore->update(evt);
}

btBoxShape *sceneBoxShape = 0;
void World::makeBox()
{
	Ogre::Camera* mCamera = mSceneMgr->getCamera("PlayerCam");
	Ogre::Vector3 size = Ogre::Vector3::ZERO;   // size of the box
	// starting position of the box
	Ogre::Vector3 position = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 3);
	// create an ordinary, Ogre mesh with texture
	Ogre::Entity *entity = mSceneMgr->createEntity(
		"Box" + Ogre::StringConverter::toString(mNumEntitiesInstanced),
		"cube.mesh"); 
	mNumEntitiesInstanced++;
	entity->setCastShadows(true);
	// we need the bounding box of the box to be able to set the size of the Bullet-box
	Ogre::AxisAlignedBox boundingB = entity->getBoundingBox();
	size = boundingB.getSize(); size /= 2.0f; // only the half needed
	size *= 0.96f;   // Bullet margin is a bit bigger so we need a smaller size
					// (Bullet 2.76 Physics SDK Manual page 18)
	entity->setMaterialName("Examples/BumpyMetal");
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);
	node->scale(0.01f, 0.01f, 0.01f);   // the cube is too big for us
	node->setPosition(position);
	size *= 0.01f;                  // don't forget to scale down the Bullet-box too
	// after that create the Bullet shape with the calculated size
	if (sceneBoxShape == 0) {
		sceneBoxShape = new btBoxShape(BtOgre::Convert::toBullet(size));
	}
	
	btVector3 localInertia(0, 0, 0);
	sceneBoxShape->calculateLocalInertia(0.3f, localInertia);

	BtOgre::RigidBodyState *mState = new BtOgre::RigidBodyState(node);
	// and the Bullet rigid body
	btRigidBody* defaultBody = new btRigidBody(0.3f, mState, sceneBoxShape, localInertia);

	mWorld->addRigidBody(defaultBody);

	defaultBody->setLinearVelocity(
			BtOgre::Convert::toBullet(mCamera->getDerivedDirection().normalisedCopy() * 10.0f)); // shooting speed
	
	// push the created objects to the deques
	mBodies.push_back(defaultBody);
}

