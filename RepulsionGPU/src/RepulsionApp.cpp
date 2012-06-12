#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "Room.h"
#include "SpringCam.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using std::sort;


#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define FBO_WIDTH		60	// THIS
#define FBO_HEIGHT		60	// TIMES THIS IS THE NUMBER OF OBJECTS
#define ROOM_FBO_RES	2
#define MAX_LIGHTS		3
#define GRAVITY			-0.02f

class RepulsionApp : public AppBasic {
  public:
	
	
	virtual void	prepareSettings( Settings *settings );
	virtual void	setup();
	void			setFboPositions( gl::Fbo &fbo );
	void			setFboVelocities( gl::Fbo &fbo );
	void			createSphere( gl::VboMesh &mesh, int res );
	void			drawSphereTri( Vec3f va, Vec3f vb, Vec3f vc, int div, Color c );
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseMove( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );
	virtual void	mouseWheel( MouseEvent event );
	virtual void	keyDown( KeyEvent event );
	void			updateTime();
	virtual void	update();
	void 			drawIntoRoomFbo();
	void			drawIntoVelocityFbo();
	void			drawIntoPositionFbo();
	virtual void	draw();
	void			drawInfoPanel();
	
	// CAMERA
	SpringCam			mSpringCam;

	// SHADERS
	gl::GlslProg		mVelocityShader;
	gl::GlslProg		mPositionShader;
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mShader;
	gl::GlslProg		mPosInitShader;
	gl::GlslProg		mVelInitShader;
	
	// TEXTURES
	gl::Texture			mIconTex;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
	// SPHERES
	gl::VboMesh			mSphereVbo;
	std::vector<Vec3f>	mPosCoords;
	std::vector<Vec3f>	mNormals;
	std::vector<Colorf>	mColors;
	
	// POSITION/VELOCITY FBOS
	ci::Vec2f			mFboSize;
	ci::Area			mFboBounds;
	gl::Fbo				mPositionFbos[2];
	gl::Fbo				mVelocityFbos[2];
	int					mThisFbo, mPrevFbo;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
	
	bool				mSaveFrames;
	int					mNumSavedFrames;
};

void RepulsionApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
//	settings->setBorderless();
}

void RepulsionApp::setup()
{
	createSphere( mSphereVbo, 1 );
	
	// CAMERA	
	mSpringCam		= SpringCam( -366.0f, getWindowAspectRatio() );

	
	// POSITION/VELOCITY FBOS
	gl::Fbo::Format format;
	format.enableColorBuffer( true, 2 );
	format.setColorInternalFormat( GL_RGBA16F_ARB );
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	mFboSize		= Vec2f( FBO_WIDTH, FBO_HEIGHT );
	mFboBounds		= Area( 0, 0, FBO_WIDTH, FBO_HEIGHT );
	mPositionFbos[0]	= gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	mPositionFbos[1]	= gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	mVelocityFbos[0]	= gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	mVelocityFbos[1]	= gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	mThisFbo			= 0;
	mPrevFbo			= 1;

	// SHADERS
	try {
		mVelocityShader = gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "Velocity.frag" ) );
		mPositionShader	= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "Position.frag" ) );
		mShader			= gl::GlslProg( loadResource( "VboPos.vert" ), loadResource( "VboPos.frag" ) );
		mRoomShader		= gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
		mPosInitShader	= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "fboPosInit.frag" ) );
		mVelInitShader	= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "fboVelInit.frag" ) );
	} catch( gl::GlslProgCompileExc e ) {
		std::cout << e.what() << std::endl;
		quit();
	}
	
	// TEXTURE FORMAT
	gl::Texture::Format mipFmt;
    mipFmt.enableMipmapping( true );
    mipFmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );    
    mipFmt.setMagFilter( GL_LINEAR );
	
	// LOAD TEXTURES
	mIconTex		= gl::Texture( loadImage( loadResource( "iconRepulsion.png" ) ), mipFmt );

	// ROOM
	gl::Fbo::Format roomFormat;
	roomFormat.setColorInternalFormat( GL_RGB );
	mRoomFbo			= gl::Fbo( APP_WIDTH/ROOM_FBO_RES, APP_HEIGHT/ROOM_FBO_RES, roomFormat );
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 300.0f, 200.0f, 300.0f ), isPowerOn, isGravityOn );	
	mRoom.init();
	
	// MOUSE
	mMousePos		= Vec2f::zero();
	mMouseDownPos	= Vec2f::zero();
	mMouseOffset	= Vec2f::zero();
	mMousePressed	= false;

	setFboPositions( mPositionFbos[0] );
	setFboPositions( mPositionFbos[1] );
	setFboVelocities( mVelocityFbos[0] );
	setFboVelocities( mVelocityFbos[1] );
	
	mSaveFrames		= false;
	mNumSavedFrames	= 0;
}

void RepulsionApp::createSphere( gl::VboMesh &vbo, int res )
{
	float X = 0.525731112119f; 
	float Z = 0.850650808352f;
	
	static Vec3f verts[12] = {
		Vec3f( -X, 0.0f, Z ), Vec3f( X, 0.0f, Z ), Vec3f( -X, 0.0f, -Z ), Vec3f( X, 0.0f, -Z ),
		Vec3f( 0.0f, Z, X ), Vec3f( 0.0f, Z, -X ), Vec3f( 0.0f, -Z, X ), Vec3f( 0.0f, -Z, -X ),
		Vec3f( Z, X, 0.0f ), Vec3f( -Z, X, 0.0f ), Vec3f( Z, -X, 0.0f ), Vec3f( -Z, -X, 0.0f ) };
	
	static GLuint triIndices[20][3] = { 
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1}, {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
	
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticNormals();
	layout.setStaticColorsRGB();
	
	mPosCoords.clear();
	mNormals.clear();
	mColors.clear();
	
	float invWidth = 1.0f/(float)FBO_WIDTH;
	float invHeight = 1.0f/(float)FBO_HEIGHT;
	for( int x = 0; x < FBO_WIDTH; ++x ) {
		for( int y = 0; y < FBO_HEIGHT; ++y ) {
			float u = ( (float)x + 0.5f ) * invWidth;
			float v = ( (float)y + 0.5f ) * invHeight;
			Colorf c = Colorf( u, v, 0.0f );
			
			for( int i=0; i<20; i++ ){
				drawSphereTri( verts[triIndices[i][0]], verts[triIndices[i][1]], verts[triIndices[i][2]], res, c );
			}
		}
	}
	vbo = gl::VboMesh( mPosCoords.size(), 0, layout, GL_TRIANGLES );	
	vbo.bufferPositions( mPosCoords );
	vbo.bufferNormals( mNormals );
	vbo.bufferColorsRGB( mColors );
	vbo.unbindBuffers();
}

void RepulsionApp::drawSphereTri( Vec3f va, Vec3f vb, Vec3f vc, int div, Color c )
{
	if( div <= 0 ){
		mColors.push_back( c );
		mColors.push_back( c );
		mColors.push_back( c );
		mPosCoords.push_back( va );
		mPosCoords.push_back( vb );
		mPosCoords.push_back( vc );
		Vec3f vn = ( va + vb + vc ) * 0.3333f;
		mNormals.push_back( va );
		mNormals.push_back( vb );
		mNormals.push_back( vc );
	} else {
		Vec3f vab = ( ( va + vb ) * 0.5f ).normalized();
		Vec3f vac = ( ( va + vc ) * 0.5f ).normalized();
		Vec3f vbc = ( ( vb + vc ) * 0.5f ).normalized();
		drawSphereTri( va, vab, vac, div-1, c );
		drawSphereTri( vb, vbc, vab, div-1, c );
		drawSphereTri( vc, vac, vbc, div-1, c );
		drawSphereTri( vab, vbc, vac, div-1, c );
	}
}

void RepulsionApp::setFboPositions( gl::Fbo &fbo )
{
	Surface32f pos( fbo.getTexture() );
	Surface32f::Iter it = pos.getIter();
	while( it.line() ){
		while( it.pixel() ){
			Vec3f r = mRoom.getRandRoomPos();
			float mass = Rand::randFloat( 20.0f, 30.0f );
			if( Rand::randFloat() < 0.05f ) 
				mass = Rand::randFloat( 50.0f, 60.0f );
			
			if( it.y() < 5  && it.x() < 50 )
				mass = Rand::randFloat( 300.0f, 5000.0f );
			else
				mass = Rand::randFloat( 50.0f, 300.0f );
			
			it.r() = r.x;
			it.g() = r.y;
			it.b() = r.z;
			it.a() = 100.0f;
		}
	}
	
	gl::Texture posTexture( pos );
	posTexture.bind();
	
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	fbo.bindFramebuffer();
	mPosInitShader.bind();
	mPosInitShader.uniform( "initTex", 0 );
	gl::drawSolidRect( mFboBounds );
	mPosInitShader.unbind();
	fbo.unbindFramebuffer();
}

void RepulsionApp::setFboVelocities( gl::Fbo &fbo )
{
	Surface32f vel( fbo.getTexture() );
	Surface32f::Iter it = vel.getIter();
	while( it.line() ){
		while( it.pixel() ){
			Vec3f r = Rand::randVec3f() * 0.1f;
			it.r() = 0.0f;//r.x;
			it.g() = 0.0f;//r.y;
			it.b() = 0.0f;//r.z;
			it.a() = 0.0f;
		}
	}
	
	gl::Texture velTexture( vel );
	velTexture.bind();
	
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	fbo.bindFramebuffer();
	mVelInitShader.bind();
	mVelInitShader.uniform( "initTex", 0 );
	gl::drawSolidRect( mFboBounds );
	mVelInitShader.unbind();
	fbo.unbindFramebuffer();
}

void RepulsionApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void RepulsionApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void RepulsionApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void RepulsionApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.4f;
}

void RepulsionApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	mRoom.adjustTimeMulti( dWheel );
}

void RepulsionApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case ' ':	mRoom.togglePower();			break;
		case 's':	mSaveFrames = !mSaveFrames;		break;
		default:									break;
	}
}

void RepulsionApp::update()
{
	mRoom.update();

	// CAMERA
	if( mMousePressed ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.01f, ( mMouseOffset ).length() * 0.01 );
	mSpringCam.update( 1.0f );
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::disableAlphaBlending();
	
	drawIntoVelocityFbo();
	drawIntoPositionFbo();
	drawIntoRoomFbo();
}



void RepulsionApp::drawIntoVelocityFbo()
{
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	gl::disableAlphaBlending();
	
	mVelocityFbos[ mThisFbo ].bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	
	mPositionFbos[ mPrevFbo ].bindTexture( 0 );
	mVelocityFbos[ mPrevFbo ].bindTexture( 1 );

	mVelocityShader.bind();
	mVelocityShader.uniform( "position", 0 );
	mVelocityShader.uniform( "velocity", 1 );
	mVelocityShader.uniform( "roomBounds", mRoom.getDims() );
	mVelocityShader.uniform( "w", FBO_WIDTH );
	mVelocityShader.uniform( "h", FBO_HEIGHT );
	mVelocityShader.uniform( "invWidth", 1.0f/(float)FBO_WIDTH );
	mVelocityShader.uniform( "invHeight", 1.0f/(float)FBO_HEIGHT );
	mVelocityShader.uniform( "dt", mRoom.getTimeDelta() );
	mVelocityShader.uniform( "mainPower", mRoom.getPower() );
	mVelocityShader.uniform( "gravity", mRoom.getGravity() );
	gl::drawSolidRect( mFboBounds );
	mVelocityShader.unbind();
	
	mVelocityFbos[ mThisFbo ].unbindFramebuffer();
}

void RepulsionApp::drawIntoPositionFbo()
{	
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	mPositionFbos[ mThisFbo ].bindFramebuffer();
	mPositionFbos[ mPrevFbo ].bindTexture( 0, 0 );
	mPositionFbos[ mPrevFbo ].bindTexture( 1, 1 );
	mVelocityFbos[ mThisFbo ].bindTexture( 2, 0 );
	mVelocityFbos[ mThisFbo ].bindTexture( 3, 1 );	
	mPositionShader.bind();
	mPositionShader.uniform( "pos0Tex", 0 );
	mPositionShader.uniform( "pos1Tex", 1 );
	mPositionShader.uniform( "vel0Tex", 2 );
	mPositionShader.uniform( "vel1Tex", 3 );
	mPositionShader.uniform( "dt", mRoom.getTimeDelta() );
	gl::drawSolidRect( mFboBounds );
	mPositionShader.unbind();
	
	mPositionFbos[ mThisFbo ].unbindFramebuffer();
}

void RepulsionApp::drawIntoRoomFbo()
{
	mRoomFbo.bindFramebuffer();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ), true );
	
	gl::setMatricesWindow( mRoomFbo.getSize(), false );
	gl::setViewport( mRoomFbo.getBounds() );
	gl::disableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	Matrix44f m;
	m.setToIdentity();
	m.scale( mRoom.getDims() );
	
	mRoomShader.bind();
	mRoomShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mRoomShader.uniform( "mMatrix", m );
	mRoomShader.uniform( "eyePos", mSpringCam.getEye() );
	mRoomShader.uniform( "roomDims", mRoom.getDims() );
	mRoomShader.uniform( "power", mRoom.getPower() );
	mRoomShader.uniform( "lightPower", mRoom.getLightPower() );
	mRoom.draw();
	mRoomShader.unbind();
	
	mRoomFbo.unbindFramebuffer();
	glDisable( GL_CULL_FACE );
}

void RepulsionApp::draw()
{
	// clear out the window with black
	gl::clear( ColorA( 0.1f, 0.1f, 0.1f, 0.0f ), true );
	
	gl::setMatricesWindow( getWindowSize(), false );
	gl::setViewport( getWindowBounds() );

	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enable( GL_TEXTURE_2D );
	gl::enableAlphaBlending();
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	// DRAW ROOM
	mRoomFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );
	
	gl::setMatrices( mSpringCam.getCam() );
	
	// DRAW PANEL
	drawInfoPanel();
	gl::enable( GL_TEXTURE_2D );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	// DRAW PARTICLES
	mPositionFbos[mThisFbo].bindTexture( 0 );
	mVelocityFbos[mThisFbo].bindTexture( 1 );
	mShader.bind();
	mShader.uniform( "currentPosition", 0 );
	mShader.uniform( "currentVelocity", 1 );
	mShader.uniform( "eyePos", mSpringCam.getEye() );
	mShader.uniform( "power", mRoom.getPower() );
	gl::draw( mSphereVbo );
	mShader.unbind();
	
	if( mSaveFrames && mNumSavedFrames < 15000 ){
		writeImage( getHomeDirectory() + "RepulsionGPU/" + toString( mNumSavedFrames ) + ".png", copyWindowSurface() );
		mNumSavedFrames ++;
	}
	
	mThisFbo	= ( mThisFbo + 1 ) % 2;
	mPrevFbo	= ( mThisFbo + 1 ) % 2;
}

void RepulsionApp::drawInfoPanel()
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
	
	// DRAW ICON
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



CINDER_APP_BASIC( RepulsionApp, RendererGl )
