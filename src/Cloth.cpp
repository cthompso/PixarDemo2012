//
//  Cloth.cpp
//  PixarDemo2012
//
//  Created by ShalinKhyati on 10/9/12.
//
//

#include "Cloth.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "MindField.h"
using namespace ci;
using namespace std;
using namespace ci::app;
Cloth *Cloth::s_Cloth= 0;
 
Cloth::Cloth()
{
    
}

Cloth::~Cloth()
{
    
}



void Cloth::Init(float angle)
{
    mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 0.1f, 1000.0f );
	mCamera.lookAt( Vec3f( 0.0f, 0.0f, -10.0f ), Vec3f::zero() );
    
    currentlyculled = true;
	mTime = 0.0;
	collisionConfig=new btSoftBodyRigidBodyCollisionConfiguration();
	dispatcher=new btCollisionDispatcher(collisionConfig);
	broadphase=new btDbvtBroadphase();
	solver=new btSequentialImpulseConstraintSolver();
	softbodySolver=new btDefaultSoftBodySolver();
	world=new btSoftRigidDynamicsWorld(dispatcher,broadphase,solver,collisionConfig,softbodySolver);
	world->setGravity(btVector3(0, -10,0));
	
    AddBody();
    

    //btTransform t;
	//t.setIdentity();
	//t.setOrigin(btVector3(0,0,0));
	   
}

void Cloth::AddBody()
{
    float s=8;
    float t = 16;
	float h= 6;
    int res_x = 60;
    int res_y = 60;
    tex_coords = new float[ (res_x - 1)*(res_y -1)*12 ];
	btSoftBody* softBody=btSoftBodyHelpers::CreatePatchUV(
                                                        world->getWorldInfo(),btVector3(-s,h,-t),btVector3(s,h,-t),
                                                        btVector3(-s,h,t),btVector3(s,h,t),res_x,res_y,0,true, tex_coords);

	softBody->m_cfg.viterations=20;
	softBody->m_cfg.piterations=8;
	softBody->setTotalMass(0.1);
        
    softBody->m_cfg.kLF			=	0.05;
    softBody->m_cfg.kDG			=	0.006;
    softBody->m_cfg.aeromodel	=	btSoftBody::eAeroModel::V_TwoSidedLiftDrag;
    
    // softBody->setWindVelocity(btVector3((randFloat() *2.0 - 1.0)* 10.0f, (randFloat()*2.0 - 1.0)* 4.0f, (randFloat()*2.0 - 1.0)* 5.0f));
	//softBody->setMass(100,100);
 
    //softBody->getCollisionShape()->setMargin(0.5);
    
    btSoftBody::Material*	pm=softBody->appendMaterial();
    pm->m_flags				-=	btSoftBody::fMaterial::DebugDraw;
    softBody->m_materials[0]->m_kLST	=	0.4;
    
  //  softBody->getCollisionShape()->setMargin(0.01);
    
    softBody->m_cfg.collisions		|=	btSoftBody::fCollision::VF_SS;
    softBody->m_cfg.collisions		|=	btSoftBody::fCollision::CL_SELF;
    
    //    softBody->randomizeConstraints();
  // softBody->generateClusters(16);
    
    softBody->generateBendingConstraints(2,pm);
    
   // softBody->m_cfg.kLF			=	0.004;
    //softBody->m_cfg.kDG			=	0.0003;
//    softBody->m_cfg.aeromodel	=	btSoftBody::eAeroModel::V_TwoSided;

   // softBody->addForce(btVector3(0,2,0),0);
   // softBody->setWindVelocity(btVector3(1,3,-1));
    
    //softBody->m_cfg.piterations=1;
	//psb->generateClusters(128);
//	softBody->generateClusters(16);
	//psb->getCollisionShape()->setMargin(0.5);
    

	//softBody->m_cfg.collisions	=	btSoftBody::fCollision::CL_SS+	btSoftBody::fCollision::CL_RS
    //+ btSoftBody::fCollision::CL_SELF
    //;
	//softBody->m_materials[0]->m_kLST=0.8;
	
    
   ResetTransform(softBody);
    
	world->addSoftBody(softBody);	//cloth
    PreRoll();
}

void Cloth::PreRoll()
{

    for(int i = 0; i < 100; i ++)
    {
        world->stepSimulation(1/60.0);
    }

}
void Cloth::ResetTransform(btSoftBody* b)
{
    btTransform		trs;
   // btQuaternion	rot;
    btVector3 pos = btVector3(5, 10, 8);
//    btVector3 pos = btVector3(5, 3, 0);
    //rot.setRotation(btVector3(1, 0, 0), btScalar(SIMD_PI/2));
    trs.setIdentity();
    trs.setOrigin(pos);
    //trs.setRotation(rot);
    b->transform(trs);

}

void Cloth::Respawn()
{
    for(int i=0;i<world->getSoftBodyArray().size();i++)
	{
		world->removeSoftBody(world->getSoftBodyArray()[i]);
		delete (world->getSoftBodyArray()[i]);
	}
    
    AddBody();
}

void Cloth::BuildMesh(btSoftBody* b)
{
    mMesh.clear();
    int tc = 0;
    for(int i=0;i<b->m_faces.size();i++)
    {
        for(int j=0;j<3;j++)
        {
            mMesh.appendVertex(
                               Vec3f(b->m_faces[i].m_n[j]->m_x.x(),
                       b->m_faces[i].m_n[j]->m_x.y(),
                       b->m_faces[i].m_n[j]->m_x.z()));
            
            mMesh.appendNormal(
                Vec3f(
                      b->m_faces[i].m_n[j]->m_n.x(),
                      b->m_faces[i].m_n[j]->m_n.y(),
                      b->m_faces[i].m_n[j]->m_n.z()
                    )
            );
            
            mMesh.appendTexCoord(Vec2f(
                                 tex_coords[tc + j*2],                             tex_coords[tc + j*2 + 1]
                                 ));
        }
                            
        int vert0 = mMesh.getNumVertices() - 3;
        int vert1 = mMesh.getNumVertices() - 2;
        int vert2 = mMesh.getNumVertices() - 1;
        mMesh.appendTriangle( vert0, vert1, vert2 );
     
        tc += 6;
    }
    
    mObjectBoundingBox = mMesh.calcBoundingBox();
}




void Cloth::Update( )
{
    mTime = mTime + getFrameRate()/60.0f;


    
    world->stepSimulation(1/60.0);
    for(int i=0;i<world->getSoftBodyArray().size();i++)
    {
        
        btSoftBody* b = world->getSoftBodyArray()[i];
    //  BuildMesh(b);
        
        float xW = 0.23 * cos(mTime*0.02 );
        float yW = 0.5 * sin(mTime * 0.04 );
        float zW = 0.15* sin(mTime*0.01);
        b->setWindVelocity(btVector3(xW, yW, zW));
        //b->setWindVelocity(btVector3(1.3, 3.9, 1.1));
        /*
        for(int i=0;i<b->m_faces.size();i++)
        {
            
            b->addAeroForceToNode(btVector3(xW, yW, zW), 10);
        }
        */
       // b->addForce(btVector3(xW, yW, zW));
       
        
        Vec3f camPos = mCamera.getCenterOfInterestPoint();
        Vec3f ps = mObjectBoundingBox.getCenter();
        camPos+=(ps-camPos)*0.05;
        mCamera.setEyePoint( camPos + ((sin(mTime*0.001) *0.5+0.5) +0.5)* 15.0 * Vec3f(0.0,0.8,-1.0));
        mCamera.setCenterOfInterestPoint( camPos);
        
        
        /*
        //! perform frustum culling **********************************************************************************
        Frustumf visibleWorld( cullCam );
    
        if(!currentlyculled)
        {
            // check if the bounding box intersects the visible world
            bool culled = ! visibleWorld.intersects( mObjectBoundingBox ) ;
            if(culled)
            {
                printf("culled\n");
               Respawn();
                PreRoll();
                currentlyculled = true;
            }
        }
        else
        {
            bool culled = ! visibleWorld.intersects( mObjectBoundingBox ) ;
            if(!culled)
            {
                currentlyculled = false;
            }
        }
         */
    }
    
    
}

void Cloth::Render()
{
    
//    gl::enableDepthRead();
//    gl::disableDepthWrite();
//    gl::enable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
   // gl::disableAlphaTest();
    
    //gl::enableAlphaBlending();
    gl::setMatrices( mCamera );
    for(int i=0;i<world->getSoftBodyArray().size();i++)
    {
        btSoftBody* b = world->getSoftBodyArray()[i];
        BuildMesh(b);
        if( mMesh.getNumTriangles() ) {
           mClothShader.bind();
            float what = mTime;
            mClothShader.uniform("mTime", what);

            gl::draw( mMesh );
            mClothShader.unbind();
        }
    }
    gl::popMatrices();
}

void Cloth::Shutdown()
{
    for(int i=0;i<world->getSoftBodyArray().size();i++)
	{
		world->removeSoftBody(world->getSoftBodyArray()[i]);
		delete (world->getSoftBodyArray()[i]);
	}
    delete dispatcher;
	delete collisionConfig;
	delete solver;
	delete broadphase;
	delete softbodySolver;
	delete world;
}


void Cloth::bindShaders()
{
    
    //string mPath = getResourcePath().generic_string();
    //string mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
   string mPath = "/Users/shalinkhyati/PixarDemo2012/resources";
    string nv = mPath + "/cloth.vert";
    string nf = mPath + "/cloth.frag";
    mClothVert = nv.c_str();
    mClothFrag = nf.c_str();
    try {
        if (true) {
            //for shader dev
            mClothShader = gl::GlslProg( loadFile( mClothVert ), loadFile( mClothFrag ) );
            
        } else {
            //for install
            //mGradientShader = gl::GlslProg( loadResource( mGradientVertex ), loadResource( mGradientFrag ) );
        }
	}
    catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
    
    
    
}
