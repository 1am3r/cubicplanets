
#include <cstdint>

#include "game/types.h"
#include "game/world/WorldParams.h"

#ifndef _CHUNK_H_
#define _CHUNK_H_

namespace GameWorld {

class World;

class Chunk
{
	// Attributes
public:
	wCoord x, y,z;

	uint8_t blocks[ChunkSizeX][ChunkSizeZ][ChunkSizeY];
	uint8_t blocksData[ChunkSizeX][ChunkSizeZ][ChunkSizeY];

	uint16_t heighestCube;

	bool isEmpty;

	// Methods
public:
	Chunk(World& world, wCoord xPos, wCoord yPos, wCoord zPos);
	~Chunk();

	void activateChunk() {};
	void deactiveChunk() {};

	Ogre::String& getChunkName();
	uint32_t getHighestCube(uint8_t x, uint8_t z);

	uint8_t getCubeType(uint8_t x, uint8_t y, uint8_t z)
	{
		return blocks[x][z][y];
	};
	uint8_t getCubeData(uint8_t x, uint8_t y, uint8_t z)
	{
		return blocksData[x][z][y];
	};
	void setCubeType(uint8_t x, uint8_t y, uint8_t z, uint8_t cubeType)
	{
		blocks[x][z][y] = cubeType;
		mIsModified = true;
		update();
	};
	void setCubeData(uint8_t x, uint8_t y, uint8_t z, uint8_t cubeData)
	{
		blocksData[x][z][y] = cubeData;
	};

	bool isChunkActive() { return mSceneAttached; };

	void setModified() { mIsModified = true; };
	
	// update after block modification
	void update();

	// ogre entity handling
	void activateEntity();
	void deactivateEntity();

	// bullet body handling
	void activatePhysicsBody();
	void deactivatePhysicsBody();

private:
	// position: 3 floats, normal: 3 floats, texture: 2 floats
	static const uint8_t VertexSize = 3 + 3 + 2;
	// default vertices count per chunk
	static const uint32_t DefaultFaces = 4096;

	enum BlockFace
	{
		LEFT = 0,
		RIGHT = 1,
		BACK = 2,
		FRONT = 3,
		BOTTOM = 4,
		TOP = 5
	};

	struct CubeFace
	{
		const uint8_t x;
		const uint8_t y;
		const uint8_t z;
		const uint8_t type;
		const BlockFace face;

		CubeFace(uint8_t xPos, uint8_t yPos, uint8_t zPos, BlockFace cubeFace, uint8_t cubeType)
		: x(xPos), y(yPos), z(zPos), face(cubeFace), type(cubeType)
		{ };
	};

private:
	// init / destroy ogre entity
	void initEntity();
	void destroyEntity();
	void generateEntityMesh();
	void removeMesh();

	// init / destroy bullet body
	void initPhysicsBody();
	void destroyPhysicsBody();

	// mesh generation
	void		generateMesh();
	void		generateVertices();
	void		getVisibleFaces(std::vector<CubeFace*>& visFaces);
	void		writeFaceVertices(const CubeFace* face, float* buffer);
	void		writeFaceIndices(uint16_t indexStart, uint16_t* buffer);
	uint32_t	getTextureId(const CubeFace* face);
	void		writeTextureCoords(const CubeFace* face, float* buffer);
	void		createBuffers(uint32_t nFaces);

protected:
	Ogre::String				mChunkName;
	Ogre::SceneNode*			mChunkScene;
	Ogre::Entity*				mChunkEntity;
	bool						mSceneAttached;
	bool						mMeshGenerated;

	btTriangleIndexVertexArray* mPhysicsIvArray;
 	btCollisionShape*			mPhysicsShape;
 	btRigidBody*				mPhysicsBody;
	bool						mPhysicsAttached;

	Ogre::MeshPtr	mMeshPtr;
	Ogre::Real*		mVertices;
	uint32_t		mNumVertices;
	uint16_t*		mIndices;
	uint32_t		mNumIndices;
	bool			mIsModified;
	bool			mVertexBufferCreated;

	World& mLevel;
};

};

#endif // #ifndef _CHUNK_H_
