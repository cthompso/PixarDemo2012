//
//  Cloth.h
//  PixarDemo2012
//
//  Created by ShalinKhyati on 10/9/12.
//
//

#ifndef __PixarDemo2012__Cloth__
#define __PixarDemo2012__Cloth__

//http://www.youtube.com/user/thecplusplusguy
//bullet physics tutorial 5, soft body
#include <iostream>
#include <vector>
#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

#include "cinder/TriMesh.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/AxisAlignedBox.h"
#include "cinder/Frustum.h"

#include "cinder/Camera.h"

using namespace ci;
using namespace cinder;

#define theCloth Cloth::getInstance()

struct bulletObject{
	int id;
	float r,g,b;
	bool hit;
	btRigidBody* body;
	bulletObject(btRigidBody* b,int i,float r0,float g0,float b0) : body(b),id(i),r(r0),g(g0),b(b0),hit(false) {}
};


class Cloth
{
public:

    static Cloth& getInstance() {
		if (!s_Cloth) s_Cloth = new Cloth();
		return *s_Cloth;
	};
    
    void Init(float angle = 50.0f);
    void Shutdown();
    void Update( CameraPersp& cullCam);
    void Render();
    void BuildMesh(btSoftBody* b);
    void AddBody();
    void bindShaders();
    void ResetTransform(btSoftBody* b);
    void PreRoll();
    void Respawn();
    
    btSoftRigidDynamicsWorld* world;
    btDispatcher* dispatcher;
    btCollisionConfiguration* collisionConfig;
    btBroadphaseInterface* broadphase;
    btConstraintSolver* solver;
    btSoftBodySolver* softbodySolver;
    std::vector<bulletObject*> bodies;
    //std::vector<ci::TriMesh*> meshes;
    ci::TriMesh mMesh;
    float * tex_coords ;
    
    gl::GlslProg mClothShader;
	const char* mClothVert;
	const char* mClothFrag;
    float mTime;
    	AxisAlignedBox3f	mObjectBoundingBox;
    bool currentlyculled;
protected:
	//! singleton instance
	static Cloth *s_Cloth;
    Cloth() ;
    ~Cloth() ;
    

};

#endif /* defined(__PixarDemo2012__Cloth__) */
