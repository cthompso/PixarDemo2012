//
//  Cubes.h
//  PixarDemo2012
//
//  Created by Colin Thompson on 10/10/12.
//
//

#ifndef Cubes_h
#define Cubes_h

#pragma once

#include "cinder/Utilities.h"

#include "Resources.h"

#include "cinder/gl/Vbo.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"


using namespace ci;
using namespace cinder;
using namespace std;

#define theCubes Cubes::getInstance()


class Cubes {
public:
    
	static Cubes& getInstance() {
		if (!s_Cubes) s_Cubes = new Cubes();
		return *s_Cubes;
	};

    void Init();
    void CreateMesh();
    void Render();
    void Update();
    void BindShaders();
    void Shutdown();

protected:
    //! singleton instance
	static Cubes *s_Cubes;
    Cubes() ;
    ~Cubes() ;
    
private:

	ci::gl::VboMesh::Layout mVboLayout;
	std::vector<ci::Vec3f> mVboVertices;
	ci::gl::VboMesh	mVboMesh;
    
    Rand mRand;
	Perlin mPerlin;
    
    double cubeTimer;
    
    gl::GlslProg mCubesShader;
	const char* mCubesVertex;
	const char* mCubesFrag;
	const char* mCubesGeom;
    
};


#endif
