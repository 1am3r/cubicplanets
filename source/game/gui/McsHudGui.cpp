#include "stdafx.h"
#include "McsHudGui.h"

#include <OGRE/Ogre.h>
#include <CEGUI.h>

McsHudGui::McsHudGui(Ogre::Root* ogreRoot, CEGUI::OgreRenderer* ceRenderer)
	: mOgreRoot(ogreRoot), mCeRenderer(ceRenderer), mFrameLines(0)
{
	
}


McsHudGui::~McsHudGui()
{
	if (mFrameLines) {
		delete mFrameLines;
	}
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

	addFrameGraph(sheet);

	sheet->addChildWindow(crosshair);
	sheet->addChildWindow(mFpsText);
	sheet->addChildWindow(mPosText);
	sheet->addChildWindow(mHitPosText);
	CEGUI::System::getSingleton().setGUISheet(sheet);
}

void McsHudGui::addFrameGraph(CEGUI::Window* sheet)
{
	Ogre::TexturePtr tex = mOgreRoot->getTextureManager()->createManual(
		"FrameGraph", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
		TimeGraphMaxFrames, TimeGraphMaxResolution, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

	Ogre::SceneManager* debugSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC);
    debugSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	Ogre::MaterialPtr frameLinesMaterial = Ogre::MaterialManager::getSingleton().create("frameLinesMaterial","Game");
	frameLinesMaterial->setReceiveShadows(false);
	frameLinesMaterial->getTechnique(0)->setLightingEnabled(false);
	frameLinesMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	frameLinesMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	
	mFrameLines = new FrameGraphRenderable(TimeGraphMaxFrames, TimeGraphMaxResolution);
	mFrameLines->setMaterial("frameLinesMaterial");

	Ogre::SceneNode* frameLinesNode = debugSceneMgr->getRootSceneNode()->createChildSceneNode("frameGraph_node");
	frameLinesNode->attachObject(mFrameLines);
	Ogre::Camera* dbg_camera = debugSceneMgr->createCamera("item_camera");
	dbg_camera->setAspectRatio(static_cast<Ogre::Real>(TimeGraphMaxFrames) / static_cast<Ogre::Real>(TimeGraphMaxResolution));
	Ogre::Viewport *v = tex->getBuffer()->getRenderTarget()->addViewport(dbg_camera);
	v->setClearEveryFrame( true );
	v->setBackgroundColour( Ogre::ColourValue::Black );



	CEGUI::Texture& guiTex = mCeRenderer->createTexture(tex);
	CEGUI::Imageset& imageSet = CEGUI::ImagesetManager::getSingleton().create("FrameGraphImageset", guiTex);
	imageSet.defineImage("FrameGraphImage", CEGUI::Point(0.0f, 0.0f), CEGUI::Size(guiTex.getSize()), CEGUI::Point(0.0f, 0.0f));
	CEGUI::Window* si = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", "FrameGraphWindow");
	si->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f, TimeGraphMaxFrames), CEGUI::UDim(0.0f, TimeGraphMaxResolution)));
	si->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0), CEGUI::UDim(1.0f, -TimeGraphMaxResolution)));
	si->setProperty("Image", CEGUI::PropertyHelper::imageToString(&imageSet.getImage("FrameGraphImage")));
	si->setAlpha(0.6f);
	si->setProperty("BackgroundEnabled", "False");
	si->setProperty("FrameEnabled", "False");

	sheet->addChildWindow(si);
}

void McsHudGui::drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real ogreTime, Ogre::Real bulletTime, Ogre::Real worldTime)
{
	if (mFrameLines) {
		mFrameLines->drawTimeLine(evt, ogreTime, bulletTime, worldTime);
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

FrameGraphRenderable::FrameGraphRenderable(uint16_t frames, uint16_t resolution)
	: mNumFrames(frames), mResolution(resolution), mLastLine(0), mSmallerScaling(0), mCurrentScaling(1.0f)
{
	mlastTimes = new TimeGraphLine[mNumFrames];

	// use identity projection and view matrices
	mUseIdentityProjection = true;
	mUseIdentityView = true;

	mNumVertices = mNumFrames * 8;
	mLineSpace = (2.0f / (mNumFrames));

	mRenderOp.vertexData = new Ogre::VertexData();

	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexCount = mNumVertices; 
	mRenderOp.vertexData->vertexStart = 0; 
	mRenderOp.operationType = Ogre::RenderOperation::OT_LINE_LIST; 
	mRenderOp.useIndexes = false;

	// Set the vertex declarations
	mRenderOp.vertexData->vertexDeclaration->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	mRenderOp.vertexData->vertexDeclaration->addElement(1, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

	createHardwareBuffers();
	fillHardwareBuffers();
}

void FrameGraphRenderable::createHardwareBuffers()
{
	// Allocate vertex buffer of the requested number of vertices (vertexCount) and bytes per vertex
	Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		mRenderOp.vertexData->vertexDeclaration->getVertexSize(0), mRenderOp.vertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

	// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vbuf);

	// Allocate vertex buffer of the requested number of vertices (vertexCount) and bytes per vertex
	vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		mRenderOp.vertexData->vertexDeclaration->getVertexSize(1), mRenderOp.vertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	
	// Set vertex buffer binding so buffer 1 is bound to our colour buffer
	mRenderOp.vertexData->vertexBufferBinding->setBinding(1, vbuf);
}

void FrameGraphRenderable::fillHardwareBuffers()
{
	Ogre::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	Ogre::Real *vertices = static_cast<Ogre::Real*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

	Ogre::HardwareVertexBufferSharedPtr vcbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(1);
	Ogre::RGBA* pColours = static_cast<Ogre::RGBA*>(vcbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	uint16_t index = 0;
	for (uint16_t i = 0; i < mNumFrames; i++) {
		float x = i * mLineSpace - 1.0f;
		
		// OgreTime line
		vertices[index +  0] = x; vertices[index +  1] = -1; vertices[index +  2] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 0.0f, 0.0f), pColours++);		// Color
		vertices[index +  3] = x; vertices[index +  4] = -1; vertices[index +  5] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 0.0f, 0.0f), pColours++);		// Color

		// BulletTime line
		vertices[index +  6] = x; vertices[index +  7] = -1; vertices[index +  8] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 1.0f, 0.0f), pColours++);		// Color
		vertices[index +  9] = x; vertices[index + 10] = -1; vertices[index + 11] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 1.0f, 0.0f), pColours++);		// Color

		// WorldTime line
		vertices[index + 12] = x; vertices[index + 13] = -1; vertices[index + 14] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 0.0f, 1.0f), pColours++);		// Color
		vertices[index + 15] = x; vertices[index + 16] = -1; vertices[index + 17] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 0.0f, 1.0f), pColours++);		// Color

		// UnknownTime line
		vertices[index + 18] = x; vertices[index + 19] = -1; vertices[index + 20] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 1.0f, 1.0f), pColours++);		// Color
		vertices[index + 21] = x; vertices[index + 22] = -1; vertices[index + 23] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 1.0f, 1.0f), pColours++);		// Color

		index += ValuesPerGraphLine;
	}
	vcbuf->unlock();
	vbuf->unlock();

	mBox.setInfinite();
}

Ogre::Real FrameGraphRenderable::getBoundingRadius(void) const
{
  return Ogre::Math::Sqrt(std::max(mBox.getMaximum().squaredLength(), mBox.getMinimum().squaredLength()));
}
 
Ogre::Real FrameGraphRenderable::getSquaredViewDepth(const Ogre::Camera* cam) const
{
   Ogre::Vector3 vMin, vMax, vMid, vDist;
   vMin = mBox.getMinimum();
   vMax = mBox.getMaximum();
   vMid = ((vMax - vMin) * 0.5) + vMin;
   vDist = cam->getDerivedPosition() - vMid;
 
   return vDist.squaredLength();
}

void FrameGraphRenderable::drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real ogreTime, Ogre::Real bulletTime, Ogre::Real worldTime)
{
	bool redraw = false;

	mlastTimes[mLastLine].set(evt.timeSinceLastFrame, ogreTime, bulletTime, worldTime);

	if (evt.timeSinceLastFrame > mCurrentScaling) {
		mCurrentScaling *= std::ceil(evt.timeSinceLastFrame / mCurrentScaling);
		redraw = true;
		mSmallerScaling = 0;
	} else {
		if (evt.timeSinceLastFrame <= mCurrentScaling) {
			mSmallerScaling++;
			if (mSmallerScaling >= mNumFrames && mCurrentScaling >= 0.01f) {
				mCurrentScaling /= 2;
				redraw = true;
				mSmallerScaling = 0;
			}
		}
	}

	if (redraw) {
		redrawGraph();
	} else {
		drawGraphLine(mlastTimes[mLastLine], mLastLine);
	}

	mLastLine++;
	if (mLastLine >= mNumFrames) {
		mLastLine = 0;
	}
}

void FrameGraphRenderable::drawGraphLine(TimeGraphLine& line, uint16_t graphLine)
{
	Ogre::Real ogreTop, bulletTop, worldTop, unknownTop;

	ogreTop = (line.ogre / mCurrentScaling);
	bulletTop = ogreTop + (line.bullet / mCurrentScaling);
	worldTop = bulletTop + (line.world / mCurrentScaling);
	unknownTop = worldTop + (line.total / mCurrentScaling);
	
	ogreTop = ogreTop * 2 - 1.0f;
	bulletTop = bulletTop * 2 - 1.0f;
	worldTop = worldTop * 2 - 1.0f;
	unknownTop = unknownTop * 2 - 1.0f;

	Ogre::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	Ogre::Real *vertices = static_cast<Ogre::Real*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

	vertices += graphLine * ValuesPerGraphLine;

	// Set the ogre line top vertex
	vertices[ 4] = ogreTop;
	// Set the bullet line vertices
	vertices[ 7] = ogreTop;
	vertices[10] = bulletTop;
	// Set the world line vertices
	vertices[13] = bulletTop;
	vertices[16] = worldTop;
	// Set the unknown line vertices
	vertices[19] = worldTop;
	vertices[22] = unknownTop;

	vbuf->unlock();
}

void FrameGraphRenderable::redrawGraph()
{
	Ogre::Real ogreTop, bulletTop, worldTop, unknownTop;

	Ogre::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	Ogre::Real *vertices = static_cast<Ogre::Real*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

	for (size_t i = 0; i < mNumFrames; i++) {
		ogreTop = (mlastTimes[i].ogre / mCurrentScaling);
		bulletTop = ogreTop + (mlastTimes[i].bullet / mCurrentScaling);
		worldTop = bulletTop + (mlastTimes[i].world / mCurrentScaling);
		unknownTop = worldTop + (mlastTimes[i].total / mCurrentScaling);

		ogreTop = ogreTop * 2 - 1.0f;
		bulletTop = bulletTop * 2 - 1.0f;
		worldTop = worldTop * 2 - 1.0f;
		unknownTop = unknownTop * 2 - 1.0f;

		// Set the ogre line top vertex
		vertices[ 4] = ogreTop;
		// Set the bullet line vertices
		vertices[ 7] = ogreTop;
		vertices[10] = bulletTop;
		// Set the world line vertices
		vertices[13] = bulletTop;
		vertices[16] = worldTop;
		// Set the unknown line vertices
		vertices[19] = worldTop;
		vertices[22] = unknownTop;

		vertices += ValuesPerGraphLine;
	}

	vbuf->unlock();
}
