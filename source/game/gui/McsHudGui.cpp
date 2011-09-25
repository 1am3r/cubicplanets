#include "stdafx.h"
#include "McsHudGui.h"

#include <OGRE/Ogre.h>
#include <CEGUI.h>

McsHudGui::McsHudGui(Ogre::Root* ogreRoot, CEGUI::OgreRenderer* ceRenderer)
	: mOgreRoot(ogreRoot), mCeRenderer(ceRenderer)
{
}


McsHudGui::~McsHudGui()
{
}



void McsHudGui::update()
{

}

void McsHudGui::init()
{
	CEGUI::MouseCursor::getSingleton().hide();

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "MCSHudDefault/DefaultHud");
	
	CEGUI::Window *crosshair = wmgr.createWindow("Vanilla/StaticImage", "MCSHud/Crosshair");
	crosshair->setProperty("Image", "set:HudImages image:cross");
	crosshair->setSize(CEGUI::UVector2(CEGUI::UDim(0, 32), CEGUI::UDim(0, 32)));
	crosshair->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f, -16), CEGUI::UDim(0.5f, -16)));
	crosshair->setProperty("BackgroundEnabled", "False");
	crosshair->setProperty("FrameEnabled", "False");

	mFpsText = wmgr.createWindow("Vanilla/StaticText", "MCSHud/FpsText");
	mFpsText->setAlpha(0.6f);
	mFpsText->setText("");
	mFpsText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 5), CEGUI::UDim(0.0f, 5)));
	mFpsText->setSize(CEGUI::UVector2(CEGUI::UDim(0.4f, 0), CEGUI::UDim(0.04f, 0)));
	mFpsText->setProperty("BackgroundEnabled", "False");
	mFpsText->setProperty("FrameEnabled", "False");

	mPosText = wmgr.createWindow("Vanilla/StaticText", "MCSHud/PosText");
	mPosText->setAlpha(0.6f);
	mPosText->setText("");
	mPosText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 5), CEGUI::UDim(0.04f, 5)));
	mPosText->setSize(CEGUI::UVector2(CEGUI::UDim(0.4f, 0), CEGUI::UDim(0.04f, 0)));
	mPosText->setProperty("BackgroundEnabled", "False");
	mPosText->setProperty("FrameEnabled", "False");

	mHitPosText = wmgr.createWindow("Vanilla/StaticText", "MCSHud/HitPosText");
	mHitPosText->setAlpha(0.6f);
	mHitPosText->setText("");
	mHitPosText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 5), CEGUI::UDim(0.08f, 5)));
	mHitPosText->setSize(CEGUI::UVector2(CEGUI::UDim(0.4f, -5), CEGUI::UDim(0.04f, 0)));
	mHitPosText->setProperty("BackgroundEnabled", "False");
	mHitPosText->setProperty("FrameEnabled", "False");

	minimaptest(sheet);

	sheet->addChildWindow(crosshair);
	sheet->addChildWindow(mFpsText);
	sheet->addChildWindow(mPosText);
	sheet->addChildWindow(mHitPosText);
	CEGUI::System::getSingleton().setGUISheet(sheet);
}

void McsHudGui::minimaptest(CEGUI::Window* sheet)
{
	Ogre::TexturePtr tex = mOgreRoot->getTextureManager()->createManual(
		"RTT",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		1024,
		256,
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_DYNAMIC_WRITE_ONLY);

	// Get the pixel buffer
	mPixelBuffer = tex->getBuffer();
	mLastLine = 0;

	CEGUI::Texture &guiTex = mCeRenderer->createTexture(tex);
	CEGUI::Imageset &imageSet = CEGUI::ImagesetManager::getSingleton().create("RTTImageset", guiTex);
	imageSet.defineImage("RTTImage",
                     CEGUI::Point(0.0f, 0.0f),
                     CEGUI::Size(guiTex.getSize().d_width,
                                 guiTex.getSize().d_height),
                     CEGUI::Point(0.0f, 0.0f));
	CEGUI::Window *si = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", "RTTWindow");
	si->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f, 1024), CEGUI::UDim(0.0f, 512)));
	si->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0), CEGUI::UDim(1.0f, -512)));
	si->setProperty("Image", CEGUI::PropertyHelper::imageToString(&imageSet.getImage("RTTImage")));
	si->setAlpha(0.6f);
	si->setProperty("BackgroundEnabled", "False");
	si->setProperty("FrameEnabled", "False");

	sheet->addChildWindow(si);
}

void McsHudGui::drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real gpuTime)
{
	uint8_t green = 0;
	uint8_t red = 0;
	uint8_t blue = 0;

	size_t height = static_cast<size_t>(evt.timeSinceLastFrame * 512);
	size_t gpu = static_cast<size_t>((gpuTime / evt.timeSinceLastFrame) * height);


	// Lock the pixel buffer and get a pixel box
	mPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
	const Ogre::PixelBox& pixelBox = mPixelBuffer->getCurrentLock();
 
	uint8_t* pDest = static_cast<uint8_t*>(pixelBox.data);

	size_t line = mLastLine * 1024 * 4;
	for (size_t i = 0; i < 1024 * 4; i += 4) {
		if (i < gpu) {
			red = 255;
			green = 0;
		} else if (i < height) {
			red = 0;
			green = 255;
		} else {
			red = 0;
			green = 0;
		}
		pDest[line + (i + 0)] = blue; // B
		pDest[line + (i + 1)] = green; // G
		pDest[line + (i + 2)] = red; // R
		pDest[line + (i + 3)] = 127; // A
	}
 
	// Unlock the pixel buffer
	mPixelBuffer->unlock();

	mLastLine++;
	if (mLastLine >= 256) {
		mLastLine = 0;
	}
}

void McsHudGui::setFps(const Ogre::RenderTarget::FrameStats& stats)
{
	std::ostringstream ostr;
	ostr.precision(1);
	ostr << "FPS: " << std::fixed << stats.lastFPS;
	ostr << " T: " << stats.triangleCount << " B: " << stats.batchCount;

	mFpsText->setText(ostr.str());
}

void McsHudGui::setHitPos(const CEGUI::String& text)
{
	mHitPosText->setText(text);
}

void McsHudGui::setPos(const CEGUI::String& text)
{
	mPosText->setText(text);
}
