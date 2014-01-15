/////////////////////////////////////////////////////////////////////
//
//    file: sceneddogllist.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE

#include "sceneddogllist.h"

#include <algorithm>

#include "scenecamera.h"
#include "scenegeom.h"
#include "scenegroup.h"
#include "scenegeomfx.h"
#include "scenelight.h"

#include "sceneblend.h"
#include "scenecull.h"
#include "scenedepth.h"
#include "scenelighting.h"
#include "scenelightpath.h"
#include "scenematerial.h"
// #include "scenepolygonmode.h"
#include "sceneprog.h"
#include "scenetexenv.h"
// #include "scenetexgen.h"
#include "scenetexture.h"
#include "scenetexturexform.h"
#include "sceneuniform.h"

#include "scenetexobj.h"
#include "scenevbo.h"
#include "sceneprogObj.h"

#include "pnimathstream.h"

#include "sceneogl.h"
#include <OpenGLES/ES2/glext.h>

using namespace std;

/////////////////////////////////////////////////////////////////////

//gl_extensions: 
//	GL_OES_byte_coordinates
//	GL_OES_compressed_paletted_texture
//	GL_OES_fixed_point
//	GL_OES_point_size_array
//	GL_OES_point_sprite
//	GL_OES_read_format
//	GL_OES_single_precision
//	GL_ARB_texture_env_combine !
//	GL_ARB_texture_env_dot3 !
//	GL_OES_draw_texture !
//	GL_IMG_texture_format_BGRA8888 !
//	GL_APPLE_texture_rectangle 
//	GL_APPLE_client_storage

/////////////////////////////////////////////////////////////////////

namespace scene {


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

texObj* configTextureObject ( texture const* textureIn )
{

	// get or create textureObject for this texture
	if ( texObj* pObj = static_cast< texObj* > ( textureIn->getTravData ( Draw ) ) )
	{
    glEnable ( GL_TEXTURE_2D ); // in if and else if cases

		pObj->config ( textureIn );
    return pObj;
	}
	else if ( textureIn->getImage () )
	{
    glEnable ( GL_TEXTURE_2D ); // in if and else if cases

		pObj = new texObj;
		pObj->config ( textureIn );
		const_cast< texture* > ( textureIn )->setTravData ( Draw, pObj );
    return pObj;
	}
	else	// not an image texture... can't create it... so disable texture target
	{
		glDisable ( GL_TEXTURE_2D );
    return 0;
	}
}

vbo* configVBO ( geomData const* geomDataIn, progObj const* pProgObj )
{
    // get or create textureObject for this texture
  if ( vbo* pObj = static_cast< vbo* > ( geomDataIn->getTravData ( Draw ) ) )
  {
    pObj->config ( geomDataIn, pProgObj );
    return pObj;
  }
  else
  {
    pObj = new vbo;
    pObj->config ( geomDataIn, pProgObj );
    const_cast< geomData* > ( geomDataIn )->setTravData ( Draw, pObj );
    return pObj;
  }
}

progObj* configProgObject ( prog const* pProg )
{
    // get or create textureObject for this texture
  if ( progObj* pObj = static_cast< progObj* > ( pProg->getTravData ( Draw ) ) )
  {
    pObj->config ( pProg );
    return pObj;
  }
  else
  {
    pObj = new progObj;
    pObj->config ( pProg );
    const_cast< prog* > ( pProg )->setTravData ( Draw, pObj );
    return pObj;
  }
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma mark ddOglTextureBind does a traversal to pre-bind texture

class ddOglTextureBind :
  public graphDd,
  public stateDd
{
  public:
    virtual void startGraph ( node const* pNode )
        {
          pNode->accept ( this );
        }
        
    virtual void startStates ( node const* pNode )
        {
          typedef node::States::const_iterator Iter;
          Iter end = pNode->getStates ().end ();
          for ( Iter cur = pNode->getStates ().begin (); cur != end; ++cur )
            cur->second->accept ( this );
        }
        
    virtual void setDefaultState ( state const* pState, state::Id id ) {}

  protected:  
    void dispatchChildren ( node const* pNode )
        {
          typedef node::Nodes::const_iterator Iter;
          Iter end = pNode->getChildren ().end ();
          for ( Iter cur = pNode->getChildren ().begin (); cur != end; ++cur )
          {
            ( *cur )->accept ( this );
          }
        }
  
  
    virtual void dispatch ( camera const* pNode ) {}
        
    virtual void dispatch ( geom const* pNode )
        {
          startStates ( pNode );
        }
        
    virtual void dispatch ( group const* pNode )
        {
          startStates ( pNode );
          dispatchChildren ( pNode );
        }
        
    virtual void dispatch ( light const* pNode ) {}
        
    virtual void dispatch ( geomFx const* pNode )
        {
          startStates ( pNode );
        }
    
    virtual void dispatch ( sndEffect const* pNode ) {}
    virtual void dispatch ( sndListener const* pNode ) {}
        
    virtual void dispatch ( blend const* pState ) {}
    virtual void dispatch ( cull const* pState ) {}
    virtual void dispatch ( depth const* pState ) {}
    virtual void dispatch ( lighting const* pState ) {}
    virtual void dispatch ( lightPath const* pState ) {}
    virtual void dispatch ( prog const* pState ) {}
    virtual void dispatch ( material const* pState ) {}
    virtual void dispatch ( texEnv const* pState ) {}
    virtual void dispatch ( texture const* pState )
        {
          configTextureObject ( pState );
        }
    virtual void dispatch ( textureXform const* pState ) {}
    virtual void dispatch ( uniform const* pState ) {}

};
    
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma mark ddOglList

ddOglList::ddOglList() :
  mCurStateId ( 0 ),
  mCurLightUnit ( 0 )
{
  const size_t ReserveNum = 256;
  mRenderList.reserve ( ReserveNum );
  
  mBuiltins = new scene::uniform;
  
  uniform::binding& mvpMat = mBuiltins->uniformOp("modelViewProjectionMatrix");
  mvpMat.set(uniform::binding::Vertex, uniform::binding::Matrix4, 1);
  
  uniform::binding& normMat = mBuiltins->uniformOp("normalMatrix");
  normMat.set(uniform::binding::Vertex, uniform::binding::Matrix3, 1);
}

ddOglList::~ddOglList()
{

}

// ddOglList::ddOglList(ddOglList const& rhs)
// {
//   
// }
// 
// ddOglList& ddOglList::operator=(ddOglList const& rhs)
// {
//   
//   return *this;
// }
// 
// bool ddOglList::operator==(ddOglList const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

ddOglList::renderItem& ddOglList::alloc ()
{
  // In the constructor we "reserve" a bunch of these...
  // when the ddOgl class needs one they call this method
  // which just increases the size of the list by one, but
  // typically not causing any actual allocation.
  // When the render pass is done, all "alloc"s are 
  // released by calling "resetRenderItems".
  
  mRenderList.resize ( mRenderList.size () + 1 );
  return mRenderList.back ();
}

void ddOglList::resetRenderItems ()
{
  mRenderList.clear ();
  mSinkPath.clear ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Do the real rendering.

#define SCENEOGLLTCOMPARE(lhs,rhs) if(!((lhs)==(rhs))) return (lhs)<(rhs)
#define SCENEOGLGTCOMPARE(lhs,rhs) if(!((lhs)==(rhs))) return (lhs)>(rhs)

struct sorter
{
  bool operator () ( ddOglList::RenderList::value_type const& lhs,
      ddOglList::RenderList::value_type const& rhs )
  {
    state const* lhsBlend = lhs.mStateSet.mStates[ state::Blend ];
    state const* rhsBlend = rhs.mStateSet.mStates[ state::Blend ];
  
      // We go to great lengths here to get the transparency sort
      // right.  Even if the pointers are different, we want things
      // to sort with respect to their enable state.
    int val = 0;
    bool hasBlend = false;

    if ( lhsBlend && lhsBlend->getEnable () )
        ++val, hasBlend = true; // Hack!!! Comma operator used!
    
    if ( rhsBlend && rhsBlend->getEnable () )
        --val;
    
    if ( val != 0 )
      return val < 0;
     
     
    // Really... we want to sort opaque geometry front-to-back and 
    // transparent geometry back-to-front... so... someday we'll fix
    // this up.  It would theoretically really help PVR chips as they
    // are tile-based deferred rendering.
    // NOTE: We are now doing the sorts as described above.
    if ( hasBlend )
    {
      SCENEOGLGTCOMPARE( lhs.distSqr, rhs.distSqr );  // Greater-than/descending dist sort.
    }
    else
    {
      SCENEOGLLTCOMPARE( lhs.distSqr, rhs.distSqr );  // Less-than/ascending dist sort.
    }

    SCENEOGLLTCOMPARE( lhs.mStateSet.mStates[ state::Prog ],
                        rhs.mStateSet.mStates[ state::Prog ] );
    SCENEOGLLTCOMPARE( lhs.mStateSet.mStates[ state::Texture0 ],
                        rhs.mStateSet.mStates[ state::Texture0 ] );
    SCENEOGLLTCOMPARE( lhs.mStateSet.mStates[ state::Texture1 ],
                        rhs.mStateSet.mStates[ state::Texture1 ] );
    SCENEOGLLTCOMPARE( lhs.mNode, rhs.mNode );      // Sort geoms together.

    return false;
  }
};

/////////////////////////////////////////////////////////////////////

void ddOglList::startList ()
{
#ifdef _WIN32
  static bool initDone = false;

#ifndef _NDEBUG
  if ( mDbgVals )
  {
    printf ( "ddOglList: %X Frame Start\n", this );
  }
#endif // _NDEBUG
  
  if ( ! initDone )
  {
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      GLubyte const* tmp = glewGetErrorString(err);
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      exit ( -2 );
    }
    
    initDone = true;
  }
#endif  // _WIN32

CheckGLError

PNIDBG
  // sort the list.
  std::sort ( mRenderList.begin (), mRenderList.end (), sorter () );
PNIDBG

  resetCurState ();

    // TODO: PRW PNIGLES1REMOVED
//  glMatrixMode ( GL_MODELVIEW );
//  glLoadIdentity ();

PNIDBG
    // Sets up prog and view mats
  execCamera ();

PNIDBG

// printf ( "size of render list = %d\n", mRenderList.size () );

    // Lame attempt to deal with some possible default state bug.
  //glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
  
  RenderList::const_iterator end = mRenderList.end ();
  for ( RenderList::const_iterator cur = mRenderList.begin ();
      cur != end;
      ++cur )
  {
//printf ( "node: %s  distSqr: %f\n", cur->mNode->getName ().c_str (), cur->distSqr );

#ifndef _NDEBUG
    if ( mDbgVals & DbgSort )
    {
      printf ( "DbgSort: node %s distSqr = %f\n",
          cur->mNode->getName ().c_str (), cur->distSqr );
    }

#endif // _NDEBUG

      // NEWMATSTACK
      // set up model mat
    mModelMat = cur->mMatrix;
    
      // TODO: PRW PNIGLES1REMOVED
//    glMatrixMode ( GL_MODELVIEW ); // TEMP... remove this after everything works.
//    glPushMatrix ();
//    glMultMatrixf ( cur->mMatrix );

 //cout << "cur->matrix =\n" << cur->mMatrix << endl;

PNIDBG
    execBuiltins();
    execStates ( cur->mStateSet );

PNIDBG
    cur->mNode->accept ( this );
    
      // TODO: PRW PNIGLES1REMOVED
//    glPopMatrix ();
  }

#ifndef _NDEBUG
  if ( mDbgVals )
  {
    printf ( "ddOglList: %p Frame End\n", this );
  }
#endif // _NDEBUG

  
PNIDBG
}

void ddOglList::execBuiltins ()
{
    // NEWMATSTACK
    // TODO: Optimize
  
  mModelViewMat = mViewMat;
  mModelViewMat.preMult(mModelMat);

  mModelViewProjectionMat = mProjMat;
  mModelViewProjectionMat.preMult(mModelViewMat);

  mModelViewProjectionMat.copyTo4x4(mBuiltins->uniformOp("modelViewProjectionMatrix").getFloats());
  
    // Handle non-uniform scaling... with inverse transpose.
    // Use camera->getNormalizeMode.
  camera const* pCam = static_cast< camera const*>(mSinkPath.getLeaf());
  if ( pCam->getNormalizeMode() != camera::NoNormalize)
  {
    mModelViewMat.invert();
    mModelViewMat.transpose();
  }

  mModelViewMat.copyTo3x3(mBuiltins->uniformOp("normalMatrix").getFloats());
  
  this->dispatch(mBuiltins.get());
}

void ddOglList::execCamera ()
{
    // NEWMATSTACK
  // Do something with matrix from camera path.
  mSinkPath.calcInverseXform ( mViewMat );

    // TODO: PRW PNIGLES1REMOVED
//  glLoadMatrixf ( mat );

// std::string tmp;
// mSinkPath.getPathString ( tmp );
// cout << " camera path = " << tmp << endl;
// cout << " camera mv mat = \n" << mat << endl;
  
  // Call camera->accept to bind all of the camera-related state.
  if ( node* pNode = mSinkPath.getLeaf () )
    pNode->accept ( this );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void ddOglList::bindTextures ( node const* pNode )
{
  ddOglTextureBind binder;
  binder.startGraph ( pNode );
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Graph methods.

void ddOglList::startGraph ( node const* pNode )
{
  // No-op.  The graph is traversed in ddOgl and the
  // unwound results of the traversal are stored in mRenderList.
}

void doClear ( camera const* pNode )
{
  unsigned int glClearMask = 0x00;
  unsigned int clearMask = pNode->getClearMask ();

  if ( clearMask & camera::Color )
  {
    glClearMask |= GL_COLOR_BUFFER_BIT;
    pni::math::vec4 color = pNode->getColorClear ();
    glClearColor ( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
    glColorMask ( 1, 1, 1, 1 );
  }
  
  if ( clearMask & camera::Depth )
  {
    glClearMask |= GL_DEPTH_BUFFER_BIT;
#ifndef WIN32
    glClearDepthf ( pNode->getDepthClear () );
#else
    glClearDepth ( pNode->getDepthClear () );
#endif
    glDepthMask ( 1 );
  }
  
  if ( clearMask & camera::Accum )
  {
    // TODO When iPhone supports it in HW!
  }
  
  if ( clearMask & camera::Stencil )
  {
    // TODO When needed.
  }
  
  glClear ( glClearMask );
}

void ddOglList::dispatch ( camera const* pNode )
{
    // NEWMATSTACK
  mProjMat = pNode->getProjectionMatrix();

  // Setup projection matrix.
    // TODO: PRW PNIGLES1REMOVED
//  glMatrixMode ( GL_PROJECTION );
//  glLoadMatrixf ( pNode->getProjectionMatrix () );

// cout << "camera proj = " << pNode->getProjectionMatrix () << endl;

    // TODO: PRW PNIGLES1REMOVED
//  glMatrixMode ( GL_MODELVIEW );
  
  // Setup viewport.
  float left, bottom, width, height;
  pNode->getViewport ( left, bottom, width, height );
  glViewport ( left, bottom, width, height );
  
  // TODO: Setup scissor.
  
  // Do clear.
  doClear ( pNode );
  
  // TODO: PRW PNIGLES1REMOVED
//  switch ( pNode->getNormalizeMode () )
//  {
//    case camera::Normalize:
//      glEnable ( GL_NORMALIZE );
//      break;
//    case camera::Rescale:
//      glEnable ( GL_RESCALE_NORMAL );
//      break;
//    case camera::NoNormalize:
//    default:
//      glDisable ( GL_NORMALIZE );
//      glDisable ( GL_RESCALE_NORMAL );
//  }
CheckGLError
}

void dbg ( geomData const* pGdata )
{
  for ( size_t num = 0; num < pGdata->getIndices ().size (); num++ )
  {
    cout << "    " << num << " = " << pGdata->getIndices ()[ num ] << endl;
  }
  
  geomData::Values const& vals = pGdata->getValues ();
  size_t stride = pGdata->getAttributes ().getValueStride ();

cout << "    stride = " << stride << endl;

  for ( size_t num = 0; num < vals.size (); num += stride )
  {
    cout << "  " << num / stride << "  " << vals[ num ] << ", " << vals[ num + 1 ] << ", " << vals[ num + 2 ] << endl;
  }
  
}

void ddOglList::dispatch ( geom const* pNode )
{
PNIDBG
    // GOTCHA: There is no validity checking here... that should
    // all be performed by ddOgl which puts only valid things
    // into this render list.
  geomData const* pData = pNode->getGeomData ();

  if ( progObj const* pProgObj = configProgObject(mCurProg.get()) )
  {
    if ( vbo* pvbo = configVBO(pData, pProgObj))
    {
      pvbo->bind(pData,pProgObj);
      glDrawElements ( GL_TRIANGLES, ( GLsizei ) pData->getIndices().size (), GL_UNSIGNED_SHORT, 0 );
    }
  }
CheckGLError
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( geomFx const* pNode )
{
  geom const* pGeom = pNode;
  dispatch ( pGeom );
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( group const* pNode )
{
PNIDBG
  // No-op.
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( light const* pNode )
{
PNIDBG
  // Invoked by dispatch( lightPath ) -> execLight ( ... ) ->
  //   node->accept ( this ) -> dispatch ( light ).

  glEnable ( mCurLightUnit );

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

	// set the color
	glLightfv ( mCurLightUnit, GL_AMBIENT, pNode->getAmbient () );
	glLightfv ( mCurLightUnit, GL_DIFFUSE, pNode->getDiffuse () );
	glLightfv ( mCurLightUnit, GL_SPECULAR, pNode->getSpecular () );

	// set the other light params based on type
	switch ( pNode->getType () )
	{
		default:
		case light::Directional:
			{
				const pni::math::vec3& tmp3 = pNode->getDirection ();
				pni::math::vec4 tmp4 ( pni::math::vec4::NoInit );
				tmp4[ 0 ] = -tmp3[ 0 ];
				tmp4[ 1 ] = -tmp3[ 1 ];
				tmp4[ 2 ] = -tmp3[ 2 ];
				tmp4[ 3 ] = 0.0f;

				glLightfv ( mCurLightUnit, GL_POSITION, tmp4 );
				glLightf ( mCurLightUnit, GL_SPOT_EXPONENT, 0.0f );
				glLightf ( mCurLightUnit, GL_SPOT_CUTOFF, 180.0f );

				glLightf ( mCurLightUnit, GL_CONSTANT_ATTENUATION, 1.0f );
				glLightf ( mCurLightUnit, GL_LINEAR_ATTENUATION, 0.0f );
				glLightf ( mCurLightUnit, GL_QUADRATIC_ATTENUATION, 0.0f );
			}
			break;
		case light::Point:
			{
				glLightfv ( mCurLightUnit, GL_POSITION, pNode->getPosition () );
				glLightf ( mCurLightUnit, GL_SPOT_EXPONENT, 0.0f );
				glLightf ( mCurLightUnit, GL_SPOT_CUTOFF, 180.0f );
				const pni::math::vec3& tmp = pNode->getAttenuation ();
				glLightf ( mCurLightUnit, GL_CONSTANT_ATTENUATION, tmp[ light::Constant ] );
				glLightf ( mCurLightUnit, GL_LINEAR_ATTENUATION, tmp[ light::Linear ] );
				glLightf ( mCurLightUnit, GL_QUADRATIC_ATTENUATION, tmp[ light::Quadratic ] );
			}
			break;
		case light::Spot:
			{
				glLightfv ( mCurLightUnit, GL_POSITION, pNode->getPosition () );
				glLightfv ( mCurLightUnit, GL_SPOT_DIRECTION, pNode->getDirection () );
				glLightf ( mCurLightUnit, GL_SPOT_EXPONENT, pNode->getExponent () );
				glLightf ( mCurLightUnit, GL_SPOT_CUTOFF, pNode->getCutoffAngle () );						
				const pni::math::vec3& tmp = pNode->getAttenuation ();
				glLightf ( mCurLightUnit, GL_CONSTANT_ATTENUATION, tmp[ light::Constant ] );
				glLightf ( mCurLightUnit, GL_LINEAR_ATTENUATION, tmp[ light::Linear ] );
				glLightf ( mCurLightUnit, GL_QUADRATIC_ATTENUATION, tmp[ light::Quadratic ] );
			}
			break;
	}

#endif // PNIGLES1REMOVED
  
CheckGLError
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( sndEffect const* pNode )
{
  // TODO
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( sndListener const* pNode )
{
  // TODO
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// State methods.

void ddOglList::startStates ( node const* pNode )
{
  // No-op.  The graph and states are already traversed when
  // this class is used.
}

void ddOglList::resetCurState ()
{
  mCurStates = stateSet ();
}

void ddOglList::execStates ( stateSet const& sSet )
{
    // Ensure that program state is set before we walk through other states.
  if ( auto pProg = sSet.mStates[ state::Prog ] )
    mCurProg = static_cast< prog const* > ( pProg );
  else
    PNIDBGSTR("no prog set for execStates... things might be ugly");

  for ( int num = 0; num < state::StateCount; ++num )
  {
    state const* pState = sSet.mStates[ num ];
    
    mCurStateId = num;
    
    // Note: we could get some speed ups here and other places if
    // we made the 'guarantee' that all states would be non-null,
    // even if we're only talking default disabled states. PRW
    
    // Note: This invokes state only if the current [graphics adapter]
    // state is considered different.
    if ( pState != mCurStates[ num ] && pState )
      pState->accept ( this );

    mCurStates[ num ] = pState;
  }
}

/////////////////////////////////////////////////////////////////////

void ddOglList::setDefaultState ( state const* pState, state::Id id )
{
  // No-op... ddOgl allows app to setup default state and
  // this class will get default state as a by-product of
  // graph traversal and setup of stateSets.
}

/////////////////////////////////////////////////////////////////////
///// Dispatch methods.


/////////////////////////////////////////////////////////////////////
// Blend related.

GLenum
blendEqToGl ( blend::BlendEquation beq )
{
 	switch ( beq )
 	{
 		default:
 		case blend::Add: return GL_FUNC_ADD;
 		case blend::Subtract: return GL_FUNC_SUBTRACT;
 		case blend::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        // Missing from GLES2.0 header for iOS (?).
// 		case blend::Min: return GL_MIN;
// 		case blend::Max: return GL_MAX;
 	}
}

int 
srcBlendTypeToGl ( blend::SrcFunc srcFunc )
{
	switch ( srcFunc )
	{
		default:
		case blend::SrcZero: return GL_ZERO;
		case blend::SrcOne: return GL_ONE;
		case blend::SrcDstColor: return GL_DST_COLOR;
		case blend::SrcOneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
		case blend::SrcAlpha: return GL_SRC_ALPHA;
		case blend::SrcOneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case blend::SrcDstAlpha: return GL_DST_ALPHA;
		case blend::SrcOneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
//		case blend::SrcAlphaSaturate: return GL_ALPHA_SATURATE;
	}
}

int
dstBlendTypeToGl ( blend::DstFunc dstFunc )
{
	switch ( dstFunc )
	{
		default:
		case blend::DstZero: return GL_ZERO;
		case blend::DstOne: return GL_ONE;
		case blend::DstSrcColor: return GL_SRC_COLOR;
		case blend::DstOneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
		case blend::DstSrcAlpha: return GL_SRC_ALPHA;
		case blend::DstOneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case blend::DstAlpha: return GL_DST_ALPHA;
		case blend::DstOneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
	}
}

int
alphaFuncTypeToGl ( blend::AlphaFunc afunc )
{
	switch ( afunc )
	{
		default:
		case blend::AlphaNever: return GL_NEVER;
		case blend::Less: return GL_LESS;
		case blend::Equal: return GL_EQUAL;
		case blend::LEqual: return GL_LEQUAL;
		case blend::Greater: return GL_GREATER;
		case blend::NotEqual: return GL_NOTEQUAL;
		case blend::GEqual: return GL_GEQUAL;
		case blend::AlphaAlways: return GL_ALWAYS;
	}
}

void ddOglList::dispatch ( blend const* pState )
{
  if ( pState )
  {
    glEnable ( GL_BLEND );

 		glBlendEquation ( blendEqToGl ( pState->getBlendEquation () ) );

		blend::SrcFunc src;
		blend::DstFunc dst;
		pState->getBlendFunc ( src, dst );
		
		glBlendFunc ( srcBlendTypeToGl ( src ), 
				dstBlendTypeToGl ( dst ) );

      // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

		// alpha test
		if ( pState->getAlphaFunc () != blend::AlphaAlways )
    {
      glEnable ( GL_ALPHA_TEST );
      glAlphaFunc ( alphaFuncTypeToGl ( pState->getAlphaFunc () ),
        pState->getAlphaRef () );
    }
    else
    {
		  glDisable ( GL_ALPHA_TEST );
    }
  #endif // PNIGLES1REMOVED
  
  }
  else
  {
		glDisable ( GL_BLEND );
      // TODO: PRW PNIGLES1REMOVED
//		glDisable ( GL_ALPHA_TEST );
  }
CheckGLError
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( cull const* pState )
{
	if ( pState->getEnable () )
	{
		glEnable ( GL_CULL_FACE );

		if ( pState->getCullFace () == cull::Front )
			glCullFace ( GL_FRONT );
		else
			glCullFace ( GL_BACK );

		if ( pState->getFrontFace () == cull::ClockWise )
			glFrontFace ( GL_CW );
		else
			glFrontFace ( GL_CCW );
	}
	else	// disabled
	{
		glDisable ( GL_CULL_FACE );
	}
CheckGLError
}

/////////////////////////////////////////////////////////////////////

int
depthFuncToGl ( depth::DepthFunc func )
{
	switch ( func )
	{

		case depth::DepthNever: return GL_NEVER;
		default:
		case depth::Less: return GL_LESS;
		case depth::Equal: return GL_EQUAL;
		case depth::LEqual: return GL_LEQUAL;
		case depth::Greater: return GL_GREATER;
		case depth::NotEqual: return GL_NOTEQUAL;
		case depth::GEqual: return GL_GEQUAL;
		case depth::DepthAlways: return GL_ALWAYS;
	}
}

void ddOglList::dispatch ( depth const* pState )
{
	if ( pState->getEnable () )
	{
		glEnable ( GL_DEPTH_TEST );

// 		const pni::math::vec2& drange = pState->getDepthRange ();
// 		glDepthRange ( drange[ depth::Near ], drange [ depth::Far ] );

		glDepthMask ( pState->getDepthMask () );

		glDepthFunc ( depthFuncToGl ( pState->getDepthFunc () ) );
	}
	else	// disabled
	{
		glDisable ( GL_DEPTH_TEST );		
	}  
CheckGLError
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( lighting const* pState )
{
    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

	if ( pState->getEnable () )
	{
		glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, 
				pState->getAmbient () );
				
		glEnable ( GL_LIGHTING );
		
// 		glLightModeli ( GL_LIGHT_MODEL_LOCAL_VIEWER, 
// 				pState->getLocalViewer () );

// 		glLightModelf ( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );

// 		glLightModeli ( GL_LIGHT_MODEL_COLOR_CONTROL,
// 				pState->getSeparateSpecular () + GL_SINGLE_COLOR );
	}
	else // disable
	{
//    float defAmbient[ 4 ] = { 0.2f, 0.2f, 0.2f, 1.0f };

		glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, 
				pState->getAmbient () );

		glDisable ( GL_LIGHTING );

		// should we do more here?
// 		glLightModelf ( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );		

// 		glLightModeli ( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE );

// 		glLightModelf ( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR );

	}

#endif // PNIGLES1REMOVED

CheckGLError
}

/////////////////////////////////////////////////////////////////////

void ddOglList::execLightPath ( nodePath const& lpath )
{

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

  // We must get the light into global 'scene' space, as we don't really
  // enjoy having lights in weird coordinate frames.
  glMatrixMode ( GL_MODELVIEW );
  glPushMatrix ();
  
  // Mult on light path matrix.
  pni::math::matrix4 mat;
  lpath.calcXform ( mat );
  glLoadMatrixf ( mat );

// std::string tmp;
// lpath.getPathString ( tmp );
// cout << " light path = " << tmp << endl;
// cout << " light mv mat = \n" << mat << endl;
  
  // Leaf node . accept this
  lpath.getLeaf ()->accept ( this );
  
  glPopMatrix ();
#endif // PNIGLES1REMOVED
}

void ddOglList::dispatch ( lightPath const* pState )
{
    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED
  for ( GLenum num = 0; num < lightPath::MaxNodePaths; ++num )
  {
    nodePath const& cur = pState->getNodePath ( num );
    mCurLightUnit = GL_LIGHT0 + num;
    
    if ( ! cur.empty () && pState->getEnable () )
      execLightPath ( cur );
    else
      glDisable ( mCurLightUnit );
  }
  
#endif // PNIGLES1REMOVED

CheckGLError
}

/////////////////////////////////////////////////////////////////////

// int
// colorModeToGl ( material::ColorMode cmode )
// {
// 	switch ( cmode )
// 	{
// 		default:
// 		case material::AmbientAndDiffuse: return GL_AMBIENT_AND_DIFFUSE;
// 		case material::Emission: return GL_EMISSION;
// 		case material::Ambient: return GL_AMBIENT;
// 		case material::Diffuse: return GL_DIFFUSE;
// 		case material::Specular: return GL_SPECULAR;
// 	}
// }

void ddOglList::dispatch ( material const* pState )
{

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

	if ( pState->getEnable () )
	{
		// material params
		glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, pState->getAmbient () );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, pState->getDiffuse () );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR, pState->getSpecular () );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, pState->getEmissive () );

		glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, pState->getShininess () );

		// color material mode
// 		material::ColorMode cmode = pState->getColorMode ();
// 		if ( cmode != material::ModeNone )
// 		{
// 			glEnable ( GL_COLOR_MATERIAL );
// 
// 			glColorMaterial ( GL_FRONT, colorModeToGl ( cmode ) );
// 		}
// 		else
// 			glDisable ( GL_COLOR_MATERIAL );

    if ( pState->getColorMaterialMode () )
      glEnable ( GL_COLOR_MATERIAL );
    else
      glDisable ( GL_COLOR_MATERIAL );
	}
	else // disabled
	{
// 		glDisable ( GL_COLOR_MATERIAL );
    glDisable ( GL_COLOR_MATERIAL );
	}
	
	  // EXPERIMENTAL
	  // 20081113 This seems to check out with no negative effects.
	  // Note that this should be safe because when not using lighting
	  // or when using lighting with color material this color will 
	  // only remain in opengl state if the geometry object, which is
	  // always evaluated last, has colors set in the value array.
	  // 20081201 This is not causing problems, but it's not always
	  // working either.  Reason is TBD.
	  // 20081202 This _is_ working.  It was failing due to a completely
	  // unrelated bug.
	glColor4f ( pState->getDiffuse ()[ 0 ], pState->getDiffuse ()[ 1 ],
	    pState->getDiffuse ()[ 2 ], pState->getDiffuse ()[ 3 ] );

#endif // PNIGLES1REMOVED

CheckGLError
}

/////////////////////////////////////////////////////////////////////

// void ddOglList::dispatch ( polygonMode const* pState )
// {
//   // TODO
// 
// }

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( prog const* pState )
{
    // Track current prog, will be needed when binding uniforms, etc.
    // Update... we don't need to do this here because it is done in
    // execStates... to make sure it's set before any other state is
    // evaluated.
//  mCurProg = pState;

    // TODO: Set prog-related state
  if (progObj* pobj = configProgObject(pState) )
    pobj->bind(pState);
}

/////////////////////////////////////////////////////////////////////

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED
int
texEnvModeToGl ( texEnv::Mode modeIn )
{

	switch ( modeIn )
	{
		default:
		case texEnv::Modulate: return GL_MODULATE;
		case texEnv::Decal: return GL_DECAL;
		case texEnv::Replace: return GL_REPLACE;
		case texEnv::Blend: return GL_BLEND;
		case texEnv::Add: return GL_ADD;
		case texEnv::Combine: return GL_COMBINE;
	}

}
#endif // PNIGLES1REMOVED

void ddOglList::dispatch ( texEnv const* pState )
{
	glActiveTexture ( GL_TEXTURE0 + mCurStateId - state::TexEnv0 );

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

	if ( pState->getEnable () )
	{
		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
				texEnvModeToGl ( pState->getMode () ) );
		
		texEnv::Mode envMode = pState->getMode ();
		if ( envMode == texEnv::Blend || envMode == texEnv::Combine )
		{
			glTexEnvfv ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,
					pState->getBlendColor () );
		}
		
		// TODO: register combiners
		if ( envMode == texEnv::Combine )
		{
			
		}
	}
	else	// disabled
	{
		// no disable for texenvs... when not set
		// it's undefined/inherited from hell.
	}
  
#endif // PNIGLES1REMOVED

CheckGLError
}

/////////////////////////////////////////////////////////////////////

// void
// applyTexGenCoord ( texGen* tgen, texGen::Coord coord, int oglCoord, int oglCoord2 )
// {
// 	texGen::Function func = tgen->getFunction ( coord );
// 
// 	if ( func != texGen::Disable )
// 	{
// 		glEnable ( oglCoord );
// 		int oglFunc = texgenFuncToGl ( tgen->getFunction ( coord ) );
// 		glTexGeni ( oglCoord2, GL_TEXTURE_GEN_MODE, oglFunc );
// 
// 		if ( func == texGen::ObjectLinear || func == texGen::EyeLinear )
// 		{
// 			int oglParam = GL_OBJECT_PLANE;
// 			if ( func == texGen::EyeLinear )
// 				oglParam = GL_EYE_PLANE;
// 
// 			math::vec3 norm;
// 			float plane[ 4 ];
// 			tgen->getPlane ( coord ).get ( norm, plane[ 3 ] );
// 			plane[ 0 ] = norm[ 0 ];
// 			plane[ 1 ] = norm[ 1 ];
// 			plane[ 2 ] = norm[ 2 ];
// 
// 			glTexGenfv ( oglCoord2, oglParam, plane );
// 		}
// 	}
// 	else
// 		glDisable ( oglCoord );
// }
// 
// void ddOglList::dispatch ( texGen const* pState )
// {
// 	// TODO Support applyFrame for texgens... ObjectLinear and EyeLinear depend
// 	// on the currently bound modelview matrix.  (From PNI)
// 
// 	glActiveTexture ( GL_TEXTURE0 + mCurStateId - state::TexGen0 );
// 
// 	if ( pState->getEnable () )
// 	{
// 		applyTexGenCoord ( pState, texGen::SCoord, GL_TEXTURE_GEN_S, GL_S );
// 		applyTexGenCoord ( pState, texGen::TCoord, GL_TEXTURE_GEN_T, GL_T );
// 		applyTexGenCoord ( pState, texGen::RCoord, GL_TEXTURE_GEN_R, GL_R );
// 		applyTexGenCoord ( pState, texGen::QCoord, GL_TEXTURE_GEN_Q, GL_Q );
// 	}
// 	else
// 	{
// 		glDisable ( GL_TEXTURE_GEN_S );
// 		glDisable ( GL_TEXTURE_GEN_T );
// 		glDisable ( GL_TEXTURE_GEN_R );
// 		glDisable ( GL_TEXTURE_GEN_Q );
// 	}
// 
// 
// }

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( texture const* pState )
{
	glActiveTexture ( GL_TEXTURE0 + mCurStateId - state::Texture0 );

	if ( pState->getEnable () )
	{
		configTextureObject ( pState );
	}
	else	// disabled
	{
    // GOTCHA: Only support 2D texture now in OES.
    // TODO: Support cube map extension.
		glDisable ( GL_TEXTURE_2D );
	}  
}

/////////////////////////////////////////////////////////////////////

void ddOglList::dispatch ( textureXform const* pState )
{
  // TODO: This class really begs the PNI "applyFrame" functionality.

	glActiveTexture ( GL_TEXTURE0 + mCurStateId - state::TextureXform0 );

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED

  glMatrixMode ( GL_TEXTURE );

	if ( pState->getEnable () )
	{
    glLoadMatrixf ( pState->getMatrix () );
	}
	else	// disabled
	{
    glLoadIdentity ();
	}

  glMatrixMode ( GL_MODELVIEW );
  
#endif // PNIGLES1REMOVED

CheckGLError
}

void ddOglList::dispatch ( uniform const* pState )
{
  if ( mCurProg )
  {
    if ( progObj* pobj = configProgObject ( mCurProg.get () ) )
    {
        // TODO: All of this should/could go in a uniforms object, and
        // that should use gl uniform buffer arrays... following
        // the same pattern as texture, vbo, prog, etc.  It will be
        // invalidated by the dirty flag already present on the uniform
        // object.

      uniform::Bindings const& bindings = pState->getBindings ();

      for ( auto& bindingIter : bindings )
      {
        uniform::binding const& binding = bindingIter.second;
        std::string const& name = bindingIter.first;
        GLsizei count = ( GLsizei ) binding.getCount ();

          // TODO: Better setup for this when we support more stages.
        GLuint glProg = binding.getStage () == uniform::binding::Vertex ? pobj->getVertexProgHandle() : pobj->getFragmentProgHandle();

CheckGLError

        GLint loc = glGetUniformLocation ( glProg, name.c_str () );

CheckGLError

        switch ( binding.getType () )
        {
          case uniform::binding::Float1: glProgramUniform1fvEXT ( glProg, loc, count, binding.getFloats () ); break;
          case uniform::binding::Float2: glProgramUniform2fvEXT ( glProg, loc, count, binding.getFloats () ); break;
          case uniform::binding::Float3: glProgramUniform3fvEXT ( glProg, loc, count, binding.getFloats () ); break;
          case uniform::binding::Float4: glProgramUniform4fvEXT ( glProg, loc, count, binding.getFloats () ); break;

          case uniform::binding::Int1: glProgramUniform1ivEXT ( glProg, loc, count, binding.getInts () ); break;
          case uniform::binding::Int2: glProgramUniform2ivEXT ( glProg, loc, count, binding.getInts () ); break;
          case uniform::binding::Int3: glProgramUniform3ivEXT ( glProg, loc, count, binding.getInts () ); break;
          case uniform::binding::Int4: glProgramUniform4ivEXT ( glProg, loc, count, binding.getInts () ); break;

            // Note: Simulator was returning error 0x501 (invalid value) for GL_TRUE to transpose param.
            // No big though, we don't need it because our matrices are in the right format.
          case uniform::binding::Matrix2: glProgramUniformMatrix2fvEXT ( glProg, loc, count, GL_FALSE, binding.getFloats () ); break;
          case uniform::binding::Matrix3: glProgramUniformMatrix3fvEXT ( glProg, loc, count, GL_FALSE, binding.getFloats () ); break;
          case uniform::binding::Matrix4: glProgramUniformMatrix4fvEXT ( glProg, loc, count, GL_FALSE, binding.getFloats () ); break;

          default:
            PNIDBGSTR ( "case not handled, getType out of range" );
            break;
        }
CheckGLError
      }
    }
  }
  else
  {
    PNIDBGSTR("no program set for uniforms");
  }
}

  
/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


