//
//  Title.cpp
//  PixarDemo2012
//
//  Created by Colin Thompson on 10/10/12.
//
//

#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include "Title.h"

using namespace ci::app;
using namespace std;



Title *Title::s_Title= 0;

Title::Title()
{
}
Title::~Title()
{
}

void Title::Init()
{
    titleTimer = 0.0f;
    BindShaders();
	branches = BranchController();
    //CAIRO
    mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);
    mTexture = loadImage( loadResource("logo.png") );    
}
void Title::BindShaders()
{

    //string mPath = getResourcePath().generic_string();
    string mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
    
    string mCVert = mPath + "/cubes.vert";
    string mCFrag = mPath + "/cubes.frag";
    string mCGeom = mPath + "/cubes.geom";
    mTitleVertex = mCVert.c_str();
    mTitleFrag = mCFrag.c_str();
    mTitleGeom = mCGeom.c_str();
    //    printf("vert is: %s\n",mGradientVertex);
    //    printf("frag is: %s\n",mGradientFrag);
	try {
        if (true) {
            //for shader dev
            int32_t maxGeomOutputVertices;
            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
            
            mTitleShader = gl::GlslProg( loadFile( mTitleVertex ), loadFile( mTitleFrag) , loadFile( mTitleGeom ), GL_POINTS, GL_TRIANGLE_STRIP, 37);//maxGeomOutputVertices );
        } else {
            //for install
            mTitleShader = gl::GlslProg( loadResource( mTitleVertex ), loadResource( mTitleFrag) , loadFile( mTitleGeom ), GL_POINTS, GL_TRIANGLE_STRIP, 37);//maxGeomOutputVertices );
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
void Title::Render()
{
//    mPerlin = Perlin(10);
//    //gl::color(1.0f, 1.0f, 1.0f,1.0f);
//    
//    float numBalls = 10.0f + (titleTimer * 30.0f);
//    float step = sqrt( (float(getWindowWidth()) * float(getWindowHeight())) / numBalls ) ;
//    float halfStep = step / 2.0f;
//    Color ballColor = Color(1.0f,1.0f,1.0f);
//    cairo::Context ctx(mySurface);
//    
//    //clear out background
//    ctx.setSourceRgba(0.0, 0.0, 0.0, 0.0);
//    ctx.setOperator(0); //clear operator
//    ctx.rectangle(getWindowBounds());
//    ctx.fill();
//    ctx.setOperator(2); //over operator
//    
//    //draw circles to screen grid
//    mPerlin.setSeed(1.0f);
//    float noiseVal = 0.0f;
//    for ( float curY = 0.0f; curY < getWindowHeight() - step; curY += step) {
//        for ( float curX = 0.0f; curX < getWindowWidth() - step; curX += step) {
//            noiseVal += (mPerlin.fBm(((curY*1.0f)+(curX*0.005f))) + 1.0f) * 0.5f;
//            ballColor.set(CM_HSV,Vec3f(fmod(noiseVal*0.4f,1.0f),
//                                       1.0f,
//                                       1.0f));
//            //                printf("noiseVal: %f\n",noiseVal);
//            ctx.setSourceRgba(ballColor.r, ballColor.g, ballColor.b, 1.0f);
//            ctx.circle(curX+halfStep,curY+halfStep,halfStep);
//            ctx.fill();
//        }
//    }
//    
//    
//    gl::Texture cairoTexture(mySurface.getSurface());
//    cairoTexture.enableAndBind();
//    gl::drawSolidRect(getWindowBounds());
//    cairoTexture.unbind();
//    
//    float make = 0.0f;
//    if (make > 0.075f) branches.addBranches(1,make*100.0f,titleTimer);
//    branches.draw();
    
//    gl::clear( ColorAf::gray( 0.2f ) );
    gl::setViewport( getWindowBounds() );
    gl::setMatricesWindow( getWindowSize(), true );
    
    mTexture.enableAndBind();
    float height = getWindowWidth()/4;
    float halfH = height/2;
    height += halfH/2;
    gl::drawSolidRect( Rectf(0.0f,height-halfH,getWindowWidth(),height+halfH) );
    mTexture.unbind();
}
void Title::Update()
{
    titleTimer += 0.1f;
    branches.update();

//    if ( mFullScreen != isFullScreen() ) {
//        setFullScreen(mFullScreen);
//        //mCamera->setAspectRatio(getWindowAspectRatio());
//        mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);
//    }
    
}
void Title::Shutdown()
{
}
