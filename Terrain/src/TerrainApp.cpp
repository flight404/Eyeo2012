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
#include "cinder/Sphere.h"
#include "Resources.h"
#include "CubeMap.h"
#include "Room.h"
#include "SpringCam.h"
#include "Terrain.h"
#include "RDiffusion.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_FBO_RES	2
#define FBO_SIZE		512
#define VBO_SIZE		600

class TerrainApp : public AppBasic {
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
	void			drawSphere();
	void			drawNodes();
	void			drawTerrain();
	void			drawInfoPanel();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// SHADERS
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mSphereShader;
	
	// TEXTURES
	gl::Texture			mIconTex;
	CubeMap				mCubeMap;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	gl::Texture			mRoomBackWallTex;
	gl::Texture			mRoomLeftWallTex;
	gl::Texture			mRoomRightWallTex;
	gl::Texture			mRoomCeilingTex;
	gl::Texture			mRoomFloorTex;
	gl::Texture			mRoomBlankTex;
	
	// ENVIRONMENT AND LIGHTING
	Color				mFogColor;
	Color				mBlueSkyColor;
	Vec3f				mLightPosDay;
	Vec3f				mLightPosNight;
	Vec3f				mLightPos, mLightDir;
	float				mLightDist;
	
	// TERRAIN
	Terrain				mTerrain;
	Vec3f				mTerrainScale;
	Color				mSandColor;
	gl::Texture			mGradientTex;
	gl::Texture			mSandNormalTex;
	float				mZoomMulti, mZoomMultiDest;
	
	// REACTION DIFFUSION
	RDiffusion			mRd;
	gl::GlslProg		mRdShader, mHeightsShader, mNormalsShader, mTerrainShader;
	gl::Texture			mGlowTex;

	// SPHERE
	Sphere				mSphere;
	Vec3f				mSpherePos, mSpherePosDest;
	
	// MOUSE
	Vec2f				mMouseRightPos;
	Vec2f				mMousePos, mMousePosNorm, mMouseDownPos, mMouseOffset;
	bool				mMouseLeftDown, mMouseRightDown;
};

void TerrainApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
	settings->setBorderless( true );
}

void TerrainApp::setup()
{	
	// CAMERA	
	mSpringCam		= SpringCam( -500.0f, getWindowAspectRatio() );

	// LOAD SHADERS
	try {
		mRoomShader		= gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
		mRdShader		= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "rd.frag" ) );
		mHeightsShader	= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "heights.frag" ) );
		mNormalsShader	= gl::GlslProg( loadResource( "passThru.vert" ), loadResource( "normals.frag" ) );
		mTerrainShader	= gl::GlslProg( loadResource( "terrain.vert" ), loadResource( "terrain.frag" ) );
		mSphereShader	= gl::GlslProg( loadResource( "sphere.vert" ), loadResource( "sphere.frag" ) );
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
	mIconTex		= gl::Texture( loadImage( loadResource( "iconTerrain.png" ) ) );
	mGlowTex		= gl::Texture( loadImage( loadResource( "glow.png" ) ) );
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
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 300.0f, 200.0f, 300.0f ), isPowerOn, isGravityOn );	
	mRoomBackWallTex	= gl::Texture( loadImage( loadResource( "roomWall0.png" ) ) );
	mRoomLeftWallTex	= gl::Texture( loadImage( loadResource( "roomWall1.png" ) ) );
	mRoomRightWallTex	= gl::Texture( loadImage( loadResource( "roomWall1.png" ) ) );
	mRoomCeilingTex		= gl::Texture( loadImage( loadResource( "roomCeiling.png" ) ) );
	mRoomFloorTex		= gl::Texture( loadImage( loadResource( "roomFloor.png" ) ) );
	mRoomBlankTex		= gl::Texture( loadImage( loadResource( "roomBlank.png" ) ) );
	
	// ENVIRONMENT AND LIGHTING
	mFogColor		= Color( 255.0f/255.0f, 255.0f/255.0f, 230.0f/255.0f );
	mSandColor		= Color( 255.0f/255.0f, 133.0f/255.0f,  67.0f/255.0f );
	mBlueSkyColor	= Color( 1.0f/255.0f, 78.0f/255.0f, 174.0f/255.0f );
	mLightPos		= Vec3f( 0.0f, 0.6f, 1.0f );
	mLightDist		= 500.0f;
	mLightDir		= mLightPos.normalized();
	
	// TERRAIN
	mTerrainScale	= Vec3f( 1.0f, 2.0f, 1.0f );
	mTerrain		= Terrain( VBO_SIZE, VBO_SIZE );
	mZoomMulti		= 1.0f;
	mZoomMultiDest	= 1.0f;
	mGradientTex	= gl::Texture( loadImage( loadResource( "gradient.png" ) ) );
	mSandNormalTex	= gl::Texture( loadImage( loadResource( "sandNormal.png" ) ) );
	mSandNormalTex.setWrap( GL_REPEAT, GL_REPEAT );
	
	// REACTION DIFFUSION
	mRd				= RDiffusion( FBO_SIZE, FBO_SIZE );

	// SPHERE
	mSpherePos		= Vec3f::zero();
	mSpherePosDest	= Vec3f::zero();
	mSphere.setCenter( mSpherePosDest );
	mSphere.setRadius( 20.0f );
	
	// MOUSE
	mMouseRightPos	= getWindowCenter();
	mMousePos		= Vec2f::zero();
	mMouseDownPos	= Vec2f::zero();
	mMouseOffset	= Vec2f::zero();
	mMousePosNorm	= Vec2f::zero();
	mMouseLeftDown	= false;
	mMouseRightDown	= false;
	
	mRoom.init();
}

void TerrainApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMouseOffset = Vec2f::zero();
	
	if( event.isLeft() ){
		mMouseLeftDown = true;
	}
	
	if( event.isRight() ){
		mMouseRightDown = true;
		mMouseRightPos	= getWindowSize() - event.getPos();
	}
}

void TerrainApp::mouseUp( MouseEvent event )
{
	if( event.isLeft() ){
		mMouseLeftDown = false;
	}
	
	if( event.isRight() ){
		mMouseRightDown = false;
		mMouseRightPos	= getWindowSize() - event.getPos();
	}
	
	mMouseOffset = Vec2f::zero();
}

void TerrainApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
	if( event.isRight() )
		mMouseRightPos	= getWindowSize() - event.getPos();
}

void TerrainApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.7f;
}

void TerrainApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	
	if( event.isShiftDown() ){
		mZoomMultiDest += dWheel * 0.001f;
		mZoomMultiDest = constrain( mZoomMultiDest, 0.1f, 1.0f );
	} else {
		mRoom.adjustTimeMulti( dWheel );
	}
}

void TerrainApp::keyDown( KeyEvent event )
{
	switch ( event.getChar() ) {
		case ' ':	mRoom.togglePower();
					mSpringCam.setPreset( 1 );	break;
		case 'f':	mRd.mParamF += 0.001f;		break;
		case 'F':	mRd.mParamF -= 0.001f;		break;
		case 'k':	mRd.mParamK += 0.001f;		break;
		case 'K':	mRd.mParamK -= 0.001f;		break;
		case 'n':	mRd.mParamN += 0.005f;		break;
		case 'N':	mRd.mParamN -= 0.005f;		break;
		case 'w':	mRd.mParamWind += 0.05f;	break;
		case 'W':	mRd.mParamWind -= 0.05f;	break;
		case '.':	mTerrainScale.x += 0.1f;	break;
		case ',':	mTerrainScale.x -= 0.1f;	break;
		case '1':	mRd.setMode(1);				break;
		case '2':	mRd.setMode(2);				break;
		case '3':	mRd.setMode(3);				break;
		case 'c':	mSpringCam.setPreset( 0 );	break;
		case 'C':	mSpringCam.setPreset( 2 );	break;
		default:								break;
	}
	
	switch( event.getCode() ){
		case KeyEvent::KEY_UP:		mMouseRightPos = Vec2f( 222.0f, 205.0f ) + getWindowCenter();	break;
		case KeyEvent::KEY_LEFT:	mMouseRightPos = Vec2f(-128.0f,-178.0f ) + getWindowCenter();	break;
		case KeyEvent::KEY_RIGHT:	mMouseRightPos = Vec2f(-256.0f, 122.0f ) + getWindowCenter();	break;
		case KeyEvent::KEY_DOWN:	mMouseRightPos = Vec2f(   0.0f,   0.0f ) + getWindowCenter();	break;
		default: break;
	}
	
	std::cout << "F: " << mRd.mParamF << " K: " << mRd.mParamK << std::endl;
}



void TerrainApp::update()
{	
	float x = mMouseRightPos.x - getWindowSize().x * 0.5f;
	float y = mSphere.getCenter().y;
	float z = mMouseRightPos.y - getWindowSize().y * 0.5f;
	mSpherePosDest = Vec3f( x, y, z );
	mSpherePos -= ( mSpherePos - mSpherePosDest ) * 0.02f;
	
	mSphere.setCenter( mSpherePos );
	
	// ROOM
	mRoom.update();
	
	mZoomMulti		-= ( mZoomMulti - mZoomMultiDest ) * 0.1f;
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::color( Color( 1, 1, 1 ) );
	gl::disableAlphaBlending();
	
	// REACTION DIFFUSION
	mRd.update( mRoom.getTimeDelta(), &mRdShader, mGlowTex, mMouseRightDown, mSphere.getCenter().xz(), mZoomMulti );
	mRd.drawIntoHeightsFbo( &mHeightsShader, mTerrainScale );
	mRd.drawIntoNormalsFbo( &mNormalsShader );
	
	// ROOM
	drawIntoRoomFbo();
	
	// CAMERA
	if( mMouseLeftDown ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.01f, ( mMouseOffset ).length() * 0.01 );
	mSpringCam.update( mRoom.getPower(), 0.5f );
}

void TerrainApp::drawIntoRoomFbo()
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

void TerrainApp::draw()
{
	float power = mRoom.getPower();
	
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
	gl::color( ColorA( power, power, power, power * 0.1f + 0.9f ) );
	
	// DRAW INFO PANEL
	drawInfoPanel();
	
	gl::enable( GL_TEXTURE_2D );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	// DRAW WALLS
	mRoom.drawWalls( mRoom.getPower(), mRoomBackWallTex, mRoomLeftWallTex, mRoomRightWallTex, mRoomCeilingTex, mRoomFloorTex, mRoomBlankTex );
	
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enable( GL_TEXTURE_2D );
	
	// DRAW TERRAIN
	drawTerrain();
	
	gl::disable( GL_TEXTURE_2D );
	
	// DRAW SPHERE
	drawSphere();
}

void TerrainApp::drawSphere()
{
	Vec3f spherePos = mSphere.getCenter();
	Vec3f roomDims	= mRoom.getDims();
	float x = ( spherePos.x + roomDims.x ) / ( roomDims.x * 2.0f );
	float y = ( spherePos.z + roomDims.z ) / ( roomDims.z * 2.0f );;
	Vec2f texCoord = Vec2f( x, y );
	
	mCubeMap.bind();
	mRd.getHeightsTexture().bind( 1 );
	mRd.getNormalsTexture().bind( 2 );
	mSphereShader.bind();
	mSphereShader.uniform( "cubeMap", 0 );
	mSphereShader.uniform( "heightsTex", 1 );
	mSphereShader.uniform( "normalsTex", 2 );
	mSphereShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mSphereShader.uniform( "terrainScale", mTerrainScale );
	mSphereShader.uniform( "eyePos", mSpringCam.getEye() );
	mSphereShader.uniform( "fogColor", mFogColor );
	mSphereShader.uniform( "sandColor", mSandColor );
	mSphereShader.uniform( "power", mRoom.getPower() );
	mSphereShader.uniform( "roomDims", mRoom.getDims() );
	mSphereShader.uniform( "texCoord", texCoord );
	mSphereShader.uniform( "sphereRadius", mSphere.getRadius() * 0.45f );
	mSphereShader.uniform( "zoomMulti", mZoomMulti );
	mSphereShader.uniform( "timePer", mRoom.getTimePer() * 1.5f + 0.5f );
	gl::draw( mSphere, 128 );
	mSphereShader.unbind();
}

void TerrainApp::drawTerrain()
{
	mRd.getHeightsTexture().bind( 0 );
	mRd.getNormalsTexture().bind( 1 );
	mGradientTex.bind( 2 );
	mSandNormalTex.bind( 3 );
	mTerrainShader.bind();
	mTerrainShader.uniform( "heightsTex", 0 );
	mTerrainShader.uniform( "normalsTex", 1 );
	mTerrainShader.uniform( "gradientTex", 2 );
	mTerrainShader.uniform( "sandNormalTex", 3 );
	mTerrainShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mTerrainShader.uniform( "terrainScale", mTerrainScale );
	mTerrainShader.uniform( "roomDims", mRoom.getDims() );
	mTerrainShader.uniform( "zoomMulti", mZoomMulti );//lerp( mZoomMulti, 1.0f, mRoom.getPower() ) );
	mTerrainShader.uniform( "power", mRoom.getPower() );
	mTerrainShader.uniform( "eyePos", mSpringCam.getEye() );
	mTerrainShader.uniform( "lightPos", mLightPos );
	mTerrainShader.uniform( "fogColor", mFogColor );
	mTerrainShader.uniform( "sandColor", mSandColor );
	mTerrainShader.uniform( "mousePosNorm", -( mMousePosNorm - Vec2f( 0.5f, 0.5f ) ) * getElapsedSeconds() * 2.0f );
	mTerrainShader.uniform( "spherePos", mSphere.getCenter() );
	mTerrainShader.uniform( "sphereRadius", mSphere.getRadius() );
	mTerrain.draw();
	mTerrainShader.unbind();
}

void TerrainApp::drawInfoPanel()
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


CINDER_APP_BASIC( TerrainApp, RendererGl )
