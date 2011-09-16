#include "stdafx.h"
#include "McsHudGui.h"


McsHudGui::McsHudGui()
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

	sheet->addChildWindow(crosshair);
	sheet->addChildWindow(mFpsText);
	sheet->addChildWindow(mPosText);
	sheet->addChildWindow(mHitPosText);
	CEGUI::System::getSingleton().setGUISheet(sheet);
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
