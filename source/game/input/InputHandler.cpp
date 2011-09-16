#include "stdafx.h"
#include "InputHandler.h"

#include "InputReceiver.h"

InputHandler::InputHandler() :
	mInputManager(0), mMouse(0), mKeyboard(0),
	mCurrentRecv(0)
{

}

InputHandler::~InputHandler()
{
	if (mWindow != NULL) {
		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
		windowClosed(mWindow);
	}
}



void InputHandler::init(Ogre::RenderWindow* renderWin)
{
	mWindow = renderWin;

	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing InputHandler ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
 
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard	= static_cast<OIS::Keyboard*>	(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse		= static_cast<OIS::Mouse*>		(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);
 
	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
}


void InputHandler::addReceiver(InputReceiver* recv)
{
	mCurrentRecv = recv;
}

void InputHandler::removeReceiver(InputReceiver* recv)
{
	mCurrentRecv = 0;
}


void InputHandler::frameUpdate()
{
	mKeyboard->capture();
	mMouse->capture();
}




//Adjust mouse clipping area
void InputHandler::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
 
//Unattach OIS before window shutdown (very important under Linux)
void InputHandler::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );
 
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}


bool InputHandler::mouseMoved(const OIS::MouseEvent &evt)
{
	if (mCurrentRecv) {
		mCurrentRecv->mouseMoved(evt);
	}

	return true;
}

bool InputHandler::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (mCurrentRecv) {
		mCurrentRecv->mousePressed(evt, id);
	}

	return true;
}
 
bool InputHandler::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (mCurrentRecv) {
		mCurrentRecv->mouseReleased(evt, id);
	}

	return true;
}
 
bool InputHandler::keyPressed(const OIS::KeyEvent &evt)
{
	if (mCurrentRecv) {
		mCurrentRecv->keyPressed(evt);
	}

    return true;
}
 
bool InputHandler::keyReleased(const OIS::KeyEvent &evt)
{
	if (mCurrentRecv) {
		mCurrentRecv->keyReleased(evt);
	}

	return true;
}

