#include "stdafx.h"

#include "MscCreativeMode.h"

#include "game/input/InputHandler.h"
#include "game/world/World.h"
#include "game/world/Chunk.h"
#include "game/gui/McsHudGui.h"

MscCreativeMode::MscCreativeMode(Ogre::Root* ogreRoot, Ogre::RenderWindow* renderWindow, InputHandler* input) :
	MscGameMode(ogreRoot, renderWindow, input),
	mFreefly(false), mCubeSelected(false)
{
}


MscCreativeMode::~MscCreativeMode()
{
}



void MscCreativeMode::init()
{
	MscGameMode::init();
	
	// Set ambient light for testing
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	Ogre::MaterialPtr hBoxMat = Ogre::MaterialManager::getSingleton().create("hBoxWire", "Game");
	hBoxMat->createTechnique()->createPass();
	hBoxMat->setLightingEnabled(false);
	hBoxMat->setAmbient(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.0f));
	hBoxMat->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PM_WIREFRAME);

	mHighlightBox = mSceneMgr->createManualObject("highlightBox");
	mHighlightBox->begin("hBoxWire", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	mHighlightBox->position(-0.005f, -0.005f, -0.005f);
	mHighlightBox->position(-0.005f,  1.005f, -0.005f);
	mHighlightBox->position(-0.005f, -0.005f,  1.005f);
	mHighlightBox->position(-0.005f,  1.005f,  1.005f);

	mHighlightBox->position(1.005f, -0.005f, -0.005f);
	mHighlightBox->position(1.005f,  1.005f, -0.005f);
	mHighlightBox->position(1.005f, -0.005f,  1.005f);
	mHighlightBox->position(1.005f,  1.005f,  1.005f);

	mHighlightBox->quad(0, 2, 3, 1);
	mHighlightBox->quad(4, 0, 1, 5);
	mHighlightBox->quad(2, 6, 7, 3);
	mHighlightBox->quad(6, 4, 5, 7);
	mHighlightBox->quad(1, 3, 7, 5);
	mHighlightBox->quad(4, 6, 2, 0);

	mHighlightBox->end();
	mHighlightBox->setVisible(false);


	mHighlightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("highlightNode");
	mHighlightNode->attachObject(mHighlightBox);
}

void MscCreativeMode::start()
{
	mLevel = new World(23L, mSceneMgr);
	mLevel->prepareSpawnRegion();

	Chunk* startChunk = mLevel->getChunk(0, 0, 0);
	if (!startChunk){
		exit(0);
	}

	Ogre::Real startPos = static_cast<Ogre::Real> (startChunk->getHighestCube(0, 0));
	startPos += 10.5;

	mCamNode->setPosition(Ogre::Real(0.5), startPos, Ogre::Real(0.5));
	mCamNode->setOrientation(Ogre::Quaternion::IDENTITY);

	MscGameMode::initCharacter();

	mRoot->addFrameListener(this);
 	mRoot->startRendering();
	mRoot->removeFrameListener(this);
}



bool MscCreativeMode::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;
 
	if (mShutDown == true)
		return false;

	mInput->frameUpdate();

	if (mDoJump && mCharacter->canJump())
		mCharacter->jump();

	Ogre::Real speedFactor = 0.33f;
	if (mCharacter->onGround()) {
		speedFactor = 1.0f;
	}

	Ogre::Vector3 walk = mDirection * speedFactor;
	walk = mCamNode->getOrientation() * walk;
	btVector3 walkDirection(walk.x, walk.y, walk.z);

	mCharacter->setWalkDirection(walkDirection);
	//mCharacter->setWalkDirection(walkDirection*walkSpeed);

	//mCharacter->updateAction(mWorld->getBulletCollisionWorld(),evt.timeSinceLastFrame);

	mLevel->update(evt);

	//btTransform tr = mCharacter->getGhostObject()->getWorldTransform();
	//btVector3 before = tr.getOrigin();

	btTransform tr = mCharacter->getGhostObject()->getWorldTransform();
	btVector3 pos = tr.getOrigin();

//	walk = OgreBulletCollisions::BtOgreConverter::to(pos - before);
//	mCamNode->translate(walk, Ogre::Node::TS_LOCAL);
	if (!mFreefly) {
		mCamNode->setPosition(pos.x(), pos.y() + 0.75f, pos.z());
	} else {
		mCamNode->translate(mDirection * 100 * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	}

	//mLevel->updatePlayerPosition((int32_t) pos.x(), (int32_t) pos.y(),(int32_t) pos.z());
	Ogre::Vector3 camPos = mCamNode->getPosition();
	mLevel->updatePlayerPosition((int32_t) camPos.x, (int32_t) camPos.y, (int32_t) camPos.z);

	mHud->update();

	
	//Picking test
	btVector3 direction = BtOgre::Convert::toBullet(mCamera->getDerivedDirection().normalisedCopy());
	btVector3 position = BtOgre::Convert::toBullet(mCamera->getDerivedPosition() - 0.002f);
	direction *= Ogre::Real(6.0f);
	btCollisionWorld::ClosestRayResultCallback ray(position, position + direction);
	mLevel->mWorld->rayTest(position, position + direction, ray);

	if (ray.hasHit()) {
		float xRay = ray.m_hitPointWorld.x(), yRay = ray.m_hitPointWorld.y(), zRay = ray.m_hitPointWorld.z();
		float xRound = boost::math::round<float>(xRay);
		float yRound = boost::math::round<float>(yRay);
		float zRound = boost::math::round<float>(zRay);

		if (std::abs(xRay - xRound) < 0.001f) xRay = xRound;
		if (std::abs(yRay - yRound) < 0.001f) yRay = yRound;
		if (std::abs(zRay - zRound) < 0.001f) zRay = zRound;

		int32_t xPos = (int32_t) std::floor(xRay);
		int32_t yPos = (int32_t) std::floor(yRay);
		int32_t zPos = (int32_t) std::floor(zRay);

		xPos -= (int32_t) (ray.m_hitNormalWorld.x() + 1) / 2;
		yPos -= (int32_t) (ray.m_hitNormalWorld.y() + 1) / 2;
		zPos -= (int32_t) (ray.m_hitNormalWorld.z() + 1) / 2;

		std::ostringstream ostr;
		ostr <<  "X: " << xPos << " Y: " << yPos << " Z: " << zPos;

		mHud->setHitPos(ostr.str());
		
		mCurSelectedCube.x = xPos;
		mCurSelectedCube.y = yPos;
		mCurSelectedCube.z = zPos;
		mCurSelectedCubeFace.x = (int32_t) ray.m_hitNormalWorld.x();
		mCurSelectedCubeFace.y = (int32_t) ray.m_hitNormalWorld.y();
		mCurSelectedCubeFace.z = (int32_t) ray.m_hitNormalWorld.z();
		mCubeSelected = true;

		mHighlightNode->setPosition(Ogre::Real(xPos), Ogre::Real(yPos), Ogre::Real(zPos));
		if (!mHighlightBox->isVisible()) {
			mHighlightBox->setVisible(true);
			mHighlightNode->needUpdate();
		}
	} else {
		if (mHighlightBox->isVisible()) {
			mHud->setHitPos("No Hit!");
			mHighlightBox->setVisible(false);
			mCubeSelected = false;
		}
	}

	std::ostringstream ostr;
	ostr.precision(4);
	ostr <<  "X: " << camPos.x << " Y: " << camPos.y << " Z: " << camPos.z;
	mHud->setPos(ostr.str());

	//FPS
	mHud->setFps(mWindow->getStatistics());

	return true;
}

bool MscCreativeMode::mouseMoved(const OIS::MouseEvent &evt)
{	    
	mCamNode->yaw(Ogre::Degree(-mRotate * evt.state.X.rel), Ogre::Node::TS_WORLD);
    
	Ogre::Degree pitch = Ogre::Degree(-mRotate * evt.state.Y.rel);
	Ogre::Degree newPitch = Ogre::Degree(mCamera->getOrientation().getPitch() + Ogre::Radian(pitch));
	if ((newPitch > Ogre::Degree(maxPitch)) || (newPitch < Ogre::Degree(-maxPitch)))
	{
		pitch -= Ogre::Math::Sign(newPitch) * (Ogre::Math::Abs(newPitch) - Ogre::Degree(90));
	}
	mCamera->pitch(pitch);

	return true;
}

bool MscCreativeMode::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{    
	switch (id) {
	case OIS::MB_Left:
		if (mCubeSelected) {
			mLevel->setCubeType(mCurSelectedCube, 0);
		}
		break;

	case OIS::MB_Right:
		if (mCubeSelected) {
			mLevel->setCubeType(mCurSelectedCube + mCurSelectedCubeFace, 4);
		}
		break;
	}

	return true;
}
 
bool MscCreativeMode::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{    
	return true;
}
 
bool MscCreativeMode::keyPressed(const OIS::KeyEvent &evt)
{    
	switch (evt.key)
    {
    case OIS::KC_ESCAPE: 
        mShutDown = true;
        break;
    
	case OIS::KC_UP:
	case OIS::KC_W:
		mDirection.z = -mMove;
		break;
 
	case OIS::KC_DOWN:
	case OIS::KC_S:
		mDirection.z = mMove;
		break;
 
	case OIS::KC_LEFT:
	case OIS::KC_A:
		mDirection.x = -mMove;
		break;
 
	case OIS::KC_RIGHT:
	case OIS::KC_D:
		mDirection.x = mMove;
		break;
 
	case OIS::KC_PGDOWN:
	case OIS::KC_E:
		mDirection.y = -mMove;
		break;
 
	case OIS::KC_PGUP:
	case OIS::KC_Q:
		mDirection.y = mMove;
		break;

	case OIS::KC_SPACE:
		mDoJump = true;
		break;
	
	case OIS::KC_B:
		mLevel->makeBox();
		break;

	case OIS::KC_Z:
		if (mFreefly) {
			mCharacter->warp(BtOgre::Convert::toBullet(mCamera->getDerivedPosition()));
			mFreefly = false;
		}
		break;

	case OIS::KC_G:
		mFreefly = !mFreefly;
		break;

	case OIS::KC_T:
		mCharacter->warp(btVector3(0, 127.0f, 0));
		break;

	case OIS::KC_F3:
		{
			int debug = mLevel->mDebugDrawer->getDebugMode();
			mLevel->mDebugDrawer->setDebugMode(!debug);
		}
		break;

	default:
        break;
    }
    return true;
}
 
bool MscCreativeMode::keyReleased(const OIS::KeyEvent &evt)
{    
	switch (evt.key)
	{
	case OIS::KC_UP:
	case OIS::KC_W:
		mDirection.z = 0;
		break;
 
	case OIS::KC_DOWN:
	case OIS::KC_S:
		mDirection.z = 0;
		break;
 
	case OIS::KC_LEFT:
	case OIS::KC_A:
		mDirection.x = 0;
		break;
 
	case OIS::KC_RIGHT:
	case OIS::KC_D:
		mDirection.x = 0;
		break;
 
	case OIS::KC_PGDOWN:
	case OIS::KC_E:
		mDirection.y = 0;
		break;
 
	case OIS::KC_PGUP:
	case OIS::KC_Q:
		mDirection.y = 0;
		break;
 	
	case OIS::KC_SPACE:
		mDoJump = false;
		break;

	default:
		break;
	}
	return true;

	return true;
}
