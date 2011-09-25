
#include "game/types.h"

#include "MscGameMode.h"

#ifndef _MSCCREATIVEMODE_H_
#define _MSCCREATIVEMODE_H_

class MscCreativeMode :
	public MscGameMode
{
public:
	MscCreativeMode(Ogre::Root* ogreRoot, Ogre::RenderWindow* renderWindow, CEGUI::OgreRenderer* ceRenderer, InputHandler* input);
	~MscCreativeMode();

	// Ogre::FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	void init();
	void start();

private:
	bool mFreefly;

	Ogre::SceneNode* mHighlightNode;
	Ogre::ManualObject* mHighlightBox;
	bool mCubeSelected;
	Point3 mCurSelectedCube;
	Point3 mCurSelectedCubeFace;
};

#endif // _MSCCREATIVEMODE_H_
