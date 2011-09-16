#include "stdafx.h"

#ifndef __INPUTHANDLER_H_
#define __INPUTHANDLER_H_

class InputReceiver;

class InputHandler : 
	public Ogre::WindowEventListener,
	public OIS::MouseListener , public OIS::KeyListener
{
public:
	InputHandler();
	~InputHandler();

	void init(Ogre::RenderWindow* renderWin);

	void addReceiver(InputReceiver* recv);
	void removeReceiver(InputReceiver* recv);

	void frameUpdate();

	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &evt);
	bool keyReleased(const OIS::KeyEvent &evt);
	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

private:
	// Ogre window
	Ogre::RenderWindow* mWindow;

	// OIS Input devices
	OIS::InputManager*	mInputManager;
	OIS::Mouse*			mMouse;
	OIS::Keyboard*		mKeyboard;

	// Current input receiver
	InputReceiver* mCurrentRecv;
};

#endif // __INPUTHANDLER_H_
