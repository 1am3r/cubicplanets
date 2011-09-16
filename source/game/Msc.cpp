#include "stdafx.h"

#include "game/Msc.h"

#include "input/InputHandler.h"

#include "game/modes/MscGameMode.h"
#include "game/modes/MscCreativeMode.h"

Msc::Msc(void)
	: mRoot(0), mWindow(0), mInput(0)
{
}


Msc::~Msc(void)
{
	if (mRoot != NULL) {
		delete mRoot;
	}
}

bool Msc::go(void)
{
    initOgre();

	mInput = new InputHandler();
	mInput->init(mWindow);

	initGui();

	MscGameMode* gMode = new MscCreativeMode(mRoot, mWindow, mInput);
	gMode->init();
	mInput->addReceiver(gMode);
	gMode->start();

	return true;
}

void Msc::initOgre()
{
	 // construct Ogre::Root
    mRoot = new Ogre::Root("", "");

#if defined(_DEBUG)
    mRoot->loadPlugin("RenderSystem_GL_d");
#else
    mRoot->loadPlugin("RenderSystem_GL");
#endif

	// setup resources
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load("resources.cfg");
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}


	Ogre::RenderSystem *rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	mRoot->setRenderSystem(rs);
	rs->setConfigOption("Full Screen", "No");
	rs->setConfigOption("Video Mode", "1280 x 800 @ 32-bit colour");

	mWindow = mRoot->initialise(true, "Msc");

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(4);

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(Ogre::String("Game"));
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(Ogre::String("GUI"));
}

void Msc::initGui()
{
	//CEGUI
	CEGUI::OgreRenderer& mRenderer = CEGUI::OgreRenderer::bootstrapSystem();

	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
	CEGUI::ImagesetManager::getSingleton().create("HudImages.imageset", "Imagesets");
}




#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        try {
			// Create application object
			Msc app;
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif