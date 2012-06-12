#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include "CubeMap.h"
#include "Controller.h"
#include "Room.h"
#include "SpringCam.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_FBO_RES	2


class BigBangApp : public AppBasic {
  public:
	virtual void	prepareSettings( Settings *settings );
	virtual void	setup();
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseMove( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );
	virtual void	mouseWheel( MouseEvent event );
	virtual void	keyDown( KeyEvent event );
	virtual void	update();
	void			drawIntoRoomFbo();
	virtual void	draw();
	void			drawInfoPanel();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// SHADERS
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mBalloonsShader;
	gl::GlslProg		mConfettiShader;
	
	// TEXTURES
	gl::Texture			mIconTex;
	CubeMap				mCubeMap;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
	// CONTROLLER
	Controller			mController;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
	float				mMouseTimePressed;
};

void BigBangApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
	settings->setBorderless( true );
}

void BigBangApp::setup()
{	
	// CAMERA	
	mSpringCam		= SpringCam( -450.0f, getWindowAspectRatio() );
	
	// LOAD SHADERS
	try {
		mRoomShader		= gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
		mBalloonsShader = gl::GlslProg( loadResource( "balloons.vert" ), loadResource( "balloons.frag" ) );
		mConfettiShader	= gl::GlslProg( loadResource( "confetti.vert" ), loadResource( "confetti.frag" ) );
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
	mIconTex		= gl::Texture( loadImage( loadResource( "iconBigBang.png" ) ) );
	mCubeMap		= CubeMap( GLsizei(512), GLsizei(512),
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
	mRoomFbo			= gl::Fbo( APP_WIDTH/ROOM_FBO_RES, APP_HEIGHT/ROOM_FBO_RES, roomFormat );
	bool isPowerOn		= true;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 350.0f, 200.0f, 350.0f ), isPowerOn, isGravityOn );	
	mRoom.init();
	
	
	// MOUSE
	mMousePos			= Vec2f::zero();
	mMouseDownPos		= Vec2f::zero();
	mMouseOffset		= Vec2f::zero();
	mMousePressed		= false;
	mMouseTimePressed	= 0.0f;

	// CONTROLLER
	mController.init( &mRoom );
}

void BigBangApp::mouseDown( MouseEvent event )
{
	mMouseDownPos		= event.getPos();
	mMousePressed		= true;
	mMouseOffset		= Vec2f::zero();
	mMouseTimePressed	= getElapsedSeconds();
}

void BigBangApp::mouseUp( MouseEvent event )
{
	mMousePressed		= false;
	mMouseOffset		= Vec2f::zero();
	
	float durationOfClick = getElapsedSeconds() - mMouseTimePressed;
	if( durationOfClick < 0.2f ) mController.checkForBalloonPop( mMousePos );
}

void BigBangApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void BigBangApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.4f;
}

void BigBangApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	mRoom.adjustTimeMulti( dWheel );
}

void BigBangApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case '1':	mController.preset( 1 );		break;
		case '2':	mController.preset( 2 );		break;
		case ' ':	mRoom.togglePower();			
					if( !mRoom.isPowerOn() )
						mController.bang();
					else
						mController.clear();
					break;
		default:									break;
	}
	
	switch( event.getCode() ){
		case KeyEvent::KEY_UP:		mSpringCam.setEye( mRoom.getCornerCeilingPos() );	break;
		case KeyEvent::KEY_DOWN:	mSpringCam.setEye( mRoom.getCornerFloorPos() );		break;
		case KeyEvent::KEY_RIGHT:	mSpringCam.resetEye();								break;
		default: break;
	}
}



void BigBangApp::update()
{	
	// CAMERA
	if( mMousePressed ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.02f, ( mMouseOffset ).length() * 0.02f );
	mSpringCam.update( 0.5f );
	
	// ROOM
	mRoom.update();
	
	// CONTROLLER
	mController.update( mSpringCam.getCam() );
	
	drawIntoRoomFbo();
}

void BigBangApp::drawIntoRoomFbo()
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

	mCubeMap.bind();
	mRoomShader.bind();
	mRoomShader.uniform( "cubeMap", 0 );
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

void BigBangApp::draw()
{
	gl::clear( ColorA( 0.1f, 0.1f, 0.1f, 0.0f ), true );
	
	gl::setMatricesWindow( getWindowSize(), false );
	gl::setViewport( getWindowBounds() );

	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	// DRAW ROOM FBO
	mRoomFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );
	
	gl::setMatrices( mSpringCam.getCam() );
	
	// DRAW INFO PANEL
	drawInfoPanel();
	
	gl::disable( GL_TEXTURE_2D );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	
	if( mController.mPresetIndex == 2 ){
		mController.drawPhysics();
	} else {
		// CONFETTI
		mConfettiShader.bind();
		mConfettiShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
		mConfettiShader.uniform( "eyePos", mSpringCam.getEye() );
		mConfettiShader.uniform( "power", mRoom.getPower() );
		mConfettiShader.uniform( "roomDims", mRoom.getDims() );
		mController.drawConfettis( &mConfettiShader );
		mConfettiShader.unbind();
		
		// BALLOONS
		gl::enable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
		mCubeMap.bind();
		mBalloonsShader.bind();
		mBalloonsShader.uniform( "cubeMap", 0 );
		mBalloonsShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
		mBalloonsShader.uniform( "eyePos", mSpringCam.getEye() );
		mBalloonsShader.uniform( "power", mRoom.getPower() );
		mBalloonsShader.uniform( "roomDims", mRoom.getDims() );
		mBalloonsShader.uniform( "att", 1.015f );
		mController.drawBalloons( &mBalloonsShader );
		mBalloonsShader.unbind();
		gl::disable( GL_CULL_FACE );
		
		// STREAMERS
		mController.drawStreamers();
	}
}

void BigBangApp::drawInfoPanel()
{
	gl::pushMatrices();
	gl::translate( mRoom.getDims() );
	gl::scale( Vec3f( -1.0f, -1.0f, 1.0f ) );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) * ( 1.0 - mRoom.getPower() ) );
	gl::enableAlphaBlending();
	
	float iconWidth		= 50.0f;
	
	float X0			= 15.0f;
	float X1			= X0 + iconWidth;
	float Y0			= mRoom.getDims().y * 2.0f - iconWidth - 15.0f;//300.0f;
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


CINDER_APP_BASIC( BigBangApp, RendererGl )
