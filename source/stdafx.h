#include <stdint.h>
#include <string>
#include <vector>
#include <math.h>
#include <list>
#include <algorithm>
#include <functional>

#include <boost/random.hpp>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <OgreRoot.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderSystem.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreMaterial.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreWindowEventUtilities.h>
#include <OgreMesh.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "physics/BtOgreExtras.h"
#include "physics/BtOgreGP.h"
#include "physics/BtOgrePG.h"

// CEGUI includes
#include "CEGUI.h"
#include "RendererModules/Ogre/CEGUIOgreRenderer.h"

#include "game/prerequisites.h"
#include "game/randomGen.h"


