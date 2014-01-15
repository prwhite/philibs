/////////////////////////////////////////////////////////////////////
//
//    file: sceneloaderase.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PSTDPROFDISABLE

#include "sceneloaderase.h"
#include "pnisceneaseparser.h"

#include "pnimatstack.h"
#include "pnimathstream.h"

#include "scenecamera.h"
#include "scenegeom.h"
#include "scenegroup.h"
#include "scenelight.h"

#include "sceneblend.h"
#include "scenecull.h"
#include "scenedepth.h"
#include "scenelighting.h"
#include "scenelightpath.h"
#include "scenematerial.h"
// #include "scenepolygonmode.h"
#include "scenetexenv.h"
// #include "scenetexgen.h"
#include "scenetexture.h"
#include "scenetexturexform.h"

#include "imgdds.h"

#include <iostream>
#include <map>
#include <algorithm>

//#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#include "timerprof.h"

/////////////////////////////////////////////////////////////////////

using namespace scene;
using namespace std;

/////////////////////////////////////////////////////////////////////

namespace loader {

/////////////////////////////////////////////////////////////////////

class helper
{
  public:
    typedef std::string Str;
    typedef std::string const Strc;
    typedef std::map< unsigned int, ::ase::node const* > AseMaterialMap;
    typedef std::map< unsigned int, state* > StateMap;
    typedef std::map< unsigned int, StateMap >  ShaderMap;
  
    ase& mAse;
    ::ase::node* mRoot;
    observer* mObserver;
    cache* mCache;
    pni::math::matStack< pni::math::matrix4 > mMatStack;
    AseMaterialMap mMatNodes;
    ShaderMap mShaderMap;
    
    helper ( ase& rAse, ::ase::node* pRoot ) :
      mAse ( rAse ),
      mRoot ( pRoot ),
      mObserver ( rAse.getObserver () ),
      mCache ( rAse.getCache () )
        {
          
        }
        
    ~helper ()
        {
          delete mRoot;
        }
        
    void addNodeToDirectory ( scene::node* pNode )
        {
          if ( mAse.getDirectory () )
            mAse.getDirectory ()->addNode ( pNode );
        }
    
		void handleMaterial ( ::ase::node const* pSrc, int id = -1 )
        {
PNIDBG
          if ( id == -1 )
            id = pSrc->getInt ();
          
          // We'll save sub-mat handling till later.  Now we just
					// catalog base material node pointers.
          
          mMatNodes[ id ] = pSrc;
        }
      
    void copyCachedStatesToNode ( node* pNode, StateMap const& rMap )
        {
          StateMap::const_iterator end = rMap.end ();
          for ( StateMap::const_iterator cur = rMap.begin ();
              cur != end;
              ++cur )
          {
            pNode->setState ( cur->second, 
                static_cast< state::Id > ( cur->first ) );
 //cout << "copying state [id] to node " << cur->first << endl;
          }

        }
        
    void processMaterial ( node* pNode, int id )
        {
PNIDBG
          ShaderMap::const_iterator found = mShaderMap.find ( id );
          if ( found != mShaderMap.end () )
          {
            // Re-use cached shader.  Copy states over to node.
            copyCachedStatesToNode ( pNode, found->second );
          }
          else
          {
            // Create new "shader" and cache it.
            StateMap& rMap = mShaderMap[ id ];
            
            AseMaterialMap::const_iterator found = mMatNodes.find ( id );
            if ( found != mMatNodes.end () )
              buildMaterial ( pNode, rMap, found->second );
            
            copyCachedStatesToNode( pNode, rMap );
          }
        }
    
    void buildMaterialProps ( StateMap& rMap, ::ase::node const* pSrc )
        {
PNIDBG
          // TODO: pni version wraps this in a test on mNeedsMaterial to
          // prevent the base material from being re-writting or something.
        
 					// HACK: So max's baked textures make the base material a "shell material"
					// which doesn't have the original material props, but some lame default
					// props.  Thus, we don't use any material params until we find the first
					// standard map... which appears to always be the original authored material.
					if ( pSrc->findLine ( "MATERIAL_CLASS" )->getQuoted () != "Standard" )
						return;

					material* pMat = new material;
					pMat->setName ( pSrc->findLine ( "MATERIAL_NAME" )->getQuoted () );

					float alpha = 1.0f - pSrc->findLine ( "MATERIAL_TRANSPARENCY" )->getFloat ();
					pni::math::vec3 tmp3;
					pni::math::vec4 tmp4;

					pSrc->findLine ( "MATERIAL_AMBIENT" )->getFloat3 ( tmp4 );
					tmp4[ 3 ] = alpha;
					pMat->setAmbient ( tmp4 );

					pSrc->findLine ( "MATERIAL_DIFFUSE" )->getFloat3 ( tmp4 );
					tmp4[ 3 ] = alpha;
					pMat->setDiffuse ( tmp4 );

					pSrc->findLine ( "MATERIAL_SPECULAR" )->getFloat3 ( tmp4 );
					tmp4 *= pSrc->findLine ( "MATERIAL_SHINESTRENGTH" )->getFloat ();
					tmp4[ 3 ] = alpha;
					pMat->setSpecular ( tmp4 );

					// NOTE: It looks like max mults the diffuse color by the float
					// represented by MATERIAL_SELFILLUM.  It's a bit annoying.
					if ( ::ase::line const* pLine = pSrc->findLine ( "MATERIAL_SELFILLUM", false ) )
					{
						tmp4[ 0 ] = tmp4[ 1 ] = tmp4[ 2 ] = pLine->getFloat ();
							tmp4 *= pMat->getDiffuse ();	// I don't like this. PRW
					}
					else
						tmp4[ 0 ] = tmp4[ 1 ] = tmp4[ 2 ] = 0.0f;
            
					tmp4[ 3 ] = alpha;
					pMat->setEmissive ( tmp4 );

					pMat->setShininess ( 127.0f * 
							pSrc->findLine ( "MATERIAL_SHINE" )->getFloat () );

					// Add blend if necessary.
					if ( alpha < 1.0f - pni::math::Trait::fuzzVal )
						rMap[ state::Blend ] = mCache->mDefBlend.get ();

					rMap[ state::Material ] = ( pMat );         
        }

  
  // There is some penciled-in stuff in this file that I don't want to
  // be noisy in every build.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

    void buildMaterial ( node* pSceneNode, StateMap& rMap, ::ase::node const* pSrc )
        {
PNIDBG
          buildMaterialProps ( rMap, pSrc );

					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_DIFFUSE", false ) )
					{
						if ( texture* pTex = processTexture ( rMap, pNode ) )
						{
						
						}
					}

					// Specular color.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_SPECULAR", false ) )
					{

					}

					// Specular level.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_SHINE", false ) )
					{

					}

					// Specular placement... really redundant with specular level.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_SHINESTRENGTH", false ) )
					{

					}

					// Right now we only properly handle opacity maps with diffuse maps.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_OPACITY", false ) )
					{
						// Gotta merge the opacity map with the diffuse map on
						// the same channel.
// 						processOpacityImage ( pShader, pNode );
					}

					// Light map.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_SELFILLUM", false ) )
					{
						if ( texture* pTex = processTexture ( rMap, pNode ) )
						{
							// TODO Maybe override texenv on this unit.
						}
					}

					// Filter color... filters light cast through trasparent objects.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_FILTERCOLOR", false ) )
					{

					}

					// Bump map.					
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_BUMP", false ) )
					{

					}

					// Reflect map.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_REFLECT", false ) )
					{
						if ( pNode->findLine ( "RRMAP_SOURCE", false ) )
						{
// 							cubeMapHelper cmHelper ( pSceneNode, pShader, pNode, mCurUnit, this );
// 							++mCurUnit;
						}
					}
					
					// Refract map.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_REFRACT", false ) )
					{

					}

					// Displacement map.
					if ( ::ase::node const* pNode = pSrc->findNode ( "MAP_DISPLACEMENT", false ) )
					{

					}
					
            // TEMP Causing an unknown crash right now.
// 					buildSubMaterials ( pSceneNode, rMap, pSrc );
        }

#pragma clang diagnostic pop

    void buildSubMaterials ( node* pNode, StateMap& rMap, ::ase::node const* pSrc )
        {
PNIDBG
          // TODO: We shouldn't let submaterials override the first standard
          // material's material props.
        
					::ase::node::ConstNodeIter end = pSrc->getChildEnd ();
					for ( ::ase::node::ConstNodeIter cur = pSrc->getChildBegin ();
							cur != end; ++cur )
					{
						if ( ( *cur )->getName () == "SUBMATERIAL" )
						{
							buildMaterial ( pNode, rMap, *cur );
						}
					}          
        }
    
    bool doStat ( std::string const& fname )
        {
          bool retVal = false;

        	if ( FILE* fid = fopen ( fname.c_str (), "rb" ) )
                {
          
		      struct stat fileStat;
		      memset ( &fileStat, 0, sizeof ( fileStat ) );

		      if ( fstat ( fileno ( fid ), &fileStat ) == 0 )
            retVal = true;
          
          fclose ( fid );
         }
          
          return retVal;
        }

    std::string tail ( std::string const& src )
        {
          // Do a tail on the file name cause we don't do search paths yet.
          // All images have to be in cwd.
PNIDBG
          std::string tmp = src;
          size_t ind = src.find_last_of ( "/\\" );
          if ( ind != std::string::npos )
            tmp = src.substr ( ind + 1 );
PNIDBG
          return tmp;        
        }
    
    bool resolveImageName( std::string const& src, std::string& dst )
        {
PNIDBG
          std::string tmp ( tail ( src ) );
PNIDBG
          if ( doStat ( tmp ) )
          {
PNIDBG
            dst = tmp;
            return true;
          }
          else
	  {
PNIDBG
            return false;
          }
        }

    texture* processTexture ( StateMap& rMap, ::ase::node const* pSrc )
        {
PNIDBG
					// We only handle bitmap textures right now.
					std::string fname;
					if ( ::ase::line const* pLine = pSrc->findLine ( "BITMAP", false ) )
						fname = pLine->getQuoted ();
					else
					{
						std::string err ( "Only bitmap maps are processed currently: " );
						err += pSrc->findLine ( "MAP_NAME" )->getQuoted ();
						mObserver->onError ( pni::pstd::error ( loader::InternalErrorTexture, err ) );
						return 0;
					}

PNIDBG
					if ( ! resolveImageName ( fname, fname ) )
						return 0;

PNIDBG
          texture* pTex = 0;
          img::base* pImg = 0;
          
PNIDBG
          // Check for texture in cache.
          cache::Textures::const_iterator found = mCache->mTextures.find ( fname );
PNIDBG
          if ( found != mCache->mTextures.end () )
          {
PNIDBG
            // In cache!
            pTex = found->second.get ();
            pImg = pTex->getImage ();
          }
          else
          {
PNIDBG
            pni::pstd::progressEvent event ( &mAse );
            if ( mAse.getProgressObserver () )
              mAse.getProgressObserver ()->onProgressUpdate ( event );

            pTex = new texture;
            mCache->mTextures[ fname ] = pTex;
            
            pTex->setName ( pSrc->findLine ( "MAP_NAME" )->getQuoted() );
            pTex->setMinFilter ( texture::MinLinearMipNearest );
            
            // TODO: Set up better handling for different image types.
            if ( (pImg = img::dds::loadHelper ( fname )) )
            {
              pTex->setImage ( pImg );
              
              if ( pImg->mBuffers.size () == 1 )
                pTex->setMinFilter ( texture::MinLinear );
            }
          }
                
PNIDBG
					// Add to shader with proper unit.
					// Our unit is 0 based, theirs is one based.
					unsigned int channel = 0;
					if ( ::ase::line const* pLine = pSrc->findLine ( "MAPPING_CHANNEL", false ) )
						channel = pLine->getInt () - 1;
					else
						mObserver->onError ( pni::pstd::error ( loader::InternalErrorTexture, 
								"No texture mapping channel specified, please use SEASE format" ) );
          
PNIDBG
          rMap[ state::Texture0 + channel ] = pTex;
          
PNIDBG
          if ( pImg && pImg->hasAlpha () )
          {
            //printf ( "  %s has alpha\n", pImg->getName ().c_str () );
            rMap[ state::Blend ] = mCache->mDefBlend.get ();
          }
          else
          {
            //printf ( "  %s has no alpha\n", pImg->getName ().c_str () );
          }
          
PNIDBG
          return pTex;
        }

    void invertGeom ( geom* pGeom )
        {
					// Setup vertex inverse xform mat.
					pni::math::matrix4 mat ( pGeom->getMatrix () );
PNIDBG
					if ( mat.isIdentity () )
						return;
PNIDBG
          geomData* pGdata = pGeom->geometryOp ();
          geomData::SizeType stride = pGdata->getAttributes ().getValueStride ();
//          const unsigned int vertOffset = 0;    // Always zero.
//           const unsigned int normOffset = pGdata->getValueOffset ( geomData::Normals );

// cout << "pre invert = \n" << mat << endl;
					mat.invert ();
// cout << "post invert = \n" << mat << endl;
				
          float* verts = &pGdata->getValues ()[ 0 ];
          
          geomData::SizeType end = pGdata->getValueCount ();
          
          for ( geomData::SizeType num = 0; num < end; num += stride, verts += stride )
          {
            pni::math::vec3 tmp ( verts );
            tmp.xformPt ( tmp, mat );
            verts[ 0 ] = tmp[ 0 ];
            verts[ 1 ] = tmp[ 1 ];
            verts[ 2 ] = tmp[ 2 ];
          }
        }
    
    void fixupXform ( node* pNode )
        {
PNIDBG
					// K... so the problem is that all ASE file xforms are world->node
					// frame... not relative to the parent frame.  Thus, we have to 
					// reconstitute the relative matrix with an invert of the parent
					// matrix and a fwd xform of this node's xform.
				
					if ( mMatStack->isIdentity () )
						return;

					pni::math::matrix4 nodeMat ( *mMatStack );
					nodeMat.invert ();
					nodeMat.preMult ( pNode->getMatrix () );
					pNode->matrixOp () = nodeMat;
PNIDBG
        }
    
    template< class Type >
    static Type minVal ( Type lhs, Type rhs )
        {
          return lhs < rhs ? lhs : rhs;
        }

    template< class Type >
    static Type maxVal ( Type lhs, Type rhs )
        {
          return lhs > rhs ? lhs : rhs;
        }
    
    void initAttributes ( geom* pGeom, ::ase::node const* pSrc )
        {
PNIDBG
            // geomData is guaranteed to be good before this is called (from processObject)
          geomData::Attributes& attributes = pGeom->geometryOp ()->attributesOp ();

          attributes.push_back ( { "position", geomData::Positions, geomData::DataType_FLOAT, geomData::PositionsComponents } );

					if ( pSrc->findNode ( "MESH_NORMALS", false ) )
            attributes.push_back ( { "normal", geomData::Normals, geomData::DataType_FLOAT, geomData::NormalsComponents } );

					// First set of UVs.
					if ( pSrc->findNode ( "MESH_TVERTLIST", false ) )
            attributes.push_back ( { "uv00", geomData::TCoords00, geomData::DataType_FLOAT, geomData::TCoords00Components } );

          // Iterate over "MESH_MAPPINGCHANNEL" children for
          // other sets of UVs.
          
					::ase::node::ConstNodeIter end = pSrc->getChildEnd ();
					for ( ::ase::node::ConstNodeIter cur = pSrc->getChildBegin (); 
              cur != end;
              ++cur )
					{
						if ( ( *cur )->getName () == "MESH_MAPPINGCHANNEL" )
						{
							// We use the ASE channel number directly.  Alternatively we
							// could go first-come, first serve for unit allocation... but that
							// is a pain to coordinate with material process which is random access.
							unsigned int unit = ( *cur )->getInt () - 1;	// We are 0-based, they are 1.
              
              // TEMP Only supporting 2 texture units.
              const unsigned int MaxUnit = 1;
              if ( unit == MaxUnit )
                attributes.push_back ( { "uv01", geomData::TCoords01, geomData::DataType_FLOAT, geomData::TCoords01Components } );
              else
                mObserver->onError ( 
                    pni::pstd::error ( InternalErrorTexture,
                    "Texture unit out of range" ) );
						}
					}
        }
        
    class iVert
    {
      public:
        iVert () :
          mVert ( 0 ),
          mNorm ( 0 ),
          mUv00 ( 0 ),
          mUv01 ( 0 ),
          mInd ( 0 )
            {
            
            }
        
        bool operator < ( iVert const& rhs ) const
            {
              if ( mVert != rhs.mVert )
                return mVert < rhs.mVert;
              if ( mNorm != rhs.mNorm )
                return mNorm < rhs.mNorm;
              if ( mUv00 != rhs.mUv00 )
                return mUv00 < rhs.mUv00;
              if ( mUv01 != rhs.mUv01 )
                return mUv01 < rhs.mUv01;
              
              return false;
            }
            
        bool operator == ( iVert const& rhs ) const
            {
              return ! ( *this < rhs || rhs < *this );
            }
              
        // Max doesn't have vert colors.
        unsigned short mVert;
        unsigned short mNorm;
        unsigned short mUv00;
        unsigned short mUv01;
        unsigned short mInd;
    };
    
    class reIndexer
    {
      public:
         ::ase::node const* mVind;
         ::ase::node const* mNind;
         ::ase::node const* mUVind00;
         ::ase::node const* mUVind01;
      
        typedef std::vector< iVert > Verts;
        
        Verts mVerts;
        unsigned int mSize;
        unsigned int mDataCount;
        
        reIndexer ( unsigned int size ) :
          mSize ( size ),
          mVind ( 0 ),
          mNind ( 0 ),
          mUVind00 ( 0 ),
          mUVind01 ( 0 ),
          mDataCount ( 0 )
            {
              mVerts.resize ( mSize );
            }
        
        void collate ()
            {
              std::sort ( mVerts.begin (), mVerts.end () );
            }
            
        void collapse ()
            {
// dbg ();
              // Set the index for the vert by just incrementing
              // through.
              mDataCount = 0;
              Verts::iterator end = mVerts.end ();
              for ( Verts::iterator cur = mVerts.begin ();
                  cur != end;
                  ++cur )
                cur->mInd = mDataCount++;
              
              // Walk through all vertexes re-assigning the main
              // index (mInd) to match the lowest matching index.
              // This provides the simplest mechanism  for overall
              // vert data sharing.
              unsigned int count = 0;
              for ( Verts::iterator cur = mVerts.begin ();
                  cur != end;
                  ++cur )
              {
                if ( cur->mInd >= count )
                {
                  cur->mInd = count;
                  ++count;
                }
              
                Verts::iterator found = 
                    find ( cur +  1, mVerts.end (), *cur );
                if ( found != end && found->mInd != cur->mInd )
                {
// cout << "  found->mInd, cur->mInd " << found->mInd << ", " << cur->mInd << endl;
                  found->mInd = cur->mInd = minVal ( found->mInd, cur->mInd );
                  mDataCount--;
                }
              }
// cout << "mDataCount, mSize = " << mDataCount << ", "  << mSize << endl;
// dbg ();
            }

        void dbg () const
            {
              cout << "reIndexer.dbg ()" << endl;
              for ( size_t num = 0; num < mVerts.size (); ++num )
              {
                iVert const& cur = mVerts[ num ];
                
                cout << "ind - pos, norm, uv0, uv1 : "
                    << cur.mInd << " - "
                    << cur.mVert << ", "
                    << cur.mNorm << ", "
                    << cur.mUv00 << ", "
                    << cur.mUv01 << endl;
              }
            }
    };
    
    void initVertIndices ( ::ase::node const* pSrc, reIndexer& ind )
        {
          typedef ::ase::node::ConstLineIter Iter;
          
          Iter end = pSrc->getLineEnd ();
          for ( Iter cur = pSrc->getLineBegin (); cur != end; ++cur )
          {
            int face = -1;
            int vals[ 3 ] = { 0 };
            
            ( *cur )->getMeshFace ( face, vals );
            
            if ( face != -1 )
            {
              // We re-use 'face' as the index we want to store into.
              face *= 3;
              ind.mVerts[ face++ ].mVert = vals[ 0 ];
              ind.mVerts[ face++ ].mVert = vals[ 1 ];
              ind.mVerts[ face   ].mVert = vals[ 2 ];
            }
          }
        }

    void initNormIndices ( ::ase::node const* pSrc, reIndexer& ind )
        {
          typedef ::ase::node::ConstLineIter Iter;
          
          Iter end = pSrc->getLineEnd ();
          int index = 0;
          for ( Iter cur = pSrc->getLineBegin (); cur != end; ++cur )
          {
            if ( ( *cur )->getName () == "MESH_VERTEXNORMAL" )
            {
//               int tmp = cur->getInt ();
              // Norms are in the same order as the ASE vert index
              // list... so just increment the counter, yo.
              int tmp = index;

              if ( tmp != -1 )
                ind.mVerts[ index++ ].mNorm = tmp;
            }
          }        
        }

    void initUvIndices ( ::ase::node const* pSrc, reIndexer& ind, unsigned int chan )
        {
          typedef ::ase::node::ConstLineIter Iter;
          
          Iter end = pSrc->getLineEnd ();
          for ( Iter cur = pSrc->getLineBegin (); cur != end; ++cur )
          {
            int face = -1;
            int vals[ 3 ] = { 0 };
            
            ( *cur )->getIntInt3 ( face, vals );
            
            if ( face != -1 )
            {
              // We re-use 'face' as the index we want to store into.
              face *= 3;
              if ( chan == 0 )
              {
                ind.mVerts[ face++ ].mUv00 = vals[ 0 ];
                ind.mVerts[ face++ ].mUv00 = vals[ 1 ];
                ind.mVerts[ face   ].mUv00 = vals[ 2 ];
              }
              else
              {
                ind.mVerts[ face++ ].mUv01 = vals[ 0 ];
                ind.mVerts[ face++ ].mUv01 = vals[ 1 ];
                ind.mVerts[ face   ].mUv01 = vals[ 2 ];
              }
            }
          }
        }

    void initIndices ( ::ase::node const* pSrc, reIndexer& ind )
        {
          // verts
          if ( (ind.mVind = pSrc->findNode ( "MESH_FACE_LIST" )) )
            initVertIndices ( ind.mVind, ind );
          // else holy shit
          
          // norms
          if ( (ind.mNind = pSrc->findNode ( "MESH_NORMALS" )) )
            initNormIndices ( ind.mNind, ind );
          
          // uv01
          if ( (ind.mUVind00 = pSrc->findNode ( "MESH_TFACELIST" )) )
            initUvIndices ( ind.mUVind00, ind, 0 );

          // uv02
          if ( ::ase::node const* pMapChan = pSrc->findNode ( "MESH_MAPPINGCHANNEL", false ) )
          {
            if ( (ind.mUVind01 = pMapChan->findNode ( "MESH_TFACELIST" )) )
              initUvIndices ( ind.mUVind01, ind, 1 );
          }
        }
        
    void processVerts ( geom* pGeom, reIndexer& ind, ::ase::node const* pSrc )
        {
          typedef std::vector< float > VertVals;
          geomData* pGdata = pGeom->geometryOp ();
          size_t stride = pGdata->getAttributes ().getValueStride ();
          // Offset for positions is always 0.
          const unsigned int TypeStride = 3;
          
          if ( ::ase::node const* pVerts = pSrc->findNode ( "MESH_VERTEX_LIST" ) )
          {
            unsigned int numVerts = pSrc->findLine ( "MESH_NUMVERTEX" )->getInt ();
            
            VertVals vertVals;
            vertVals.resize ( numVerts * TypeStride );

            // Copy all of the positions into vector of floats.
            size_t count = 0;
            ::ase::node::ConstLineIter end = pVerts->getLineEnd ();
            for ( ::ase::node::ConstLineIter cur = pVerts->getLineBegin ();
                cur != end;
                ++cur )
            {
              int vertInd = -1;
              ( *cur )->getIntFloat3 ( vertInd, &vertVals[ count * TypeStride ] );
              ++count;
            }
            
            // Copy out of the vector of floats into geomData.
            geomData::Values& values = pGdata->getValues ();
            for ( size_t num = 0; num < ind.mSize; ++num )
            {
              size_t dst = ind.mVerts[ num ].mInd  * stride;
              size_t src = ind.mVerts[ num ].mVert * TypeStride;
// cout << "dst, src " << dst << ", " << src << endl;
              values[ dst++ ] = vertVals[ src++ ];
              values[ dst++ ] = vertVals[ src++ ];
              values[ dst   ] = vertVals[ src   ];
            }
          }
        }

    void processNormals ( geom* pGeom, reIndexer& ind, ::ase::node const* pSrc )
        {
          typedef std::vector< float > VertVals;
          geomData* pGdata = pGeom->geometryOp ();
          geomData::SizeType stride = pGdata->getAttributes ().getValueStride ();
          geomData::SizeType offset = pGdata->getAttributes ().getValueOffset ( geomData::Normals );
          const unsigned int TypeStride = 3;
          
          if ( ::ase::node const* pVerts = pSrc->findNode ( "MESH_NORMALS" ) )
          {
            unsigned int numVerts = ind.mSize;

            VertVals vertVals;
            vertVals.resize ( numVerts * TypeStride );

            // Copy all of the positions into vector of floats.
            unsigned int count = 0;
            ::ase::node::ConstLineIter end = pVerts->getLineEnd ();
            for ( ::ase::node::ConstLineIter cur = pVerts->getLineBegin ();
                cur != end;
                ++cur )
            {
              if ( ( *cur )->getName () == "MESH_VERTEXNORMAL" )
              {
                int vertInd = -1;
                ( *cur )->getIntFloat3 ( vertInd, &vertVals[ count * TypeStride ] );

// unsigned int base = count * TypeStride;
// cout << " norm 1 = " << base << " : " << vertVals[ base ] << ", " << vertVals[ base + 1 ]
//     << ", " << vertVals[ base + 2 ] << endl;
                ++count;
              }
            }
            
            // Copy out of the vector of floats into geomData.
            geomData::Values& values = pGdata->getValues ();
            for ( geomData::SizeType num = 0; num < ind.mSize; ++num )
            {
              geomData::SizeType dst = ind.mVerts[ num ].mInd  * stride + offset;
              geomData::SizeType src = ind.mVerts[ num ].mNorm * TypeStride;
// cout << "num,  mNorm = " << num << ", " << ind.mVerts[ num ].mNorm << endl;
// cout << "dst, src " << dst << ", " << src << endl;
// cout << " norm 2 = " << vertVals[ src ] << ", " << vertVals[ src + 1 ]
//     << ", " << vertVals[ src + 2 ] << endl;
              values[ dst++ ] = vertVals[ src++ ];
              values[ dst++ ] = vertVals[ src++ ];
              values[ dst   ] = vertVals[ src   ];
            }
          }
        }

    void processUvs ( geom* pGeom, reIndexer& ind, ::ase::node const* pSrc, unsigned int unit = 0 )
        {
PNIDBG
          typedef std::vector< float > VertVals;
          geomData* pGdata = pGeom->geometryOp ();
          size_t stride = pGdata->getAttributes ().getValueStride ();
          geomData::AttributeType tcoord = ( unit ==  0 ) ? geomData::TCoords00 : geomData::TCoords01;
          size_t offset = pGdata->getAttributes ().getValueOffset ( tcoord );
         // Offset for positions is always 0.
          const unsigned int TypeStride = 2;

// cout << "uv val offset = " << offset << endl;
          
          if ( ::ase::node const* pVerts = pSrc->findNode ( "MESH_TVERTLIST" ) )
          {
            unsigned int numVerts = pSrc->findLine ( "MESH_NUMTVERTEX" )->getInt ();
            
            VertVals vertVals;
            vertVals.resize ( numVerts * TypeStride );

            // Copy all of the positions into vector of floats.
            unsigned int count = 0;
            ::ase::node::ConstLineIter end = pVerts->getLineEnd ();
            for ( ::ase::node::ConstLineIter cur = pVerts->getLineBegin ();
                cur != end;
                ++cur )
            {
              int vertInd = -1;
              ( *cur )->getIntFloat2 ( vertInd, &vertVals[ count * TypeStride ] );

// unsigned int base = count * TypeStride;
// cout << " uv 1 = " << base << " : " << vertVals[ base ] << ", " << vertVals[ base + 1 ] << endl;
              ++count;
            }
            
            // Copy out of the vector of floats into geomData.
            geomData::Values& values = pGdata->getValues ();
            for ( size_t num = 0; num < ind.mSize; ++num )
            {
              size_t dst = ind.mVerts[ num ].mInd * stride + offset;
              size_t src = unit == 0 ? 
                  ind.mVerts[ num ].mUv00 * TypeStride
                  : ind.mVerts[ num ].mUv01 * TypeStride;
// cout << "num,  mUv00 = " << num << ", " << ind.mVerts[ num ].mUv00 << endl;
// cout << "dst, src " << dst << ", " << src << endl;
// cout << " uv 2 = " << vertVals[ src ] << ", " << vertVals[ src + 1 ] << endl;
              values[ dst++ ] =        vertVals[ src++ ];
              values[ dst   ] = 1.0f - vertVals[ src   ]; // HACK!!! To flip images.
//               values[ dst   ] = vertVals[ src   ];
            }
          }
        }

    void processSubUvs ( geom* pGeom, reIndexer& ind, ::ase::node const* pSrc )
        {
PNIDBG
					::ase::node::ConstNodeIter end = pSrc->getChildEnd ();
					for ( ::ase::node::ConstNodeIter cur = pSrc->getChildBegin (); 
              cur != end;
              ++cur )
					{
						if ( ( *cur )->getName () == "MESH_MAPPINGCHANNEL" )
						{
							// We use the ASE channel number directly.  Alternatively we
							// could go first-come, first serve for unit allocation... but that
							// is a pain to coordinate with material process which is random access.
							unsigned int unit = ( *cur )->getInt () - 1;	// We are 0-based, they are 1.
              
              // TEMP Only supporting 2 texture units.
              const unsigned int MaxUnit = 1;
              if ( unit == MaxUnit )
                processUvs ( pGeom, ind, pSrc, unit );
              else
                mObserver->onError ( 
                    pni::pstd::error ( InternalErrorTexture,
                    "Texture unit out of range" ) );
						}
					}
        }
    
    void processIndices ( geom* pGeom, reIndexer& ind )
        {
          geomData* pGdata = pGeom->geometryOp ();
          
          geomData::Indices& indices  = pGdata->getIndices ();
          for ( size_t num = 0; num < ind.mSize; ++num )
            indices[ num ] = ind.mVerts[ num ].mInd;
        }
    
    void processObject ( node* pParent, ::ase::node const* pSrc )
        {
          geom* pGeom = new geom;
          pGeom->setName ( pSrc->findLine ( "NODE_NAME" )->getQuoted () );
          addNodeToDirectory ( pGeom );

          pni::pstd::progressEvent event ( &mAse );
          if ( mAse.getProgressObserver () )
            mAse.getProgressObserver ()->onProgressUpdate ( &mAse );
          mObserver->onPreNode ( pGeom );

          processXform ( pGeom, pSrc );
// PNIDBGSTR( pGeom->getName ().c_str () );
          

					// Now start processing the mesh.
					::ase::node const* pMesh = pSrc->findNode ( "MESH" );
					unsigned int numFaces = pMesh->findLine ( "MESH_NUMFACES" )->getInt ();
          unsigned int elements = numFaces * 3; // Size of index list.

          // Figure out geom data attributes and total size.
          geomData* pGdata = new geomData;
          pGeom->setGeomData ( pGdata );
          initAttributes ( pGeom, pMesh );

          reIndexer ind ( elements );
          initIndices ( pMesh, ind );
          ind.collapse ();
          pGdata->resizeTrisWithCurrentAttributes ( ind.mDataCount, numFaces );

// cout << "getValueStride  = " << pGdata->getValueStride () << endl;
          processVerts ( pGeom, ind, pMesh );
          processNormals ( pGeom, ind, pMesh );
          processUvs ( pGeom, ind, pMesh, 0 );
          processSubUvs ( pGeom, ind, pMesh );
          processIndices ( pGeom, ind );

          invertGeom ( pGeom );

// pGdata->dbg ();

          fixupXform ( pGeom );
          pParent->addChild ( pGeom );

          if ( ::ase::line const* pLine = pSrc->findLine ( "MATERIAL_REF", false ) )
          {
            processMaterial ( pGeom, pLine->getInt () );
          }
          else
          {
            // TODO: Really?  On the parent?
//               pParent->setState ( mLoadAttrs.mpCache->mpDefMaterial.get () );
          }
          
          mObserver->onPostNode ( pGeom );
        }
    
    
    void processXform ( node* pNode, ::ase::node const* pSrc )
        {
PNIDBG
					::ase::node const* pTm = pSrc->findNode ( "NODE_TM" );
PNIDBG
					pni::math::matrix4 mat;
					pni::math::vec4 row ( 0.0f, 0.0f, 0.0f, 0.0f );
PNIDBG
					pTm->findLine ( "TM_ROW0" )->getFloat3 ( row );
					mat.setRow ( 0, row );
					pTm->findLine ( "TM_ROW1" )->getFloat3 ( row );
					mat.setRow ( 1, row );
					pTm->findLine ( "TM_ROW2" )->getFloat3 ( row );
					mat.setRow ( 2, row );
					pTm->findLine ( "TM_ROW3" )->getFloat3 ( row );
					row[ 3 ] = 1.0f;
					mat.setRow ( 3, row );
PNIDBG
          // Let's try and catch identity matrices here during the load
          // and not pay for them every frame.
          pni::math::matrix4 ident;
          if ( ident.equal ( mat ) )
            pNode->getMatrix ().setIdentity ();
          else
  					pNode->matrixOp () = mat;
PNIDBG
        }
        
    void processHelper ( group* pGrp, ::ase::node const* pSrc )
        {
PNIDBG
// 					processXform ( pGrp, pSrc );

					// Ignoring other helper fields for now.
        }
        
    void processGroup ( node* pParent, ::ase::node const* pSrc )
        {
PNIDBG
					group* pGrp = new group;
					pGrp->setName ( pSrc->getQuoted () );
          addNodeToDirectory ( pGrp );
          mObserver->onPreNode ( pGrp );

					// Sets xform.				
					processHelper ( pGrp, pSrc->findNode ( "HELPEROBJECT", false ) );
					
					// Store full xform.
					pni::math::matrix4 pmat = pGrp->getMatrix ();

					// Fixup the xform.
					fixupXform ( pGrp );
					
					// Push the original full xform on the stack.
					mMatStack.push ();
					*mMatStack = pmat;
					
					pParent->addChild ( pGrp );
					
					processChildren ( pGrp, pSrc );
					
					mMatStack.pop ();        
        
          mObserver->onPostNode ( pGrp );
        }
    
    void processLight ( node* pParent, ::ase::node const* pSrc )
        {
PNIDBG
          // TODO
        }
    
    void processChildren ( node* pParent, ::ase::node const* pSrcParent )
        {
					if ( ! pSrcParent )
						return;
						
					::ase::node::ConstNodeIter end = pSrcParent->getChildEnd ();
					for ( ::ase::node::ConstNodeIter cur = pSrcParent->getChildBegin ();
							cur != end; ++cur )
					{
						processChild ( pParent, *cur );
					}        
        }
        
    void processChild ( node* pParent, ::ase::node const* pSrc )
        {
PNIDBG
          Strc& srcName = pSrc->getName ();
          
					if ( srcName == "GEOMOBJECT" )
					{
						processObject ( pParent, pSrc );
					}
					else if ( srcName == "GROUP" )
					{
						processGroup ( pParent, pSrc );
					}
// 					else if ( srcName == "HELPEROBJECT" )
// 					{
// 						// Nada... handled in processGroup.
// 					}
					else if ( srcName == "LIGHTOBJECT" )
					{
						processLight ( pParent, pSrc );
					}
					else if ( srcName == "3DSMAX_ASCIIEXPORT" )
					{
						// Root is basic... just do it here.
						pParent->setName ( pSrc->findLine ( "COMMENT" )->getQuoted () );
						
						processChildren ( pParent, pSrc );
					}
					else if ( srcName == "MATERIAL_LIST" )
					{
						processChildren ( pParent, pSrc );
					}
					else if ( srcName == "SCENE" )
					{
						// Nada for now.
					}
					else if ( srcName == "MATERIAL" )
					{
						handleMaterial ( pSrc );
					}
          
					else
					{
						// Huh... unknown... what are we gonna do with this?
						std::string err ( "Unknown ASE node type: " );
						err += pSrc->getName ();
						mObserver->onError ( pni::pstd::error ( loader::InternalErrorHierarchy, err ) );						
					}
        }
        
};
  
/////////////////////////////////////////////////////////////////////

ase::ase()
{
  
}

ase::~ase()
{
  
}

// ase::ase(ase const& rhs)
// {
//   
// }
// 
// ase& ase::operator=(ase const& rhs)
// {
//   
//   return *this;
// }
// 
// bool ase::operator==(ase const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from base

scene::node* ase::load ( std::string const& fname )
{

PSTDPROFCALL(static pstd::timerProf tp ( "ase::load - all", 1 ) );
PSTDPROFCALL(tp.start () );

  ::ase::parser parser;

PSTDPROFCALL(static pstd::timerProf tpParse ( "ase::load - parse", 1 ) );
PSTDPROFCALL(tpParse.start () );

  ::ase::node* pParseRoot = parser.parse ( fname );
  
  if ( pParseRoot )
  {
    helper aHelper ( *this, pParseRoot );

PSTDPROFCALL(tpParse.stop () );

    pni::pstd::progressEvent event ( this );
    emitProgressStart ( event );
    
    group* pRoot = new group;
    aHelper.addNodeToDirectory ( pRoot );
    
    mObserver->onPreNode ( pRoot );
    
    aHelper.processChild ( pRoot, aHelper.mRoot );
    
    mObserver->onPostNode ( pRoot );

    pRoot->setName ( fname );

    emitProgressEnd ( event );

PSTDPROFCALL(tp.stop () );

    return pRoot;
  }
  else
  {
    std::string err ( "Could not parse file: " + fname );
    mObserver->onError ( pni::pstd::error ( loader::ParseFailed, err ) );
    
    return nullptr;
  }
}

/////////////////////////////////////////////////////////////////////


} // end of namespace loader 


