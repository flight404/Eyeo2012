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
#include "cinder/params/Params.h"

#include "SpringCam.h"
#include "Controller.h"
#include "Room.h"


#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_FBO_RES	2
#define MAX_LIGHTS		8
#define GRAVITY			-0.2f

using namespace ci;
using namespace ci::app;
using namespace std;

class AnnihilationApp : public AppBasic {
  public:
	void			prepareSettings( Settings *settings );
	void			setup();
	virtual void	keyDown( KeyEvent event );
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseMove( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );
	virtual void	mouseWheel( MouseEvent event );
	virtual void	update();

	virtual void	draw();
	void			drawIntoRoomFbo();
	void			drawIntoParticlesFbo();
	void			drawInfoPanel();
	void			drawWarning();
	void			resizeParticlesFbo();
	void			blurParticlesFbo();
	void			drawToLightsSurface();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// POINT LIGHTS
	int					mNumLights;
	float				mInvNumLights;
	Surface32f			mLightsSurface;
	gl::Texture			mLightsTex;
	
	// SHADERS
	gl::GlslProg		mMatterShader;
	gl::GlslProg		mAntimatterShader;
	gl::GlslProg		mNodesShader;
	gl::GlslProg		mParticleShader;
	gl::GlslProg		mHairShader;
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mContrastShader;
	gl::GlslProg		mBlurShader;
	
	// TEXTURES
	CubeMap				mCubeMap;
	gl::Texture			mGlowTex;
	gl::Texture			mSmokeTex;
	gl::Texture			mKernelTex;
	gl::Texture			mShadowTex;
	gl::Texture			mSpectrumTex;
	gl::Texture			mWarningTex;
	gl::Texture			mIconTex;
	

	// FBO
	int					mThisFbo, mNextFbo;
	gl::Fbo				mParticlesFbo;
	gl::Fbo				mBloomFbo[2];
	Vec2i				mFboSize;
	Rectf				mFboRect;
	Area				mFboArea;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
	// PARTICLE CONTROLLER
	Controller			mController;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
	
	// CONDITIONS
	bool				mWarning;
	int					mWarningCount;
	float				mWarningThresh;
};

void AnnihilationApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
	settings->setFrameRate( 60.0f );
	settings->setBorderless();
}

void AnnihilationApp::setup()
{	
	// CAMERA	
	mSpringCam		= SpringCam( -575.0f, getWindowAspectRatio() );
	
	// POINT LIGHTS
	mNumLights			= MAX_LIGHTS;
	mInvNumLights		= 1.0f/(float)mNumLights;
	gl::Texture::Format hdTexFormat;
	hdTexFormat.setInternalFormat( GL_RGBA32F_ARB );
	mLightsTex			= gl::Texture( mNumLights, 2, hdTexFormat );
	mLightsSurface		= Surface32f( mNumLights, 2, false );
	
	// SHADERS
	try {
		mRoomShader			= gl::GlslProg( loadResource( "room.vert"	),		loadResource( "room.frag" ) );
		mParticleShader		= gl::GlslProg( loadResource( "particle.vert" ),	loadResource( "particle.frag" ) );
		mBlurShader			= gl::GlslProg( loadResource( "passThru.vert" ),	loadResource( "blur.frag" ) );
		mContrastShader		= gl::GlslProg( loadResource( "passThru.vert" ),	loadResource( "contrast.frag" ) );
		mMatterShader		= gl::GlslProg( loadResource( "matter.vert" ),		loadResource( "matter.frag" ) );
		mHairShader			= gl::GlslProg( loadResource( "hair.vert"	),		loadResource( "hair.frag" ) );
		mAntimatterShader	= gl::GlslProg( loadResource( "antimatter.vert" ),	loadResource( "antimatter.frag" ) );
		mNodesShader		= gl::GlslProg( loadResource( "nodes.vert" ),		loadResource( "nodes.frag" ) );
	} catch( gl::GlslProgCompileExc e ) {
		std::cout << e.what() << std::endl;
		quit();
	}
	
	// TEXTURE FORMAT
	gl::Texture::Format mipFmt;
    mipFmt.enableMipmapping( true );
    mipFmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );    
    mipFmt.setMagFilter( GL_LINEAR );
	
	// TEXTURES
	mIconTex		= gl::Texture( loadImage( loadResource( "iconAttraction.png" ) ), mipFmt );
	mGlowTex		= gl::Texture( loadImage( loadResource( "glow.png" ) ) );
	mSmokeTex		= gl::Texture( loadImage( loadResource( "smoke.png" ) ) );
	mKernelTex		= gl::Texture( loadImage( loadResource( "kernel.png" ) ) );
	mShadowTex		= gl::Texture( loadImage( loadResource( "shadow.png" ) ) );
	mSpectrumTex	= gl::Texture( loadImage( loadResource( "spectrum.png" ) ) );
	mWarningTex		= gl::Texture( loadImage( loadResource( "warning.png" ) ) );
	mCubeMap = CubeMap( GLsizei(512), GLsizei(512),
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
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 500.0f, 280.0f, 350.0f ), isPowerOn, isGravityOn );	
	mRoom.init();

	// FBO
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_RGB );
	
	gl::Fbo::Format formatAlpha;
	formatAlpha.setColorInternalFormat( GL_RGBA32F_ARB );
	
	int fboXRes		= APP_WIDTH/ROOM_FBO_RES;
	int fboYRes		= APP_HEIGHT/ROOM_FBO_RES;	
	mFboSize		= Vec2i( fboXRes, fboYRes );
	mFboRect		= Rectf( 0.0f, 0.0f, (float)fboXRes, (float)fboYRes );
	mFboArea		= Area( 0, 0, fboXRes, fboYRes );
	
	mParticlesFbo	= gl::Fbo( APP_WIDTH, APP_HEIGHT, formatAlpha );
	mParticlesFbo.bindFramebuffer();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ), true );
	mParticlesFbo.unbindFramebuffer();
	
	mRoomFbo		= gl::Fbo( fboXRes, fboYRes, format );
	mRoomFbo.bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	mRoomFbo.unbindFramebuffer();
	
	mThisFbo = 0;
	mNextFbo = 1;
	mBloomFbo[0] = gl::Fbo( fboXRes, fboYRes, format );
	mBloomFbo[1] = gl::Fbo( fboXRes, fboYRes, format );
	
	// MOUSE
	mMousePos		= Vec2f::zero();
	mMouseDownPos	= Vec2f::zero();
	mMouseOffset	= Vec2f::zero();
	mMousePressed	= false;

	// CONTROLLER
	mController.init( &mRoom );
	
	// CONDITIONS
	mWarning		= false;
	mWarningCount	= 0;
	mWarningThresh	= 0.2f;
}

void AnnihilationApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void AnnihilationApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void AnnihilationApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void AnnihilationApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.4f;
}

void AnnihilationApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	mRoom.adjustTimeMulti( dWheel );
}


void AnnihilationApp::keyDown( KeyEvent event )
{
	switch ( event.getChar() ) {
		case ' ':	mRoom.togglePower();			break;
		case '1':	mController.mDistPer = 1.0f;	break;
		case '2':	mController.mDistPer = 0.9f;	break;
		case '3':	mController.mDistPer = 0.8f;	break;
		case '4':	mController.mDistPer = 0.7f;	break;
		case '5':	mController.mDistPer = 0.6f;	break;
		case '6':	mController.mDistPer = 0.5f;	break;
		case '7':	mController.mDistPer = 0.4f;	break;
		case '8':	mController.mDistPer = 0.3f;	break;
		case '9':	mController.mDistPer = 0.2f;	break;
		case '0':	mController.mDistPer = 0.1f;	break;
		default:	break;
	}
}

void AnnihilationApp::update()
{
	// ROOM
	mRoom.update();
	
	// IF THE FRAMERATE DROPS TOO LOW, WARNING
	if( mController.mBroken ){
		mWarning = true;
	}
	
	// ONLY RESET WARNING AFTER FRAMERATE RESUMES + 20 FRAMES
	if( mWarning ) mWarningCount ++;
	if( mWarningCount > 20 ){
		mWarningCount = 0;
		mWarning = false;
	}
	
	// CAMERA
	if( mMousePressed ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.02f, ( mMouseOffset ).length() * 0.02f );
	mSpringCam.update( 0.5f );
	
	// CONTROLLER
	if( mController.mBroken ){
		mRoom.setPower( false );
		mController.mBroken = false;
	} 
	
	mController.mMatter->mPos.x		= mMousePos.x - getWindowCenter().x;
	mController.mAntimatter->mPos.x	= getWindowWidth() - mMousePos.x - getWindowCenter().x;
	mController.update();

	mNumLights		= math<int>::min( (int)mController.mShockwaves.size(), MAX_LIGHTS );
	
	drawIntoRoomFbo();
	drawIntoParticlesFbo();
	resizeParticlesFbo();
	blurParticlesFbo();
	gl::disableAlphaBlending();
	drawToLightsSurface();
}


void AnnihilationApp::drawIntoRoomFbo()
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
	mRoomShader.uniform( "att", 1.075f );
	mRoomShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mRoomShader.uniform( "mMatrix", m );
	mRoomShader.uniform( "eyePos", mSpringCam.getEye() );
	mRoomShader.uniform( "roomDims", mRoom.getDims() );
	mRoomShader.uniform( "mainPower", mRoom.getPower() );
	mRoomShader.uniform( "lightPower", mRoom.getLightPower() );
	mRoomShader.uniform( "antimatter", Vec4f( mController.mAntimatter->mPos.xyz(), mController.mAntimatter->mRadius ) );
	mRoomShader.uniform( "matter", Vec4f( mController.mMatter->mPos.xyz(), mController.mMatter->mRadius ) );
	mRoom.draw();
	mRoomShader.unbind();
	
	mRoomFbo.unbindFramebuffer();
	glDisable( GL_CULL_FACE );
}


void AnnihilationApp::drawIntoParticlesFbo()
{
	mParticlesFbo.bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ), true );
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	//	gl::setMatricesWindow( getWindowSize(), false );
	gl::setMatrices( mSpringCam.getCam() );
	gl::setViewport( getWindowBounds() );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	// DRAW ANTIMATTER SHADOW
	gl::disableDepthWrite();
	mShadowTex.bind();
	mController.drawMatterShadow( &mRoom );
	
	// DRAW ANTIMATTER
	gl::disable( GL_TEXTURE_2D );
	gl::enableDepthWrite();
	mAntimatterShader.bind();
	mAntimatterShader.uniform( "mainPower", mRoom.getPower() );
	mAntimatterShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mAntimatterShader.uniform( "radius", mController.mAntimatter->mRadius * 2.0f * ( 1.0f + mRoom.getPower() * 0.35f ) );
	mAntimatterShader.uniform( "pos", mController.mAntimatter->mPos );
	mAntimatterShader.uniform( "eyePos", mSpringCam.getEye() );
	mController.drawAntimatter();
	mAntimatterShader.unbind();
	
	mParticlesFbo.unbindFramebuffer();
}



void AnnihilationApp::draw()
{
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
	
	gl::setMatrices( mSpringCam.mCam );
	
	// DRAW PANEL
	drawInfoPanel();
	gl::enable( GL_TEXTURE_2D );
	
	gl::setMatricesWindow( getWindowSize(), false );
	
	// DRAW PARTICLES FBO
	mParticlesFbo.bindTexture();
	gl::drawSolidRect( getWindowBounds() );
	
	gl::enableAdditiveBlending();
	
	// DRAW PARTICLES BLOOM OVERLAY
	if( mRoom.isPowerOn() ){
		mBloomFbo[mThisFbo].bindTexture();
		gl::drawSolidRect( getWindowBounds() );
	}

	gl::setMatrices( mSpringCam.getCam() );	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	gl::disable( GL_TEXTURE_2D );
	
	// DRAW MATTER
	mCubeMap.bind();
	mLightsTex.bind( 1 );
	mMatterShader.bind();
	mMatterShader.uniform( "cubeMap", 0 );
	mMatterShader.uniform( "lightsTex", 1 );
	mMatterShader.uniform( "numLights", (float)mNumLights );
	mMatterShader.uniform( "invNumLights", mInvNumLights );
	mMatterShader.uniform( "invNumLightsHalf", mInvNumLights * 0.5f );
	mMatterShader.uniform( "att", 1.015f );
	mMatterShader.uniform( "time", (float)getElapsedSeconds() );
	mMatterShader.uniform( "radius", mController.mMatter->mRadius );
	mMatterShader.uniform( "pos", mController.mMatter->mPos );
	mMatterShader.uniform( "eyePos", mSpringCam.getEye() );
	mMatterShader.uniform( "mainPower", mRoom.getPower() );
	mMatterShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mMatterShader.uniform( "roomDim", mRoom.getDims() );
	mMatterShader.uniform( "antimatter", Vec4f( mController.mAntimatter->mPos.xyz(), mController.mAntimatter->mRadius ) );
	mController.drawMatter();
	mMatterShader.unbind();
	
	gl::color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
	
	// DRAW HAIRS
	mCubeMap.bind();
	mLightsTex.bind( 1 );
	mController.drawHairs();
	mHairShader.bind();
	mHairShader.uniform( "cubeMap", 0 );
	mHairShader.uniform( "lightsTex", 1 );
	mHairShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mHairShader.uniform( "eyePos", mSpringCam.getEye() );
	mHairShader.uniform( "mainPower", mRoom.getPower() );
	mHairShader.uniform( "numLights", (float)mNumLights );
	mHairShader.uniform( "invNumLights", mInvNumLights );
	mHairShader.uniform( "invNumLightsHalf", mInvNumLights * 0.5f );
	mHairShader.uniform( "att", 1.15f );
	mHairShader.uniform( "time", (float)getElapsedSeconds() );
	gl::draw( mController.mHairMesh );
	mHairShader.unbind();

//	mAntimatterShader.bind();
//	mAntimatterShader.uniform( "radius", mController.mAntimatter->mRadius );
//	mAntimatterShader.uniform( "pos", mController.mAntimatter->mPos );
//	mAntimatterShader.uniform( "eyePos", mSpringCam.getEye() );
//	mAntimatterShader.uniform( "mainPower", mRoom.getPower() );
//	mAntimatterShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
//	mAntimatterShader.uniform( "roomDim", mRoom.getDims() );
//	mController.drawAntimatter();
//	mAntimatterShader.unbind();
	
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	// DRAW NODES
	mNodesShader.bind();
	mNodesShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mNodesShader.uniform( "mainPower", mRoom.getPower() );
	mController.drawNodes( &mNodesShader );
	mNodesShader.unbind();

	// DRAW SHOCKWAVES
	gl::disableDepthWrite();
	gl::disableDepthRead();
	gl::enable( GL_TEXTURE_2D );
	gl::enableAdditiveBlending();
	
	mGlowTex.bind();
	mController.drawShockwaves( mSpringCam.mCam.getEyePoint() );
	
	gl::enableDepthRead();
	
	// DRAW SMOKES
	Vec3f right, up;
	mSpringCam.mCam.getBillboardVectors( &right, &up );
	mSmokeTex.bind();
	mController.drawSmokes( right, up );
	
	// MORE ANTIMATTER GLOW
//	mGlowTex.enableAndBind();
//	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.5f ) );
//	Vec2f rad = Vec2f( 250.0f, 250.0f ) * mMainPower;
//	drawSphericalBillboard( mSpringCam.mEye, mController.mAntimatter.mPos, rad, 0.0f );
	
	gl::disable( GL_TEXTURE_2D );
	gl::enableAdditiveBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	mController.drawSparks();
	
	if( mWarning ){
		drawWarning();
	}
	
	if( getElapsedFrames()%60 == 0 ){
		std::cout << "FPS = " << getAverageFps() << std::endl;
	}
	
//	params::InterfaceGl::draw();
	
//	writeImage( getHomeDirectory() + "Matter/" + toString( getElapsedFrames() ) + ".png", copyWindowSurface() );
}

void AnnihilationApp::drawWarning()
{
	gl::enable( GL_TEXTURE_2D );
	gl::setMatricesWindow( getWindowSize() );
	gl::color( Color( 1, 1, 1 ) );
	mWarningTex.bind();
	gl::drawSolidRect( getWindowBounds() );
}

void AnnihilationApp::drawInfoPanel()
{
	gl::pushMatrices();
	gl::translate( mRoom.getDims() );
	gl::scale( Vec3f( -1.0f, -1.0f, 1.0f ) );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) * ( 1.0 - mRoom.getPower() ) );
	gl::enableAlphaBlending();
	
	float iconWidth		= 75.0f;
	
	float X0			= 15.0f;
	float X1			= X0 + iconWidth;
	float Y0			= 470.0f;
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

void AnnihilationApp::resizeParticlesFbo()
{
	mBloomFbo[mThisFbo].bindFramebuffer();
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboArea );
	mParticlesFbo.bindTexture();
	mContrastShader.bind();
	mContrastShader.uniform( "tex", 0 );
	gl::drawSolidRect( mFboRect );
	mContrastShader.unbind();
	mBloomFbo[mThisFbo].unbindFramebuffer();
}

void AnnihilationApp::blurParticlesFbo()
{
	for( int i=0; i<2; i++ ){
		mThisFbo = ( mThisFbo + 1 ) % 2;
		mNextFbo = ( mThisFbo + 1 ) % 2;
		
		mBloomFbo[mThisFbo].bindFramebuffer();
		gl::clear( Color( 0, 0, 0 ) );
		mBloomFbo[mNextFbo].bindTexture( 0 );	
		mKernelTex.bind( 1 );
		mBlurShader.bind();
		float kernelRes		= (float)mKernelTex.getWidth();
		float invKernelRes	= 1.0f/(kernelRes-1);
		mBlurShader.uniform( "kernelRes", kernelRes );
		mBlurShader.uniform( "invKernelRes", invKernelRes );
		mBlurShader.uniform( "fboTex", 0 );
		mBlurShader.uniform( "kernelTex", 1 );
		if( i%2 == 0 ){
			mBlurShader.uniform( "orientationVector", Vec2f( 1.0f, 0.0f ) );
		} else {
			mBlurShader.uniform( "orientationVector", Vec2f( 0.0f, getWindowAspectRatio() ) );
		}
		
		mBlurShader.uniform( "blurAmt", 0.025f );
		gl::setMatricesWindow( mFboSize );
		gl::setViewport( mFboArea );
		gl::drawSolidRect( mFboRect );
		mBlurShader.unbind();
		mBloomFbo[mThisFbo].unbindFramebuffer();
	}
}


void AnnihilationApp::drawToLightsSurface()
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
					iter.a() = 0.0f;
				} else { // if the light shouldnt exist, put it way out there
					iter.r() = 0.0f;
					iter.g() = 0.0f;
					iter.b() = 0.0f;
					iter.a() = 0.0f;
				}
			} else {	// set light color
				if( index < (int)mController.mShockwaves.size() ){
					float agePer = mController.mShockwaves[index].mAgePer;
					iter.r() = mController.mShockwaves[index].mColor.r;
					iter.g() = mController.mShockwaves[index].mColor.g;
					iter.b() = mController.mShockwaves[index].mColor.b;
					iter.a() = agePer;
				} else { 
					iter.r() = 0.0f;
					iter.g() = 0.0f;
					iter.b() = 0.0f;
					iter.a() = 0.0f;
				}
			}
		}
	}
	
	mLightsTex = gl::Texture( mLightsSurface );
}



CINDER_APP_BASIC( AnnihilationApp, RendererGl )
