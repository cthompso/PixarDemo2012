#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/CinderMath.h"
#include "cinder/Utilities.h"

#include "cinder/Camera.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"

#include "cinder/Surface.h"

#include "Resources.h"

#include "KissFFT.h"

#include "cinder/Path2d.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"

#include "ParticleController.h"
#include "Particle.h"

#include "cinder/Rand.h"


#include "MindField.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class PixarDemo2012 : public ci::app::AppBasic
{

public:

	void						draw();
	void						setup();
	void						shutdown();
	void						update();
    void                        keyDown( KeyEvent event);
	void                        mouseDown( MouseEvent event );

    void createMesh();
	void renderGradientFBO();
    void renderCairoFBO();
    void bindShaders();
    
private:

	// Audio file
	ci::audio::SourceRef		mAudioSource;
	ci::audio::PcmBuffer32fRef	mBuffer;
	ci::audio::TrackRef			mTrack;

	// Analyzer
	KissRef						mFft;
    
    //memory spheres
    ParticleController          mParticleController;
    
    double mTime;
    bool drawScreenUV;
    bool drawCairoFBO;
    bool drawFFT;
    bool mFullScreen;
    bool drawMindField;
    bool drawFPS;
    bool drawCubes;
    
    // FONT
    Font mFont;
    
    ci::CameraPersp				mCamera;
    
    //uv texture billboard
	gl::Texture mTexture;
    
    //cairo billboard
	Perlin mPerlin;
    cairo::SurfaceImage mySurface;
    
    //noise billboard
	gl::Fbo mGradientFBO;
    
    // vbo mesh
    ci::gl::VboMesh     mCylinder;
    ci::gl::VboMesh     mCubes;
	ci::gl::VboMesh::Layout mCubesLayout;
    std::vector<uint32_t> mVboIndices;
	ci::gl::VboMesh::Layout mVboLayout;
	std::vector<ci::Vec2f> mVboTexCoords;
	std::vector<ci::Vec3f> mVboVertices;
	ci::gl::VboMesh	mVboMesh;
    Rand mRand;
    
    
    //shaders
    
    // noise billboard shaders
    gl::GlslProg mGradientShader;
	const char* mGradientVertex;
	const char* mGradientFrag;
    
    // vbo shaders
    gl::GlslProg mCubesShader;
	const char* mCubesVertex;
	const char* mCubesFrag;
	const char* mCubesGeom;
    
    Vec3f mNextCamPoint;
    float mLerper;
    
  };

// MOUSE & KEY CONTROL
void PixarDemo2012::mouseDown( MouseEvent event )
{
}
void PixarDemo2012::keyDown( KeyEvent event )
{
    if ( event.getChar() == 'u' ) drawScreenUV = !drawScreenUV;
    if ( event.getChar() == 'm' ) drawMindField = !drawMindField;
    if ( event.getChar() == 'c' ) drawCairoFBO = !drawCairoFBO;
    if ( event.getChar() == 'f' ) drawFFT = !drawFFT;
    if ( event.getChar() == 't' ) drawFPS = !drawFPS;
    if ( event.getChar() == 'v' ) drawCubes = !drawCubes;
    if ( event.getChar() == 'x' ) mFullScreen = !mFullScreen;
    if ( event.getChar() == 's' ) bindShaders();

}

void PixarDemo2012::bindShaders()
{
    printf("%s\n", glGetString(GL_VERSION));   
    theMindField.bindShaders();
    string mPath = getResourcePath().generic_string();
    mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
    string mVert = mPath + "/fbo.vert";
    string mFrag = mPath + "/fbo.frag";
    mGradientVertex = mVert.c_str();
    mGradientFrag = mFrag.c_str();
    
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
            
            mGradientShader = gl::GlslProg( loadFile( mGradientVertex ), loadFile( mGradientFrag ) );
            mCubesShader = gl::GlslProg( loadFile( mCubesVertex ), loadFile( mCubesFrag) , loadFile( mCubesGeom ), GL_POINTS, GL_TRIANGLE_STRIP, 37);//maxGeomOutputVertices );
        } else {
            //for install
            mGradientShader = gl::GlslProg( loadResource( mGradientVertex ), loadResource( mGradientFrag ) );
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


void PixarDemo2012::createMesh()
{
    
    int32_t numCubes = 6000;
    //int32_t mMeshLength = 2;
	float delta = 0.001f;
	float theta = 0.0f;
    double limit = 200.0f;
    
	for (int32_t x = 0; x < numCubes; x++) {
        //		for (int32_t y = 0; y < mMeshLength; y++)
		{
			//mVboIndices.push_back(x * mMeshLength + y);
			//mVboTexCoords.push_back(Vec2f((float)x / (float)mMeshWidth, (float)y / (float)mMeshLength));
			//Vec3f position((float)x - (float)mMeshWidth * 0.01f, (float)y - (float)mMeshLength * 0.01f, 0.0f);
            mPerlin = Perlin(3,double(x));

            mPerlin.setSeed(1);
            double xx = mPerlin.fBm(randVec3f());
            mPerlin.setSeed(2);
            double yy = mPerlin.fBm(randVec3f());
            mPerlin.setSeed(3);
            double zz = mPerlin.fBm(randVec3f());
            Vec3f position( xx*limit, yy*limit, zz*limit );
			mVboVertices.push_back(position);
            mVboTexCoords.push_back(Vec2f(x/numCubes,x/numCubes));
			theta += delta;
		}
    }
	// Create VBO
	if (mVboMesh)
		mVboMesh.reset();
    mVboMesh = ci::gl::VboMesh(mVboVertices.size(),mVboIndices.size(), mVboLayout, GL_POINTS);
//	mVboMesh.bufferIndices(mVboIndices);
	mVboMesh.bufferPositions(mVboVertices);
	mVboMesh.bufferTexCoords2d(mVboTexCoords.size(), mVboTexCoords);
	mVboMesh.unbindBuffers();
    
	// Clean up
	mVboIndices.clear();
	mVboTexCoords.clear();
	mVboVertices.clear();
    
}

void PixarDemo2012::renderGradientFBO()
{
    gl::enableAlphaBlending();
    
    gl::SaveFramebufferBinding bindingSaver;
    
    mGradientFBO.bindFramebuffer();
    
    gl::setViewport( mGradientFBO.getBounds() );
    
    gl::setMatricesWindow( getWindowSize(), true );
    
    gl::clear( ColorA(0.0f,0.0f,0.0f,1.0f) );
    
	glDisable( GL_TEXTURE_2D );
    
    //    mGradientFBO.bindTexture(0);
    mGradientShader.bind();
    float what = mTime;
    mGradientShader.uniform("mTime", what);
    mGradientShader.uniform("resolution", Vec2f((float)getWindowWidth(),(float)getWindowHeight()));
    gl::drawSolidRect( getWindowBounds() );
    mGradientShader.unbind();
    //    mGradientFBO.unbindTexture();
    
    gl::popMatrices();
}

void PixarDemo2012::renderCairoFBO() {
    
    mPerlin = Perlin(10);
    
    float numBalls = 10.0f + (mTime * 30.0f);
    float step = sqrt( (float(getWindowWidth()) * float(getWindowHeight())) / numBalls ) ;
    float halfStep = step / 2.0f;
    Color ballColor = Color(1.0f,1.0f,1.0f);
    cairo::Context ctx(mySurface);
    
    //clear out background
    ctx.setSourceRgba(0.0, 0.0, 0.0, 0.0);
    ctx.setOperator(0); //clear operator
    ctx.rectangle(getWindowBounds());
    ctx.fill();
    ctx.setOperator(2); //over operator
    
    //draw circles to screen grid
    mPerlin.setSeed(1.0f);
    float noiseVal = 0.0f;
    for ( float curY = 0.0f; curY < getWindowHeight() - step; curY += step) {
        for ( float curX = 0.0f; curX < getWindowWidth() - step; curX += step) {
            noiseVal += (mPerlin.fBm(((curY*1.0f)+(curX*0.005f))) + 1.0f) * 0.5f;
            ballColor.set(CM_HSV,Vec3f(fmod(noiseVal*0.4f,1.0f),
                                       1.0f,
                                       1.0f));
            //                printf("noiseVal: %f\n",noiseVal);
            ctx.setSourceRgba(ballColor.r, ballColor.g, ballColor.b, 1.0f);
            ctx.circle(curX+halfStep,curY+halfStep,halfStep);
            ctx.fill();
        }
    }
    
    
    gl::Texture cairoTexture(mySurface.getSurface());
    cairoTexture.enableAndBind();
    gl::drawSolidRect(getWindowBounds());
    cairoTexture.unbind();
    
}

// Draw
void PixarDemo2012::draw()
{
    //render Gradient FBO
    renderGradientFBO();
    
	// clear out the window with black
	// Set up window
    gl::setViewport( getWindowBounds() );
	gl::setMatrices( mCamera );
	gl::clear( ColorAf::gray( 0.2f ) );
    
    if ( drawMindField ) {
    	gl::enableDepthRead();
        gl::enableDepthWrite();        
        theMindField.Render();
        gl::disableDepthRead();
        gl::disableDepthWrite();

        if ( drawFPS ) {
            gl::setMatricesWindow( getWindowSize(), true );
            string mString;
            mString = str(boost::format("Framerate: %f") % getAverageFps() );
            gl::drawString( mString, Vec2f( 10.0f, 10.0f ), Color::white(), mFont );
        }
        
        return;
    }

    // draw FBO bg
    gl::setMatricesWindow( getWindowSize(), true );
    gl::color(1.0f,1.0f,1.0f);
    gl::draw( mGradientFBO.getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
    
    // reset camera for geometry
    gl::popMatrices();
    
    gl::setMatrices( mCamera );
    
    // draw VBO
    if ( drawCubes ) {
        gl::enableDepthRead();
        gl::enableDepthWrite();
        gl::enable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        
        //gl::enableAlphaBlending();
        
        mCubesShader.bind();
        mCubesShader.uniform("mTime", (float)mTime);
        gl::draw( mVboMesh );
        mCubesShader.unbind();
        //gl::draw( mVboMesh );
        
        //gl::popMatrices();

        gl::disable(GL_CULL_FACE);
        gl::disableDepthRead();
        gl::disableDepthWrite();
    }
    
    gl::setMatricesWindow( getWindowSize(), true );
    
    // draw UV texture billboard
    if ( drawScreenUV ) {
        gl::color(1.0f, 1.0f, 1.0f,0.5f);
        mTexture.enableAndBind();
        gl::drawSolidRect(getWindowBounds());
        mTexture.unbind();
    }
    
    // draw Cairo context
    if ( drawCairoFBO ) {
        gl::color(1.0f, 1.0f, 1.0f,1.0f);
        renderCairoFBO();
    }

    // DRAW MEM SPEHRES AND WAVEFORMS
	// Check init flag
	if ( mFft ) {

		// Get data in the frequency (transformed) and time domains
		float * freqData = mFft->getAmplitude();
		float * timeData = mFft->getData();
		int32_t dataSize = mFft->getBinSize();

		// Cast data size to float
		float dataSizef = (float)dataSize;

		// Get dimensions
		float scale = ( (float)getWindowWidth() - 20.0f ) / dataSizef;
		float windowHeight = (float)getWindowHeight();

		// Use polylines to depict time and frequency domains
		PolyLine<Vec2f> freqLine;
		PolyLine<Vec2f> timeLine;

        float make = 0.0f;
		// Iterate through data
		for ( int32_t i = 0; i < dataSize; i++ ) {

			// Do logarithmic plotting for frequency domain
			float logSize = math<float>::log( dataSizef );
			float x = (float)( ( math<float>::log( (float)i ) / logSize ) * dataSizef );
			float y = math<float>::clamp( freqData[ i ] * ( x / dataSizef ) * ( math<float>::log( ( dataSizef - (float)i ) ) ), 0.0f, 2.0f );
            
            if (y > make) make = y;

			// Plot points on lines for tme domain
			freqLine.push_back( Vec2f(        x * scale + 10.0f,            -y * ( windowHeight - 20.0f ) * 1.75f + ( windowHeight - 10.0f ) ) );
			timeLine.push_back( Vec2f( (float)i * scale + 10.0f, timeData[ i ] * ( windowHeight - 20.0f ) * 0.25f + ( windowHeight * 0.25f + 10.0f ) ) );

		}

		// Draw signals
        if ( drawFFT ) {
            gl::draw( freqLine );
            gl::draw( timeLine );
        }
        gl::color(1.0f, 1.0f, 1.0f);
        if (make > 0.075f) mParticleController.addParticles(1,make*100.0f,mTime);
        mParticleController.draw();

	}
    
    if ( drawFPS ) {
        string mString;
        mString = str(boost::format("Framerate: %f") % getAverageFps() );
        gl::drawString( mString, Vec2f( 10.0f, 10.0f ), Color::white(), mFont );
    }
}

// Set up
void PixarDemo2012::setup()
{

    // BASIC SETUP
    mTime           = 0.0f;
    drawScreenUV    = false;
    drawCairoFBO    = false;
    drawMindField   = false;
    drawFFT         = false;
    mFullScreen     = false;
    drawFPS         = true;
    drawCubes       = true;
    
    mNextCamPoint = mNextCamPoint = Vec3f(randFloat(-10,10), randFloat(-10,10), randFloat(-10,10));
    mLerper = 0.0f;
    
	setFrameRate( 60.0f );
	setWindowSize( 1000, 600 );

    mFont = Font( loadResource("Calibri.ttf"), 18.0f );
    
	// RENDER PREFS
	//gl::enable( GL_LINE_SMOOTH );
	//glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	gl::color( ColorAf::white() );
    //    gl::enable( GL_POLYGON_SMOOTH );
    //	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    //	gl::enable( GL_NORMALIZE );
    //	gl::enableAlphaBlending();
	
	// LOAD AUDIO
	mAudioSource = audio::load( loadResource("diva.m4a") );//RES_SAMPLE ) );
	mTrack = audio::Output::addTrack( mAudioSource, false );
	mTrack->enablePcmBuffering( true );
	mTrack->play();
    
    
    // PARTICLES
	mParticleController = ParticleController();

    // VBO SETUP
    //	mVboLayout.setStaticIndices();
	mVboLayout.setStaticPositions();
    mVboLayout.setStaticTexCoords2d();
    createMesh();
    
    
    // FBOs
	gl::Fbo::Format format;
	mGradientFBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);
    mTexture = loadImage( loadResource("uv.png") );
    
    // SHADERS
    bindShaders();
    
    
	// CAMERA
	mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 0.1f, 1000.0f );
	mCamera.lookAt( Vec3f( 0.0f, 0.0f, -10.0f ), Vec3f::zero() );
    
    //mCamera.setEyePoint(Vec3f(10.0, 10.0, -10.0));
    //mCamera.setCenterOfInterestPoint(Vec3f(0,0,0));
    
    //CAIRO
    mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);
    
    theMindField.Init();

}

// Called on exit
void PixarDemo2012::shutdown() 
{

	// Stop track
	mTrack->enablePcmBuffering( false );
	mTrack->stop();
	if ( mFft ) {
		mFft->stop();
	}

    // Clean up
	if (mCubesShader)
		mCubesShader.reset();
	if (mGradientShader)
		mGradientShader.reset();
	mVboIndices.clear();
	mVboTexCoords.clear();
	mVboVertices.clear();
	if (mVboMesh)
		mVboMesh.reset();
    if (mCylinder)
        mCylinder.reset();
    
    theMindField.Shutdown();
}

// Runs update logic
void PixarDemo2012::update()
{
    
    mTime += 0.01f;

    mParticleController.update();
  
    if ( mFullScreen != isFullScreen() ) {
        setFullScreen(mFullScreen);
        mCamera.setAspectRatio(getWindowAspectRatio());
        mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);
    }
    
	// Check if track is playing and has a PCM buffer available
	if ( mTrack->isPlaying() && mTrack->isPcmBuffering() ) {

		// Get buffer
		mBuffer = mTrack->getPcmBuffer();
		if ( mBuffer && mBuffer->getInterleavedData() ) {

			// Get sample count
			uint32_t sampleCount = mBuffer->getInterleavedData()->mSampleCount;
			if ( sampleCount > 0 ) {

				// Initialize analyzer
				if ( !mFft ) {
					mFft = Kiss::create( sampleCount );
				}

				// Analyze data
				if ( mBuffer->getInterleavedData()->mData != 0 ) {
					mFft->setData( mBuffer->getInterleavedData()->mData );
				}

			}

		}

	}
    
    if ( drawMindField ) {
        theMindField.Update();
    }
        Vec3f pos = mCamera.getEyePoint();
        
        Vec3f newPos = pos.lerp(mLerper, mNextCamPoint);
        mCamera.setEyePoint(newPos);
        mCamera.setCenterOfInterestPoint(Vec3f(0,0,0));
        
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
        
//    }    
    
}

// Start application
CINDER_APP_BASIC( PixarDemo2012, RendererGl )
