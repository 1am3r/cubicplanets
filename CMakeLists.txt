cmake_minimum_required (VERSION 2.8)

project (CubicPlanets)

# The version number.
set (CubicPlanets_VERSION_MAJOR 0)
set (CubicPlanets_VERSION_MINOR 0)
set (CubicPlanets_VERSION_REVISION 1)

if(WIN32)
  set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif(WIN32)

if(UNIX)
  if(EXISTS "/usr/local/lib/OGRE/cmake")
    set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
  elseif(EXISTS "/usr/lib/OGRE/cmake")
    set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
  else ()
    set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
  endif(EXISTS "/usr/local/lib/OGRE")
endif(UNIX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

if (CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
 
if(NOT OIS_FOUND)
  message(SEND_ERROR "Failed to find OIS.")
endif()
 
# Find Boost
set(Boost_USE_STATIC_LIBS TRUE)
if (MINGW)
  # this is probably a bug in CMake: the boost find module tries to look for
  # boost libraries with name libboost_*, but CMake already prefixes library
  # search names with "lib". This is the workaround.
  set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
endif ()
set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0")
set(OGRE_BOOST_COMPONENTS thread date_time system filesystem)
find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
find_package(Boost QUIET)
add_definitions(-DBOOST_ALL_NO_LIB)
set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})

find_package(OgreOggSound REQUIRED)
find_package(Bullet REQUIRED)
find_package(CEGUI REQUIRED COMPONENTS Ogre)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${OIS_INCLUDE_DIRS})
include_directories(${OGRE_INCLUDE_DIRS})
include_directories(${OGREOGGSOUND_INCLUDE_DIRS})
include_directories(${BULLET_INCLUDE_DIRS})
include_directories(${CEGUI_INCLUDE_DIRS})

#Visual Studio related macros. 
include(VS_Macros)

add_subdirectory(source)
