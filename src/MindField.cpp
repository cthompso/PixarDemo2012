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

    mSphere =  Sphere(pos, Rand::randFloat(0.09, 0.3));
}

void Neuron::Shutdown()
{
    
}

void Neuron::Render()
{
    gl::color( Color( c + 0.2f, 0.2f, 0.5f ) );
    gl::drawSphere( pos, mSphere.getRadius() );
}

void Neuron::SetBad()
{
    c = 0.8f;
}

void Neuron::Update()
{
    t = t + 0.0001 * getFrameRate();
    if(t >= 1.0)
    {
        t = 0.0;
        Vec3f pTemp = p2;
        p2 = p1;
        p1 = pTemp;
    }
    pos = p1.lerp(t, p2);
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

    mDeleted= false;
    mA = a;
    mB = b;
    int numP = 5;
    // BSpline
    for(int i = 0; i < numP; i++)
    {
        mBasePoints.push_back(theMindField.mNeurons[mA]->pos.lerp( i / float((numP-1)), theMindField.mNeurons[mB]->pos));
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

    
    int numP = mBasePoints.size();
    for( size_t i = 0; i < numP; ++i ) {
        Vec3f p1 = theMindField.mNeurons[mA]->pos;
        Vec3f p2 = theMindField.mNeurons[mB]->pos;
        float l = (p1 - p2).length();
         mBasePoints[i] = p1.lerp( i / float(numP - 1), p2);
       //mCurPoints = mBasePoints;
        
        float dx = 0;
        float dy = 0;
        float dz = 0;
        if( i > 0 && ( i < numP - 1 ) ) {
            dx = sin( t*i * l )*0.02f;
            dy = sin( t*i * l ) * 0.15;
            dz = cos( t*i * l )*0.2f;
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
	mNumSegs = 8;
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
    mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 0.1f, 1000.0f );
	mCamera.lookAt( Vec3f( 0.0f, 0.0f, -10.0f ), Vec3f::zero() );
    
    mNextCamPoint = mNextCamPoint = Vec3f(randFloat(-10,10), randFloat(-10,10), randFloat(-10,10));
    mLerper = 0.0f;
    
    kBXDiv = (kXHigh - kXLow)/ kBX;
    kBYDiv =  (kYHigh - kYLow)/ kBY;
    kBZDiv =  (kZHigh - kZLow)/ kBZ;
    
    const int numNeurons = 800;
    for(int i = 0; i < numNeurons; i++)
    {
        Neuron* newNeuron = new Neuron;
        newNeuron->Init();
        mNeurons.push_back(newNeuron);
    }
    
    UpdateAxons();
    
    gl::Fbo::Format format;
	mSourceFBO = gl::Fbo( getWindowWidth(), getWindowHeight(), format);
    mBlur1FBO = gl::Fbo( getWindowWidth()/2, getWindowHeight()/2, format);
    mBlur2FBO = gl::Fbo( getWindowWidth()/2, getWindowHeight()/2, format);
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
    mSourceFBO.bindFramebuffer();
    
    gl::setViewport( mSourceFBO.getBounds() );
    
    gl::setMatricesWindow( getWindowSize(), true );
    gl::setMatrices( mCamera );
    
    glClear(GL_DEPTH_BUFFER_BIT);
    gl::clear( ColorA(0.2f,0.2f,0.2f,1.0f) );
    
    // bind neuron shaders
    mNeuronShader.bind();
    for(int i = 0; i < mNeurons.size(); i++)
    {
        mNeurons[i]->Render();
    }

    mNeuronShader.unbind();
    mAxonShader.bind();
    // bind axon shaders
    for( list<Axon*>::iterator p = mAxons.begin(); p != mAxons.end(); ++p )
    {
        (*p)->Render();
    }
    mAxonShader.unbind();
    
    mSourceFBO.unbindFramebuffer();
    
    gl::popMatrices();
    
    
    // draw FBO bg
    gl::setMatricesWindow( getWindowSize(), true );
    gl::color(1.0f,1.0f,1.0f);
    gl::draw( mSourceFBO.getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
   // gl::draw( mSourceFBO.getDepthTexture(), Rectf( 0, 0, getWindowWidth(), getWindowHeight()) );
    // reset camera for geometry
    gl::popMatrices();
    
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
        
        float distVal = 5.5;
        
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

void MindField::Update()
{
    
    //camera update
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


void MindField::cartesianToSpherical( double & r,
                     double & theta,
                     double & phi,
                     double   x,
                     double   y,
                     double   z )
{
    if ( ( r = sqrt( x * x + y * y + z * z ) ) != 0.0 )
    {
        theta = acos( z / r );
        phi   = atan2( y, x );
    }
    else
        theta = phi = 0.0;
}



void MindField::sphericalToCartesian( double & x,
                     double & y,
                     double & z,
                     double   r,
                     double   theta,
                     double   phi )
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
    mNeuronVert = nv.c_str();
    mNeuronFrag = nf.c_str();
    string av = mPath + "/axon.vert";
    string af = mPath + "/axon.frag";
    mAxonVert = av.c_str();
    mAxonFrag = af.c_str();
    try {
        if (true) {
            //for shader dev
            mNeuronShader = gl::GlslProg( loadFile( mNeuronVert ), loadFile( mNeuronFrag ) );
            mAxonShader = gl::GlslProg( loadFile( mAxonVert ), loadFile( mAxonFrag ) );
          
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
//////////////////////////////////////////////////////////////////////////////////////////// MindField END