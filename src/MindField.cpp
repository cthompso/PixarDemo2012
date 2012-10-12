//
//  MindField.cpp
//  PixarDemo2012
//
//  Created by Shalin Shodhan on 10/6/12.
//
//

#include "MindField.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"


#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"

#include "cinder/Camera.h"
#include "Resources.h"

using namespace ci::app;
using namespace std;

MindField *MindField::s_MindField= 0;
//////////////////////////////////////////////////////////////////////////////////////////// Neuron START
 
Neuron::Neuron()
{
    
}

Neuron::~Neuron()
{
    
}

void Neuron::Init()
{
    t = 0.0f;
    c = 0.0f;
    mAmp = randFloat(0.0, 0.1);
    
    r = randFloat(1.0, 15.0);
    oldR = r;
    phi =randFloat(0.0, 360.0) * pi / 180.0;
    theta = randFloat(0.0, 180.0) * pi / 180.0;
    randSpeed = randFloat(0.0001, 0.008);
    randSpeed2 = randFloat(0.0001, 0.005);
    mDistVal = r /2.0;//randFloat(2.0, 6.0);
    if(mDistVal < 0.5)
    {
        mDistVal = 0.5;
    }
    
    randDirection = false;
    if(randInt(0,100)%2)
    {
        randDirection = true;
    }
    
    F  = randVec3f();
    V = Vec3f::zero();
    A = V;
    m = randFloat(1.0);
    
    pos.x = Rand::randFloat(kXLow, kXHigh);
    pos.y = Rand::randFloat(kYLow, kYHigh);
    pos.z = Rand::randFloat(kZLow, kZHigh);
    
    p1 = pos;
    
    p1.x = Rand::randFloat(kXLow, kXHigh);
    p1.y = Rand::randFloat(kYLow, kYHigh);
    p1.z = Rand::randFloat(kZLow, kZHigh);
    
    p2.x = Rand::randFloat(kXLow, kXHigh);
    p2.y = Rand::randFloat(kYLow, kYHigh);
    p2.z = Rand::randFloat(kZLow, kZHigh);

    mSphere =  Sphere(pos, Rand::randFloat(0.04, 0.15));
}

void Neuron::Shutdown()
{
    
}

void Neuron::Render()
{
   // gl::color( Color( c + 0.2f, 0.2f, 0.5f ) );
    gl::drawSphere( pos, mSphere.getRadius() );
}

void Neuron::SetBad()
{
    c = 0.8f;
}

void Neuron::Update()
{
   // t = t +  getFrameRate()/60000.0f;
    
    mAmp = mAmp - 0.00001 *getFrameRate();
    if(mAmp < 0.0)
    {
        mAmp = 0.0;
    }
/*
    if(mAmp > 0.08 && r == oldR)
    {
       // printf("here");
        oldR += randFloat(-0.1, 0.1);
        if(oldR > 15.0 )
            oldR = 15.0;
        if(oldR < -15.0)
            oldR = -15.0;
    }
  */
   // r = oldR + sin(0.0002 * getElapsedFrames());
    if(randDirection)
    {
        phi = phi + getFrameRate() *randSpeed*0.01 - theMindField.xAmp* getFrameRate()*0.00001;
        theta = theta + getFrameRate() * randSpeed2*0.01 - theMindField.xAmp* getFrameRate()*0.00001;;
    }
    else
    {
        
        phi = phi - getFrameRate() *randSpeed*0.01 + theMindField.xAmp* getFrameRate()*0.00001;
        theta = theta - getFrameRate() * randSpeed2*0.01 + theMindField.xAmp* getFrameRate()*0.00001;;
        
    }
/*    if(r != oldR)
    {
        r += (oldR - r) * 0.00001*getFrameRate();
        if( r - oldR < 0.005)
        {
            r = oldR;
        }
    }
  */
    if(phi < 0)
    {
        phi += 2 * pi;
    }
    if(phi > 2 * pi)
    {
        phi -= 2*pi;
    }
    float x, y, z;
    theMindField.sphericalToCartesian(x, y,z, r,  theta, phi);
    pos = Vec3f(x, y, z);
    
    /*
    if(t >= 1.0)
    {
        t = 0.0;
        Vec3f pTemp = p2;
        p2 = p1;
        p1 = pTemp;
    }
    pos = p1.lerp(t, p2);
     */
    
//    printf("%f %f %f\n", pos.x, pos.y, pos.z);

    /*
    A = F / m;
    pos = V * t + 0.5 * A * t* t;
    V = V + A*t;
    
   */
    
}

//////////////////////////////////////////////////////////////////////////////////////////// Neuron END



//////////////////////////////////////////////////////////////////////////////////////////// Axon START

Axon::Axon()
{
    
}

Axon::~Axon()
{
    
}

void Axon::Init(int a, int b)
{
    mAmp = 0.0;
    mDamp = randFloat(0.01, 1.0);
    mDeleted= false;
    mA = a;
    mB = b;
    int numP = 8;
    // BSpline
    for(int i = 0; i < numP; i++)
    {
        mBasePoints.push_back(theMindField.mNeurons[mA]->pos.lerp( i / float((numP-2)), theMindField.mNeurons[mB]->pos));
    }

	mCurPoints = mBasePoints;
    
    ResetTube();
}

void Axon::Shutdown()
{
    mDeleted = true;
    /*
   // mTube.Shutdown();
    mBasePoints.clear();
    mCurPoints.clear();
    //mBSpline.clear();
    mTubeMesh.clear();
     */
}

void Axon::Render()
{
    if(mDeleted)
        return;
    if( mTubeMesh.getNumTriangles() ) {
		gl::color( ColorA( 1, 1, 1, 1 ) );
		gl::draw( mTubeMesh );
	}
}


void Axon::Update()
{
    
	// BSpline

    float t = getElapsedSeconds(); // XXX pull this here or send as a param??

    float amp =  theMindField.mNeurons[mA]->mAmp * mDamp;
    mAmp = amp;
    int numP = mBasePoints.size();
    for( size_t i = 0; i < numP; ++i ) {
        Vec3f p1 = theMindField.mNeurons[mA]->pos;
        Vec3f p2 = theMindField.mNeurons[mB]->pos;
        float l = (p1 - p2).length();
         mBasePoints[i] = p1.lerp( i / float(numP - 2), p2);
       //mCurPoints = mBasePoints;
        
        float dx = 0;
        float dy = 0;
        float dz = 0;
        //float amp = abs(sin(t)) * 0.1;

        if( i > 0 && ( i < numP -1 ) ) {
            dx = sin( t*i * l ) * (amp + 0.001);
            dy = sin( t*i * l ) * (amp + 0.002);
            dz = cos( t*i * l )* (amp + 0.001);
        }
        mCurPoints[i] = mBasePoints[i] + Vec3f( dx, dy, dz );
         
    }


    
    // BSpline
  /*  for(int i = 0; i < 5; i++)
    {
        Vec3f p1 = theMindField.mNeurons[mA]->pos;
        Vec3f p2 = theMindField.mNeurons[mB]->pos;
        Vec3f offsetVector;
        offsetVector.x = Rand::randFloat(kXLow, kXHigh);
        offsetVector.y = Rand::randFloat(kYLow, kYHigh);
        offsetVector.z = Rand::randFloat(kZLow, kZHigh);
        offsetVector.normalize();
        Vec3f diff = p1 - p2;
        mBasePoints[i] = p1.slerp(i / 4.0, p2 + offsetVector);
        
        //mBasePoints[i] = theMindField.mNeurons[mA]->pos.lerp(i / 4.0, theMindField.mNeurons[mB]->pos);
    }
   
	mCurPoints = mBasePoints;
   */
    
    ResetTube();
    if(mDeleted)
        printf("HEY I AM DELETED\n");
        
}

void Axon::ResetTube()
{
	// called from init and update
    
	int  degree = 3;
	bool loop = false;
	bool open = true;
	mBSpline = BSpline3f( mCurPoints, degree, loop, open );
	
	// Tube
	mNumSegs = 16;
	mTube.setBSpline( mBSpline ); // this dude is built and animated dynamically
	mTube.setNumSegments( mNumSegs );
	mTube.sampleCurve();
    mTube.buildPTF();
    mTube.buildMesh( &mTubeMesh );
    


}

//////////////////////////////////////////////////////////////////////////////////////////// Axon END


//////////////////////////////////////////////////////////////////////////////////////////// MindField START

MindField::MindField() :
mNeurons(),
mAxons()
{
    
}

MindField::~MindField()
{
    
}

void MindField::Init()
{
    mTime = 0.0;
    mBlurView = 0.8;
    
    xAmp = 0.0;
    yAmp = 0.0;
    mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 0.25f, 1000.0f );
	mCamera.lookAt( Vec3f( 0.0f, 0.0f, -10.0f ), Vec3f::zero() );
    
    mNextCamPoint = mNextCamPoint = Vec3f(randFloat(-10,10), randFloat(-10,10), randFloat(-10,10));
    mLerper = 0.0f;
    
    kBXDiv = (kXHigh - kXLow)/ kBX;
    kBYDiv =  (kYHigh - kYLow)/ kBY;
    kBZDiv =  (kZHigh - kZLow)/ kBZ;
    
    const int numNeurons = 350;
    for(int i = 0; i < numNeurons; i++)
    {
        Neuron* newNeuron = new Neuron;
        newNeuron->Init();
        mNeurons.push_back(newNeuron);
    }
    
    UpdateAxons();
    
    gl::Fbo::Format format;
	mSourceFBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);
    mBlur1FBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);
    mBlur2FBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);
    mDepthFBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);
    
}

void MindField::Shutdown()
{
    mSourceFBO.reset();
    mBlur1FBO.reset();
    mBlur2FBO.reset();
    mDepthFBO.reset();

    for(int i = 0; i < mNeurons.size(); i++)
    {
        mNeurons[i]->Shutdown();
        delete mNeurons[i];
    }
    mNeurons.clear();
    
    for( list<Axon*>::iterator p = mAxons.begin(); p != mAxons.end(); ++p )
    {
        (*p)->Shutdown();
        delete (*p);
        p = mAxons.erase(p);
    }
    mAxons.clear();
    if (mNeuronShader)
		mNeuronShader.reset();
    if (mAxonShader)
		mAxonShader.reset();
}

void MindField::Render()
{

    Area viewport = gl::getViewport();
    
    // SOURCE
    mSourceFBO.bindFramebuffer();
    
    gl::setViewport( mSourceFBO.getBounds() );
    
    
   // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

 // gl::clear( ColorA(0.2f,0.2f,0.2f,1.0f) );
     gl::clear( ColorA(0.0f,0.0f,0.0f,1.0f) );
    gl::setMatrices( mCamera );
    
    
    gl::setMatricesWindow( getWindowSize(), true );
    
     gl::enableAlphaBlending();
    mBGShader.bind();
    float what = mTime;
    mBGShader.uniform("mTime", what);
    mBGShader.uniform("resolution", Vec2f((float)getWindowWidth(),(float)getWindowHeight()));
    gl::drawSolidRect( getWindowBounds() ); //mSourceFBO.getBounds());
    mBGShader.unbind();
    
    gl::popMatrices();
    gl::setMatrices( mCamera );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::enable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    gl::disableAlphaBlending();
    gl::enableAdditiveBlending();
    
    glEnable(GL_DEPTH_TEST);
    
    //NEURON
    
    // bind neuron shaders
    mNeuronShader.bind();
    for(int i = 0; i < mNeurons.size(); i++)
    {
        mNeuronShader.uniform("energy",mNeurons[i]->mAmp);
        mNeurons[i]->Render();
    }
    mNeuronShader.unbind();
    
    // AXON
    mAxonShader.bind();
    // bind axon shaders
    for( list<Axon*>::iterator p = mAxons.begin(); p != mAxons.end(); ++p )
    {
        mAxonShader.uniform("energy",(*p)->mAmp);
        (*p)->Render();
    }
    mAxonShader.unbind();
    
    
    mSourceFBO.unbindFramebuffer();
    
    glDisable(GL_DEPTH_TEST);
    
    gl::disable(GL_CULL_FACE);
    
    gl::enableAlphaBlending();
    
    gl::popMatrices();
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    // Blur HORIZ
    mBlurShader.bind();
    mBlurShader.uniform("tex0",0);
    mBlurShader.uniform("sampleOffset",Vec2f(1.0f/mBlur1FBO.getWidth(), 0.0f));
    gl::setViewport( mBlur1FBO.getBounds() );
	mBlur1FBO.bindFramebuffer();
    mSourceFBO.bindTexture(0);
    gl::pushMatrices();
    gl::setMatricesWindow(getWindowSize(),  false);
    gl::clear( Color::black() );
    gl::drawSolidRect( mBlur1FBO.getBounds() );
    gl::popMatrices();
    mSourceFBO.unbindTexture();
	mBlur1FBO.unbindFramebuffer();
    
    // Blur VERT
    mBlurShader.uniform("sampleOffset",Vec2f(0.0f, 1.0f/mBlur2FBO.getHeight()));
    gl::setViewport( mBlur2FBO.getBounds() );
	mBlur2FBO.bindFramebuffer();
    mBlur1FBO.bindTexture(0);
    gl::pushMatrices();
    gl::setMatricesWindow(getWindowSize(),  false);
    gl::clear( Color::black() );
    gl::drawSolidRect( mBlur2FBO.getBounds() );
    gl::popMatrices();
    mBlur1FBO.unbindTexture();
	mBlur2FBO.unbindFramebuffer();
    mBlurShader.unbind();
    
    
    // draw Composite
    mDOFShader.bind();
    // bind two inputs
    mDOFShader.uniform("tex0",0);
    mDOFShader.uniform("tex1",1);
    mDOFShader.uniform("tex2",2);
    mDOFShader.uniform("blurView",mBlurView);
    
    mSourceFBO.bindTexture(0);
    mBlur2FBO.bindTexture(1);
    mSourceFBO.bindDepthTexture(2);
   
    // draw screen quad
    gl::setMatricesWindow( getWindowSize(), true );
    gl::color(1.0f,1.0f,1.0f);
    
    gl::drawSolidRect(Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
   // gl::drawStrokedRect(  Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
   // gl::drawRect(mSourceFBO.getDepthTexture(), Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );

    mSourceFBO.unbindTexture();
    mBlur2FBO.unbindTexture();
    //mSourceFBO.getDepthTexture().unbind();
    mDOFShader.unbind();
    
    // reset camera for geometry
    gl::popMatrices();
    
    
    // restore the viewport
	//gl::setViewport( viewport )
}

void MindField::UpdateAxons()
{
    // lets insanely set up n^2 axons
    for(int i = 0; i < mNeurons.size(); i++)
    {
        Neuron* n = mNeurons[i];
        int bx = floor( (n->pos.x + abs(kXLow) ) / kBXDiv);
        int by = floor( (n->pos.y + abs(kYLow) ) / kBYDiv);
        int bz = floor( (n->pos.z + abs(kZLow)) / kBZDiv);
        
        float distVal = n->mDistVal;
        
        // dumbly check against all other neurons
        for(int j = 0; j < mBins[bx][by][bz].size(); j++)
        {
            int idx = mBins[bx][by][bz][j];
            Neuron* m =  mNeurons[idx];
             if(i == idx)
            {
                continue;
            }
            float  d=n->pos.distanceSquared (m->pos);
            if( d < distVal )
            {
                if(n->mAmp < m->mAmp)
                {
                    n->mAmp = m->mAmp - 0.001;
                }
                else
                {
                    m->mAmp = n->mAmp - 0.001;
                }
                CreateAxon(i, idx);
            }

        }
        
        
        tNeuronIndices vals = mAxonMap[i];
        if (!vals.empty()) {
            
            for( tNeuronIndices::iterator p = vals.begin(); p != vals.end(); p++)
            {
                
                float  d=n->pos.distanceSquared (mNeurons[(*p)]->pos);
                if( d >= distVal )
                {
                    
                    RemoveAxon(i, (*p));
                }
            }
        }


        
    }
}

bool MindField::AxonExists(int a, int b)
{
    tNeuronIndices vals = mAxonMap[a];
    if (!vals.empty()) {
        
        for( tNeuronIndices::iterator p = vals.begin(); p != vals.end(); p++)
        {
            if(*p == b)
            {
                return true;
            }
        }
    }

    return false;
}

void MindField::CreateAxon(int i, int j)
{
    if(AxonExists(i,j) || AxonExists(j, i))
    {
        return;
    }
    
    Axon* newAxon = new Axon;
    newAxon->Init(i, j);
    char buff[100];
    sprintf(buff, "%d_%d", i, j);
    std::string key = buff;
    mAxonData[key] =  newAxon;
    mAxonMap[i].push_back(j);
  //  mAxonMap[j].push_back(i);
    mAxons.push_back(newAxon);
    
}

void MindField::RemoveAxon(int i, int j)
{
    if(AxonExists(i,j))
    {
        //cNeuronPair np;
        //np.a = i;
        //np.b = j;

        char buff[100];
        sprintf(buff, "%d_%d", i, j);
        std::string key = buff;

        Axon* oldAxon = mAxonData[key];
        
        if(oldAxon)
        {
            oldAxon->Shutdown();
            mAxons.remove(oldAxon);
            delete oldAxon;
            mAxonData.erase(key);
        }
        else
        {
            mNeurons[i]->SetBad();
             mNeurons[j]->SetBad();
        }
        
        mAxonMap[i].remove(j);


     
    }
    
    /*
    
    if(AxonExists(j, i))
    {
        cNeuronPair np;
        np.a = j;
        np.b = i;
        Axon* oldAxon = mAxonData[np];
        
       
        if(oldAxon)
        {
     
            oldAxon->Shutdown();
           mAxons.remove(oldAxon);
            delete oldAxon;
            
             mAxonData.erase(np);
            mAxonMap[j].remove(i);
        }

        
    }
     */
    
}

void MindField::ClearBins()
{
    for(int i = 0; i < kBX; i++)
    {
            for(int j = 0; j < kBY; j++)
            {
                for(int k = 0; k < kBZ; k++)
                {
                    mBins[i][j][k].clear();
                }
            }
    }
}
void MindField::SetAmps(float x)
{
    
    xAmp = x ;
    yAmp = x ;
    
    if(xAmp > 0.1)
    {
        xAmp = 0.1;
    }
    
    for(int i = 0; i < mNeurons.size(); i++)
    {
        if(i%10 == 0)
        {
            Neuron* n = mNeurons[i];
            n->mAmp = xAmp;
        }
    }
}
void MindField::Update()
{
  mTime += 0.01f;

    //camera update
    Vec3f pos = mCamera.getEyePoint();
    
    Vec3f newPos = pos.lerp(mLerper, mNextCamPoint);
    mCamera.setEyePoint(newPos);
    mCamera.setCenterOfInterestPoint(Vec3f(0,0,0));
    
    mLerper = mLerper + 0.0000001* getFrameRate();
    float camBound = 8.0;
    
    float d = newPos.length();
    float maxLength = Vec3f(camBound, camBound, camBound).length();
    float t = d / maxLength;
    mBlurView = lerp(0.9,1.5,  t);
    

    if(mLerper >= 1.0)
    {
        mLerper = 0.0;
        mNextCamPoint = Vec3f(randFloat(-camBound,camBound), randFloat(-camBound,camBound), randFloat(-camBound,camBound));
    }
    
    if(pos.distance(mNextCamPoint) < 5)
    {
        mLerper = 0.0;
        mNextCamPoint = Vec3f(randFloat(-camBound, camBound), randFloat(-camBound,camBound), randFloat(-camBound,camBound));
    }
    // end camera update
    
    
    for(int i = 0; i < mNeurons.size(); i++)
    {
        Neuron* n = mNeurons[i];
        n->Update();
        int bx = floor( (n->pos.x + abs(kXLow) ) / kBXDiv);
        int by = floor( (n->pos.y + abs(kYLow) ) / kBYDiv);
        int bz = floor( (n->pos.z + abs(kZLow)) / kBZDiv);
        theMindField.mBins[bx][by][bz].push_back(i);
    }
    
    

     for( list<Axon*>::iterator p = mAxons.begin(); p != mAxons.end(); ++p )
    {
        (*p)->Update();
        
    }
    
    UpdateAxons();
    ClearBins();
    
    
    
}


void MindField::cartesianToSpherical( float & r,
                     float & theta,
                     float & phi,
                     float   x,
                     float   y,
                     float   z )
{
    if ( ( r = sqrt( x * x + y * y + z * z ) ) != 0.0 )
    {
        theta = acos( z / r );
        phi   = atan2( y, x );
    }
    else
        theta = phi = 0.0;
}



void MindField::sphericalToCartesian( float & x,
                     float & y,
                     float & z,
                     float   r,
                     float   theta,
                     float   phi )
{
    if ( r < 0.0 )
        throw "Negative radius in sphericalToCartesian()";
    
    x = r * sin( theta ) * cos( phi );
    y = r * sin( theta ) * sin( phi );
    z = r * cos( theta );
}


void MindField::bindShaders()
{
    
    //string mPath = getResourcePath().generic_string();
    string mPath = "/Users/colin/Dev/cinder_projects/PixarDemo2012/resources";
   //  string mPath = "/Users/shalinkhyati/PixarDemo2012/resources";
    string nv = mPath + "/neuron.vert";
    string nf = mPath + "/neuron.frag";
    string bv = mPath + "/blur.vert";
    string bf = mPath + "/blur.frag";
    string dv = mPath + "/dof.vert";
    string df = mPath + "/dof.frag";
    string av = mPath + "/axon.vert";
    string af = mPath + "/axon.frag";
    string bgv = mPath + "/bgMind.vert";
    string bgf = mPath + "/bgMind.frag";
    
    mNeuronVert = nv.c_str();
    mNeuronFrag = nf.c_str();
    mBlurVert = bv.c_str();
    mBlurFrag = bf.c_str();
    mDOFVert = dv.c_str();
    mDOFFrag = df.c_str();
    mAxonVert = av.c_str();
    mAxonFrag = af.c_str();
    mBGVert = bgv.c_str();
    mBGFrag = bgf.c_str();
    
    try {
        if (false) {
            //for shader dev
            mNeuronShader = gl::GlslProg( loadFile( mNeuronVert ), loadFile( mNeuronFrag ) );
            mAxonShader = gl::GlslProg( loadFile( mAxonVert ), loadFile( mAxonFrag ) );
            mBlurShader = gl::GlslProg( loadFile( mBlurVert ), loadFile( mBlurFrag ) );
            mDOFShader =  gl::GlslProg( loadFile( mDOFVert ), loadFile( mDOFFrag ) );
            mBGShader = gl::GlslProg( loadFile( mBGVert ), loadFile( mBGFrag ) );
        } else {
            //for install
            mNeuronShader = gl::GlslProg( loadResource( NERUON_VERT ), loadResource( NEURON_FRAG ) );
            mAxonShader = gl::GlslProg( loadResource( AXON_VERT ), loadResource( AXON_FRAG ) );
            mBlurShader = gl::GlslProg( loadResource( BLUR_VERT ), loadResource( BLUR_FRAG ) );
            mDOFShader =  gl::GlslProg( loadResource( DOF_VERT ), loadResource( DOF_FRAG ) );
            mBGShader = gl::GlslProg( loadResource( BGMIND_VERT ), loadResource( BGMIND_FRAG ) );
        }
	}
    catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load mind field shaders" << std::endl;
	}

    
    
}
//////////////////////////////////////////////////////////////////////////////////////////// MindField END