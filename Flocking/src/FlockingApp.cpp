#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "Resources.h"
#include "SpringCam.h"
#include "Room.h"
#include "Controller.h"

#include "Lantern.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_WIDTH		350
#define ROOM_HEIGHT		200
#define ROOM_DEPTH		150
#define ROOM_FBO_RES	2
#define FBO_DIM			50//167
#define P_FBO_DIM		5
#define MAX_LANTERNS	5

class FlockingApp : public AppBasic {
public:
	virtual void		prepareSettings( Settings *settings );
	virtual void		setup();
	void				adjustFboDim( int offset );
	void				initialize();
	void				setFboPositions( gl::Fbo fbo );
	void				setFboVelocities( gl::Fbo fbo );
	void				setPredatorFboPositions( gl::Fbo fbo );
	void				setPredatorFboVelocities( gl::Fbo fbo );
	void				initVbo();
	void				initPredatorVbo();
	virtual void		mouseDown( MouseEvent event );
	virtual void		mouseUp( MouseEvent event );
	virtual void		mouseMove( MouseEvent event );
	virtual void		mouseDrag( MouseEvent event );
	virtual void		mouseWheel( MouseEvent event );
	virtual void		keyDown( KeyEvent event );
	virtual void		update();
	void				drawIntoRoomFbo();
	void				drawInfoPanel();
	void				drawIntoVelocityFbo();
	void				drawIntoPositionFbo();
	void				drawIntoPredatorVelocityFbo();
	void				drawIntoPredatorPositionFbo();
	void				drawIntoLanternsFbo();
	void				drawGlows();
	void				drawNebulas();
	virtual void		draw();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// TEXTURES
	gl::Texture			mLanternGlowTex;
	gl::Texture			mGlowTex;
	gl::Texture			mNebulaTex;
	gl::Texture			mIconTex;
	
	// SHADERS
	gl::GlslProg		mVelocityShader;
	gl::GlslProg		mPositionShader;
	gl::GlslProg		mP_VelocityShader;
	gl::GlslProg		mP_PositionShader;
	gl::GlslProg		mLanternShader;
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mShader;
	gl::GlslProg		mP_Shader;
	gl::GlslProg		mGlowShader;
	gl::GlslProg		mNebulaShader;
	
	// CONTROLLER
	Controller			mController;

	// LANTERNS (point lights)
	gl::Fbo				mLanternsFbo;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
	// VBOS
	gl::VboMesh			mVboMesh;
	gl::VboMesh			mP_VboMesh;
	
	// POSITION/VELOCITY FBOS
	gl::Fbo::Format		mRgba16Format;
	int					mFboDim;
	ci::Vec2f			mFboSize;
	ci::Area			mFboBounds;
	gl::Fbo				mPositionFbos[2];
	gl::Fbo				mVelocityFbos[2];
	int					mP_FboDim;
	ci::Vec2f			mP_FboSize;
	ci::Area			mP_FboBounds;
	gl::Fbo				mP_PositionFbos[2];
	gl::Fbo				mP_VelocityFbos[2];
	int					mThisFbo, mPrevFbo;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
	
	bool				mSaveFrames;
	int					mNumSavedFrames;
	
	bool				mInitUpdateCalled;
};

void FlockingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
//	settings->setBorderless();
}

void FlockingApp::setup()
{
	// CAMERA	
	mSpringCam			= SpringCam( -420.0f, getWindowAspectRatio() );
	
	// POSITION/VELOCITY FBOS
	mRgba16Format.setColorInternalFormat( GL_RGBA16F_ARB );
	mRgba16Format.setMinFilter( GL_NEAREST );
	mRgba16Format.setMagFilter( GL_NEAREST );
	mThisFbo			= 0;
	mPrevFbo			= 1;
	
	// LANTERNS
	mLanternsFbo		= gl::Fbo( MAX_LANTERNS, 2, mRgba16Format );
	
	// TEXTURE FORMAT
	gl::Texture::Format mipFmt;
    mipFmt.enableMipmapping( true );
    mipFmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );    
    mipFmt.setMagFilter( GL_LINEAR );
	
	// TEXTURES
	mLanternGlowTex		= gl::Texture( loadImage( loadResource( RES_LANTERNGLOW_PNG ) ) );
	mGlowTex			= gl::Texture( loadImage( loadResource( RES_GLOW_PNG ) ) );
	mNebulaTex			= gl::Texture( loadImage( loadResource( RES_NEBULA_PNG ) ) );
	mIconTex			= gl::Texture( loadImage( loadResource( "iconFlocking.png" ) ), mipFmt );
	
	// LOAD SHADERS
	try {
		mVelocityShader		= gl::GlslProg( loadResource( RES_PASSTHRU_VERT ),	loadResource( RES_VELOCITY_FRAG ) );
		mPositionShader		= gl::GlslProg( loadResource( RES_PASSTHRU_VERT ),	loadResource( RES_POSITION_FRAG ) );
		mP_VelocityShader	= gl::GlslProg( loadResource( RES_PASSTHRU_VERT ),	loadResource( RES_P_VELOCITY_FRAG ) );
		mP_PositionShader	= gl::GlslProg( loadResource( RES_PASSTHRU_VERT ),	loadResource( RES_P_POSITION_FRAG ) );
		mLanternShader		= gl::GlslProg( loadResource( RES_LANTERN_VERT ),	loadResource( RES_LANTERN_FRAG ) );
		mRoomShader			= gl::GlslProg( loadResource( RES_ROOM_VERT ),		loadResource( RES_ROOM_FRAG ) );
		mShader				= gl::GlslProg( loadResource( RES_VBOPOS_VERT ),	loadResource( RES_VBOPOS_FRAG ) );
		mP_Shader			= gl::GlslProg( loadResource( RES_P_VBOPOS_VERT ),	loadResource( RES_P_VBOPOS_FRAG ) );
		mGlowShader			= gl::GlslProg( loadResource( RES_PASSTHRU_VERT ),	loadResource( RES_GLOW_FRAG ) );
		mNebulaShader		= gl::GlslProg( loadResource( RES_PASSTHRU_VERT ),	loadResource( RES_NEBULA_FRAG ) );
	} catch( gl::GlslProgCompileExc e ) {
		std::cout << e.what() << std::endl;
		quit();
	}
	
	// ROOM
	gl::Fbo::Format roomFormat;
	roomFormat.setColorInternalFormat( GL_RGB );
	mRoomFbo			= gl::Fbo( APP_WIDTH/ROOM_FBO_RES, APP_HEIGHT/ROOM_FBO_RES, roomFormat );
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( ROOM_WIDTH, ROOM_HEIGHT, ROOM_DEPTH ), isPowerOn, isGravityOn );	
	mRoom.init();
	
	// CONTROLLER
	mController			= Controller( &mRoom, MAX_LANTERNS );
	
	// MOUSE
	mMousePos			= Vec2f::zero();
	mMouseDownPos		= Vec2f::zero();
	mMouseOffset		= Vec2f::zero();
	mMousePressed		= false;
	
	mSaveFrames			= false;
	mNumSavedFrames		= 0;
	
	mInitUpdateCalled	= false;
	
	initialize();
}

void FlockingApp::initialize()
{
	gl::disableAlphaBlending();
	gl::disableDepthWrite();
	gl::disableDepthRead();
	
	mFboDim				= FBO_DIM;
	mFboSize			= Vec2f( mFboDim, mFboDim );
	mFboBounds			= Area( 0, 0, mFboDim, mFboDim );
	mPositionFbos[0]	= gl::Fbo( mFboDim, mFboDim, mRgba16Format );
	mPositionFbos[1]	= gl::Fbo( mFboDim, mFboDim, mRgba16Format );
	mVelocityFbos[0]	= gl::Fbo( mFboDim, mFboDim, mRgba16Format );
	mVelocityFbos[1]	= gl::Fbo( mFboDim, mFboDim, mRgba16Format );
	
	setFboPositions( mPositionFbos[0] );
	setFboPositions( mPositionFbos[1] );
	setFboVelocities( mVelocityFbos[0] );
	setFboVelocities( mVelocityFbos[1] );
	
	
	mP_FboDim			= P_FBO_DIM;
	mP_FboSize			= Vec2f( mP_FboDim, mP_FboDim );
	mP_FboBounds		= Area( 0, 0, mP_FboDim, mP_FboDim );
	mP_PositionFbos[0]	= gl::Fbo( mP_FboDim, mP_FboDim, mRgba16Format );
	mP_PositionFbos[1]	= gl::Fbo( mP_FboDim, mP_FboDim, mRgba16Format );
	mP_VelocityFbos[0]	= gl::Fbo( mP_FboDim, mP_FboDim, mRgba16Format );
	mP_VelocityFbos[1]	= gl::Fbo( mP_FboDim, mP_FboDim, mRgba16Format );
	
	setPredatorFboPositions( mP_PositionFbos[0] );
	setPredatorFboPositions( mP_PositionFbos[1] );
	setPredatorFboVelocities( mP_VelocityFbos[0] );
	setPredatorFboVelocities( mP_VelocityFbos[1] );
	
	initVbo();
	initPredatorVbo();
}

void FlockingApp::setFboPositions( gl::Fbo fbo )
{	
	// FISH POSITION
	Surface32f posSurface( fbo.getTexture() );
	Surface32f::Iter it = posSurface.getIter();
	while( it.line() ){
		float y = (float)it.y()/(float)it.getHeight() - 0.5f;
		while( it.pixel() ){
			float per		= (float)it.x()/(float)it.getWidth();
			float angle		= per * M_PI * 2.0f;
			float radius	= 100.0f;
			float cosA		= cos( angle );
			float sinA		= sin( angle );
			Vec3f p			= Vec3f( cosA, y, sinA ) * radius;
			
			it.r() = p.x;
			it.g() = p.y;
			it.b() = p.z;
			it.a() = Rand::randFloat( 0.7f, 1.0f );	// GENERAL EMOTIONAL STATE. 
		}
	}
	
	gl::Texture posTexture( posSurface );
	fbo.bindFramebuffer();
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	gl::draw( posTexture );
	fbo.unbindFramebuffer();
}

void FlockingApp::setFboVelocities( gl::Fbo fbo )
{
	// FISH VELOCITY
	Surface32f velSurface( fbo.getTexture() );
	Surface32f::Iter it = velSurface.getIter();
	while( it.line() ){
		while( it.pixel() ){
			float per		= (float)it.x()/(float)it.getWidth();
			float angle		= per * M_PI * 2.0f;
			float cosA		= cos( angle );
			float sinA		= sin( angle );
			Vec3f p			= Vec3f( cosA, 0.0f, sinA );
			it.r() = p.x;
			it.g() = p.y;
			it.b() = p.z;
			it.a() = 1.0f;
		}
	}
	
	gl::Texture velTexture( velSurface );
	fbo.bindFramebuffer();
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	gl::draw( velTexture );
	fbo.unbindFramebuffer();
}

void FlockingApp::setPredatorFboPositions( gl::Fbo fbo )
{	
	// PREDATOR POSITION
	Surface32f posSurface( fbo.getTexture() );
	Surface32f::Iter it = posSurface.getIter();
	while( it.line() ){
		while( it.pixel() ){
			Vec3f r = Rand::randVec3f() * 50.0f;
			it.r() = r.x;
			it.g() = r.y;
			it.b() = r.z;
			it.a() = Rand::randFloat( 0.7f, 1.0f );	// GENERAL EMOTIONAL STATE. 
		}
	}
	
	gl::Texture posTexture( posSurface );
	fbo.bindFramebuffer();
	gl::setMatricesWindow( mP_FboSize, false );
	gl::setViewport( mP_FboBounds );
	gl::draw( posTexture );
	fbo.unbindFramebuffer();
}

void FlockingApp::setPredatorFboVelocities( gl::Fbo fbo )
{
	// PREDATOR VELOCITY
	Surface32f velSurface( fbo.getTexture() );
	Surface32f::Iter it = velSurface.getIter();
	while( it.line() ){
		while( it.pixel() ){
			Vec3f r = Rand::randVec3f() * 3.0f;
			it.r() = r.x;
			it.g() = r.y;
			it.b() = r.z;
			it.a() = 1.0f;
		}
	}
	
	gl::Texture velTexture( velSurface );
	fbo.bindFramebuffer();
	gl::setMatricesWindow( mP_FboSize, false );
	gl::setViewport( mP_FboBounds );
	gl::draw( velTexture );
	fbo.unbindFramebuffer();
}

void FlockingApp::initVbo()
{
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticNormals();
	
	int numVertices = mFboDim * mFboDim;
	// 5 points make up the pyramid
	// 8 triangles make up two pyramids
	// 3 points per triangle
	
	mVboMesh		= gl::VboMesh( numVertices * 8 * 3, 0, layout, GL_TRIANGLES );
	
	float s = 1.5f;
	Vec3f p0( 0.0f, 0.0f, 2.0f );
	Vec3f p1( -s, -s, 0.0f );
	Vec3f p2( -s,  s, 0.0f );
	Vec3f p3(  s,  s, 0.0f );
	Vec3f p4(  s, -s, 0.0f );
	Vec3f p5( 0.0f, 0.0f, -5.0f );
	
	Vec3f n;
	Vec3f n0 = Vec3f( 0.0f, 0.0f, 1.0f );
	Vec3f n1 = Vec3f(-1.0f,-1.0f, 0.0f ).normalized();
	Vec3f n2 = Vec3f(-1.0f, 1.0f, 0.0f ).normalized();
	Vec3f n3 = Vec3f( 1.0f, 1.0f, 0.0f ).normalized();
	Vec3f n4 = Vec3f( 1.0f,-1.0f, 0.0f ).normalized();
	Vec3f n5 = Vec3f( 0.0f, 0.0f,-1.0f );
	
	vector<Vec3f>		positions;
	vector<Vec3f>		normals;
	vector<Vec2f>		texCoords;
	
	for( int x = 0; x < mFboDim; ++x ) {
		for( int y = 0; y < mFboDim; ++y ) {
			float u = (float)x/(float)mFboDim;
			float v = (float)y/(float)mFboDim;
			Vec2f t = Vec2f( u, v );
			
			positions.push_back( p0 );
			positions.push_back( p1 );
			positions.push_back( p2 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p1 + p2 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p2 );
			positions.push_back( p3 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p2 + p3 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p3 );
			positions.push_back( p4 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p3 + p4 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p4 );
			positions.push_back( p1 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p4 + p1 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			
			
			
			positions.push_back( p5 );
			positions.push_back( p1 );
			positions.push_back( p4 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p1 + p4 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p2 );
			positions.push_back( p1 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p2 + p1 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p3 );
			positions.push_back( p2 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p3 + p2 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p4 );
			positions.push_back( p3 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p4 + p3 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			
		}
	}
	
	mVboMesh.bufferPositions( positions );
	mVboMesh.bufferTexCoords2d( 0, texCoords );
	mVboMesh.bufferNormals( normals );
	mVboMesh.unbindBuffers();
}

void FlockingApp::initPredatorVbo()
{
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticNormals();
	
	int numVertices = mP_FboDim * mP_FboDim;
	// 5 points make up the pyramid
	// 8 triangles make up two pyramids
	// 3 points per triangle
	
	mP_VboMesh		= gl::VboMesh( numVertices * 8 * 3, 0, layout, GL_TRIANGLES );
	
	float s = 5.0f;
	Vec3f p0( 0.0f, 0.0f, 3.0f );
	Vec3f p1( -s*1.3f, 0.0f, 0.0f );
	Vec3f p2( 0.0f, s * 0.5f, 0.0f );
	Vec3f p3( s*1.3f, 0.0f, 0.0f );
	Vec3f p4( 0.0f, -s * 0.5f, 0.0f );
	Vec3f p5( 0.0f, 0.0f, -12.0f );
	

	
	Vec3f n;
	Vec3f n0 = Vec3f( 0.0f, 0.0f, 1.0f );
	Vec3f n1 = Vec3f(-1.0f, 0.0f, 0.0f ).normalized();
	Vec3f n2 = Vec3f( 0.0f, 1.0f, 0.0f ).normalized();
	Vec3f n3 = Vec3f( 1.0f, 0.0f, 0.0f ).normalized();
	Vec3f n4 = Vec3f( 0.0f,-1.0f, 0.0f ).normalized();
	Vec3f n5 = Vec3f( 0.0f, 0.0f,-1.0f );
	
	vector<Vec3f>		positions;
	vector<Vec3f>		normals;
	vector<Vec2f>		texCoords;
	
	for( int x = 0; x < mP_FboDim; ++x ) {
		for( int y = 0; y < mP_FboDim; ++y ) {
			float u = (float)x/(float)mP_FboDim;
			float v = (float)y/(float)mP_FboDim;
			Vec2f t = Vec2f( u, v );
			
			positions.push_back( p0 );
			positions.push_back( p1 );
			positions.push_back( p2 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p1 + p2 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p2 );
			positions.push_back( p3 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p2 + p3 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p3 );
			positions.push_back( p4 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p3 + p4 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p4 );
			positions.push_back( p1 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p0 + p4 + p1 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			
			
			
			positions.push_back( p5 );
			positions.push_back( p1 );
			positions.push_back( p4 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p1 + p4 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p2 );
			positions.push_back( p1 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p2 + p1 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p3 );
			positions.push_back( p2 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p3 + p2 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p4 );
			positions.push_back( p3 );
			texCoords.push_back( t );
			texCoords.push_back( t );
			texCoords.push_back( t );
			n = ( p5 + p4 + p3 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			
		}
	}
	
	mP_VboMesh.bufferPositions( positions );
	mP_VboMesh.bufferTexCoords2d( 0, texCoords );
	mP_VboMesh.bufferNormals( normals );
	mP_VboMesh.unbindBuffers();
}


void FlockingApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void FlockingApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void FlockingApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void FlockingApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos );
}

void FlockingApp::mouseWheel( MouseEvent event )
{
	float dWheel = event.getWheelIncrement();
	mRoom.adjustTimeMulti( dWheel );
}

void FlockingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == ' ' ){
		mRoom.togglePower();
	} else if( event.getChar() == 'l' ){
		mController.addLantern( mRoom.getRandCeilingPos() );
	}
}

void FlockingApp::update()
{
	if( !mInitUpdateCalled ){
		mInitUpdateCalled = true;
	}
	
	// ROOM
	mRoom.update( mSaveFrames );
	
	// CONTROLLER
	mController.update();

	// CAMERA
	if( mMousePressed ){
		mSpringCam.dragCam( ( mMouseOffset ) * 0.01f, ( mMouseOffset ).length() * 0.01f );
	}
	mSpringCam.update( 0.3f );
	
	gl::disableAlphaBlending();
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::color( Color( 1, 1, 1 ) );

	drawIntoVelocityFbo();
	drawIntoPositionFbo();
	drawIntoPredatorVelocityFbo();
	drawIntoPredatorPositionFbo();
	drawIntoRoomFbo();
	drawIntoLanternsFbo();
}


// FISH VELOCITY
void FlockingApp::drawIntoVelocityFbo()
{
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	mVelocityFbos[ mThisFbo ].bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	
	mPositionFbos[ mPrevFbo ].bindTexture( 0 );
	mVelocityFbos[ mPrevFbo ].bindTexture( 1 );
	mP_PositionFbos[ mPrevFbo ].bindTexture( 2 );
	mLanternsFbo.bindTexture( 3 );
	
	mVelocityShader.bind();
	mVelocityShader.uniform( "positionTex", 0 );
	mVelocityShader.uniform( "velocityTex", 1 );
	mVelocityShader.uniform( "predatorPositionTex", 2 );
	mVelocityShader.uniform( "lanternsTex", 3 );
	mVelocityShader.uniform( "numLights", (float)mController.mNumLanterns );
	mVelocityShader.uniform( "invNumLights", 1.0f/(float)MAX_LANTERNS );
	mVelocityShader.uniform( "invNumLightsHalf", 1.0f/(float)MAX_LANTERNS * 0.5f );
	mVelocityShader.uniform( "att", 1.015f );
	mVelocityShader.uniform( "roomBounds", mRoom.getDims() );
	mVelocityShader.uniform( "fboDim", mFboDim );
	mVelocityShader.uniform( "invFboDim", 1.0f/(float)mFboDim );
	mVelocityShader.uniform( "pFboDim", mP_FboDim );
	mVelocityShader.uniform( "pInvFboDim", 1.0f/(float)mP_FboDim );
	mVelocityShader.uniform( "dt", mRoom.mTimeAdjusted );
	mVelocityShader.uniform( "power", mRoom.getPower() );
	gl::drawSolidRect( mFboBounds );
	mVelocityShader.unbind();
	
	mVelocityFbos[ mThisFbo ].unbindFramebuffer();
}

// FISH POSITION
void FlockingApp::drawIntoPositionFbo()
{	
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	mPositionFbos[ mThisFbo ].bindFramebuffer();
	mPositionFbos[ mPrevFbo ].bindTexture( 0 );
	mVelocityFbos[ mThisFbo ].bindTexture( 1 );
	
	mPositionShader.bind();
	mPositionShader.uniform( "position", 0 );
	mPositionShader.uniform( "velocity", 1 );
	mPositionShader.uniform( "dt", mRoom.mTimeAdjusted );
	gl::drawSolidRect( mFboBounds );
	mPositionShader.unbind();
	
	mPositionFbos[ mThisFbo ].unbindFramebuffer();
}

// PREDATOR VELOCITY
void FlockingApp::drawIntoPredatorVelocityFbo()
{
	gl::setMatricesWindow( mP_FboSize, false );
	gl::setViewport( mP_FboBounds );
	
	mP_VelocityFbos[ mThisFbo ].bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	
	mP_PositionFbos[ mPrevFbo ].bindTexture( 0 );
	mP_VelocityFbos[ mPrevFbo ].bindTexture( 1 );
	mPositionFbos[ mPrevFbo ].bindTexture( 2 );
	mLanternsFbo.bindTexture( 3 );
	mP_VelocityShader.bind();
	mP_VelocityShader.uniform( "positionTex", 0 );
	mP_VelocityShader.uniform( "velocityTex", 1 );
	mP_VelocityShader.uniform( "preyPositionTex", 2 );
	mP_VelocityShader.uniform( "lanternsTex", 3 );
	mP_VelocityShader.uniform( "numLights", (float)mController.mNumLanterns );
	mP_VelocityShader.uniform( "invNumLights", 1.0f/(float)MAX_LANTERNS );
	mP_VelocityShader.uniform( "invNumLightsHalf", 1.0f/(float)MAX_LANTERNS * 0.5f );
	mP_VelocityShader.uniform( "att", 1.015f );
	mP_VelocityShader.uniform( "roomBounds", mRoom.getDims() );
	mP_VelocityShader.uniform( "fboDim", mP_FboDim );
	mP_VelocityShader.uniform( "invFboDim", 1.0f/(float)mP_FboDim );
	mP_VelocityShader.uniform( "preyFboDim", mFboDim );
	mP_VelocityShader.uniform( "invPreyFboDim", 1.0f/(float)mFboDim );
	
	mP_VelocityShader.uniform( "dt", mRoom.mTimeAdjusted );
	mP_VelocityShader.uniform( "power", mRoom.getPower() );
	gl::drawSolidRect( mP_FboBounds );
	mP_VelocityShader.unbind();
	
	mP_VelocityFbos[ mThisFbo ].unbindFramebuffer();
}

// PREDATOR POSITION
void FlockingApp::drawIntoPredatorPositionFbo()
{
	gl::setMatricesWindow( mP_FboSize, false );
	gl::setViewport( mP_FboBounds );
	
	mP_PositionFbos[ mThisFbo ].bindFramebuffer();
	mP_PositionFbos[ mPrevFbo ].bindTexture( 0 );
	mP_VelocityFbos[ mThisFbo ].bindTexture( 1 );
	
	mP_PositionShader.bind();
	mP_PositionShader.uniform( "position", 0 );
	mP_PositionShader.uniform( "velocity", 1 );
	mP_PositionShader.uniform( "dt", mRoom.mTimeAdjusted );
	gl::drawSolidRect( mP_FboBounds );
	mP_PositionShader.unbind();
	
	mP_PositionFbos[ mThisFbo ].unbindFramebuffer();
}

void FlockingApp::drawIntoRoomFbo()
{
	gl::setMatricesWindow( mRoomFbo.getSize(), false );
	gl::setViewport( mRoomFbo.getBounds() );
	
	mRoomFbo.bindFramebuffer();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ), true );
	
	
	gl::disableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	Matrix44f m;
	m.setToIdentity();
	m.scale( mRoom.getDims() );
	
	mLanternsFbo.bindTexture();
	mRoomShader.bind();
	mRoomShader.uniform( "lanternsTex", 0 );
	mRoomShader.uniform( "numLights", (float)mController.mNumLanterns );
	mRoomShader.uniform( "invNumLights", 1.0f/(float)MAX_LANTERNS );
	mRoomShader.uniform( "invNumLightsHalf", 1.0f/(float)MAX_LANTERNS * 0.5f );
	mRoomShader.uniform( "att", 1.25f );
	mRoomShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mRoomShader.uniform( "mMatrix", m );
	mRoomShader.uniform( "eyePos", mSpringCam.mEye );
	mRoomShader.uniform( "roomDims", mRoom.getDims() );
	mRoomShader.uniform( "power", mRoom.getPower() );
	mRoomShader.uniform( "lightPower", mRoom.getLightPower() );
	mRoomShader.uniform( "timePer", mRoom.getTimePer() * 1.5f + 0.5f );
	mRoom.draw();
	mRoomShader.unbind();
	
	mRoomFbo.unbindFramebuffer();
	glDisable( GL_CULL_FACE );
}

void FlockingApp::draw()
{
	if( !mInitUpdateCalled ){
		return;
	}
	gl::clear( ColorA( 0.1f, 0.1f, 0.1f, 0.0f ), true );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	gl::setMatricesWindow( getWindowSize(), false );
	gl::setViewport( getWindowBounds() );
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enable( GL_TEXTURE_2D );
	gl::enableAlphaBlending();
	
	// DRAW ROOM
	mRoomFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );
	
	gl::setMatrices( mSpringCam.getCam() );
	gl::setViewport( getWindowBounds() );
	
	gl::enableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	// DRAW PARTICLES
	mPositionFbos[mPrevFbo].bindTexture( 0 );
	mPositionFbos[mThisFbo].bindTexture( 1 );
	mVelocityFbos[mThisFbo].bindTexture( 2 );
	mLanternsFbo.bindTexture( 3 );
	mShader.bind();
	mShader.uniform( "prevPosition", 0 );
	mShader.uniform( "currentPosition", 1 );
	mShader.uniform( "currentVelocity", 2 );
	mShader.uniform( "lightsTex", 3 );
	mShader.uniform( "numLights", (float)mController.mNumLanterns );
	mShader.uniform( "invNumLights", 1.0f/(float)MAX_LANTERNS );
	mShader.uniform( "invNumLightsHalf", 1.0f/(float)MAX_LANTERNS * 0.5f );
	mShader.uniform( "att", 1.05f );
	mShader.uniform( "eyePos", mSpringCam.mEye );
	mShader.uniform( "power", mRoom.getPower() );
	gl::draw( mVboMesh );
	mShader.unbind();
	
	// DRAW PREDATORS
	mP_PositionFbos[mPrevFbo].bindTexture( 0 );
	mP_PositionFbos[mThisFbo].bindTexture( 1 );
	mP_VelocityFbos[mThisFbo].bindTexture( 2 );
	mLanternsFbo.bindTexture( 3 );
	mP_Shader.bind();
	mP_Shader.uniform( "prevPosition", 0 );
	mP_Shader.uniform( "currentPosition", 1 );
	mP_Shader.uniform( "currentVelocity", 2 );
	mP_Shader.uniform( "lightsTex", 3 );
	mP_Shader.uniform( "numLights", (float)mController.mNumLanterns );
	mP_Shader.uniform( "invNumLights", 1.0f/(float)MAX_LANTERNS );
	mP_Shader.uniform( "invNumLightsHalf", 1.0f/(float)MAX_LANTERNS * 0.5f );
	mP_Shader.uniform( "att", 1.05f );
	mP_Shader.uniform( "eyePos", mSpringCam.mEye );
	mP_Shader.uniform( "power", mRoom.getPower() );
	gl::draw( mP_VboMesh );
	mP_Shader.unbind();
	
	// DRAW LANTERN GLOWS
	if( mRoom.isPowerOn() ){
		gl::disableDepthWrite();
		gl::enableAdditiveBlending();
		float c =  mRoom.getPower();
		gl::color( Color( c, c, c ) );
		mLanternGlowTex.bind();
		mController.drawLanternGlows( mSpringCam.mBillboardRight, mSpringCam.mBillboardUp );
		
		drawGlows();
		drawNebulas();
	}
	
	gl::disable( GL_TEXTURE_2D );
	gl::enableDepthWrite();
	gl::enableAdditiveBlending();
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	
	// DRAW LANTERNS
	mLanternShader.bind();
	mLanternShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mLanternShader.uniform( "eyePos", mSpringCam.mEye );
	mLanternShader.uniform( "mainPower", mRoom.getPower() );
	mLanternShader.uniform( "roomDim", mRoom.getDims() );
	mController.drawLanterns( &mLanternShader );
	mLanternShader.unbind();

	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	
	// DRAW PANEL
	drawInfoPanel();

	
	if( false ){	// DRAW POSITION AND VELOCITY FBOS
		gl::color( Color::white() );
		gl::setMatricesWindow( getWindowSize() );
		gl::enable( GL_TEXTURE_2D );
		mPositionFbos[ mThisFbo ].bindTexture();
		gl::drawSolidRect( Rectf( 5.0f, 5.0f, 105.0f, 105.0f ) );
		
		mPositionFbos[ mPrevFbo ].bindTexture();
		gl::drawSolidRect( Rectf( 106.0f, 5.0f, 206.0f, 105.0f ) );
		
		mVelocityFbos[ mThisFbo ].bindTexture();
		gl::drawSolidRect( Rectf( 5.0f, 106.0f, 105.0f, 206.0f ) );
		
		mVelocityFbos[ mPrevFbo ].bindTexture();
		gl::drawSolidRect( Rectf( 106.0f, 106.0f, 206.0f, 206.0f ) );
	}
	
	mThisFbo	= ( mThisFbo + 1 ) % 2;
	mPrevFbo	= ( mThisFbo + 1 ) % 2;
	
//	if( mSaveFrames ){
//		writeImage( getHomeDirectory() + "Flocking/" + toString( mNumSavedFrames ) + ".png", copyWindowSurface() );
//		mNumSavedFrames ++;
//	}
	
	if( getElapsedFrames()%60 == 0 ){
		console() << "FPS = " << getAverageFps() << std::endl;
	}
}

void FlockingApp::drawGlows()
{
	mGlowTex.bind( 0 );
	mGlowShader.bind();
	mGlowShader.uniform( "glowTex", 0 );
	mGlowShader.uniform( "roomDims", mRoom.getDims() );
	mController.drawGlows( &mGlowShader, mSpringCam.mBillboardRight, mSpringCam.mBillboardUp );
	mGlowShader.unbind();
}

void FlockingApp::drawNebulas()
{
	mNebulaTex.bind( 0 );
	mNebulaShader.bind();
	mNebulaShader.uniform( "nebulaTex", 0 );
	mNebulaShader.uniform( "roomDims", mRoom.getDims() );	
	mController.drawNebulas( &mNebulaShader, mSpringCam.mBillboardRight, mSpringCam.mBillboardUp );
	mNebulaShader.unbind();
}

void FlockingApp::drawInfoPanel()
{
	gl::pushMatrices();
	gl::translate( mRoom.getDims() );
	gl::scale( Vec3f( -1.0f, -1.0f, 1.0f ) );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) * ( 1.0 - mRoom.getPower() ) );
	gl::enableAlphaBlending();
	
	float iconWidth		= 50.0f;
	
	float X0			= 15.0f;
	float X1			= X0 + iconWidth;
	float Y0			= 300.0f;
	float Y1			= Y0 + iconWidth;
	
	// DRAW ROOM NUM AND DESC
	float c = mRoom.getPower() * 0.5f + 0.5f;
	gl::color( ColorA( c, c, c, 0.5f ) );
	gl::draw( mIconTex, Rectf( X0, Y0, X1, Y1 ) );
	
	c = mRoom.getPower();
	gl::color( ColorA( c, c, c, 0.5f ) );
	gl::disable( GL_TEXTURE_2D );
	
	// DRAW TIME BAR
	float timePer		= mRoom.getTimePer();
	gl::drawSolidRect( Rectf( Vec2f( X0, Y1 + 2.0f ), Vec2f( X0 + timePer * ( iconWidth ), Y1 + 2.0f + 4.0f ) ) );
	
	// DRAW FPS BAR
	float fpsPer		= getAverageFps()/60.0f;
	gl::drawSolidRect( Rectf( Vec2f( X0, Y1 + 4.0f + 4.0f ), Vec2f( X0 + fpsPer * ( iconWidth ), Y1 + 4.0f + 6.0f ) ) );
	
	
	gl::popMatrices();
}


// HOLDS DATA FOR LANTERNS AND PREDATORS
void FlockingApp::drawIntoLanternsFbo()
{
	Surface32f lanternsSurface( mLanternsFbo.getTexture() );
	Surface32f::Iter it = lanternsSurface.getIter();
	while( it.line() ){
		while( it.pixel() ){
			int index = it.x();
			
			if( it.y() == 0 ){ // set light position
				if( index < (int)mController.mLanterns.size() ){
					it.r() = mController.mLanterns[index].mPos.x;
					it.g() = mController.mLanterns[index].mPos.y;
					it.b() = mController.mLanterns[index].mPos.z;
					it.a() = mController.mLanterns[index].mRadius;
				} else { // if the light shouldnt exist, put it way out there
					it.r() = 0.0f;
					it.g() = 0.0f;
					it.b() = 0.0f;
					it.a() = 1.0f;
				}
			} else {	// set light color
				if( index < (int)mController.mLanterns.size() ){
					it.r() = mController.mLanterns[index].mColor.r;
					it.g() = mController.mLanterns[index].mColor.g;
					it.b() = mController.mLanterns[index].mColor.b;
					it.a() = 1.0f;
				} else { 
					it.r() = 0.0f;
					it.g() = 0.0f;
					it.b() = 0.0f;
					it.a() = 1.0f;
				}
			}
		}
	}

	mLanternsFbo.bindFramebuffer();
	gl::setMatricesWindow( mLanternsFbo.getSize(), false );
	gl::setViewport( mLanternsFbo.getBounds() );
	gl::draw( gl::Texture( lanternsSurface ) );
	mLanternsFbo.unbindFramebuffer();
}



CINDER_APP_BASIC( FlockingApp, RendererGl )
