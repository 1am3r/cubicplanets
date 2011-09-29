#include <array>

#include <OGRE/Ogre.h>

#ifndef _MCSHUDGUI_H_
#define _MCSHUDGUI_H_

class FrameGraphRenderable;

class McsHudGui
{
public:
	static const uint16_t TimeGraphMaxFrames = 512;
	static const uint16_t TimeGraphMaxResolution = 256;

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

	void drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real ogreTime, Ogre::Real bulletTime, Ogre::Real worldTime);

private:
	void addFrameGraph(CEGUI::Window* sheet);

private:
	Ogre::Root* mOgreRoot;
	CEGUI::OgreRenderer* mCeRenderer;

	FrameGraphRenderable* mFrameLines;
};


class FrameGraphRenderable
	: public Ogre::SimpleRenderable
{
public:
	FrameGraphRenderable(uint16_t frames, uint16_t resolution);
	~FrameGraphRenderable()
	{
		delete[] mlastTimes;
		delete mRenderOp.vertexData;
	};

	void drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real ogreTime, Ogre::Real bulletTime, Ogre::Real worldTime);

	// Implementation of Ogre::SimpleRenderable
	virtual Ogre::Real getBoundingRadius(void) const;
	// Implementation of Ogre::SimpleRenderable
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

protected:
	// Override this method to prevent parent transforms (rotation,translation,scale)
	void FrameGraphRenderable::getWorldTransforms( Ogre::Matrix4* xform ) const
	{
		// return identity matrix to prevent parent transforms
		*xform = Ogre::Matrix4::IDENTITY;
	};

protected:
	struct TimeGraphLine {
		Ogre::Real unknown;
		Ogre::Real ogre;
		Ogre::Real bullet;
		Ogre::Real world;

		TimeGraphLine()
		{
			set(0.0f, 0.0f, 0.0f, 0.0f);
		};

		TimeGraphLine(Ogre::Real unknownTime, Ogre::Real ogreTime, Ogre::Real bulletTime, Ogre::Real worldTime)
		{
			set(unknownTime, bulletTime, ogreTime, worldTime);
		};

		void set(Ogre::Real unknownTime, Ogre::Real ogreTime, Ogre::Real bulletTime, Ogre::Real worldTime)
		{
			unknown = unknownTime;
			ogre = ogreTime;
			bullet = bulletTime;
			world = worldTime;
		};
	};

	static const size_t VertexPerGraphLine = 8;
	static const size_t ValuesPerGraphLine = VertexPerGraphLine * 3;

private:
	void createHardwareBuffers();
	void fillHardwareBuffers();

	void drawGraphLine(TimeGraphLine& line, uint16_t graphLine);
	void redrawGraph();

private:
	uint16_t mNumFrames;
	uint16_t mResolution;
	
	size_t mNumVertices;

	TimeGraphLine* mlastTimes;
	uint16_t mLastLine;
	uint16_t mSmallerScaling;
	Ogre::Real mCurrentScaling;
	Ogre::Real mLineSpace;
};

#endif // _MCSHUDGUI_H_
