

// Stdlib includes
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

// Boost includes
#include <boost/random.hpp>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lexical_cast.hpp>


// Ogre includes
#include <OgreRoot.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderSystem.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreWindowEventUtilities.h>
#include <OgreMesh.h>

// OIS includes
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>


// Bullet includes
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"


// CEGUI includes
#include "CEGUI.h"
#include "RendererModules/Ogre/CEGUIOgreRenderer.h"

