#include "stdafx.h"

#include "game/types.h"
#include "game/world/WorldParams.h"
#include "game/world/Chunk.h"
#include "game/world/ChunkStorage.h"
#include "game/world/World.h"

namespace GameWorld {

Chunk::Chunk(World& world, wCoord xPos, wCoord yPos, wCoord zPos)
	: mLevel(world), mStorage(world.getStorage()), mChunkEntity(0), mChunkScene(0), mSceneAttached(false), mMeshGenerated(false),
	  mPhysicsIvArray(0), mPhysicsShape(0), mPhysicsBody(0), mPhysicsAttached(false),
	  mX(xPos), mY(yPos), mZ(zPos), mHighestCube(ChunkSizeY - 1), isEmpty(false),
	  mNumVertices(0), mNumIndices(0), mVertices(0), mIndices(0), mIsModified(true), mVertexBufferCreated(false)
{
	initChunk();

	blocks.fill(0);
	blocksData.fill(0);
}

Chunk::Chunk(World& world, wCoord xPos, wCoord yPos, wCoord zPos, std::istream& data)
	: mLevel(world), mStorage(world.getStorage()), mChunkEntity(0), mChunkScene(0), mSceneAttached(false), mMeshGenerated(false),
	  mPhysicsIvArray(0), mPhysicsShape(0), mPhysicsBody(0), mPhysicsAttached(false),
	  mX(xPos), mY(yPos), mZ(zPos), mHighestCube(ChunkSizeY - 1), isEmpty(false),
	  mNumVertices(0), mNumIndices(0), mVertices(0), mIndices(0), mIsModified(true), mVertexBufferCreated(false)
{
	initChunk();

	data.read(reinterpret_cast<char*>(&mHighestCube), sizeof(mHighestCube));
	char empty = 0;
	data.read(&empty, sizeof(empty));
	isEmpty = (empty != 0);

	data.read(reinterpret_cast<char*>(blocks.data()), blocks.size() * sizeof(blocks[0]));
	data.read(reinterpret_cast<char*>(blocksData.data()), blocksData.size() * sizeof(blocksData[0]));
}

Chunk::~Chunk()
{
	destroyEntity();
	destroyPhysicsBody();
	removeMesh();

	if (mVertices != 0) {
		delete[] mVertices;
	}

	if (mIndices != 0) {
		delete[] mIndices;
	}
}

void Chunk::initChunk()
{
	std::ostringstream chunkNameStream;
	chunkNameStream << "chunk" << mX << "_" << mY << "_" << mZ;
	mChunkName.assign(chunkNameStream.str()); 
}

void Chunk::saveToStream(std::ostream& data)
{
	data.write(reinterpret_cast<char*>(&mHighestCube), sizeof(mHighestCube));
	char empty = static_cast<char>(isEmpty);
	data.write(&empty, sizeof(empty));

	data.write(reinterpret_cast<char*>(blocks.data()), blocks.size() * sizeof(blocks[0]));
	data.write(reinterpret_cast<char*>(blocksData.data()), blocksData.size() * sizeof(blocksData[0]));
}


uint32_t Chunk::getHighestCube(uint8_t xPos, uint8_t zPos)
{
	for (uint8_t yPos = (ChunkSizeY - 1); yPos != 0; yPos--) {
		if (blocks[getCubeIndex(xPos, yPos, zPos)] != 0) {
			return yPos;
		}
	}

	return 0;
}




void Chunk::activateEntity()
{
	if (mSceneAttached) return;

	if (mChunkScene == 0) initEntity();

	if (isEmpty) return;

	Ogre::SceneNode& parentScene = mLevel.getChunkScene();
	parentScene.addChild(mChunkScene);
	mSceneAttached = true;
}

void Chunk::deactivateEntity()
{
	if (mChunkScene != 0 && mSceneAttached) {
		mLevel.getChunkScene().removeChild(mChunkScene);
		mSceneAttached = false;
	}
}


void Chunk::activatePhysicsBody()
{
	if (!mPhysicsAttached) {
		if (mPhysicsBody == 0) initPhysicsBody();

		if (isEmpty) return;

		mLevel.getPhysicsWorld().addRigidBody(mPhysicsBody);
		mPhysicsAttached = true;
	}
}

void Chunk::deactivatePhysicsBody()
{
	if (mPhysicsAttached) {
		mLevel.getPhysicsWorld().removeRigidBody(mPhysicsBody);
		mPhysicsAttached = false;
	}
}


void Chunk::update()
{
	if (mIsModified) {
		initEntity();

		if (isEmpty) return;

		initPhysicsBody();
		activateEntity();
		activatePhysicsBody();
	}
}

void Chunk::initEntity()
{
	if (mSceneAttached) deactivateEntity();

	if (mChunkScene == 0) {
		if (!mMeshGenerated) {
			generateEntityMesh();
			if (isEmpty) {
				mMeshGenerated = false;
				return;
			}
		}
		
		mChunkScene = mLevel.getSceneMgr().createSceneNode(mChunkName);

		mChunkEntity = mLevel.getSceneMgr().createEntity(Ogre::String("Ent").append(mChunkName), mChunkName);
		mChunkEntity->setMaterialName("Game/Terrain");
		mChunkScene->attachObject(mChunkEntity);
		
		mChunkScene->setPosition(
			Ogre::Real(mX * ChunkSizeX), 
			Ogre::Real(mY * ChunkSizeY), 
			Ogre::Real(mZ * ChunkSizeZ));
	} else {
		generateEntityMesh();
		
		if (mChunkEntity == 0) {
			mChunkEntity = mLevel.getSceneMgr().createEntity(Ogre::String("Ent").append(mChunkName), mChunkName);
			mChunkEntity->setMaterialName("Game/Terrain");
			mChunkScene->attachObject(mChunkEntity);
		}
	}
}

void Chunk::destroyEntity()
{
	if (mSceneAttached) deactivateEntity();

	if (mChunkScene != 0) {
		mChunkScene->detachAllObjects();
		mLevel.getSceneMgr().destroySceneNode(mChunkScene);
		mChunkScene = 0;
	}

	if (mChunkEntity != 0) {
		mLevel.getSceneMgr().destroyEntity(mChunkEntity);
		mChunkEntity = 0;
	}
}

void Chunk::generateEntityMesh()
{
	generateMesh();

	mMeshGenerated = true;
}

void Chunk::removeMesh()
{
	if (mMeshGenerated) {
		if (mChunkEntity != 0) {
			mChunkScene->detachObject(mChunkEntity);
			mLevel.getSceneMgr().destroyEntity(mChunkEntity);
			mChunkEntity = 0;
		}

		Ogre::MeshManager::getSingleton().remove(mChunkName);
		mVertexBufferCreated = false;
		mMeshPtr.setNull();
	}
};


void Chunk::initPhysicsBody()
{
	if (mPhysicsAttached) deactivatePhysicsBody();
	
	if (mIsModified) generateVertices();

	if (isEmpty) return;

	btCollisionShape* newShape;
	btTriangleIndexVertexArray* newIvArray = new btTriangleIndexVertexArray();
	btIndexedMesh mesh;
	
	mesh.m_numTriangles = mNumIndices / 3;
	mesh.m_triangleIndexBase = (const unsigned char *) mIndices;
	mesh.m_triangleIndexStride = 3 * sizeof(uint16_t);
	mesh.m_numVertices = mNumVertices;
	mesh.m_vertexBase = (const unsigned char *) mVertices;
	mesh.m_vertexStride = VertexSize * sizeof(float);
	newIvArray->addIndexedMesh(mesh, PHY_SHORT);

	newShape = new btBvhTriangleMeshShape(newIvArray, true);
	newShape->setMargin(0.1f);

	if (mPhysicsBody == 0) {
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3((btScalar)
			(btScalar) (mX * ChunkSizeX), 
			(btScalar) (mY * ChunkSizeY), 
			(btScalar) (mZ * ChunkSizeZ))); 
		btMotionState* mState = new btDefaultMotionState(trans);
		btRigidBody::btRigidBodyConstructionInfo cInfo(0.0f, mState, newShape);

		mPhysicsBody = new btRigidBody(cInfo);
		mPhysicsBody->setCollisionFlags(mPhysicsBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	} else {
		mPhysicsBody->setCollisionShape(newShape);
	}

	if (mPhysicsIvArray != 0) {
		delete mPhysicsIvArray;
	}
	mPhysicsIvArray = newIvArray;

	if (mPhysicsShape != 0) {
		delete mPhysicsShape;
	}
	mPhysicsShape = newShape;
}

void Chunk::destroyPhysicsBody()
{
	deactivatePhysicsBody();

	if (mPhysicsBody != 0) {
		delete mPhysicsBody->getMotionState();
		delete mPhysicsBody;
		mPhysicsBody = 0;
	}

	if (mPhysicsShape != 0) {
		delete mPhysicsShape;
		mPhysicsShape = 0;
	}

	if (mPhysicsIvArray != 0) {
		delete mPhysicsIvArray;
		mPhysicsIvArray = 0;
	}
}


// Mesh generation
void Chunk::getVisibleFaces(std::vector<CubeFace*>& visFaces)
{
	Chunk* leftChunk	= mStorage.getChunkLocal(mX - 1, mY    , mZ    );
	Chunk* rightChunk	= mStorage.getChunkLocal(mX + 1, mY    , mZ    );
	Chunk* backChunk	= mStorage.getChunkLocal(mX	 , mY    , mZ - 1);
	Chunk* frontChunk	= mStorage.getChunkLocal(mX	 , mY    , mZ + 1);
	Chunk* bottomChunk	= mStorage.getChunkLocal(mX	 , mY - 1, mZ    );
	Chunk* topChunk		= mStorage.getChunkLocal(mX	 , mY + 1, mZ    );

	for (uint8_t xLocal = 0; xLocal < ChunkSizeX; xLocal++) 
	{
		for (uint8_t zLocal = 0; zLocal < ChunkSizeZ; zLocal++)
		{
			for (uint8_t yLocal = 0; yLocal < ChunkSizeY; yLocal++)
			{
				uint8_t cubeType = getCubeTypeLocal(xLocal, yLocal, zLocal);
				if (cubeType != 0)
				{

					CubeFace* cFace = 0;
					if (xLocal == 0) {
						if ((leftChunk != 0) && (leftChunk->getCubeTypeLocal(ChunkSizeX - 1, yLocal, zLocal) == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, LEFT, cubeType));
						}
						if (getCubeTypeLocal(xLocal + 1, yLocal, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, RIGHT, cubeType));
						}
					} else if (xLocal == ChunkSizeX - 1) {
						if (getCubeTypeLocal(xLocal - 1, yLocal, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, LEFT, cubeType));
						}
						if ((rightChunk != 0) && (rightChunk->getCubeTypeLocal(0, yLocal, zLocal) == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, RIGHT, cubeType));
						}
					} else {
						if (getCubeTypeLocal(xLocal - 1, yLocal, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, LEFT, cubeType));
						}
						if (getCubeTypeLocal(xLocal + 1, yLocal, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, RIGHT, cubeType));
						}
					}

					if (zLocal == 0) {
						if ((backChunk != 0) && (backChunk->getCubeTypeLocal(xLocal, yLocal, ChunkSizeZ - 1) == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BACK, cubeType));
						}
						if (getCubeTypeLocal(xLocal, yLocal, zLocal + 1) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, FRONT, cubeType));
						}
					} else if (zLocal == ChunkSizeZ - 1) {
						if (getCubeTypeLocal(xLocal, yLocal, zLocal - 1) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BACK, cubeType));
						}
						if ((frontChunk != 0) && (frontChunk->getCubeTypeLocal(xLocal, yLocal, 0) == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, FRONT, cubeType));
						}
					} else {
						if (getCubeTypeLocal(xLocal, yLocal, zLocal - 1) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BACK, cubeType));
						}
						if (getCubeTypeLocal(xLocal, yLocal, zLocal + 1) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, FRONT, cubeType));
						}
					}

					if (yLocal == 0) {
						if ((bottomChunk != 0) && (bottomChunk->getCubeTypeLocal(xLocal, ChunkSizeY - 1, zLocal) == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BOTTOM, cubeType));
						}
						if (getCubeTypeLocal(xLocal, yLocal + 1, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, TOP, cubeType));
						}
					} else if (yLocal == ChunkSizeY - 1) {
						if (getCubeTypeLocal(xLocal, yLocal - 1, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BOTTOM, cubeType));
						}
						if ((topChunk != 0) && (topChunk->getCubeTypeLocal(xLocal, 0, zLocal) == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, TOP, cubeType));
						}
					} else {
						if (getCubeTypeLocal(xLocal, yLocal - 1, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BOTTOM, cubeType));
						}
						if (getCubeTypeLocal(xLocal, yLocal + 1, zLocal) == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, TOP, cubeType));
						}
					}
				}
			}
		}
	}
}

void Chunk::createBuffers(uint32_t nFaces)
{
	const uint32_t nVertices = nFaces * 4;
	const uint32_t vBufCount = VertexSize * nVertices;
	float* vertexData = new float [vBufCount];
	mVertices = vertexData;
	mNumVertices = nVertices;

	const uint32_t iBufCount = 2 * 3 * nFaces;
	uint16_t* faces = new uint16_t [iBufCount];
	mIndices = faces;
	mNumIndices = iBufCount;
}

uint32_t Chunk::getTextureId(const CubeFace* face)
{
	uint32_t textureId = 0;

	switch (face->type)
	{
	case 1:
		textureId = 1;
		break;

	case 2:
		if (face->face == TOP)
			textureId = 0;
		else if (face->face == BOTTOM)
			textureId = 2;
		else
			textureId = 3;
		break;

	case 3:
		textureId = 2;
		break;

	case 4:
		textureId = 16;
		break;
	
	case 5:
		textureId = 4;
		break;


	case 7:
		textureId = 17;
		break;


	case 9:
		textureId = 205;
		break;


	case 12:
		textureId = 18;
		break;

	case 13:
		textureId = 19;
		break;

	case 14:
		textureId = 32;
		break;

	case 15:
		textureId = 33;
		break;

	case 16:
		textureId = 34;
		break;

	case 79:
		textureId = 67;
		break;

	default:
		textureId = 7;
	}

	return textureId;
}

void Chunk::writeFaceVertices(const CubeFace* face, float* buffer)
{
	static const uint8_t xyzOffsets[6][4][3] = {
		{{0, 1, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 1}},
		{{1, 1, 1}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0}},
		{{1, 1, 0}, {1, 0, 0}, {0, 0, 0}, {0, 1, 0}},
		{{0, 1, 1}, {0, 0, 1}, {1, 0, 1}, {1, 1, 1}},
		{{0, 0, 1}, {0, 0, 0}, {1, 0, 0}, {1, 0, 1}},
		{{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}
	};
	const uint8_t (*xyzOff)[4][3];

	static const float textureSize = (1.0 - 0.5 / 256.0) / 16.0;
	static const float uvOff[4][2] = {{0, 0}, {0, textureSize}, {textureSize, textureSize}, {textureSize, 0}};
	float uvCoords[2] = {0, 0};

	writeTextureCoords(face, uvCoords);

	
	const Ogre::Vector3* normal;
	switch (face->face)
	{
	case LEFT:
		normal = &Ogre::Vector3::NEGATIVE_UNIT_X;
		xyzOff = &xyzOffsets[LEFT];
		break;
	case RIGHT:
		normal = &Ogre::Vector3::UNIT_X;
		xyzOff = &xyzOffsets[RIGHT];
		break;
	case BACK:
		normal = &Ogre::Vector3::NEGATIVE_UNIT_Z;
		xyzOff = &xyzOffsets[BACK];
		break;
	case FRONT:
		normal = &Ogre::Vector3::UNIT_Z;
		xyzOff = &xyzOffsets[FRONT];
		break;
	case BOTTOM:
		normal = &Ogre::Vector3::NEGATIVE_UNIT_Y;
		xyzOff = &xyzOffsets[BOTTOM];
		break;
	case TOP:
		normal = &Ogre::Vector3::UNIT_Y;
		xyzOff = &xyzOffsets[TOP];
		break;
	}
	
	uint8_t bufIndex = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		// write position
		buffer[bufIndex + 0] = (float) ((face->x + (*xyzOff)[i][0]));
		buffer[bufIndex + 1] = (float) ((face->y + (*xyzOff)[i][1]));
		buffer[bufIndex + 2] = (float) ((face->z + (*xyzOff)[i][2]));

		// write normal
		buffer[bufIndex + 3] = normal->x;
		buffer[bufIndex + 4] = normal->y;
		buffer[bufIndex + 5] = normal->z;

		// write texture coordinates
		buffer[bufIndex + 6] = uvCoords[0] + uvOff[i][0];
		buffer[bufIndex + 7] = uvCoords[1] + uvOff[i][1];

		bufIndex += (3 + 3 + 2);
	}
}

void Chunk::writeFaceIndices(uint16_t indexStart, uint16_t* buffer)
{
	buffer[0] = indexStart + 0;
	buffer[1] = indexStart + 1;
	buffer[2] = indexStart + 2;

	buffer[3] = indexStart + 0;
	buffer[4] = indexStart + 2;
	buffer[5] = indexStart + 3;
}

void Chunk::writeTextureCoords(const CubeFace* face, float* buffer)
{
	uint32_t textureId = getTextureId(face);

	uint8_t uIndex = textureId & 0x0F;
	uint8_t vIndex = (textureId & 0xF0) >> 4;

	buffer[0] = (float) (uIndex / 16.0);
	buffer[1] = (float) (vIndex / 16.0);
}

void Chunk::generateVertices()
{
	if (mIsModified) {
		if (mVertices != 0) {
			delete mVertices;
			mVertices = 0;
		}
		if (mIndices != 0) {
			delete mIndices;
			mIndices = 0;
		}

		std::vector<CubeFace*> visFaces;
		visFaces.reserve(DefaultFaces);

		getVisibleFaces(visFaces);

		uint32_t nFaces = static_cast<uint32_t> (visFaces.size());

		if (nFaces == 0) {
			isEmpty = true;
			return;
		} 
		isEmpty = false;

		createBuffers(nFaces);

		uint32_t vBufIndex = 0;
		uint32_t iBufIndex = 0;
		uint32_t curVertex = 0;
		while (!visFaces.empty())
		{
			CubeFace* curFace = visFaces.back();
			visFaces.pop_back();
		
			writeFaceVertices(curFace, &mVertices[vBufIndex]);
			writeFaceIndices(curVertex, &mIndices[iBufIndex]);

			vBufIndex += VertexSize * 4;
			curVertex += 4;
			iBufIndex += 6;
			delete curFace;
		}

		mIsModified = false;
	}
}

void Chunk::generateMesh()
{
	if (!mIsModified) return;

	generateVertices();

	if (isEmpty) {

		return;
	}

	uint32_t numVertices, numIndices;
	if (mNumVertices > DefaultFaces * 4) {
		// more vertices than the default buffer can hold -> allocate new one;
		if (mVertexBufferCreated) {
			removeMesh();
		}

		numVertices = mNumVertices;
		numIndices = mNumIndices;
	} else {
		numVertices = DefaultFaces * 4;
		numIndices = DefaultFaces * 6;
	}

	if (!mVertexBufferCreated) {
		mMeshPtr = Ogre::MeshManager::getSingleton().createManual(mChunkName, "Game");
		Ogre::SubMesh* sub = mMeshPtr->createSubMesh();
		
		/// Create vertex data structure for 8 vertices shared between submeshes
		mMeshPtr->sharedVertexData = new Ogre::VertexData();
		mMeshPtr->sharedVertexData->vertexCount = mNumVertices;
 
		/// Create declaration (memory format) of vertex data
		Ogre::VertexDeclaration* decl = mMeshPtr->sharedVertexData->vertexDeclaration;
    
		size_t offset = 0;
		// 1st buffer
		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
		decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    
		/// Allocate vertex buffer of the requested number of vertices (vertexCount) and bytes per vertex (offset)
		Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().
			createVertexBuffer(offset, numVertices, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    
		/// Upload the vertex data to the card
		vbuf->writeData(0, (decl->getVertexSize(0) * mNumVertices), mVertices, true);
 
		/// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
		Ogre::VertexBufferBinding* bind = mMeshPtr->sharedVertexData->vertexBufferBinding; 
		bind->setBinding(0, vbuf);

		/// Allocate index buffer of the requested number of vertices (ibufCount) 
		Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
			createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, numIndices, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
 
		/// Upload the index data to the card
		ibuf->writeData(0, (ibuf->getIndexSize() * mNumIndices), mIndices, true);
 
		/// Set parameters of the submesh
		sub->indexData->indexBuffer = ibuf;
		sub->indexData->indexCount = mNumIndices;

		/// Set bounding information (for culling)
		mMeshPtr->_setBounds(Ogre::AxisAlignedBox(0, 0, 0, Ogre::Real(ChunkSizeX), Ogre::Real(mHighestCube), Ogre::Real(ChunkSizeZ)));
		mMeshPtr->_setBoundingSphereRadius((Ogre::Real) std::sqrt((float) (2 * 16 * 16 + 128 * 128)));
 
		/// Notify -Mesh object that it has been loaded
		mMeshPtr->load();

		mVertexBufferCreated = true;
	} else {
		Ogre::VertexDeclaration* decl = mMeshPtr->sharedVertexData->vertexDeclaration;

		mMeshPtr->sharedVertexData->vertexCount = mNumVertices;
		Ogre::SubMesh* sub = mMeshPtr->getSubMesh(0);
		sub->indexData->indexCount = mNumIndices;

		/// Upload the new vertex data to the card
		Ogre::HardwareVertexBufferSharedPtr vbuf = mMeshPtr->sharedVertexData->vertexBufferBinding->getBuffer(0);
		vbuf->writeData(0, (decl->getVertexSize(0) * mNumVertices), mVertices, true);

		/// Upload the index data to the card
		Ogre::HardwareIndexBufferSharedPtr ibuf = sub->indexData->indexBuffer;
		ibuf->writeData(0, (ibuf->getIndexSize() * mNumIndices), mIndices, true);

		mMeshPtr->_setBounds(Ogre::AxisAlignedBox(0, 0, 0, Ogre::Real(ChunkSizeX), Ogre::Real(mHighestCube), Ogre::Real(ChunkSizeZ)));
		mMeshPtr->load();
	}
}

};
