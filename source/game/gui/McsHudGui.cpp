#include "stdafx.h"
#include "McsHudGui.h"

#include <OGRE/Ogre.h>
#include <CEGUI.h>

const Ogre::Real McsHudGui::TimeGraphLineSpace = (2.0f / (TimeGraphMaxFrames));

McsHudGui::McsHudGui(Ogre::Root* ogreRoot, CEGUI::OgreRenderer* ceRenderer)
	: mOgreRoot(ogreRoot), mCeRenderer(ceRenderer)
{
	mlastTimes.fill(TimeGraphLine(0.0f, 0.0f, 0.0f, 0.0f));
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

	// Get the pixel buffer
	mPixelBuffer = tex->getBuffer();
	mLastLine = 0;
	mSmallerScaling = 0;
	mCurrentScaling = 1.0f;



	Ogre::SceneManager* debugSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC);
    debugSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	Ogre::MaterialPtr frameLinesMaterial = Ogre::MaterialManager::getSingleton().create("frameLinesMaterial","Game");
	frameLinesMaterial->setReceiveShadows(false);
	frameLinesMaterial->getTechnique(0)->setLightingEnabled(false);
	frameLinesMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	frameLinesMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);

	// Create the mesh via the MeshManager
	Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual("frameGraphMesh", "Game");
	/// Create one submesh
	Ogre::SubMesh* sub = msh->createSubMesh();
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	
	size_t numVertices = TimeGraphMaxFrames * 8;
	size_t numIndices = TimeGraphMaxFrames * 8;
	float* vertices = new float[numVertices * 3];
	Ogre::RGBA* colours = new Ogre::RGBA[numVertices];
	Ogre::RGBA* pColours = colours;
	uint16_t* indices = new uint16_t[numIndices];
	uint16_t index = 0;
	for (uint16_t i = 0; i < TimeGraphMaxFrames; i++) {
		float x = i * TimeGraphLineSpace - 1.0f;
		
		// OgreTime line
		vertices[index +  0] = x; vertices[index +  1] = -1; vertices[index +  2] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 0.0f, 0.0f), pColours++);		// Color
		vertices[index +  3] = x; vertices[index +  4] =  0; vertices[index +  5] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 0.0f, 0.0f), pColours++);		// Color

		// BulletTime line
		vertices[index +  6] = x; vertices[index +  7] =  0; vertices[index +  8] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 1.0f, 0.0f), pColours++);		// Color
		vertices[index +  9] = x; vertices[index + 10] =  0; vertices[index + 11] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 1.0f, 0.0f), pColours++);		// Color

		// WorldTime line
		vertices[index + 12] = x; vertices[index + 13] =  0; vertices[index + 14] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 0.0f, 1.0f), pColours++);		// Color
		vertices[index + 15] = x; vertices[index + 16] =  0; vertices[index + 17] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(0.0f, 0.0f, 1.0f), pColours++);		// Color

		// UnknownTime line
		vertices[index + 18] = x; vertices[index + 19] =  0; vertices[index + 20] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 1.0f, 1.0f), pColours++);		// Color
		vertices[index + 21] = x; vertices[index + 22] =  0; vertices[index + 23] = 0;	// Vertex
		rs->convertColourValue(Ogre::ColourValue(1.0f, 1.0f, 1.0f), pColours++);		// Color

		index += 24;
	}
	for (uint16_t i = 0; i < TimeGraphMaxFrames * 8; i++) {
		indices[i] = i;
	}

	// Create vertex data structure for numVertices vertices shared between submeshes
	msh->sharedVertexData = new Ogre::VertexData();
	msh->sharedVertexData->vertexCount = numVertices;

	// Create declaration (memory format) of vertex data
	Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
	decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	size_t offset = Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	// Allocate vertex buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
	Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	// Upload the vertex data to the card
	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
	// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
	Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding;
	bind->setBinding(0, vbuf);

	decl->addElement(1, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	offset = Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
	// Allocate vertex buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
	vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	// Upload the vertex data to the card
	vbuf->writeData(0, vbuf->getSizeInBytes(), colours, true);
	// Set vertex buffer binding so buffer 1 is bound to our colour buffer
	bind->setBinding(1, vbuf);

	// Allocate index buffer of the requested number of vertices (ibufCount) 
	Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
		Ogre::HardwareIndexBuffer::IT_16BIT, numIndices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	// Upload the index data to the card
	ibuf->writeData(0, ibuf->getSizeInBytes(), indices, true);

	// Set parameters of the submesh
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = numIndices;
	sub->indexData->indexStart = 0;
	// Set bounding information (for culling)
	Ogre::AxisAlignedBox aabb; aabb.setInfinite(); 
	msh->_setBounds(aabb);
	msh->_setBoundingSphereRadius(Ogre::Real(10.0f));
	// Notify -Mesh object that it has been loaded
	msh->load();

	Ogre::Entity* frameLines = debugSceneMgr->createEntity("frameGraphEnt", "frameGraphMesh");
	frameLines->setMaterialName("frameLinesMaterial");

	//Ogre::SimpleRenderable* frameLines = debugSceneMgr->

	delete[] vertices;
	delete[] colours;
	delete[] indices;

	Ogre::SceneNode* frameLinesNode = debugSceneMgr->getRootSceneNode()->createChildSceneNode("frameGraph_node");
	frameLinesNode->attachObject(frameLines);
	Ogre::Camera* dbg_camera = debugSceneMgr->createCamera("item_camera");
	dbg_camera->setAspectRatio(static_cast<Ogre::Real>(TimeGraphMaxFrames) / static_cast<Ogre::Real>(TimeGraphMaxResolution));
	Ogre::Viewport *v = mPixelBuffer->getRenderTarget()->addViewport(dbg_camera);
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

void McsHudGui::drawTimeLine(const Ogre::FrameEvent& evt, Ogre::Real bulletTime, Ogre::Real ogreTime, Ogre::Real worldTime)
{






	return;




	//bool redraw = false;

	//mlastTimes[mLastLine].set(evt.timeSinceLastFrame, 0, ogreTime, 0);

	//if (evt.timeSinceLastFrame > mCurrentScaling) {
	//	mCurrentScaling *= std::ceil(evt.timeSinceLastFrame / mCurrentScaling);
	//	redraw = true;
	//	mSmallerScaling = 0;
	//} else {
	//	if (evt.timeSinceLastFrame < mCurrentScaling / 2) {
	//		mSmallerScaling++;
	//		if (mSmallerScaling >= 256 && (mCurrentScaling / 2) >= 0.6f) {
	//			mCurrentScaling /= 2;
	//			redraw = true;
	//			mSmallerScaling = 0;
	//		}
	//	}
	//}

	//if (redraw) {
	//	for (uint16_t i = 0; i < TimeGraphMaxLines; i++) {
	//		drawGraphLine(mlastTimes[i], i);
	//	}
	//} else {
	//	drawGraphLine(mlastTimes[mLastLine], mLastLine);
	//}

	//mLastLine++;
	//if (mLastLine >= TimeGraphMaxLines) {
	//	mLastLine = 0;
	//}
}

void McsHudGui::drawGraphLine(TimeGraphLine& line, uint16_t graphLine)
{
	uint16_t ogreLine, bulletLine, worldLine, unknownLine;

	ogreLine = static_cast<uint16_t>((line.ogre / mCurrentScaling) * TimeGraphMaxResolution);
	bulletLine = static_cast<uint16_t>((line.bullet / mCurrentScaling) * TimeGraphMaxResolution);
	worldLine = static_cast<uint16_t>((line.world / mCurrentScaling) * TimeGraphMaxResolution);
	unknownLine = static_cast<uint16_t>((line.total / mCurrentScaling) * TimeGraphMaxResolution);

	bulletLine += ogreLine;
	worldLine += bulletLine;

	// Lock the pixel buffer and get a pixel box
	mPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
	const Ogre::PixelBox& pixelBox = mPixelBuffer->getCurrentLock();
 
	uint32_t* pDest = static_cast<uint32_t*>(pixelBox.data);
	// Point to start of the line
	pDest += graphLine * TimeGraphMaxResolution;

	// Start with drawing the ogreTime
	for (size_t i = 0; i < ogreLine; i++) {
		pDest[i] = TimeGraphOgreColor;
	}

	// Now the bulletTime
	for (size_t i = ogreLine; i < bulletLine; i++) {
		pDest[i] = TimeGraphBulletColor;
	}
	
	// Now the worldTime
	for (size_t i = bulletLine; i < worldLine; i++) {
		pDest[i] = TimeGraphWorldColor;
	}
	
	// Now the unknownTime
	for (size_t i = worldLine; i < unknownLine; i++) {
		pDest[i] = TimeGraphUnknownColor;
	}

	// Now black for the rest of the line
	for (size_t i = unknownLine; i < TimeGraphMaxResolution; i++) {
		pDest[i] = TimeGraphBlackColor;
	}

	// Done, unlock the pixel buffer
	mPixelBuffer->unlock();
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
