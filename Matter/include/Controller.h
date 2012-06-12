#pragma once
#include "cinder/gl/Gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "Particle.h"
#include "Photon.h"
#include "Shockwave.h"
#include "Smoke.h"
#include "SpringCam.h"
#include <list>
#include <vector>

class Controller {
public:
	struct AnchorVertex {
        ci::Vec3f vertex;
        ci::Vec4f color;
    };
	
	Controller();
	void init( Room *room );
	void createSphere( ci::gl::VboMesh &mesh, int res );
	void drawSphereTri( ci::Vec3f va, ci::Vec3f vb, ci::Vec3f vc, int div );
	void applyShockwavesToCam( SpringCam &cam );
	void applyShockwavesToTime();
	void applyForceToParticles();
	bool didParticlesCollide( const ci::Vec3f &dir, const ci::Vec3f &dirNormal, const float dist, const float sumRadii, const float sumRadiiSqrd, ci::Vec3f *moveVec );
	void annihilate( Particle *p1, Particle *p2 );
	void update();
	void reset();
	void clearRoom();
	void draw( ci::gl::GlslProg *shader );
	void drawParticleShadows();
	void drawPhotons();
	void drawShockwaves( const ci::Vec3f &camPos );
	void drawSmokes( const ci::Vec3f &right, const ci::Vec3f &up );
	void drawSphericalBillboard( const ci::Vec3f &camEye, const ci::Vec3f &objPos, const ci::Vec2f &scale, float rotInRadians );
	void finish();
	void addParticles( int amt );
	void addParticle( float charge, const ci::Vec3f &pos );
	void removeParticles( int amt );
	void preset( int i );

	Room					*mRoom;
	
	int						mNumMatter;
	int						mNumAntimatter;
	
	std::list<Particle>		mParticles;
	std::list<Photon>		mPhotons;
	std::vector<Shockwave>	mShockwaves;
	std::vector<Smoke>		mSmokes;
	
	ci::gl::VboMesh			mSphereLo, mSphereHi;
	int						mIndex;
	std::vector<uint32_t>	mIndices;
	std::vector<ci::Vec3f>	mPosCoords;
	std::vector<ci::Vec3f>	mNormals;
	std::vector<ci::Vec2f>	mTexCoords;
	
	int mPreset;
	bool mRecycle;
};