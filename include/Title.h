//
//  Title.h
//  PixarDemo2012
//
//  Created by Colin Thompson on 10/10/12.
//
//

#ifndef Title_h
#define Title_h

#pragma once

#include "cinder/Utilities.h"

#include "Resources.h"

#include "cinder/Camera.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Path2d.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"

#include "BranchController.h"
#include "Branch.h"

using namespace ci;
using namespace cinder;
using namespace std;

#define theTitle Title::getInstance()



class Title {
public:
    
	static Title& getInstance() {
		if (!s_Title) s_Title = new Title();
		return *s_Title;
	};

    ci::CameraPersp	cubesCamera;
    
    void Init();
    void CreateMesh();
    void Render();
    void Update();
    void BindShaders();
    void Shutdown();

protected:
    //! singleton instance
	static Title *s_Title;
    Title() ;
    ~Title() ;
    
private:

    
    //uv texture billboard
	gl::Texture mTexture;

    BranchController branches;
    
    Vec3f mNextCamPoint;
    float mLerper;

    cairo::SurfaceImage mySurface;    
    
	ci::gl::VboMesh::Layout mVboLayout;
	std::vector<ci::Vec3f> mVboVertices;
	ci::gl::VboMesh	mVboMesh;
    
    Rand mRand;
	Perlin mPerlin;
    
    double titleTimer;
    
    gl::GlslProg mTitleShader;
	const char* mTitleVertex;
	const char* mTitleFrag;
	const char* mTitleGeom;
    
};



#endif
