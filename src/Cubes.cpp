//
//  Cubes.cpp
//  PixarDemo2012
//
//  Created by Colin Thompson on 10/10/12.
//
//

#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include "Cubes.h"

using namespace ci::app;
using namespace std;

Cubes *Cubes::s_Cubes= 0;

Cubes::Cubes()
{
}
Cubes::~Cubes()
{
}

void Cubes::Init()
{
	cubesCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 0.1f, 1000.0f );
	cubesCamera.lookAt( Vec3f( 0.0f, 0.0f, -10.0f ), Vec3f::zero() );

    mNextCamPoint = mNextCamPoint = Vec3f(randFloat(-10,10), randFloat(-10,10), randFloat(-10,10));
    mLerper = 0.0f;

	mVboLayout.setStaticPositions();
    
    CreateMesh();
    BindShaders();    
}
void Cubes::CreateMesh()
{    
    int32_t numCubes = 6000;
    double limit = 200.0f;
    int32_t x = 0;
	while (x < numCubes) {
        mPerlin = Perlin(3,double(x));
        
        mPerlin.setSeed(1);
        double xx = mPerlin.fBm(randVec3f());
        mPerlin.setSeed(2);
        double yy = mPerlin.fBm(randVec3f());
        mPerlin.setSeed(3);
        double zz = mPerlin.fBm(randVec3f());
        
        Vec3f position( xx*limit, yy*limit, zz*limit );
        
        if ( position.distance( Vec3f(0.0f,0.0f,0.0f) ) > 2.0f ) {
            mVboVertices.push_back(position);
            x++;            
        } else {
            continue;
        }
    }
	// Create VBO
	if (mVboMesh)
		mVboMesh.reset();
    mVboMesh = ci::gl::VboMesh(mVboVertices.size(),0, mVboLayout, GL_POINTS);
	mVboMesh.bufferPositions(mVboVertices);
	mVboMesh.unbindBuffers();
    
	// Clean up
	mVboVertices.clear();    
}
void Cubes::BindShaders()
{

    //string mPath = getResourcePath().generic_string();
    string mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
    
    string mCVert = mPath + "/cubes.vert";
    string mCFrag = mPath + "/cubes.frag";
    string mCGeom = mPath + "/cubes.geom";
    mCubesVertex = mCVert.c_str();
    mCubesFrag = mCFrag.c_str();
    mCubesGeom = mCGeom.c_str();
    //    printf("vert is: %s\n",mGradientVertex);
    //    printf("frag is: %s\n",mGradientFrag);
	try {
        if (true) {
            //for shader dev
            int32_t maxGeomOutputVertices;
            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
            
            mCubesShader = gl::GlslProg( loadFile( mCubesVertex ), loadFile( mCubesFrag) , loadFile( mCubesGeom ), GL_POINTS, GL_TRIANGLE_STRIP, 37);//maxGeomOutputVertices );
        } else {
            //for install
            mCubesShader = gl::GlslProg( loadResource( mCubesVertex ), loadResource( mCubesFrag) , loadFile( mCubesGeom ), GL_POINTS, GL_TRIANGLE_STRIP, 37);//maxGeomOutputVertices );
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
void Cubes::Render()
{
        gl::setMatrices( cubesCamera );
        gl::enableDepthRead();
        gl::enableDepthWrite();
        gl::enable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        mCubesShader.bind();
        mCubesShader.uniform("mTime", (float)cubeTimer);
        gl::draw( mVboMesh );
        mCubesShader.unbind();
        gl::disable(GL_CULL_FACE);
        gl::disableDepthRead();
        gl::disableDepthWrite();
}
void Cubes::Update()
{
    cubeTimer += 0.1f;

    Vec3f pos = cubesCamera.getEyePoint();
    
    Vec3f newPos = pos.lerp(mLerper, mNextCamPoint);
    cubesCamera.setEyePoint(newPos);
    cubesCamera.setCenterOfInterestPoint(Vec3f(0,0,0));
    
    mLerper = mLerper + 0.0000001* getFrameRate();
    if(mLerper >= 1.0)
    {
        mLerper = 0.0;
        mNextCamPoint = Vec3f(randFloat(-10,10), randFloat(-10,10), randFloat(-10,10));
    }
    
    if(pos.distance(mNextCamPoint) < 5)
    {
        mLerper = 0.0;
        mNextCamPoint = Vec3f(randFloat(-10,10), randFloat(-10,10), randFloat(-10,10));
    }
    
}
void Cubes::Shutdown()
{
	if (mCubesShader)
		mCubesShader.reset();
	mVboVertices.clear();
	if (mVboMesh)
		mVboMesh.reset();    
}