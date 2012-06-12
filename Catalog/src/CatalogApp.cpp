#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/Text.h"
#include "cinder/Font.h"
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
#include "Resources.h"
#include "Room.h"
#include "SpringCam.h"
#include "Star.h"

#include "boost/filesystem.hpp"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace ci;
using namespace ci::app;
using namespace boost;
using namespace std;

using std::string;
using std::istringstream;
using std::stringstream;

#define APP_WIDTH		1280
#define APP_HEIGHT		720
#define ROOM_FBO_RES	2
#define MAX_TIMEMULTI	120
#define GRAVITY			-0.02f
#define FBO_WIDTH		78	//
#define FBO_HEIGHT		78	//

class CatalogApp : public AppBasic {
  public:
	virtual void	prepareSettings( Settings *settings );
	virtual void	setup();
	void			initBrightVbo();
	void			initFaintVbo();
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseMove( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );
	virtual void	mouseWheel( MouseEvent event );
	virtual void	keyDown( KeyEvent event );
	void			setFboPositions( gl::Fbo &fbo );
	void		parseData( const fs::path &path );
	Vec3f		convertToCartesian( double ra, double dec, double dist );
	void		createStar( const std::string &text, int lineNumber );
	void			setView( int homeIndex, int destIndex );
	virtual void	update();
	void			selectStar( bool wasRightClick );
	void			drawIntoRoomFbo();
	virtual void	draw();
	void			drawInfoPanel();
	
	// CAMERA
	SpringCam			mSpringCam;
	
	// SHADERS
	gl::GlslProg		mRoomShader;
	gl::GlslProg		mBrightStarsShader;
	gl::GlslProg		mFaintStarsShader;
	
	// TEXTURES
	gl::Texture			mIconTex;
	gl::Texture			mMilkyWayTex;
	gl::Texture			mSpectrumTex;
	
	// ROOM
	Room				mRoom;
	gl::Fbo				mRoomFbo;
	
	// ROOM PANEL
	gl::Texture			mRoomNumTex;
	gl::Texture			mRoomDescTex;
	
	// POSITION/VELOCITY FBOS
	gl::Fbo				mPositionFbo;
	
	// FONT
	Font				mFontBlackT, mFontBlackS, mFontBlackM, mFontBlackL;
	gl::TextureFontRef	mTextureFontT, mTextureFontS, mTextureFontM, mTextureFontL;
	
	// MOUSE
	Vec2f				mMousePos, mMouseDownPos, mMouseOffset;
	bool				mMousePressed, mMouseRightPressed;
	float				mMouseTimePressed;
	float				mMouseTimeReleased;
	float				mMouseTimeThresh;
	bool				mWasRightButtonLastClicked;

	// STARS
	gl::VboMesh			mBrightVbo;
	gl::VboMesh			mFaintVbo;
	
	std::vector<Star>	mStars;
	std::vector<Star*>	mBrightStars;
	std::vector<Star*>	mFaintStars;
	std::vector<Star*>	mNamedStars;
	std::vector<Star*>	mTouringStars;
	
	gl::Texture			mStarTex;
	gl::Texture			mStarGlowTex;
	gl::Texture			mDarkStarTex;
	
	float				mScale, mScaleDest;
	float				mScalePer, mMaxScale;
	
	int					mTotalTouringStars;
	int					mTouringStarIndex;
	Star				*mHomeStar;
	Star				*mDestStar;

	float				mDataTimer;
	
	// RENDER OPTIONS
	bool				mRenderNames;
	bool				mRenderFaintStars;
	bool				mRenderBrightStars;
};

void CatalogApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
	settings->setBorderless( true );
}

void CatalogApp::setup()
{	
	// CAMERA	
	mSpringCam		= SpringCam( -350.0f, getWindowAspectRatio() );

	// LOAD SHADERS
	try {
		mBrightStarsShader	= gl::GlslProg( loadResource( "brightStars.vert" ), loadResource( "brightStars.frag" ) );
		mFaintStarsShader	= gl::GlslProg( loadResource( "faintStars.vert" ), loadResource( "faintStars.frag" ) );
		mRoomShader			= gl::GlslProg( loadResource( "room.vert" ), loadResource( "room.frag" ) );
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
	mIconTex		= gl::Texture( loadImage( loadResource( "iconCatalog.png" ) ), mipFmt );
	mMilkyWayTex	= gl::Texture( loadImage( loadResource( "milkyWay.jpg" ) ) );
	mStarTex		= gl::Texture( loadImage( loadResource( "star.png" ) ), mipFmt );
	mStarGlowTex	= gl::Texture( loadImage( loadResource( "starGlow.png" ) ), mipFmt );
	mDarkStarTex	= gl::Texture( loadImage( loadResource( "darkStar.png" ) ) );
	mSpectrumTex	= gl::Texture( loadImage( loadResource( "spectrum.png" ) ) );
	
	// FONTS
	mFontBlackT		= Font( "Arial", 8 );
	mFontBlackS		= Font( "Arial", 12 );
	mFontBlackM		= Font( "Arial", 20 );
	mFontBlackL		= Font( "Arial", 64 );
	mTextureFontT	= gl::TextureFont::create( mFontBlackT );
	mTextureFontS	= gl::TextureFont::create( mFontBlackS );
	mTextureFontM	= gl::TextureFont::create( mFontBlackM );
	mTextureFontL	= gl::TextureFont::create( mFontBlackL );
	
	// ROOM
	gl::Fbo::Format roomFormat;
	roomFormat.setColorInternalFormat( GL_RGB );
	mRoomFbo			= gl::Fbo( APP_WIDTH/ROOM_FBO_RES, APP_HEIGHT/ROOM_FBO_RES, roomFormat );
	bool isPowerOn		= false;
	bool isGravityOn	= true;
	mRoom				= Room( Vec3f( 350.0f, 200.0f, 350.0f ), isPowerOn, isGravityOn );	
	mRoom.init();
	
	// MOUSE
	mMousePos		= Vec2f::zero();
	mMouseDownPos	= Vec2f::zero();
	mMouseOffset	= Vec2f::zero();
	mMousePressed	= false;
	mMouseTimePressed	= 0.0f;
	mMouseTimeReleased	= 0.0f;
	mMouseTimeThresh	= 0.2f;
	mWasRightButtonLastClicked = false;

	// FBOS
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_RGBA32F_ARB );
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	mPositionFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	
	// STARS
	mScaleDest	= 0.2f;
	mScale		= 0.2f;
	mMaxScale	= 400.0f;
	mScalePer	= mScale/mMaxScale;
	parseData( getResourcePath( "starData.txt" ) );
	mTotalTouringStars = mTouringStars.size();
	mHomeStar	= NULL;
	mDestStar	= NULL;
	
	std::cout << mStars.size() << std::endl;

	setFboPositions( mPositionFbo );
	
	initFaintVbo();
	initBrightVbo();
	
	mDataTimer	= 0.0f;
	
	// RENDER OPTIONS
	mRenderNames			= true;
	mRenderFaintStars		= true;
	mRenderBrightStars		= true;
}


void CatalogApp::setFboPositions( gl::Fbo &fbo )
{
	int numBrightStars = mBrightStars.size();

	int index = 0;
	
	Surface32f posSurface( fbo.getTexture() );
	Surface32f::Iter it = posSurface.getIter();
	while( it.line() ){
		while( it.pixel() ){
			Vec3f pos = Vec3f( 1000000.0f, 0.0f, 0.0f );
			float col = 0.4f;
			float rad = 0.0f;
			if( index < numBrightStars ){
				pos = mBrightStars[index]->mPos;
				col = mBrightStars[index]->mColor;
				rad = floor( constrain( ( ( 6.0f - ( mBrightStars[index]->mAbsoluteMag ) )/6.0f ), 0.3f, 1.0f ) * 3.0f * 1000 );
			}
			it.r() = pos.x;
			it.g() = pos.y;
			it.b() = pos.z;
			it.a() = rad + col;

			index ++;
		}
	}

	gl::Texture posTexture( posSurface );
	fbo.bindFramebuffer();
	gl::setMatricesWindow( fbo.getSize(), false );
	gl::setViewport( fbo.getBounds() );
	gl::clear( ColorA( 0, 0, 0, 0 ), true );
	gl::draw( posTexture );
	fbo.unbindFramebuffer();
}

void CatalogApp::initBrightVbo()
{
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticColorsRGB();
	
	int numVertices = FBO_WIDTH * FBO_HEIGHT;
	// 1 quad per particle
	// 2 triangles make up the quad
	// 3 points per triangle
	mBrightVbo		= gl::VboMesh( numVertices * 2 * 3, 0, layout, GL_TRIANGLES );
	
	float s = 0.5f;
	Vec3f p0( -s, -s, 0.0f );
	Vec3f p1( -s,  s, 0.0f );
	Vec3f p2(  s,  s, 0.0f );
	Vec3f p3(  s, -s, 0.0f );
	
	Vec2f t0( 0.0f, 0.0f );
	Vec2f t1( 0.0f, 1.0f );
	Vec2f t2( 1.0f, 1.0f );
	Vec2f t3( 1.0f, 0.0f );
	
	vector<Vec3f>		positions;
	vector<Vec2f>		texCoords;
	vector<Color>		colors;
	
	for( int x = 0; x < FBO_WIDTH; ++x ) {
		for( int y = 0; y < FBO_HEIGHT; ++y ) {
			float u = (float)x/(float)FBO_WIDTH;
			float v = (float)y/(float)FBO_HEIGHT;
			Color c = Color( u, v, 0.0f );
			
			positions.push_back( p0 );
			positions.push_back( p1 );
			positions.push_back( p2 );
			texCoords.push_back( t0 );
			texCoords.push_back( t1 );
			texCoords.push_back( t2 );
			colors.push_back( c );
			colors.push_back( c );
			colors.push_back( c );
			
			positions.push_back( p0 );
			positions.push_back( p2 );
			positions.push_back( p3 );
			texCoords.push_back( t0 );
			texCoords.push_back( t2 );
			texCoords.push_back( t3 );
			colors.push_back( c );
			colors.push_back( c );
			colors.push_back( c );
		}
	}
	
	mBrightVbo.bufferPositions( positions );
	mBrightVbo.bufferTexCoords2d( 0, texCoords );
	mBrightVbo.bufferColorsRGB( colors );
	mBrightVbo.unbindBuffers();
}

void CatalogApp::initFaintVbo()
{
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticColorsRGB();
	
	int numFaintStars	= mFaintStars.size();
	mFaintVbo			= gl::VboMesh( numFaintStars, 0, layout, GL_POINTS );
	vector<Vec3f> positions;
	vector<Color> colors;

	for( int i=0; i<numFaintStars; i++ ){
		positions.push_back( mFaintStars[i]->mPos );
		colors.push_back( Color( mFaintStars[i]->mColor, 0.0f, 0.0f ) );
	}
	
	mFaintVbo.bufferPositions( positions );
	mFaintVbo.bufferColorsRGB( colors );
	mFaintVbo.unbindBuffers();
}

void CatalogApp::mouseDown( MouseEvent event )
{	
	if( event.isRight() ){
//		mArcball.mouseDown( event.getPos() );
		mWasRightButtonLastClicked = true;
	} else {
		mWasRightButtonLastClicked = false;
	}
	mMouseTimePressed	= getElapsedSeconds();
	mMouseDownPos = event.getPos();
	mMousePressed = true;
	mMouseOffset = Vec2f::zero();
}

void CatalogApp::mouseUp( MouseEvent event )
{
	mMouseTimeReleased	= getElapsedSeconds();
	mMousePressed = false;
	mMouseOffset = Vec2f::zero();
}

void CatalogApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void CatalogApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mMouseOffset = ( mMousePos - mMouseDownPos ) * 0.4f;
}

void CatalogApp::mouseWheel( MouseEvent event )
{
	float dWheel	= event.getWheelIncrement();
	mScaleDest		= constrain( mScaleDest - dWheel, 0.2f, mMaxScale );
}

void CatalogApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case ' ':	mRoom.togglePower();			break;
		case 'n':	mRenderNames = !mRenderNames;	break;
		case 'b':	mRenderBrightStars = !mRenderBrightStars;	break;
		case 'f':	mRenderFaintStars = !mRenderFaintStars;		break;
		case '1':	setView( 0, 1 );				break;
		case '2':	setView( 0, 2 );				break;
		case '3':	setView( 0, 3 );				break;
		case '4':	setView( 3, 0 );				break;
		default:									break;
	}
}

void CatalogApp::setView( int homeIndex, int destIndex )
{
	mHomeStar	= mTouringStars[ homeIndex ];
	mDestStar	= mTouringStars[ destIndex ];
	
	mDataTimer	= 0.0f;
}

void CatalogApp::update()
{	
	if( getElapsedSeconds() - mMouseTimePressed < mMouseTimeThresh && !mMousePressed ){
		mMouseTimePressed = 0.0f;
		selectStar( mWasRightButtonLastClicked );
	}
	
	mRoom.update();
	
	if( mRoom.isPowerOn() ) mScaleDest -= ( mScaleDest - mMaxScale ) * 0.05f;
	mScale -= ( mScale - mScaleDest ) * 0.02f;
	mScalePer = mScale/mMaxScale;
	
	// CAMERA
	if( mHomeStar != NULL ){
		mSpringCam.setEye( mHomeStar->mPos + Vec3f( 100.0f, 0.0f, 40.0f ) );
	}
	
	if( mDestStar != NULL ){
		mSpringCam.setCenter( mDestStar->mPos );
	}

	if( mMousePressed ) 
		mSpringCam.dragCam( ( mMouseOffset ) * 0.01f, ( mMouseOffset ).length() * 0.01 );
	mSpringCam.update( 0.25f );
	
	BOOST_FOREACH( Star* &s, mBrightStars ){
		s->update( mSpringCam.getCam(), mScale );
	}
	
	drawIntoRoomFbo();
}

void CatalogApp::selectStar( bool wasRightClick )
{
	std::cout << "select star" << std::endl;
	Star *touchedStar = NULL;
	float closestDist = 100000.0f;
	BOOST_FOREACH( Star* &s, mBrightStars ){
		if( s->mDistToMouse < 40.0f ){
			if( s->mDistToMouse < closestDist ){
				closestDist = s->mDistToMouse;
				touchedStar = s;
			}
		}
	}
	
	if( touchedStar ){
		if( wasRightClick ){
			mHomeStar = touchedStar;
		} else {
			std::cout << "TOUCHED " << touchedStar->mName << std::endl;
			if( mDestStar ){
				mDestStar->mIsSelected = false;
			}
			mDestStar = touchedStar;
			mDestStar->mIsSelected = true;
		}
	}
}

void CatalogApp::drawIntoRoomFbo()
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
	mRoomShader.uniform( "eyePos", mSpringCam.mCam.getEyePoint() );
	mRoomShader.uniform( "roomDims", mRoom.getDims() );
	mRoomShader.uniform( "power", mRoom.getPower() );
	mRoomShader.uniform( "lightPower", mRoom.getLightPower() );
	mRoom.draw();
	mRoomShader.unbind();
	
	mRoomFbo.unbindFramebuffer();
	glDisable( GL_CULL_FACE );
}

void CatalogApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::color( ColorA( 1, 1, 1, 1 ) );

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
	
	gl::setMatricesWindow( getWindowSize(), true );
	
	float power = mRoom.getPower();
	gl::color( Color( power, power, power ) );
	if( power < 0.5f ){
		gl::enableAlphaBlending();
	} else {
		gl::enableAdditiveBlending();
	}

	gl::setMatrices( mSpringCam.getCam() );
	
	// DRAW PANEL
	if( power < 0.1f )
		drawInfoPanel();
	

	gl::enable( GL_TEXTURE_2D );
	
	gl::pushMatrices();
//	gl::multModelView( mArcball.getQuat() );
	
	// DRAW MILKYWAY
	if( power > 0.01f ){
		gl::pushMatrices();
		gl::translate( mSpringCam.getCam().getEyePoint() );
		gl::rotate( Vec3f( 75.0f, 0.0f, 0.0f ) );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, power * mScalePer ) );
		mMilkyWayTex.bind();
		gl::drawSphere( Vec3f::zero(), 195000.0f, 32 );
		gl::popMatrices();
	}
	
	gl::disable( GL_TEXTURE_2D );
	
	// FAINT STARS
	if( mRenderFaintStars ){
		if( power < 0.5f )
			gl::enableAlphaBlending();
		else
			gl::enableAdditiveBlending();
		
		mSpectrumTex.bind( 0 );
		mFaintStarsShader.bind();
		mFaintStarsShader.uniform( "spectrumTex", 0 );
		mFaintStarsShader.uniform( "scale", mScale );
		mFaintStarsShader.uniform( "power", mRoom.getPower() );
		mFaintStarsShader.uniform( "time", (float)getElapsedSeconds() );
		mFaintStarsShader.uniform( "roomDims", mRoom.getDims() );
		gl::draw( mFaintVbo );
		mFaintStarsShader.unbind();
	}

	gl::enable( GL_TEXTURE_2D );
	
	// DRAW STARS
	if( mRenderBrightStars ){
		if( power < 0.5f ){
			gl::enableAlphaBlending();
			mDarkStarTex.bind( 1 );
		} else {
			gl::enableAdditiveBlending();
			mStarTex.bind( 1 );
		}
	
		mPositionFbo.bindTexture( 0 );
		mSpectrumTex.bind( 2 );
		mBrightStarsShader.bind();
		mBrightStarsShader.uniform( "position", 0 );
		mBrightStarsShader.uniform( "starTex", 1 );
		mBrightStarsShader.uniform( "spectrumTex", 2 );
		mBrightStarsShader.uniform( "scale", mScale );
		mBrightStarsShader.uniform( "power", mRoom.getPower() );
		mBrightStarsShader.uniform( "roomDims", mRoom.getDims() );
		mBrightStarsShader.uniform( "mvMatrix", mSpringCam.mCam.getModelViewMatrix() );
		mBrightStarsShader.uniform( "eyePos", mSpringCam.mCam.getEyePoint() );
		if( power > 0.5f ){
			mBrightStarsShader.uniform( "texScale", 0.5f );
			gl::draw( mBrightVbo );
			
			// IF YOU WANT MORE GLOW AROUND MAJOR STARS, USE THESE TWO LINES
//			mStarGlowTex.bind( 1 );
//			mBrightStarsShader.uniform( "texScale", 0.1f );
			gl::draw( mBrightVbo );
		} else {
			mBrightStarsShader.uniform( "texScale", 0.5f );
			gl::draw( mBrightVbo );
		}
		mBrightStarsShader.unbind();
	}
	
	gl::disable( GL_TEXTURE_2D );
	
	gl::popMatrices();
	
	
	// DRAW NAMES
	if( mRenderNames ){
		if( power < 0.5f ){
			gl::enableAlphaBlending();
		} else {
			gl::enableAdditiveBlending();
		}
		gl::setMatricesWindow( getWindowSize(), true );
		
		BOOST_FOREACH( Star* &s, mNamedStars ){
			s->drawName( mMousePos, power * mScalePer, math<float>::max( sqrt( mScalePer ) - 0.1f, 0.0f ) );
		}
	}
	
	if( getElapsedFrames()%60 == 59 ){
		std::cout << "FPS: " << getAverageFps() << std::endl;
	}
}

void CatalogApp::drawInfoPanel()
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

void CatalogApp::parseData( const fs::path &path )
{
	std::string line;
	std::ifstream myfile( path.c_str() );
	
	if( myfile.is_open() ){
		int i=0;
		while( !myfile.eof() ){
			std::getline( myfile, line );
			createStar( line, i );
			++i;
		}
		
		myfile.close();
	} else std::cout << "Unable to open file";
}

void CatalogApp::createStar( const std::string &text, int lineNumber )
{
	char_separator<char> sep(",");
	tokenizer< char_separator<char> > tokens(text, sep);
	int index = 0;
	double ra, dec, dist;
	float appMag, absMag;
	float colIndex;
	std::string name;
	std::string spectrum;
	//0			 1    2  3   4    5      6      7        8
	//lineNumber,name,ra,dec,dist,appMag,absMag,spectrum,colIndex;
	BOOST_FOREACH(string t, tokens)
	{
		if( index == 1 ){
			if( t.length() > 1 ){
				name = t;
			} else {
				name = "";
			}
		} else if( index == 2 ){
			ra = lexical_cast<double>(t);
			
		} else if( index == 3 ){
			dec = lexical_cast<double>(t);
			
		} else if( index == 4 ){
			dist = lexical_cast<double>(t);
			
		} else if( index == 5 ){
			appMag = lexical_cast<float>(t);
			
		} else if( index == 6 ){
			absMag = lexical_cast<float>(t);
			
		} else if( index == 7 ){
			spectrum = t;
			
		} else if( index == 8 ){
			if( t != " " ){
				colIndex = lexical_cast<float>(t);
			} else {
				colIndex = 0.0f;
			}
		}
		
		index ++;
	}
	
	Vec3f pos = convertToCartesian( ra, dec, dist );
	
	float mag = ( 80 - appMag ) * 0.1f;
	Color col = Color( mag, mag, mag );
	
	float color = constrain( colIndex, 0.0f, 1.0f );//0.0f;
//	if( name.length() > 0 ){
//		char sp = spectrum[0];
//		
//		switch( sp ){
//			case 'O':	color = 0.0f;	break;
//			case 'B':	color = 0.166f;	break;
//			case 'A':	color = 0.2f;	break;
//			case 'F':	color = 0.5f;	break;
//			case 'G':	color = 0.666f;	break;
//			case 'K':	color = 0.833f;	break;
//			case 'M':	color = 1.0f;	break;
//			default:					break;
//		}
//	}
	
	// THIS FEELS WRONG. ASK ABOUT THE RIGHT WAY TO DO THIS.
	Star *star = new Star( pos, appMag, absMag, color, name, spectrum, mFontBlackT, mFontBlackS );
	mStars.push_back( *star );
	
	if( appMag < 6.0f || name.length() > 1 ){
		mBrightStars.push_back( star );
	} else {
		mFaintStars.push_back( star );
	}
	
	if( name.length() > 1 ){
		mNamedStars.push_back( star );
		
		if( name == "Sol" || name == "Sirius" || name == "Vega" || name == "Gliese 581" ){
			mTouringStars.push_back( star );
			std::cout << "ADDED TOURING STAR: " << star->mName << " " << star->mPos << std::endl;
		}
	}
}

Vec3f CatalogApp::convertToCartesian( double ra, double dec, double dist )
{
	Vec3f pos;
	float RA = toRadians( ra * 15.0 );
	float DEC = toRadians( dec );
	
	pos.x = ( dist * cos( DEC ) ) * cos( RA ); 
	pos.y = ( dist * cos( DEC ) ) * sin( RA );
	pos.z = dist * sin( DEC );
	
	return pos;
}



CINDER_APP_BASIC( CatalogApp, RendererGl )
