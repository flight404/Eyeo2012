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
#include "Controller.h"
#include "Room.h"
#include "SpringCam.h"

//#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define APP_WIDTH			1280
#define APP_HEIGHT			720
#define ROOM_FBO_RES		2
#define MAX_ANTS			700
#define NUM_SPECIAL_ANTS	100

#define FBO_WIDTH			700
#define FBO_HEIGHT			700

class AntMillApp : public AppBasic {
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
	void			clearPheromoneFbos();
	void			drawIntoPheromoneFbo();
	void			drawIntoBlurFbo();
	void			blurFbo();
	void			readFboPixels();
	Vec2i			toFboVec( const Vec2f &pos );
	void			drawIntoRoomFbo();
	virtual void	draw();
	void			drawInfoPanel();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// SHADERS
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mPheromoneShader;
	gl::GlslProg		mBlurShader;
	
	// TEXTURES
	gl::Texture			mPinTex;
	gl::Texture			mKernelTex;
	gl::Texture			mHomeTex;
	gl::Texture			mIconTex;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
	// FBOS
	ci::Vec2f			mFboSize;
	ci::Area			mFboBounds;
	gl::Fbo				mFbos[2];
	gl::Fbo				mBlurFbos[2];
	int					mThisFbo, mPrevFbo;

	// CONTROLLER
	Controller			mController;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
	
	// SAVEFRAMES
	bool				mSaveFrames;
	int					mNumSavedFrames;
};

void AntMillApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
//	settings->setBorderless();
}

void AntMillApp::setup()
{	
	Rand::randomize();
	
	// CAMERA	
	mSpringCam		= SpringCam( -550.0f, getWindowAspectRatio() );
	
	// FBOS
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_RGB );
	format.setWrap( GL_REPEAT, GL_REPEAT );
	mFboBounds = Area( 0.0f, 0.0f, FBO_WIDTH, FBO_HEIGHT );
	mFboSize   = Vec2f( FBO_WIDTH, FBO_HEIGHT );
	for( int i=0; i<2; i++ ){
		mFbos[i]	 = gl::Fbo( FBO_HEIGHT, FBO_HEIGHT, format );	
		mBlurFbos[i] = gl::Fbo( FBO_HEIGHT, FBO_HEIGHT, format );
	}
	clearPheromoneFbos();

	mThisFbo	= 0;
	mPrevFbo	= 1;
	
	// LOAD SHADERS
	try {
		mRoomShader		= gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
		mBlurShader		= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "blur.frag" ) );
		mPheromoneShader= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "pheromone.frag" ) );
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
	mPinTex			= gl::Texture( loadImage( loadResource( "pin.png" ) ), mipFmt );
	mKernelTex		= gl::Texture( loadImage( loadResource( "kernel.png" ) ) );
	mHomeTex		= gl::Texture( loadImage( loadResource( "home.png" ) ) );
	mIconTex		= gl::Texture( loadImage( loadResource( "iconAntMill.png" ) ), mipFmt );
	
	// ROOM
	gl::Fbo::Format roomFormat;
	roomFormat.setColorInternalFormat( GL_RGB );
	mRoomFbo			= gl::Fbo( APP_WIDTH/ROOM_FBO_RES, APP_HEIGHT/ROOM_FBO_RES, roomFormat );
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 350.0f, 200.0f, 350.0f ), isPowerOn, isGravityOn );	
	mRoom.init();

	// MOUSE
	mMousePos			= Vec2f::zero();
	mMouseDownPos		= Vec2f::zero();
	mMouseOffset		= Vec2f::zero();
	mMousePressed		= false;
	
	// CONTROLLER
	mController			= Controller( &mRoom );
	mController.init( MAX_ANTS, NUM_SPECIAL_ANTS );	
	
	// SAVE FRAMES
	mSaveFrames			= false;
	mNumSavedFrames		= 0;
}

void AntMillApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void AntMillApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void AntMillApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void AntMillApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.4f;
}

void AntMillApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	mRoom.adjustTimeMulti( dWheel );
}

void AntMillApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case ' ':	mRoom.togglePower();		break;
		case 'i':	mController.init( MAX_ANTS, NUM_SPECIAL_ANTS );	
					clearPheromoneFbos();		
			break;
		default:								break;
	}
	
	switch( event.getCode() ){
		case KeyEvent::KEY_UP:		mSpringCam.setEye( mRoom.getCornerCeilingPos() );	break;
		case KeyEvent::KEY_DOWN:	mSpringCam.setEye( mRoom.getCornerFloorPos() );		break;
		case KeyEvent::KEY_RIGHT:	mSpringCam.resetEye();								break;
		default: break;
	}
}

void AntMillApp::clearPheromoneFbos()
{
	for( int i=0; i<2; i++ ){
		mFbos[i].bindFramebuffer();
		gl::clear( Color::black() );
		mFbos[i].unbindFramebuffer();
		
		mBlurFbos[i].bindFramebuffer();
		gl::clear( Color::black() );
		mBlurFbos[i].unbindFramebuffer();
	}
}

void AntMillApp::update()
{	
	// CAMERA
	if( mMousePressed ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.02f, ( mMouseOffset ).length() * 0.02f );
	
	mSpringCam.update( 0.5f );
	
	// ROOM
	mRoom.update( mSaveFrames );
	
	// CONTROLLER
	mController.update( mRoom.getTimeDelta(), mRoom.getTick() );
	
	drawIntoPheromoneFbo();
	drawIntoBlurFbo();
	blurFbo();
	readFboPixels();
	drawIntoRoomFbo();
}

void AntMillApp::drawIntoPheromoneFbo()
{	
	Vec3f dims = mRoom.getDims();
	CameraOrtho orthoCam = CameraOrtho( dims.x, -dims.x, dims.z, -dims.z, -dims.y-1.0f, dims.y+1.0f );
	
	mFbos[ mThisFbo ].bindFramebuffer();
	gl::clear( Color( 0, 0, 0 ), false );
	
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enable( GL_TEXTURE_2D );
	gl::enableAlphaBlending();
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	
	// DRAW OLD FBO
	mFbos[ mPrevFbo ].bindTexture();
	mHomeTex.bind( 1 );
	mPheromoneShader.bind();
	mPheromoneShader.uniform( "fboTex", 0 );
	mPheromoneShader.uniform( "homeTex", 1 );
	mPheromoneShader.uniform( "homePos", mController.mHomePos );
	mPheromoneShader.uniform( "homeRadius", mController.mHomeRadius );
	mPheromoneShader.uniform( "foodPos", mController.mFoodPos );
	mPheromoneShader.uniform( "foodRadius", mController.mFoodRadius );
	mPheromoneShader.uniform( "fboSize", Vec2f( FBO_WIDTH, FBO_HEIGHT ) );
	gl::drawSolidRect( mFbos[ mPrevFbo ].getBounds() );
	mPheromoneShader.unbind();
		
	gl::disable( GL_TEXTURE_2D );
	gl::enableAdditiveBlending();
	
	gl::setMatrices( orthoCam );
	gl::pushMatrices();
	gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
	
	// DRAW THE ANT TAIL POSITIONS
	Color outBound = Color( 0.2f, 0.0f, 0.0f );
	Color inBound  = Color( 0.0f, 1.0f, 0.0f );
	mController.drawAntTails( outBound, inBound );
	gl::popMatrices();
	
	mFbos[ mThisFbo ].unbindFramebuffer();	
}

void AntMillApp::drawIntoBlurFbo()
{
	mBlurFbos[mThisFbo].bindFramebuffer();
	gl::clear( Color( 0.0f, 0.0f, 0.0f ), true );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	gl::setMatricesWindow( mFboSize, false );
	gl::setViewport( mFboBounds );
	
	gl::enable( GL_TEXTURE_2D );
	mFbos[ mThisFbo ].bindTexture();
	gl::drawSolidRect( mFboBounds );
	mBlurFbos[mThisFbo].unbindFramebuffer();
}

void AntMillApp::blurFbo()
{
	for( int i=0; i<2; i++ ){
		mThisFbo = ( mThisFbo + 1 ) % 2;
		mPrevFbo = ( mThisFbo + 1 ) % 2;
		
		mBlurFbos[mThisFbo].bindFramebuffer();
		gl::clear( ColorA( 0, 0, 0, 0 ), true );
		mBlurFbos[mPrevFbo].bindTexture( 0 );	
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
		
		// BLUR AMOUNT
		gl::setMatricesWindow( mFboSize, false );
		gl::setViewport( mFboBounds );
		gl::drawSolidRect( mFboBounds );
		mBlurShader.unbind();
		
		mBlurFbos[mThisFbo].unbindFramebuffer();
	}
}

void AntMillApp::readFboPixels()
{
	mBlurFbos[mThisFbo].bindFramebuffer();
	for( int i=0; i<mController.mAnts.size(); i++ ){
		if( i%2 == getElapsedFrames()%2 ){
			GLfloat pixel[4];
			Ant *ant = &mController.mAnts[i];
			
			Vec3f p1   = ant->mLeftSensorPos;
			Vec2i pos1 = toFboVec( p1.xz() );
			glReadPixels( pos1.x, pos1.y, 1, 1, GL_RGB, GL_FLOAT, (void *)pixel );
			float r1 = pixel[0];
			float g1 = pixel[1];
			float b1 = pixel[2];
			
			Vec3f p2   = ant->mRightSensorPos;
			Vec2i pos2 = toFboVec( p2.xz() );
			glReadPixels( pos2.x, pos2.y, 1, 1, GL_RGB, GL_FLOAT, (void *)pixel );
			float r2 = pixel[0];// * 0.99f;
			float g2 = pixel[1];// * 0.99f;
			float b2 = pixel[2];// * 0.99f;

			if( ant->mHasFood ){
				ant->turn( r1 - r2 );
				ant->turn( b1*1.0f - b2*1.0f );
				ant->turn( g1*0.1f - g2*0.1f );
			} else { // NO FOOD
				ant->turn( r1*0.1f - r2*0.1f );
				ant->turn( g1*1.0f - g2*1.0f );
			}
		}
	}
	mBlurFbos[mThisFbo].unbindFramebuffer();
}

Vec2i AntMillApp::toFboVec( const Vec2f &pos )
{
	int xi = FBO_WIDTH - (int)( pos.x + mRoom.getDims().x );
	int zi = (int)( pos.y + mRoom.getDims().z );
	
	return Vec2i( xi, zi );
}

void AntMillApp::drawIntoRoomFbo()
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

void AntMillApp::draw()
{
	float c = mRoom.getPower();
	
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
	
	// DRAW ANT FBO (TO BECOME PHEREMONE)
	gl::color( Color( c, c, c ) );
	gl::enableAdditiveBlending();
	mBlurFbos[mThisFbo].bindTexture();
	gl::drawBillboard( Vec3f( 0.0f, mRoom.getFloorLevel(), 0.0f ), Vec2f( mRoom.getDims().x, mRoom.getDims().z ) * 2.0, 180.0f, Vec3f::xAxis(), Vec3f::zAxis() );
	
	gl::disable( GL_TEXTURE_2D );
	gl::enableAlphaBlending();
	gl::color( Color( 0, 0, 0 ) );
	
	// DRAW ANTS
	mController.drawAnts();
	
	mController.drawFoods();
	
//	mController.drawSprings();
	
	gl::enable( GL_TEXTURE_2D );
	gl::disableDepthRead();
	gl::disableDepthWrite();
	
	// DRAW INFO PANEL
	drawInfoPanel();
	
	gl::enable( GL_TEXTURE_2D );
	gl::enableDepthRead();
	
	// DRAW PINS
	mPinTex.bind();
	if( mController.mSpecialAnts.size() > 0 ){
		for( int i=0; i<mController.mSpecialAnts.size(); i++ ){
			Ant *ant	= mController.mSpecialAnts[i];
			gl::color( Color( CM_HSV, ant->mColor, 1.0f, 1.0f ) );
			Vec3f pos	= ant->mPos;
			gl::drawBillboard( pos, Vec2f( 30.0f, 60.0f ), 0.0f, ant->mPinPerp, ant->mPinUp );			// HEAVY LOAD MOD
//			gl::drawBillboard( ant->mPos, Vec2f( 50.0f, 100.0f ), 0.0f, Vec3f::xAxis(), Vec3f::yAxis() );	// NORMAL
		}
	}
	
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();

	
	mThisFbo	= ( mThisFbo + 1 ) % 2;
	mPrevFbo	= ( mThisFbo + 1 ) % 2;
	
	if( getElapsedFrames()%60 == 59 ){
		std::cout << "FPS: " << getAverageFps() << std::endl;
		std::cout << "num special ants = " << mController.mSpecialAnts.size() << std::endl;
		std::cout << "num regular ants = " << mController.mAnts.size() << std::endl;
	}
	
	if( mSaveFrames ){
		writeImage( getHomeDirectory() + "AntMill/" + toString( mNumSavedFrames ) + ".png", copyWindowSurface() );
		mNumSavedFrames ++;
	}
}

void AntMillApp::drawInfoPanel()
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



CINDER_APP_BASIC( AntMillApp, RendererGl )
