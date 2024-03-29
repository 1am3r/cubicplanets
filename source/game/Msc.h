
#include <OGRE/Ogre.h>

#ifndef __MSC_H_
#define __MSC_H_

class InputHandler;

class Msc
{
public:
	Msc();
	~Msc();
	
	bool go();

private:
	void initOgre();
	void initGui();

private:
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
	CEGUI::OgreRenderer* mRenderer;
	InputHandler* mInput;
};

#endif // #ifndef __MSC_H_
