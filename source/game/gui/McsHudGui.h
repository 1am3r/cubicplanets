
#ifndef _MCSHUDGUI_H_
#define _MCSHUDGUI_H_

class McsHudGui
{
public:
	McsHudGui();
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

};

#endif // _MCSHUDGUI_H_
