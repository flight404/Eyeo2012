#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
#include "Resources.h"
#include "CubeMap.h"
//#include "cinder/params/Params.h"

#include "SpringCam.h"
#include "Controller.h"
#include "Room.h"

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define NUM_INIT_PARTICLES 650
#define ROOM_FBO_RES	2
#define MAX_LIGHTS		8
using namespace ci;
using namespace ci::app;
using namespace std;



class MatterApp : public AppBasic {
  public:
	void			prepareSettings( Settings *settings );
	void			setup();
	virtual void	keyDown( KeyEvent event );
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseWheel( MouseEvent event );
	virtual void	mouseMove( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );
	virtual void	update();
	void			updateArcball();
	void			updateTime();
	void			drawWarning();
	virtual void	draw();
	void			drawIntoRoomFbo();
	
	void			resizeParticles();
	void			blurParticles();
	void			drawIntoParticlesFbo();
	void			drawToLightsSurface();
	void			drawInfoPanel();
	
	// PARAMS
//	params::InterfaceGl	mParams;
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// POINT LIGHTS
	int					mNumLights;
	float				mInvNumLights;
	Surface32f			mLightsSurface;
	gl::Texture			mLightsTex;
	
	// TEXTURES
	CubeMap				mCubeMap;
	gl::Texture			mGlowsTex;
	gl::Texture			mSmokeTex;
	gl::Texture			mKernelTex;
	gl::Texture			mShadowTex;
	gl::Texture			mWarningTex;
	gl::Texture			mIconTex;
	
	// SHADERS
	gl::GlslProg		mParticleShader;
	gl::GlslProg		mRoomShader;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	float				mRoomScale;
	
	// FBO
	gl::Fbo				mParticlesFbo;
	gl::Fbo				mParticlesBloomFbo[2];
	Vec2i				mFboSize;
	Rectf				mFboRect;
	Area				mFboArea;
	int					mThisFbo, mNextFbo;
	
	// CONDITIONS
	bool				mWarning;
	int					mWarningCount;
	float				mWarningThresh;
	
	// CONTROLLER
	Controller			mController;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
};

void MatterApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
//	settings->setBorderless( true );
}

void MatterApp::setup()
{	
	// CAMERA	
	mSpringCam		= SpringCam( -450.0f, getWindowAspectRatio() );
	
	// POINT LIGHTS
	mNumLights			= MAX_LIGHTS;
	mInvNumLights		= 1.0f/(float)mNumLights;
	gl::Texture::Format hdTexFormat;
	hdTexFormat.setInternalFormat( GL_RGB32F_ARB );
	mLightsTex			= gl::Texture( mNumLights, 2, hdTexFormat );
	mLightsSurface		= Surface32f( mNumLights, 2, false );
	
	// SETUP CUBEMAP
	mCubeMap = CubeMap( GLsizei(512), GLsizei(512),
					   Surface8u( loadImage( loadResource( RES_CUBE1_ID ) ) ),
					   Surface8u( loadImage( loadResource( RES_CUBE2_ID ) ) ),
					   Surface8u( loadImage( loadResource( RES_CUBE3_ID ) ) ),
					   Surface8u( loadImage( loadResource( RES_CUBE4_ID ) ) ),
					   Surface8u( loadImage( loadResource( RES_CUBE5_ID ) ) ),
					   Surface8u( loadImage( loadResource( RES_CUBE6_ID ) ) )
					   );
	
	// TEXTURES
	mGlowsTex		= gl::Texture( loadImage( loadResource( "glow.png" ) ) );
	mSmokeTex		= gl::Texture( loadImage( loadResource( "smoke.png" ) ) );
	mShadowTex		= gl::Texture( loadImage( loadResource( "shadow.png" ) ) );
	mWarningTex		= gl::Texture( loadImage( loadResource( "warning.png" ) ) );
	mIconTex		= gl::Texture( loadImage( loadResource( "iconMatter.png" ) ) );
	
	// SHADERS
	try {
		mRoomShader			= gl::GlslProg( loadResource( "room.vert"	),		loadResource( "room.frag" ) );
		mParticleShader		= gl::GlslProg( loadResource( "particle.vert" ),	loadResource( "particle.frag" ) );	
	} catch( gl::GlslProgCompileExc e ) {
		std::cout << e.what() << std::endl;
		quit();
	}

	// FBO
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_RGB );
	
	gl::Fbo::Format formatAlpha;
//	std::cout << gl::Fbo::getMaxSamples() << std::endl;
//	formatAlpha.setSamples( 2 ); // not using because the result is speckly, unknown why
	formatAlpha.setColorInternalFormat( GL_RGBA32F_ARB );
	
	int fboXRes		= APP_WIDTH/ROOM_FBO_RES;
	int fboYRes		= APP_HEIGHT/ROOM_FBO_RES;	
	mFboSize		= Vec2i( fboXRes, fboYRes );
	mFboRect		= Rectf( 0.0f, 0.0f, (float)fboXRes, (float)fboYRes );
	mFboArea		= Area( 0, 0, fboXRes, fboYRes );
	mRoomFbo		= gl::Fbo( fboXRes, fboYRes, format );
	
	mParticlesFbo	= gl::Fbo( APP_WIDTH, APP_HEIGHT, formatAlpha );
	
	mParticlesFbo.bindFramebuffer();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ), true );
	mParticlesFbo.unbindFramebuffer();
	
	mThisFbo = 0;
	mNextFbo = 1;
	mParticlesBloomFbo[0] = gl::Fbo( fboXRes, fboYRes, format );
	mParticlesBloomFbo[1] = gl::Fbo( fboXRes, fboYRes, format );
	
	// CONDITIONS
	mWarning		= false;
	mWarningCount	= 0;
	mWarningThresh	= 0.2f;

	// ROOM
	gl::Fbo::Format roomFormat;
	roomFormat.setColorInternalFormat( GL_RGB );
	mRoomFbo			= gl::Fbo( APP_WIDTH/ROOM_FBO_RES, APP_HEIGHT/ROOM_FBO_RES, roomFormat );
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 350.0f, 200.0f, 350.0f ), isPowerOn, isGravityOn );	
	mRoom.init();
	mRoomScale			= 1.0f;

	// PARTICLE CONTROLLER
	mController.init( &mRoom );
	
	// MOUSE
	mMousePos		= Vec2f::zero();
	mMouseDownPos	= Vec2f::zero();
	mMouseOffset	= Vec2f::zero();
	mMousePressed	= false;
	
//	mParams = params::InterfaceGl( "Matter/Antimatter", Vec2i( 200, 150 ) );
//	mParams.addParam( "Time Multi", &mTimeMulti, "min=0.0 max=120.0 step=1.0 keyIncr=t keyDecr=T" );
}





void MatterApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
	
	if( event.isRight() ){
		mSpringCam.setFov( 40.0f );
	}
}

void MatterApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
	
	if( event.isRight() ){
		mSpringCam.setFov( 65.0f );
	}
}

void MatterApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void MatterApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos * Vec2f( 1.0f, 2.0f ) - mMouseDownPos * Vec2f( 1.0f, 2.0f ) ) * 0.8f;
}

void MatterApp::mouseWheel( MouseEvent event )
{
	float dWheel = event.getWheelIncrement();
	
	if( event.isShiftDown() ){
		mRoomScale = constrain( mRoomScale - dWheel * 0.001f, 0.1f, 1.0f );
	} else {
		mRoom.adjustTimeMulti( dWheel );
	}
}



void MatterApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case ' ' : mRoom.togglePower();
				   mRoom.toggleGravity();
				   break;
		case 'g' : mRoom.toggleGravity();			break;
		case 'r' : mController.reset();				break;
		case 'c' : mController.clearRoom();			break;
		case '1' : mController.preset( 1 );			break;
		case '2' : mController.preset( 2 );			break;
		case '3' : mController.preset( 3 );			break;
		case '4' : mController.preset( 4 );			break;
		case '5' : mController.preset( 5 );			break;
		case '6' : mController.preset( 6 );			break;
		case '7' : mController.preset( 7 );			break;
		case '8' : mController.preset( 8 );			break;
		case '9' : mController.preset( 9 );			break;
		case '0' : mController.preset( 10 );		break;
		default:									break;
	}
}

void MatterApp::update()
{
	// ROOM
	mRoom.update( mRoomScale );
	
	// IF THE FRAMERATE DROPS TOO LOW, WARNING
	if( mRoom.mTimeDelta > mWarningThresh && mRoom.mTime > 3.0f ){
		mWarning = true;
		mController.clearRoom();
		mRoomScale = 1.0f;
		mRoom.mIsPowerOn = false;
	}
	
	// ONLY RESET WARNING AFTER FRAMERATE RESUMES + 20 FRAMES
	if( mWarning ) mWarningCount ++;
	if( mWarningCount > 120 ){
		mWarningCount = 0;
		mWarning = false;
	}
	
	// CAMERA
	mSpringCam.update( 0.5f );
	if( mMousePressed ){
		mSpringCam.dragCam( ( mMouseOffset ) * 0.01f, ( mMouseOffset ).length() * 0.01 );
	}
	
	// CONTROLLER
	mController.applyShockwavesToCam( mSpringCam );
	mController.applyForceToParticles();
	mController.update();
	
	mNumLights		= math<int>::min( (int)mController.mShockwaves.size(), MAX_LIGHTS );

	drawIntoRoomFbo();
	drawIntoParticlesFbo();
	drawToLightsSurface();

	// SOUND CONTROLLER
	mController.applyShockwavesToTime();
	
	if( getElapsedFrames()%60 == 0 ){
		std::cout << "FPS = " << getAverageFps() << std::endl;
	}
}


void MatterApp::drawIntoRoomFbo()
{
	mRoomFbo.bindFramebuffer();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ), true );
	
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboArea );
	gl::disableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	Matrix44f m;
	m.setToIdentity();
	m.scale( mRoom.getDims() );
	
	mLightsTex.bind( 0 );
	mRoomShader.bind();
	mRoomShader.uniform( "lightsTex", 0 );
	mRoomShader.uniform( "numLights", (float)mNumLights );
	mRoomShader.uniform( "invNumLights", mInvNumLights );
	mRoomShader.uniform( "invNumLightsHalf", mInvNumLights * 0.5f );
	mRoomShader.uniform( "att", 1.015f );
	mRoomShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mRoomShader.uniform( "mMatrix", m );
	mRoomShader.uniform( "eyePos", mSpringCam.mCam.getEyePoint() );
	mRoomShader.uniform( "roomDim", mRoom.getDims() );
	mRoomShader.uniform( "power", mRoom.getPower() );
	mRoomShader.uniform( "lightPower", mRoom.getLightPower() );
	mRoomShader.uniform( "timePer", mRoom.getTimePer() * 1.5f + 0.5f );
	mRoom.draw();
	mRoomShader.unbind();
	
	mRoomFbo.unbindFramebuffer();
	glDisable( GL_CULL_FACE );
}

void MatterApp::drawIntoParticlesFbo()
{
	mParticlesFbo.bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	//	gl::setMatricesWindow( getWindowSize(), false );
	gl::setMatrices( mSpringCam.getCam() );
	gl::setViewport( mParticlesFbo.getBounds() );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
//	// DRAW PANEL
//	gl::color( ColorA( mMainPower, mMainPower, mMainPower, mMainPower * 0.1f + 0.9f ) );
//	mRoomPanelTex.bind();
//	mRoom.drawPanel( mRoomPanelTex, 0.8f );
	
	gl::enableAlphaBlending();
	gl::disableDepthWrite();
	mShadowTex.bind();
	mController.drawParticleShadows();
	
	gl::enableDepthWrite();
	mCubeMap.bind();
	mLightsTex.bind( 1 );
	mParticleShader.bind();
	mParticleShader.uniform( "cubeMap", 0 );
	mParticleShader.uniform( "lightsTex", 1 );
	mParticleShader.uniform( "numLights", (float)mNumLights );
	mParticleShader.uniform( "invNumLights", mInvNumLights );
	mParticleShader.uniform( "invNumLightsHalf", mInvNumLights * 0.5f );
	mParticleShader.uniform( "att", 1.015f );
	mParticleShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mParticleShader.uniform( "eyePos", mSpringCam.mCam.getEyePoint() );
	mParticleShader.uniform( "time", (float)getElapsedSeconds() );
	mParticleShader.uniform( "mainPower", mRoom.getPower() );
	mParticleShader.uniform( "roomDim", mRoom.getDims() );
	mController.draw( &mParticleShader );
	mParticleShader.unbind();
	
	mParticlesFbo.unbindFramebuffer();
}

void MatterApp::draw()
{
	float power = mRoom.getPower();
	
	gl::clear( ColorA( 0, 0, 0, 0 ), true );
	
	gl::setMatricesWindow( getWindowSize(), false );
	gl::setViewport( getWindowBounds() );
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();
	gl::enable( GL_TEXTURE_2D );
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	// DRAW ROOM
	mRoomFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );

	gl::setMatrices( mSpringCam.getCam() );

	gl::color( Color( power, power, power ) * 0.85f + 0.15f );
	drawInfoPanel();
	gl::enable( GL_TEXTURE_2D );
	
	gl::setMatricesWindow( getWindowSize(), false );
	gl::enableAlphaBlending();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	// DRAW PARTICLES FBO
	mParticlesFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );
	
	gl::enableAdditiveBlending();
	
	gl::setMatrices( mSpringCam.getCam() );	
	gl::enableDepthRead();
	
	// DRAW SHOCKWAVES
	mGlowsTex.bind();
	mController.drawShockwaves( mSpringCam.mCam.getEyePoint() );

//	gl::enableAlphaBlending();
	
	// DRAW SMOKES
	Vec3f right, up;
	mSpringCam.mCam.getBillboardVectors( &right, &up );
	mSmokeTex.bind();
	mController.drawSmokes( right, up );
	
	gl::disable( GL_TEXTURE_2D );
	mController.drawPhotons();
	
	if( power > 0.99f )
		mController.finish();
	
	if( mWarning ){
		drawWarning();
	}
//	params::InterfaceGl::draw();
	
//	writeImage( getHomeDirectory() + "Matter/" + toString( getElapsedFrames() ) + ".png", copyWindowSurface() );
}

void MatterApp::drawWarning()
{
	gl::enable( GL_TEXTURE_2D );
	gl::setMatricesWindow( getWindowSize() );
	gl::color( Color( 1, 1, 1 ) );
	mWarningTex.bind();
	gl::drawSolidRect( getWindowBounds() );
}

void MatterApp::drawInfoPanel()
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

void MatterApp::drawToLightsSurface()
{
	Surface32f::Iter iter = mLightsSurface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			int index = iter.x();
			
			if( iter.y() == 0 ){ // set light position
				if( index < (int)mController.mShockwaves.size() ){
					iter.r() = mController.mShockwaves[index].mPos.x;
					iter.g() = mController.mShockwaves[index].mPos.y;
					iter.b() = mController.mShockwaves[index].mPos.z;
				} else { // if the light shouldnt exist, put it way out there
					iter.r() = 0.0f;
					iter.g() = 0.0f;
					iter.b() = 0.0f;
				}
			} else {	// set light color
				if( index < (int)mController.mShockwaves.size() ){
					float agePer = mController.mShockwaves[index].mAgePer;
					iter.r() = mController.mShockwaves[index].mColor.r * agePer;
					iter.g() = mController.mShockwaves[index].mColor.g * agePer;
					iter.b() = mController.mShockwaves[index].mColor.b * agePer;
				} else { 
					iter.r() = 0.0f;
					iter.g() = 0.0f;
					iter.b() = 0.0f;
				}
			}
			
		}
	}
	
	mLightsTex = gl::Texture( mLightsSurface );
}

CINDER_APP_BASIC( MatterApp, RendererGl )
