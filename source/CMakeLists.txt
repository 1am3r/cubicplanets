set(TARGET_NAME "CubicPlanets")

SET( ${TARGET_NAME}_HEADER
  stdafx.h
  game/types.h
  game/randomGen.h
  game/Msc.h
  game/gui/McsHudGui.h
  game/gui/MscMainMenu.h
  game/input/InputHandler.h
  game/input/InputReceiver.h
  game/modes/MscCreativeMode.h
  game/modes/MscGameMode.h
  game/world/Chunk.h
  game/world/ChunkPillar.h
  game/world/ChunkStorage.h
  game/world/World.h
  game/world/WorldRegion.h
  physics/BtOgreExtras.h
  physics/BtOgreGP.h
  physics/BtOgrePG.h
  terrain/TerrainGenerator.h
  terrain/noise/Noise2D.h
  terrain/noise/BillowNoise2D.h
  terrain/noise/GradientNoise2D.h
  terrain/noise/RidgedMultifractalNoise2D.h
  terrain/noise/SimplexNoise2D.h
)

SET( ${TARGET_NAME}_SOURCE
  stdafx.cpp
  game/Msc.cpp
  game/gui/McsHudGui.cpp
  game/gui/MscMainMenu.cpp
  game/input/InputHandler.cpp
  game/input/InputReceiver.cpp
  game/modes/MscCreativeMode.cpp
  game/modes/MscGameMode.cpp
  game/world/Chunk.cpp
  game/world/ChunkPillar.cpp
  game/world/ChunkStorage.cpp
  game/world/World.cpp
  game/world/WorldRegion.cpp
  physics/BtOgre.cpp
  terrain/TerrainGenerator.cpp
  terrain/noise/Noise2D.cpp
  terrain/noise/BillowNoise2D.cpp
  terrain/noise/GradientNoise2D.cpp
  terrain/noise/RidgedMultifractalNoise2D.cpp
  terrain/noise/SimplexNoise2D.cpp
)

IF(MSVC)
  #Create virtual folder structure for visual studio.
  cmp_IDE_SOURCE_PROPERTIES("${${TARGET_NAME}_SOURCE}")
  cmp_IDE_HEADER_PROPERTIES("${${TARGET_NAME}_HEADER}")
  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" ${TARGET_NAME}_SOURCE)
ENDIF(MSVC)

if(MSVC)
  set(EXE_PREFIX "WIN32")
else ()
  set(EXE_PREFIX "")
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#Specify required files for target executable
add_executable(${TARGET_NAME}
  ${EXE_PREFIX}
  ${${TARGET_NAME}_HEADER}
  ${${TARGET_NAME}_SOURCE}
)

if(MSVC)
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX _d)
endif(MSVC)

#Link required libraies
target_link_libraries(${TARGET_NAME}
  ${OGRE_LIBRARIES}
  ${OIS_LIBRARIES}
  ${Boost_LIBRARIES}
  ${OGREOGGSOUND_LIBRARIES}
  ${BULLET_LIBRARIES}
  ${CEGUI_LIBRARIES}
)



