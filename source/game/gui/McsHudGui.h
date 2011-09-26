#include <array>

#include <OGRE/Ogre.h>

#ifndef _MCSHUDGUI_H_
#define _MCSHUDGUI_H_

class McsHudGui
{
public:
	struct TimeGraphLine {
		Ogre::Real total;
		Ogre::Real bullet;
		Ogre::Real ogre;
		Ogre::Real world;

		TimeGraphLine()
		{
			set(0.0f, 0.0f, 0.0f, 0.0f);
		};

		TimeGraphLine(Ogre::Real totalTime, Ogre::Real bulletTime, Ogre::Real ogreTime, Ogre::Real worldTime)
		{
			set(totalTime, bulletTime, ogreTime, worldTime);
		};

		void set(Ogre::Real totalTime, Ogre::Real bulletTime, Ogre::Real ogreTime, Ogre::Real worldTime)
		{
			total = totalTime;
			bullet = bulletTime;
			ogre = ogreTime;
			world = worldTime;
		};
	};
	static const uint16_t TimeGraphMaxFrames = 512;
	static const uint16_t TimeGraphMaxResolution = 256;
	static const Ogre::Real TimeGraphLineSpace;
	static const uint32_t TimeGraphBlackColor   = (127 << 24) | (  0 << 16) | (  0 << 8) | (  0 << 0);
	static const uint32_t TimeGraphOgreColor    = (127 << 24) | (255 << 16) | (  0 << 8) | (  0 << 0);
	static const uint32_t TimeGraphBulletColor  = (127 << 24) | (  0 << 16) | (255 << 8) | (  0 << 0);
	static const uint32_t TimeGraphWorldColor   = (127 << 24) | (  0 << 16) | (  0 << 8) | (255 << 0);
	static const uint32_t TimeGraphUnknownColor = (127 << 24) | (255 << 16) | (255 << 8) | (255 << 0);

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

	void drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real bulletTime, Ogre::Real ogreTime, Ogre::Real worldTime);

private:
	void addFrameGraph(CEGUI::Window* sheet);
	void drawGraphLine(TimeGraphLine& line, uint16_t graphLine);

private:
	Ogre::Root* mOgreRoot;
	CEGUI::OgreRenderer* mCeRenderer;

	std::array<TimeGraphLine, TimeGraphMaxFrames> mlastTimes;
	uint16_t mLastLine;
	uint16_t mSmallerScaling;
	Ogre::Real mCurrentScaling;
	Ogre::HardwarePixelBufferSharedPtr mPixelBuffer;
};

#endif // _MCSHUDGUI_H_
