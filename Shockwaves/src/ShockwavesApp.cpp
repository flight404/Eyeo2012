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


//#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_FBO_RES	2
#define GRID_DIM		45


class ShockwavesApp : public AppBasic {
  public:
	virtual void		prepareSettings( Settings *settings );
	virtual void		setup();
	void				initVbo();
	virtual void		mouseDown( MouseEvent event );
	virtual void		mouseUp( MouseEvent event );
	virtual void		mouseMove( MouseEvent event );
	virtual void		mouseDrag( MouseEvent event );
	virtual void		mouseWheel( MouseEvent event );
	virtual void		keyDown( KeyEvent event );
	virtual void		update();
	void				drawIntoRoomFbo();
	virtual void		draw();
	void				drawInfoPanel();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// SHADERS
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mShockwaveShader;
	gl::GlslProg		mNodeShader;
	
	// TEXTURES
	gl::Texture			mSmokeTex;
	gl::Texture			mGlowTex;
	gl::Texture			mIconTex;
	CubeMap				mCubeMap;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
//	// POSITION/VELOCITY FBOS
//	ci::Vec2f			mFboSize;
//	ci::Area			mFboBounds;
//	int					mThisFbo, mPrevFbo;
	
	// CONTROLLER
	Controller			mController;
	
	// VBOS
	gl::VboMesh			mVboMesh;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed;
	
//	// PARAMS
//	params::InterfaceGl	mParams;
//	bool				mShowParams;

};

void ShockwavesApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
	settings->setBorderless();
}

void ShockwavesApp::setup()
{	
	// CAMERA	
	mSpringCam		= SpringCam( -450.0f, getWindowAspectRatio() );
	
//	// FBOS
//	gl::Fbo::Format format;
//	format.setColorInternalFormat( GL_RGBA16F_ARB );
//	mThisFbo		= 0;
//	mPrevFbo		= 1;
	
	// LOAD SHADERS
	try {
		mRoomShader		 = gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
		mShockwaveShader = gl::GlslProg( loadResource( "shockwave.vert" ), loadResource( "shockwave.frag" ) );
		mNodeShader		 = gl::GlslProg( loadResource( "node.vert" ), loadResource( "node.frag" ) );
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
	mIconTex		= gl::Texture( loadImage( loadResource( "iconShockwaves.png" ) ), mipFmt );
	mSmokeTex		= gl::Texture( loadImage( loadResource( "smoke.png" ) ), mipFmt );
	mGlowTex		= gl::Texture( loadImage( loadResource( "glow.png" ) ), mipFmt );
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
	
	// CONTROLLER
	mController.init( &mRoom, GRID_DIM );
	
//	// PARAMS
//	mShowParams		= false;
//	mParams			= params::InterfaceGl( "Flocking", Vec2i( 200, 150 ) );
//	mParams.addParam( "Time Multi", &mTimeMulti, "min=0.0 max=180.0 step=1.0 keyIncr=t keyDecr=T" );
}


void ShockwavesApp::initVbo()
{
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticNormals();
	
	int numVertices = GRID_DIM * GRID_DIM;
	// 5 points make up the pyramid
	// 8 triangles make up two pyramids
	// 3 points per triangle
	
	mVboMesh		= gl::VboMesh( numVertices * 8 * 3, 0, layout, GL_TRIANGLES );
	
	float s = 10.0f;
	Vec3f p0( 0.0f,    s, 0.0f );
	Vec3f p1(   -s, 0.0f, 0.0f );
	Vec3f p2( 0.0f, 0.0f,    s );
	Vec3f p3(    s, 0.0f, 0.0f );
	Vec3f p4( 0.0f, 0.0f,   -s );
	Vec3f p5( 0.0f,   -s, 0.0f );
	
	Vec3f n;
	Vec3f n0 = Vec3f( 0.0f, 0.0f, 1.0f );
	Vec3f n1 = Vec3f(-1.0f,-1.0f, 0.0f );
	Vec3f n2 = Vec3f(-1.0f, 1.0f, 0.0f );
	Vec3f n3 = Vec3f( 1.0f, 1.0f, 0.0f );
	Vec3f n4 = Vec3f( 1.0f,-1.0f, 0.0f );
	Vec3f n5 = Vec3f( 0.0f, 0.0f,-1.0f );
	
	vector<Vec3f>		positions;
	vector<Vec3f>		normals;
	
	for( int x = 0; x < GRID_DIM; ++x ) {
		for( int y = 0; y < GRID_DIM; ++y ) {
			positions.push_back( p0 );
			positions.push_back( p1 );
			positions.push_back( p2 );
			n = ( p0 + p1 + p2 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p2 );
			positions.push_back( p3 );
			n = ( p0 + p2 + p3 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p3 );
			positions.push_back( p4 );
			n = ( p0 + p3 + p4 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p0 );
			positions.push_back( p4 );
			positions.push_back( p1 );
			n = ( p0 + p4 + p1 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			
			
			
			positions.push_back( p5 );
			positions.push_back( p1 );
			positions.push_back( p4 );
			n = ( p5 + p1 + p4 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p2 );
			positions.push_back( p1 );
			n = ( p5 + p2 + p1 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p3 );
			positions.push_back( p2 );
			n = ( p5 + p3 + p2 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			positions.push_back( p5 );
			positions.push_back( p4 );
			positions.push_back( p3 );
			n = ( p5 + p4 + p3 ).normalized();
			normals.push_back( n );
			normals.push_back( n );
			normals.push_back( n );
			
			
		}
	}
	
	mVboMesh.bufferPositions( positions );
	mVboMesh.bufferNormals( normals );
	mVboMesh.unbindBuffers();
}


void ShockwavesApp::mouseDown( MouseEvent event )
{
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void ShockwavesApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void ShockwavesApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void ShockwavesApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.4f;
}

void ShockwavesApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	mRoom.adjustTimeMulti( dWheel );
}

void ShockwavesApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case '1':	mController.preset( 1 );	break;
		case '2':	mController.preset( 2 );	break;
		case ' ':	mRoom.togglePower();		break;
		case 's':	mController.explode();		break;
		default:								break;
	}
	
	switch( event.getCode() ){
		case KeyEvent::KEY_UP:		mSpringCam.setEye( mRoom.getCornerCeilingPos() );	break;
		case KeyEvent::KEY_DOWN:	mSpringCam.setEye( mRoom.getCornerFloorPos() );		break;
		case KeyEvent::KEY_RIGHT:	mSpringCam.resetEye();								break;
		default: break;
	}
}



void ShockwavesApp::update()
{	
	// CAMERA
	if( mMousePressed ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.02f, ( mMouseOffset ).length() * 0.02f );
	mSpringCam.update( 0.5f );
	
	// ROOM
	mRoom.update();

	// CONTROLLER
	mController.update( mRoom.getTimeDelta(), mRoom.getTick() );
	
	drawIntoRoomFbo();
}

void ShockwavesApp::drawIntoRoomFbo()
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
	mCubeMap.bind();
	mRoomShader.bind();
	//	mRoomShader.uniform( "lightsTex", 0 );
	//	mRoomShader.uniform( "numLights", (float)mNumLights );
	//	mRoomShader.uniform( "invNumLights", mInvNumLights );
	//	mRoomShader.uniform( "invNumLightsHalf", mInvNumLights * 0.5f );
	//	mRoomShader.uniform( "att", 1.015f );
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

void ShockwavesApp::draw()
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
	
	// DRAW NODES
	mNodeShader.bind();
	mNodeShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mNodeShader.uniform( "eyePos", mSpringCam.getEye() );
	mNodeShader.uniform( "power", mRoom.getPower() );
	mNodeShader.uniform( "roomDims", mRoom.getDims() );
	mController.drawNodes( &mNodeShader );
	mNodeShader.unbind();
	
	// DRAW NODE CONNECTIONS
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 0.4f ) );
	mController.drawConnections();
	
	// DRAW SHOCKWAVES
//	mController.drawShockwaveCenters();
	
	gl::disableDepthWrite();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	gl::enable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	mShockwaveShader.bind();
	mShockwaveShader.uniform( "mvpMatrix", mSpringCam.mMvpMatrix );
	mShockwaveShader.uniform( "eyePos", mSpringCam.getEye() );
	mShockwaveShader.uniform( "power", mRoom.getPower() );
	mShockwaveShader.uniform( "roomDims", mRoom.getDims() );
	mController.drawShockwaves( &mShockwaveShader );
	mShockwaveShader.unbind();
	gl::disable( GL_CULL_FACE );
	
	gl::enable( GL_TEXTURE_2D );
	
	// DRAW SMOKES
	Vec3f right, up;
	mSpringCam.mCam.getBillboardVectors( &right, &up );
	mSmokeTex.bind();
	mController.drawSmokes( right, up );
	
	gl::enableAdditiveBlending();
	
	// DRAW GLOWS
	mGlowTex.bind();
	mController.drawGlows( right, up );
	
//	gl::disableDepthRead();
//	gl::disableDepthWrite();
//	
//	// DRAW INFO PANEL
//	drawInfoPanel();

//	// DRAW PARAMS WINDOW
//	if( mShowParams ){
//		gl::setMatricesWindow( getWindowSize() );
//		params::InterfaceGl::draw();
//	}
	
//	mThisFbo	= ( mThisFbo + 1 ) % 2;
//	mPrevFbo	= ( mThisFbo + 1 ) % 2;
	
	if( getElapsedFrames()%60 == 59 )
		std::cout << "FPS: " << getAverageFps() << std::endl;
}

void ShockwavesApp::drawInfoPanel()
{
	gl::pushMatrices();
	gl::translate( mRoom.getDims() );
	gl::scale( Vec3f( -1.0f, -1.0f, 1.0f ) );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) * ( 1.0 - mRoom.getPower() ) );
	gl::enableAlphaBlending();
	
	float iconWidth		= 50.0f;
	
	float X0			= 15.0f;
	float X1			= X0 + iconWidth;
	float Y0			= 50.0f;
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


CINDER_APP_BASIC( ShockwavesApp, RendererGl )
