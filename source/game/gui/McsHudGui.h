#include <OGRE/Ogre.h>

#ifndef _MCSHUDGUI_H_
#define _MCSHUDGUI_H_

class McsHudGui
{
public:
	McsHudGui(Ogre::Root* ogreRoot, CEGUI::OgreRenderer* ceRenderer);
	virtual ~McsHudGui();

	virtual void update();
	virtual void init();

	//testing stuff
	void setFps(const Ogre::RenderTarget::FrameStats& stats);
	void setHitPos(const CEGUI::String& text);
	void setPos(const CEGUI::String& text);
	CEGUI::Window *mFpsText;
	CEGUI::Window *mPosText;
	CEGUI::Window *mHitPosText;

	void minimaptest(CEGUI::Window* sheet);
	Ogre::HardwarePixelBufferSharedPtr mPixelBuffer;
	uint16_t mLastLine;

	void drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real gpuTime);

private:
	Ogre::Root* mOgreRoot;
	CEGUI::OgreRenderer* mCeRenderer;
};

#endif // _MCSHUDGUI_H_
