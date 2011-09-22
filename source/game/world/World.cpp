#include "stdafx.h"

#include <cmath>

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

	mRange = 2;
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


void World::UpdateCachedChunks(wCoord xDiff, wCoord yDiff, wCoord zDiff)
{
	for (wCoord x = abs(xDiff); x > 0; x--) {
		for (wCoord z = 0; z < mDimension; z++) {
			for (wCoord y = 0; y < mDimension; y++) {
				wCoord xRes = mCurX + sign(xDiff) * (mRange + x);
				wCoord yRes = mCurY + y - mRange;
				wCoord zRes = mCurZ + z - mRange;
				updateCachedChunk(xRes, yRes, zRes);
			}
		}
	}

	for (wCoord z = abs(zDiff); z > 0; z--) {
		for (wCoord x = 0; x < mDimension; x++) {
			for (wCoord y = 0; y < mDimension; y++) {
				wCoord zRes = mCurZ + sign(zDiff) * (mRange + z);
				wCoord xRes = mCurX + x - mRange;
				wCoord yRes = mCurY + y - mRange;
				updateCachedChunk(xRes, yRes, zRes);
			}
		}
	}

	for (wCoord y = abs(yDiff); y > 0; y--) {
		for (wCoord x = 0; x < mDimension; x++) {
			for (wCoord z = 0; z < mDimension; z++) {
				wCoord yRes = mCurY + sign(yDiff) * (mRange + y);
				wCoord xRes = mCurX + x - mRange;
				wCoord zRes = mCurZ + z - mRange;
				updateCachedChunk(xRes, yRes, zRes);
			}
		}
	}
}

void World::updateCachedChunk(wCoord x, wCoord y, wCoord z)
{
	Chunk* curChunk = getCachedChunk(x, y, z);
	if (curChunk != 0 && curChunk->isChunkActive()) {
		curChunk->deactivateEntity();
	}
	curChunk = getChunk(x, y, z);
	setCachedChunk(x, y, z, curChunk);
	curChunk->activateEntity();
	curChunk->activatePhysicsBody();
}

Chunk* World::getCachedChunk(wCoord x, wCoord y, wCoord z)
{
	return mChunks[(positiveMod(x, mDimension) * mDimension + positiveMod(z, mDimension)) * mDimension + positiveMod(y, mDimension)];
}

void World::setCachedChunk(wCoord x, wCoord y, wCoord z, Chunk* chunk)
{
	mChunks[(positiveMod(x, mDimension) * mDimension + positiveMod(z, mDimension)) * mDimension + positiveMod(y, mDimension)] = chunk;
}

void World::prepareRegion(wCoord x, wCoord y, wCoord z)
{
	mCurX = x; mCurY = y; mCurZ = z;

	for (wCoord x = -mRange; x <= mRange; x++) {
		for (wCoord z = -mRange; z <= mRange; z++) {
			for (wCoord y = -mRange; y <= mRange; y++) {
				wCoord xRes = mCurX + x;
				wCoord yRes = mCurY + y;
				wCoord zRes = mCurZ + z;
				updateCachedChunk(xRes, yRes, zRes);
			}
		}
	}
}


void World::updateChunk(Chunk& curChunk)
{
	curChunk.setModified();
	curChunk.update();
}

void World::updateChunk(wCoord xPos, wCoord yPos, wCoord zPos)
{
	updateChunk(*getChunk(xPos, yPos, zPos));
}


void World::setCubeType(Point3& position, uint8_t cubeType)
{
	setCubeType(position.x, position.y, position.z, cubeType);
}

void World::setCubeType(wCoord x, wCoord y, wCoord z, uint8_t cubeType)
{
	uint8_t xCube, yCube, zCube;
	makeCubeCoords(x, xCube, y, yCube, z, zCube);

	wCoord xChunk, yChunk, zChunk;
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


void World::moveCurrentPosition(wCoord xDiff, wCoord yDiff, wCoord zDiff)
{
	UpdateCachedChunks(xDiff, yDiff, zDiff);
	mCurX += xDiff;
	mCurY += yDiff;
	mCurZ += zDiff;
}

void World::setCurrentPosition(wCoord x, wCoord y, wCoord z)
{
	prepareRegion(x, y, z);
}

void World::updatePlayerPosition(wCoord x, wCoord y, wCoord z)
{
	wCoord xNew, yNew, zNew;

	makeChunkCoords(x, xNew, y, yNew, z, zNew);

	xNew -= mCurX;
	yNew -= mCurY;
	zNew -= mCurZ;

	if ((xNew != 0) || (yNew != 0) || (zNew != 0)) {
		moveCurrentPosition(xNew, yNew, zNew);
	}
}

void World::update(const Ogre::FrameEvent& evt)
{
	mWorld->stepSimulation(evt.timeSinceLastFrame);
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

