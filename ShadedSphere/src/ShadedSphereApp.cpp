#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include "CubeMap.h"
#include "Room.h"
#include "SpringCam.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_FBO_RES	2

class ShadedSphereApp : public AppBasic {
  public:
	virtual void	prepareSettings( Settings *settings );
	virtual void	setup();
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseMove( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );
	virtual void	update();
	void			drawIntoRoomFbo();
	virtual void	draw();
	void			drawSphere();

	
	// CAMERA
	SpringCam			mSpringCam;
	
	// SHADERS
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mSphereShader;
	
	// TEXTURES
	CubeMap				mCubeMap;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;

	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
};

void ShadedSphereApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
}

void ShadedSphereApp::setup()
{
	// CAMERA	
	mSpringCam			= SpringCam( -466.0f, getWindowAspectRatio() );
	
	// LOAD SHADERS
	try {
		mRoomShader		= gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
		mSphereShader	= gl::GlslProg( loadResource( "sphere.vert" ), loadResource( "sphere.frag" ) );
	} catch( gl::GlslProgCompileExc e ) {
		std::cout << e.what() << std::endl;
		quit();
	}
	
	// LOAD TEXTURES
	mCubeMap			= CubeMap( GLsizei(512), GLsizei(512),
								  Surface8u( loadImage( loadResource( RES_CUBE1_ID ) ) ),
								  Surface8u( loadImage( loadResource( RES_CUBE2_ID ) ) ),
								  Surface8u( loadImage( loadResource( RES_CUBE3_ID ) ) ),
								  Surface8u( loadImage( loadResource( RES_CUBE4_ID ) ) ),
								  Surface8u( loadImage( loadResource( RES_CUBE5_ID ) ) ),
								  Surface8u( loadImage( loadResource( RES_CUBE6_ID ) ) )
								  );

	// ROOM
	gl::Fbo::Format roomFormat;
	roomFormat.setColorInternalFormat( GL_RGB );
	int fboXRes			= APP_WIDTH/ROOM_FBO_RES;
	int fboYRes			= APP_HEIGHT/ROOM_FBO_RES;
	mRoomFbo			= gl::Fbo( fboXRes, fboYRes, roomFormat );
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 350.0f, 200.0f, 350.0f ), isPowerOn, isGravityOn );
	mRoom.init();
	
	// MOUSE
	mMousePos			= Vec2f::zero();
	mMouseDownPos		= Vec2f::zero();
	mMouseOffset		= Vec2f::zero();
	mMousePressed		= false;
}

void ShadedSphereApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void ShadedSphereApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void ShadedSphereApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void ShadedSphereApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos );
}

void ShadedSphereApp::update()
{
	// ROOM
	mRoom.update();

	// CAMERA
	if( mMousePressed ){
		mSpringCam.dragCam( ( mMouseOffset ) * 0.01f, ( mMouseOffset ).length() * 0.01 );
	}
	mSpringCam.update( 0.4f );

	// ROOM
	drawIntoRoomFbo();
}

void ShadedSphereApp::drawIntoRoomFbo()
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
	
	//	mLightsTex.bind( 0 );
	mRoomShader.bind();
	mRoomShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mRoomShader.uniform( "mMatrix", m );
	mRoomShader.uniform( "eyePos", mSpringCam.getEye() );
	mRoomShader.uniform( "roomDims", mRoom.getDims() );
	mRoomShader.uniform( "mainPower", mRoom.getPower() );
	mRoomShader.uniform( "lightPower", mRoom.getLightPower() );
	mRoom.draw();
	mRoomShader.unbind();
	
	mRoomFbo.unbindFramebuffer();
	glDisable( GL_CULL_FACE );
}

void ShadedSphereApp::draw()
{
	gl::clear( ColorA( 0, 0, 0, 0 ), true );
	gl::color( ColorA( 1, 1, 1, 1 ) );
	
	gl::setMatricesWindow( getWindowSize(), false );
	gl::setViewport( getWindowBounds() );

	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	
	// DRAW ROOM
	mRoomFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );
	
	gl::setMatrices( mSpringCam.getCam() );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	drawSphere();
}

void ShadedSphereApp::drawSphere()
{
	gl::color( ColorA( 1, 1, 1, 1 ) );
	gl::disable( GL_TEXTURE_2D );
	mCubeMap.bind();
	mSphereShader.bind();
	mSphereShader.uniform( "cubeMap", 0 );
	mSphereShader.uniform( "radius", 70.0f );
	mSphereShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mSphereShader.uniform( "eyePos", mSpringCam.getEye() );
	mSphereShader.uniform( "power", mRoom.getPower() );
	mSphereShader.uniform( "roomDim", mRoom.getDims() );
	gl::drawSphere( Vec3f::zero(), 1.0f, 128 );
	mSphereShader.unbind();
}

CINDER_APP_BASIC( ShadedSphereApp, RendererGl )
