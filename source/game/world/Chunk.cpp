#include "stdafx.h"

#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World& world, wCoord xPos, wCoord yPos, wCoord zPos)
	: mLevel(world), mChunkEntity(0), mChunkScene(0), mSceneAttached(false), mMeshGenerated(false),
	  mPhysicsIvArray(0), mPhysicsShape(0), mPhysicsBody(0), mPhysicsAttached(false),
	  x(xPos), y(yPos), z(zPos), heighestCube(ChunkSizeY - 1), isEmpty(false),
	  mNumVertices(0), mNumIndices(0), mVertices(0), mIndices(0), mIsModified(true), mVertexBufferCreated(false)
{
	std::ostringstream chunkNameStream;
	chunkNameStream << "chunk" << x << "_" << y << "_" << z;
	mChunkName.assign(chunkNameStream.str()); 

	memset(&blocks, 0, sizeof(blocks));
	memset(&blocksData, 0, sizeof(blocksData));
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


Ogre::String& Chunk::getChunkName()
{
	return mChunkName;
}

uint32_t Chunk::getHighestCube(uint8_t xPos, uint8_t zPos)
{
	for (uint8_t yPos = 127; yPos != 0; yPos--) {
		if (blocks[xPos][zPos][yPos] != 0) {
			return yPos;
		}
	}

	return 127;
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
			Ogre::Real(x * Chunk::ChunkSizeX), 
			Ogre::Real(y * Chunk::ChunkSizeY), 
			Ogre::Real(z * Chunk::ChunkSizeZ));
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
			(btScalar) (x * Chunk::ChunkSizeX), 
			(btScalar) (y * Chunk::ChunkSizeY), 
			(btScalar) (z * Chunk::ChunkSizeZ))); 
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
	Chunk* leftChunk	= mLevel.getChunk(x - 1, y    , z    );
	Chunk* rightChunk	= mLevel.getChunk(x + 1, y    , z    );
	Chunk* backChunk	= mLevel.getChunk(x	   , y    , z - 1);
	Chunk* frontChunk	= mLevel.getChunk(x	   , y    , z + 1);
	Chunk* bottomChunk	= mLevel.getChunk(x	   , y - 1, z    );
	Chunk* topChunk		= mLevel.getChunk(x	   , y + 1, z    );

	for (uint8_t xLocal = 0; xLocal < Chunk::ChunkSizeX; xLocal++) 
	{
		for (uint8_t zLocal = 0; zLocal < Chunk::ChunkSizeZ; zLocal++)
		{
			for (uint8_t yLocal = 0; yLocal < Chunk::ChunkSizeY; yLocal++)
			{
				uint8_t cubeType = blocks[xLocal][zLocal][yLocal];
				if (cubeType != 0)
				{

					CubeFace* cFace = 0;
					if (xLocal == 0) {
						if ((leftChunk != 0) && (leftChunk->blocks[Chunk::ChunkSizeX - 1][zLocal][yLocal] == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, LEFT, cubeType));
						}
						if (blocks[xLocal + 1][zLocal][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, RIGHT, cubeType));
						}
					} else if (xLocal == Chunk::ChunkSizeX - 1) {
						if (blocks[xLocal - 1][zLocal][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, LEFT, cubeType));
						}
						if ((rightChunk != 0) && (rightChunk->blocks[0][zLocal][yLocal] == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, RIGHT, cubeType));
						}
					} else {
						if (blocks[xLocal - 1][zLocal][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, LEFT, cubeType));
						}
						if (blocks[xLocal + 1][zLocal][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, RIGHT, cubeType));
						}
					}

					if (zLocal == 0) {
						if ((backChunk != 0) && (backChunk->blocks[xLocal][Chunk::ChunkSizeZ - 1][yLocal] == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BACK, cubeType));
						}
						if (blocks[xLocal][zLocal + 1][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, FRONT, cubeType));
						}
					} else if (zLocal == Chunk::ChunkSizeZ - 1) {
						if (blocks[xLocal][zLocal - 1][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BACK, cubeType));
						}
						if ((frontChunk != 0) && (frontChunk->blocks[xLocal][0][yLocal] == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, FRONT, cubeType));
						}
					} else {
						if (blocks[xLocal][zLocal - 1][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BACK, cubeType));
						}
						if (blocks[xLocal][zLocal + 1][yLocal] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, FRONT, cubeType));
						}
					}

					if (yLocal == 0) {
						if ((bottomChunk != 0) && (bottomChunk->blocks[xLocal][zLocal][Chunk::ChunkSizeY - 1] == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BOTTOM, cubeType));
						}
						if (blocks[xLocal][zLocal][yLocal + 1] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, TOP, cubeType));
						}
					} else if (yLocal == Chunk::ChunkSizeY - 1) {
						if (blocks[xLocal][zLocal][yLocal - 1] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BOTTOM, cubeType));
						}
						if ((topChunk != 0) && (topChunk->blocks[xLocal][zLocal][0] == 0)) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, TOP, cubeType));
						}
					} else {
						if (blocks[xLocal][zLocal][yLocal - 1] == 0) {
							visFaces.push_back(new CubeFace(xLocal, yLocal, zLocal, BOTTOM, cubeType));
						}
						if (blocks[xLocal][zLocal][yLocal + 1] == 0) {
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
		mMeshPtr->_setBounds(Ogre::AxisAlignedBox(0, 0, 0, Ogre::Real(Chunk::ChunkSizeX), Ogre::Real(heighestCube), Ogre::Real(Chunk::ChunkSizeZ)));
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

		mMeshPtr->_setBounds(Ogre::AxisAlignedBox(0, 0, 0, Ogre::Real(Chunk::ChunkSizeX), Ogre::Real(heighestCube), Ogre::Real(Chunk::ChunkSizeZ)));
		mMeshPtr->load();
	}
}