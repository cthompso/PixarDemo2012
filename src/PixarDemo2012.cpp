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

#include "ParticleController.h"
#include "Particle.h"

#include "cinder/Rand.h"

#include "Title.h"
#include "MindField.h"
#include "Cubes.h"
#include "Cloth.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class PixarDemo2012 : public ci::app::AppBasic
{

    enum tSequences {
        kTitle = 0,
        kCircles,
        kCubes,
        kCloth,
        kMindField,
        kMaxSeq,
    };
public:

	void draw();
	void setup();
	void shutdown();
	void update();
    void keyDown( KeyEvent event);
	void mouseDown( MouseEvent event );

	void renderGradientFBO();
    void renderCairoFBO();
    void bindShaders();
    void generateWaveforms();
    
    void SetSeq();
    void NextSeq();
    void drawFader();
    
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
    bool drawTitle;
    bool drawScreenUV;
    bool drawCairoFBO;
    bool drawFFT;
    bool mFullScreen;
    bool drawMindField;
    bool drawFPS;
    bool drawCubes;
    bool drawCloth;

    //balls and music stuff
    float makeBall;    
    
    // FONT
    Font mFont;
    
    ci::CameraPersp mCamera;
    
    //cairo billboard
	Perlin mPerlin;

    
    //noise billboard
	gl::Fbo mGradientFBO;
        
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
    
    int mCurrSeq;
    
    float mFadeVal;
    bool doFade;
    bool mFadeDir;
    
    std::vector<float> mCuts;
    int mCurrentCut;
    bool mAutoCut;
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
    if ( event.getChar() == 'b' ) drawCloth = !drawCloth;
    if ( event.getChar() == 'x' ) mFullScreen = !mFullScreen;
    if ( event.getChar() == 's' ) bindShaders();
    if ( event.getChar() == 'n' ) {doFade = true;printf("%f\n",mTime);}

}

void PixarDemo2012::NextSeq()
{
    mCurrSeq = mCurrSeq + 1;
    if(mCurrSeq == kMaxSeq)
    {
        mCurrSeq = 1; // skip title
    }
    SetSeq();
    
}

void PixarDemo2012::SetSeq()
{
    drawScreenUV = false;
    
    if(mCurrSeq == kTitle)
    {
        drawTitle       = true;
        drawScreenUV    = true;
        drawCairoFBO    = true;
        drawMindField   = false;
        drawFFT         = false;
        drawCubes       = false;
        drawCloth       = false;
    }
    else if(mCurrSeq == kCircles)
    {
        drawTitle       = false;
        drawScreenUV    = true;
        drawCairoFBO    = true;
        drawMindField   = false;
        drawFFT         = false;
        drawCubes       = false;
        drawCloth       = false;
    }
    else if(mCurrSeq == kCubes)
    {
        drawTitle       = false;
        drawScreenUV    = true;
        drawCairoFBO    = true;
        drawMindField   = false;
        drawFFT         = false;
        drawCubes       = true;
        drawCloth       = false;
    }
    else if(mCurrSeq == kCloth)
    {
        drawTitle       = false;
        drawScreenUV    = false;
        drawCairoFBO    = false;
        drawMindField   = false;
        drawFFT         = false;
        drawCubes       = false;
        drawCloth       = true;
    }
    else if(mCurrSeq == kMindField)
    {
        drawTitle       = false;
        drawScreenUV    = false;
        drawCairoFBO    = false;
        drawMindField   = true;
        drawFFT         = false;
        drawCubes       = false;
        drawCloth       = false;
    }
}

void PixarDemo2012::bindShaders()
{
    printf("%s\n", glGetString(GL_VERSION));
    
    theMindField.bindShaders();
    theCubes.BindShaders();
    theCloth.bindShaders();
    theTitle.BindShaders();
    
     
    string mPath = getResourcePath().generic_string();
    mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
//  mPath = "/Users/shalinkhyati/PixarDemo2012/resources";
    string mVert = mPath + "/fbo.vert";
    string mFrag = mPath + "/fbo.frag";
    mGradientVertex = mVert.c_str();
    mGradientFrag = mFrag.c_str();
    
	try {
        if (false) {
            //for shader dev
            int32_t maxGeomOutputVertices;
            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
            
            mGradientShader = gl::GlslProg( loadFile( mGradientVertex ), loadFile( mGradientFrag ) );
        } else {
            //for install
            mGradientShader = gl::GlslProg( loadResource( FBO_VERT ), loadResource( FBO_FRAG ) );
        }
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load fbo shaders" << std::endl;
	}
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


// Draw
void PixarDemo2012::draw()
{

    gl::clear( ColorA(0.0f,0.0f,0.0f,1.0f) );
   
    //render Gradient FBO
    if(!drawMindField && !drawCloth)
    {
        renderGradientFBO();
    }

    gl::setViewport( getWindowBounds() );
    gl::setMatricesWindow( getWindowSize(), true );

    gl::clear( ColorA(0.0f,0.0f,0.0f,1.0f) );
    
    // draw FBO bg
    gl::color(1.0f,1.0f,1.0f);
    gl::draw( mGradientFBO.getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
    
    if ( drawTitle ) {
        if ( mTime > 10.0f ) drawTitle = false;
        theTitle.Render();
    }

    
    // FFT stuff
    generateWaveforms();
    
    if ( drawMindField ) {
        theMindField.Render();
        drawFader();
        return;
    }
    if ( drawCubes ) {
        gl::popMatrices();
        gl::setMatrices( theCubes.cubesCamera );
        theCubes.Render();
    }    
    if ( drawCloth ) {

        theCloth.Render();
        drawFader();
        return;
    }
    
    
    // Balls
    if ( !drawCubes ) {
        gl::setMatricesWindow( getWindowSize(), true );
        if (makeBall > 0.075f) mParticleController.addParticles(1,makeBall*100.0f,mTime);
        mParticleController.draw();
    }
    if ( drawFPS ) {
        string mString;
        mString = str(boost::format("FRAMERATE: %f") % getAverageFps() );
        gl::drawString( mString, Vec2f( 10.0f, 10.0f ), Color::white(), mFont );
    }
    
    drawFader();

}

void PixarDemo2012::drawFader()
{
    gl::setViewport( getWindowBounds() );
    gl::setMatricesWindow( getWindowSize(), true );
    
    if(doFade)
    {
        glColor4f(0,0,0,mFadeVal);
        gl::drawSolidRect(getWindowBounds());
        if(mFadeDir)
        {
            mFadeVal = mFadeVal + getFrameRate() * 0.0001;
        }
        else
        {
            mFadeVal = mFadeVal - getFrameRate() * 0.0001;
        }
        if(mFadeVal > 1.0)
        {
            mFadeVal = 1.0;
            NextSeq();
            mFadeDir = !mFadeDir;
        }
        if(mFadeVal < 0.0)
        {
            mFadeVal = 0.0;
            mFadeDir = !mFadeDir;
            doFade = false;
        }
    }
}

void PixarDemo2012::generateWaveforms()
{
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
        
        makeBall = 0.0f;
        float greatestFreq = 0.0;
		// Iterate through data
		for ( int32_t i = 0; i < dataSize; i++ ) {
            
			// Do logarithmic plotting for frequency domain
			float logSize = math<float>::log( dataSizef );
			float x = (float)( ( math<float>::log( (float)i ) / logSize ) * dataSizef );
			float y = math<float>::clamp( freqData[ i ] * ( x / dataSizef ) * ( math<float>::log( ( dataSizef - (float)i ) ) ), 0.0f, 2.0f );
            
            if (y > makeBall) makeBall = y;
            
            if(y > greatestFreq)
                greatestFreq = y;
           
            
			// Plot points on lines for tme domain
			freqLine.push_back( Vec2f(        x * scale + 10.0f,            -y * ( windowHeight - 20.0f ) * 1.75f + ( windowHeight - 10.0f ) ) );
			timeLine.push_back( Vec2f( (float)i * scale + 10.0f, timeData[ i ] * ( windowHeight - 20.0f ) * 0.25f + ( windowHeight * 0.25f + 10.0f ) ) );
            
		}
        //printf("%f\n", greatestFreq);
         theMindField.SetAmps(greatestFreq);
		// Draw signals
        if ( drawFFT ) {
            gl::draw( freqLine );
            gl::draw( timeLine );
        }
        
	}
    
}

// Set up
void PixarDemo2012::setup()
{

    // BASIC SETUP
    mTime           = 0.0f;
    drawTitle       = true;
    drawScreenUV    = true;
    drawCairoFBO    = true;
    drawMindField   = false;
    drawFFT         = false;
    mFullScreen     = true;
    drawFPS         = false;
    drawCubes       = false;
    drawCloth       = false;
    
	setFrameRate( 60.0f );
 //setFullScreen(true);
setWindowSize( 1000, 600 );

//    mFont = Font( loadResource("Calibri.ttf"), 18.0f );
    mFont = Font( loadResource("synchro.ttf"), 18.0f );
    
	// LOAD AUDIO
	mAudioSource = audio::load( loadResource("diva.m4a") );
	mTrack = audio::Output::addTrack( mAudioSource, false );
	mTrack->enablePcmBuffering( true );
    mTrack->play();
    
    // SHADERS
    bindShaders();
    
    // PARTICLES
	mParticleController = ParticleController();

    // FBOs
	gl::Fbo::Format format;
	mGradientFBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);

    mCurrSeq = kTitle;
    SetSeq();
    doFade = false;
    mFadeVal = 0.0f;
    mFadeDir = true;
    
    mCurrentCut = 0;

    mCuts.push_back(4.0); // title
    mCuts.push_back(mCuts[mCuts.size()-1]+13.0); // balls
    mCuts.push_back(mCuts[mCuts.size()-1]+15.0); // cubes
    mCuts.push_back(mCuts[mCuts.size()-1]+20.0); // cloth
    mCuts.push_back(mCuts[mCuts.size()-1]+20.0); // mind
    mCuts.push_back(mCuts[mCuts.size()-1]+10.0); // balls
    mCuts.push_back(mCuts[mCuts.size()-1]+26.0); // cubes
    mCuts.push_back(mCuts[mCuts.size()-1]+26.0); // cloth
    
    

    mAutoCut = true;
    // SCENES
    theCubes.Init();
    
    theTitle.Init();
    
    theMindField.Init();
    
    theCloth.Init();
}

void PixarDemo2012::shutdown() 
{
	// Stop track
	mTrack->enablePcmBuffering( false );
	mTrack->stop();
	if ( mFft ) {
		mFft->stop();
	}

    theTitle.Shutdown();
    theCloth.Shutdown();
    theCubes.Shutdown();
    theMindField.Shutdown();    
}

void PixarDemo2012::update()
{
    if ( mFullScreen != isFullScreen() ) {
        setFullScreen(mFullScreen);
//        mCamera->setAspectRatio(getWindowAspectRatio());
//        mySurface = cairo::SurfaceImage(getWindowWidth(),getWindowHeight(),true);
		if ( isFullScreen() == true ) {
			hideCursor();
		} else {
			showCursor();
		}        
    }
    
    mTime += 0.01f;
    float kFudge = 0.1; // ten times our step
    
    if(mAutoCut && mTime > mCuts[mCurrentCut] && mTime < mCuts[mCurrentCut] + kFudge )
    {
        doFade = true;
        mCurrentCut = mCurrentCut + 1;
        if(mCurrentCut >= mCuts.size())
        {
            mCurrentCut = mCuts.size()-1;
        }

    }
    mParticleController.update();
  
    if ( drawTitle ) {
        theTitle.Update();
    }
    
    if ( drawCloth ) {
        theCloth.Update(  );
    }
    
    if ( drawCubes ) {
        theCubes.Update();
    }
    
    if ( drawMindField ) {
        theMindField.Update();
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
