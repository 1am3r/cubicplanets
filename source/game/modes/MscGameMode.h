
#include "game/input/InputReceiver.h"
#include "game/world/World.h"

#ifndef __MSCGAMEMODE_H_
#define __MSCGAMEMODE_H_

class InputHandler;
class McsHudGui;

class MscGameMode :
	public InputReceiver, 
	public Ogre::FrameListener
{
public:
	MscGameMode(Ogre::Root* ogreRoot, Ogre::RenderWindow* renderWindow, CEGUI::OgreRenderer* ceRenderer, InputHandler* input);
	virtual ~MscGameMode();

	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameStarted(const Ogre::FrameEvent &evt);
	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg) = 0;
	virtual bool keyReleased(const OIS::KeyEvent &arg) = 0;
	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg) = 0;
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) = 0;
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) = 0;


	virtual void init();
	virtual void start() = 0;

protected:
	void initCharacter();

protected:
	static const Ogre::Degree maxPitch;

	Ogre::Root*			mRoot;
	Ogre::RenderWindow* mWindow;
	CEGUI::OgreRenderer* mCeRenderer;
	InputHandler*		mInput;
	McsHudGui* mHud;

	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera*		mCamera;
	Ogre::SceneNode*	mCamNode;	// The SceneNode the camera is attached to

	Ogre::Real			mRotate;	// The rotate constant
	Ogre::Real			mMove;		// The movement constant
	Ogre::Vector3		mDirection;	// Value to move in the correct direction



	bool mDoJump;
	GameWorld::World* mLevel;
	btKinematicCharacterController* mCharacter;
	btPairCachingGhostObject* mGhostObject;

	bool mShutDown;
};

#endif // __MSCGAMEMODE_H_
