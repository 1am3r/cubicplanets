#include "stdafx.h"
#include "MscGameMode.h"

#include "game/world/World.h"
#include "game/world/Chunk.h"
#include "game/gui/McsHudGui.h"

const Ogre::Degree MscGameMode::maxPitch = Ogre::Degree(90);

MscGameMode::MscGameMode(Ogre::Root* ogreRoot, Ogre::RenderWindow* renderWindow, InputHandler* input) :
	mRoot(ogreRoot), mWindow(renderWindow), mInput(input),
	mRotate(Ogre::Real(0.13)), mMove(Ogre::Real(0.15)), mDirection(Ogre::Vector3::ZERO),
	mLevel(0), mHud(0),
	mDoJump(false), mShutDown(false)
{

}


MscGameMode::~MscGameMode()
{
	if (mLevel) {
		delete mLevel;
	}

	if (mHud) {
		delete mHud;
	}
}


void MscGameMode::init()
{
	// Create the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");

	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setNearClipDistance(Ogre::Real(0.1f));
    mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode");
   	mCamNode->attachObject(mCamera);

	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
 
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	mHud = new McsHudGui();
	mHud->init();
}

void MscGameMode::initCharacter()
{
	btTransform startTransform;
	startTransform.setIdentity ();
	startTransform.setOrigin (btVector3(mCamNode->getPosition().x,mCamNode->getPosition().y,mCamNode->getPosition().z));
	mGhostObject = new btPairCachingGhostObject();
	mGhostObject->setWorldTransform(startTransform);

	mLevel->mWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	Ogre::Vector3 size = Ogre::Vector3(0.5f, 1.6f, 0.8f) * 0.5f * 0.96f;

	btConvexShape* cylinder = new btCylinderShape(btVector3(size.x, size.y, size.z));
	mGhostObject->setCollisionShape (cylinder);
	mGhostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.35);
	mCharacter = new btKinematicCharacterController (mGhostObject,cylinder,stepHeight);

	mLevel->mWorld->addCollisionObject(
		mGhostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
	mLevel->mWorld->addAction(mCharacter);
	mLevel->mWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(
		mGhostObject->getBroadphaseHandle(),mLevel->mWorld->getDispatcher());

	mCharacter->reset();
	mCharacter->warp(btVector3(mCamNode->getPosition().x,mCamNode->getPosition().y,mCamNode->getPosition().z));

	//setting some parameters
	mCharacter->setMaxJumpHeight(1.8f);
}

bool MscGameMode::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return true;
}

bool MscGameMode::frameStarted(const Ogre::FrameEvent &evt)
{
   mLevel->mDebugDrawer->step();

   return true;
}

