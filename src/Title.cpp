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

using namespace ci;
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
    nameAlpha = 0.0f;
    titleAlpha = 100.0f;
    titleFade = 1.0f;
    titleTimer = 0.0f;
    BindShaders();
	branches = BranchController();
    //CAIRO
    mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);
    mTexture = loadImage( loadResource("logo.png") );
//    mTexture = loadImage( loadResource(LOGO_PNG) );
}
void Title::BindShaders()
{

    //string mPath = getResourcePath().generic_string();
    string mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
    
    string mCVert = mPath + "/title.vert";
    string mCFrag = mPath + "/title.frag";

    mTitleVertex = mCVert.c_str();
    mTitleFrag = mCFrag.c_str();

    //    printf("vert is: %s\n",mGradientVertex);
    //    printf("frag is: %s\n",mGradientFrag);
	try {
        if (true) {
            //for shader dev
            int32_t maxGeomOutputVertices;
            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
            
            mTitleShader = gl::GlslProg( loadFile( mTitleVertex ), loadFile( mTitleFrag) );
        } else {
            //for install
            mTitleShader = gl::GlslProg( loadResource( TITLE_VERT ), loadResource( TITLE_FRAG) );
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
//    clear out background
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

    gl::setViewport( getWindowBounds() );
    gl::setMatricesWindow( getWindowSize(), true );
    
//    int total = 10;
//    if (total > 0.075f) branches.addBranches(1,1.0f,titleTimer);
//    branches.draw(ctx);

    
//    gl::Texture cairoTexture(mySurface.getSurface());
//    cairoTexture.enableAndBind();
//    gl::drawSolidRect(getWindowBounds());
//    cairoTexture.unbind();
    

    Font mFont;
    mFont = Font( loadResource("synchro.ttf"), 100.0f );
    string dyams;
    dyams = str( boost::format("DANGEROUS YAMS") );
    string loscarlos;
    loscarlos = str( boost::format("LOS CARLOS") );
    if ( titleTimer > 15.5f && titleTimer < 20.0f) {
        if ( nameAlpha < 1.0f ) nameAlpha += 0.05f;
    } else if ( titleTimer > 20.5f && titleTimer < 22.7f ) {
        nameAlpha = 1.0f;
    }
    
    nameAlpha *= 0.98;
    
    gl::drawString( dyams, Vec2f( 80.0f+titleTimer, getWindowHeight()/2.0f - 40.0f ), ColorA(1.0f,1.0f,1.0f,nameAlpha), mFont );
    gl::drawString( loscarlos, Vec2f( getWindowWidth() - 900.0f - titleTimer, getWindowHeight()/2.0f + 40.0f ), ColorA(1.0f,1.0f,1.0f,nameAlpha), mFont );

//    mFbo.bindTexture(0);
//    mShader.bind();
//    mShader.uniform( "offsetAmt", offsetAmt);


    if ( titleTimer > 25.0f ) {
        titleAlpha *= 0.9f;
        if (titleTimer > 45.0f ) titleFade *= 0.99f;
//        mTexture.enableAndBind();
        gl::color( ColorA(0.0f,0.0f,0.0f,0.0f) );
        mTexture.bind(0);
        float height = getWindowWidth()/4;
        float halfH = height/2;
        height += halfH/2;
        height += 250.0f;
        mTitleShader.bind();
        mTitleShader.uniform( "tex0", 0);
        mTitleShader.uniform("mfade", (float)titleFade);
        mTitleShader.uniform("mtime", (float)titleTimer);
        mTitleShader.uniform("ease", (float)titleAlpha);
        gl::drawSolidRect( Rectf(0.0f,height-halfH,getWindowWidth(),height+halfH) );
        mTitleShader.unbind();
        mTexture.unbind();
    }
}
void Title::Update()
{
    titleTimer += 0.1f;
    branches.update();    
}
void Title::Shutdown()
{
}
