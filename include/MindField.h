//
//  Neuron.h
//  PixarDemo2012
//
//  Created by Shalin Shodhan on 10/6/12.
//
//
 
#ifndef __PixarDemo2012__Neuron__
#define __PixarDemo2012__Neuron__

#include "cinder/Vector.h"
#include "cinder/Sphere.h"
#include "Tube.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/Camera.h"

#include "cinder/gl/Fbo.h"

#include <iostream>
#include <vector>
#include <map>

using namespace ci;
using namespace cinder;

#define theMindField MindField::getInstance()

const float kXLow = -20.0f;
const float kXHigh = 20.0f;
const float kYLow = -20.0f;
const float kYHigh  = 20.0f;
const float kZLow = -20.0f;
const float kZHigh  = 20.0f;

const int kBX = 5;
const int kBY = 5;
const int kBZ = 5;



// Basically a sphere
class Neuron {
public:
    Neuron() ;
    ~Neuron();
    
    void Init();
    void Shutdown();
    
    void Update();
    void Render();
    
    void SetBad();
    
    Vec3f pos;
    
    Vec3f p1;
    Vec3f p2;
    float t;
    
    float r, phi, theta;
    
    float c;
    
    Sphere mSphere;
    float mDistVal;
    
    float randSpeed;
    float randSpeed2;
    bool randDirection;
    
    Vec3f F;
    float m;
    Vec3f A;
    Vec3f V;
    
    float mAmp;
    float oldR;

};

// A tube between two spheres
class Axon {
public:
    Axon () ;
    ~Axon() ;
    
    void Init(int a, int b);
    void Shutdown();
    
    void Update();
    void Render();
    void ResetTube();
    
    int mA;
    int mB;
    
    bool mDeleted;
    float mAmp;
    float mWidth;
    float mDamp;
    // Tubulation
    Tube					mTube;
	std::vector<Vec3f>		mBasePoints;
	std::vector<Vec3f>		mCurPoints;
	BSpline3f				mBSpline;
	TriMesh					mTubeMesh;
    int32_t					mNumSegs;
	int						mShape;


};


typedef std::list<int> tNeuronIndices;
typedef std::vector<int> tNeuronIdxVector;

typedef std::map<int, tNeuronIndices> tAxonMap;
typedef std::map<std::string, Axon*> tAxonPairMap;

class MindField {
  
public:
    
	static MindField& getInstance() {
		if (!s_MindField) s_MindField = new MindField();
		return *s_MindField;
	};
    

    
    void Init();
    void Shutdown();
    
    void Update();
    void Render();
    
    void CreateAxon(int a, int b);
    void UpdateAxons();
    bool AxonExists(int a, int b);
    void RemoveAxon(int a, int b);
    void ClearBins();
    void bindShaders();
    void SetAmps(float x  );
    
    
    void sphericalToCartesian( float & x,
                                         float & y,
                                         float & z,
                                         float   r,
                                         float   theta,
                                         float   phi );
    
    void cartesianToSpherical( float & r,
                                         float & theta,
                                         float & phi,
                                         float   x,
                                         float   y,
                                         float   z );
    std::vector<Neuron* > mNeurons;
    std::list<Axon* > mAxons;
    
    float kBXDiv;
    float kBYDiv;
    float kBZDiv;
   tNeuronIdxVector mBins[kBX][kBY][kBZ];
    tAxonMap mAxonMap;
    tAxonPairMap mAxonData;
    
    
    gl::GlslProg mBGShader;
	const char* mBGVert;
	const char* mBGFrag;

    
    gl::GlslProg mNeuronShader;
	const char* mNeuronVert;
	const char* mNeuronFrag;
    
    gl::GlslProg mAxonShader;
	const char* mAxonVert;
	const char* mAxonFrag;
    
    gl::GlslProg mBlurShader;
	const char* mBlurVert;
	const char* mBlurFrag;

    gl::GlslProg mDOFShader;
	const char* mDOFVert;
	const char* mDOFFrag;
    
    //fbo
	gl::Fbo mSourceFBO;
    gl::Fbo mBlur1FBO;
    gl::Fbo mBlur2FBO;
    gl::Fbo mDepthFBO;
    
    float mTime;
    ci::CameraPersp				mCamera;
    
    Vec3f mNextCamPoint;
    float mLerper;
    
    float xAmp, yAmp;
    
    float mBlurView;
protected:	
	//! singleton instance
	static MindField *s_MindField;
    MindField() ;
    ~MindField() ;
};

#endif /* defined(__PixarDemo2012__Neuron__) */
