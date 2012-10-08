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
    
};

// MOUSE & KEY CONTROL
void PixarDemo2012::mouseDown( MouseEvent event )
{
}
void PixarDemo2012::keyDown( KeyEvent event )
{
    if ( event.getChar() == 'u' ) drawScreenUV = !drawScreenUV;
    if ( event.getChar() == 'c' ) drawCairoFBO = !drawCairoFBO;
    if ( event.getChar() == 'f' ) drawFFT = !drawFFT;
    if ( event.getChar() == 'x' ) mFullScreen = !mFullScreen;
    if ( event.getChar() == 's' ) bindShaders();
}

void PixarDemo2012::bindShaders()
{
    
    string mPath = getResourcePath().generic_string();
    mPath = "/Users/colin/Dropbox/cinder_projects/PixarDemo2012/resources";
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
            mCubesShader = gl::GlslProg( loadFile( mCubesVertex ), loadFile( mCubesFrag) , loadFile( mCubesGeom ), GL_POINTS, GL_TRIANGLE_STRIP, maxGeomOutputVertices );
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
    
    int32_t mMeshWidth = 4;
    //int32_t mMeshLength = 2;
	float delta = 0.001f;
	float theta = 0.0f;
	for (float x = 0.0f; x < mMeshWidth; x++) {
        //		for (int32_t y = 0; y < mMeshLength; y++)
		{
			//mVboIndices.push_back(x * mMeshLength + y);
			//mVboTexCoords.push_back(Vec2f((float)x / (float)mMeshWidth, (float)y / (float)mMeshLength));
			//Vec3f position((float)x - (float)mMeshWidth * 0.01f, (float)y - (float)mMeshLength * 0.01f, 0.0f);
            cout << "creating mesh" << endl;
            Vec3f position((float)x * 0.0f, (float)x * 0.0f, 0.0f);
			mVboVertices.push_back(position);
			theta += delta;
		}
    }
	// Create VBO
	if (mVboMesh)
		mVboMesh.reset();
    mVboMesh = ci::gl::VboMesh(mVboVertices.size(),mVboIndices.size(), mVboLayout, GL_POINTS);
	//mVboMesh.bufferIndices(mVboIndices);
	mVboMesh.bufferPositions(mVboVertices);
	//mVboMesh.bufferTexCoords2d(0, mVboTexCoords);
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
    gl::clear( ColorAf::gray( 0.0f ) );
    
    // draw FBO bg
    gl::setMatricesWindow( getWindowSize(), true );
    gl::color(1.0f,1.0f,1.0f);
    gl::draw( mGradientFBO.getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
    
    // reset camera for geometry
    gl::popMatrices();
    gl::setMatrices( mCamera );
    
    // draw VBO
    mCubesShader.bind();
    gl::draw( mVboMesh );
    mCubesShader.unbind();
    gl::popMatrices();
    
    gl::setMatricesWindow( getWindowSize(), true );
    
    // draw UV texture billboard
    if ( drawScreenUV ) {
        gl::color(1.0f, 1.0f, 1.0f,0.5f);
        mTexture.enableAndBind();
        gl::drawSolidRect(getWindowBounds());
        mTexture.unbind();
        //mGradientFBO.bindTexture();
        //gl::drawSolidRect(getWindowBounds());
        //mGradientFBO.unbindTexture();
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
        if (make > 0.075f) mParticleController.addParticles(1,make*80.0f,mTime);
        mParticleController.draw();
        mParticleController.update();
	}

}

// Set up
void PixarDemo2012::setup()
{

    // BASIC SETUP
    mTime           = 0.0f;
    drawScreenUV    = true;
    drawCairoFBO    = true;
    drawFFT         = false;
    mFullScreen     = false;
    
	setFrameRate( 60.0f );
	setWindowSize( 600, 600 );

	// RENDER PREFS
	gl::enable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	gl::color( ColorAf::white() );
    //    gl::enable( GL_POLYGON_SMOOTH );
    //	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    //	gl::enable( GL_NORMALIZE );
    //	gl::enableAlphaBlending();
    //	gl::enableDepthRead();
    //	gl::enableDepthWrite();
	
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
    //	mVboLayout.setStaticTexCoords2d();
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
    
    
    //CAIRO
    mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);

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
}

// Runs update logic
void PixarDemo2012::update()
{
    
    mTime += 0.01f;

    if ( mFullScreen != isFullScreen() ) {
        setFullScreen(mFullScreen);
        mCamera.setAspectRatio(getWindowAspectRatio());
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

}

// Start application
CINDER_APP_BASIC( PixarDemo2012, RendererGl )
