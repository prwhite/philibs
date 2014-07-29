/////////////////////////////////////////////////////////////////////
//
//    file: sceneloaderase.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PSTDPROFDISABLE

#include <cstdint>

#include "sceneloaderassimp.h"

#include "scenegroup.h"
#include "scenegeom.h"
#include "scenecommon.h"

#include "pnisearchpath.h"
#include "imgfactory.h"

#include "assimp/Importer.hpp"
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "assimp/Logger.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/cimport.h"

/////////////////////////////////////////////////////////////////////

using namespace scene;

namespace {

class helper
{
  public:

    typedef img::factory::LoadFuture ImgFuture;

    struct ImgFutureTuple
    {
      ImgFuture imgFuture;
      scene::node* mNode;
      scene::texture* mTex;
    };

    typedef std::vector< ImgFutureTuple* > ImgFutures;

    helper ( loader::assimp* pLoader )
      : mLoader ( pLoader )
        {}

    bool checkFile ( std::string const& fname )
      {
        return pni::pstd::searchPath::doStat(fname);
      }

    node* load ( std::string const& fname )
      {
        mFname = fname;
      
        if ( checkFile(fname))
        {
          group* pRoot = new group;

          Assimp::Logger* pLog = Assimp::DefaultLogger::create(
              ASSIMP_DEFAULT_LOG_NAME, Assimp::Logger::NORMAL, aiDefaultLogStream_STDOUT);

          size_t const MaxVerts = 0x7fff; // 32k
          importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MaxVerts);
//          importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, MaxVerts / 3);  // No point
          importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

          aiScene const* mScene = importer.ReadFile(fname.c_str(),
//              aiProcessPreset_TargetRealtime_Quality |
              aiProcess_JoinIdenticalVertices |
              aiProcess_ImproveCacheLocality |
              aiProcess_SplitLargeMeshes |
              aiProcess_Triangulate |
              aiProcess_GenSmoothNormals |
              aiProcess_ValidateDataStructure |
              aiProcess_GenUVCoords |
              aiProcess_SortByPType |
              aiProcess_FlipUVs);

          pRoot->setName(fname);

          processScene(mScene, pRoot);
          
          finishTextureFutures();

          delete pLog;

          return pRoot;
        }
        else
        {
          PNIDBGSTR("Could not load file " + fname);
          return 0;
        }
      }

      void processNode ( aiNode const* pSrc, node* pDstParent )
        {
            // Figure out what kind of node to instantiate
          node* pNode = new group;
          pDstParent->addChild(pNode);

          pNode->setName(pSrc->mName.C_Str());
          processXform (pSrc, pNode);
          processMeshes(pSrc, pNode);
          processChildren(pSrc, pNode);
        }

      void processChildren ( aiNode const* pSrc, node* pDstParent )
        {
          for ( size_t num = 0; num < pSrc->mNumChildren; ++num )
          {
            aiNode const* pChild = pSrc->mChildren[ num ];

            processNode(pChild, pDstParent);
          }
        }

      void processScene ( aiScene const* pSrc, node* pDst )
        {
          mScene = pSrc;

            // TODO: Do scene-specific stuff

            // Now process root node.
          processNode ( pSrc->mRootNode, pDst );
        }

      void processXform ( aiNode const* pSrc, node* pDst )
        {
          if( ! pSrc->mTransformation.IsIdentity())
          {
            aiMatrix4x4 const& srcMat = pSrc->mTransformation;
            matrix4& dstMat = pDst->matrixOp();

              // ASSIMP is row major, PHI and OpenGL are column major, so
              // we set what is a transpose...
            dstMat.set(
                srcMat.a1, srcMat.b1, srcMat.c1, srcMat.d1,
                srcMat.a2, srcMat.b2, srcMat.c2, srcMat.d2,
                srcMat.a3, srcMat.b3, srcMat.c3, srcMat.d3,
                srcMat.a4, srcMat.b4, srcMat.c4, srcMat.d4 );
          }
        }

      void processMeshes ( aiNode const* pSrc, node* pDst )
        {
          for ( size_t num = 0; num < pSrc->mNumMeshes; ++num )
          {
            aiMesh const* pSrcMesh = mScene->mMeshes[ pSrc->mMeshes[ num ] ];

              // Only strictly want triangles right now!!!
              // TODO: Support lines and points?  Probably not.
            if ( pSrcMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE )
            {

              geom* pGeom = new geom;
              pDst->addChild(pGeom);

              pGeom->setName(pSrcMesh->mName.C_Str());

                // OPTIMIZATION: We could share meshes at this stage if we built
                // a cache that matches the aiScenes mesh list.  Probably not a
                // big win, considering the state of incoming data.
              processMesh ( pSrcMesh, pGeom );
            }
            else
              PNIDBGSTR("received mesh with non-triangle prim type, skipping");
          }
        }

      void setupGeomDataAttributes ( aiMesh const* pSrc, geomData* pData )
        {
            // First get the attribute bindings right, so we can properly allocate.
          geomData::Binding& bindings = pData->mBinding;

          bindings.push_back ( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, sizeof(float), geomData::PositionComponents } );

          if ( pSrc->mNormals )
            bindings.push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, sizeof ( float ), geomData::NormalComponents } );

            // TEMP: We only take the first color channel currently
          if ( pSrc->mColors[ 0 ] )
            bindings.push_back ( { CommonAttributeNames[ geomData::Color ], geomData::Color, geomData::DataType_FLOAT, sizeof ( float ), geomData::ColorComponents } );

          static_assert ( AI_MAX_NUMBER_OF_TEXTURECOORDS <= geomData::NumTexUnits, "Number of texcoords supported by ASSIMP changed!" );

          for ( size_t num = 0; num < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++num )
          {
              // There is some funny biz with offsetting from TCoord00 as we walk through the array... and
              // yet we use geomData::TCoord00Components throughout, because the number of UV components is always the same.
            if ( pSrc->mTextureCoords[ num ] )
              bindings.push_back ( { CommonAttributeNames[ geomData::TCoord ], geomData::TCoord, geomData::DataType_FLOAT, sizeof ( float ), geomData::TCoordComponents, num } );
          }
        }

      void copySrcDst ( size_t count, size_t components, float const* src, size_t srcStride, float* dst, size_t dstStride )
        {
          if ( ! src )
            return;   // Early return / short circuit... we don't mind if someone
                      // sends in an attribute that isn't set... it's cleaner to
                      // deal with that here.

          float const* srcEnd = src + srcStride * count;
          float* dstEnd = dst + dstStride * count;

          for ( size_t num = 0; num < count; ++num )
          {
            float const* srcp = src;
            float* dstp = dst;

            for ( size_t comp = 0; comp < components; ++comp )
              *dstp++ = *srcp++;

            src += srcStride;
            dst += dstStride;
          }

          assert ( src = srcEnd );
          assert ( dst = dstEnd );
        }

      void processMesh ( aiMesh const* pSrc, geom* pDst )
        {
          if ( ! pSrc->mVertices )
          {
            PNIDBGSTR("received mesh with no vert positions!!! " + pSrc->mName );
            return;  // Early return / short curcuit
          }

          geomData* pData = new geomData;
          pDst->setGeomData( pData );

          setupGeomDataAttributes(pSrc, pData);

            // Allocate the actual vertex and index buffers
          pData->resizeTrisWithCurrentBinding(pSrc->mNumVertices, pSrc->mNumFaces);

            // Fill in vertex attribute array values
          size_t const dstStrideBytes = pData->mBinding.getValueStrideBytes();
          size_t const dstStride = dstStrideBytes / sizeof(float);

            // In all cases, were casting AI vector types to float*... luckily they
            // are packed PODs with only floats, so this _should_ work.
            // Let's freak out statically if there is a problem.
          static_assert ( sizeof ( aiVector3D ) == sizeof ( float ) * geomData::PositionComponents,
              "aiVector3D size is not compatible with 3 floats" );
          static_assert ( sizeof ( aiColor4D ) == sizeof ( float ) * geomData::ColorComponents,
              "aiVector3D size is not compatible with 4 floats" );

          if ( pSrc->mVertices )
            copySrcDst(pSrc->mNumVertices, geomData::PositionComponents,
                ( float* ) pSrc->mVertices, geomData::PositionComponents,
                pData->getElementPtr<float>(0, geomData::Position), dstStride);

          if ( pSrc->mNormals )
            copySrcDst(pSrc->mNumVertices, geomData::NormalComponents,
                ( float* ) pSrc->mNormals, geomData::NormalComponents,
                pData->getElementPtr<float>(0, geomData::Normal), dstStride);

          if ( pSrc->mColors[ 0 ] )
            copySrcDst(pSrc->mNumVertices, geomData::ColorComponents,
                ( float* ) pSrc->mColors[ 0 ], geomData::ColorComponents,
                pData->getElementPtr<float>(0,geomData::Color), dstStride);

          for ( size_t num = 0; num < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++num )
          {
            if ( pSrc->mTextureCoords[ num ] )
              copySrcDst(pSrc->mNumVertices, geomData::TCoordComponents,
                  ( float* ) pSrc->mTextureCoords[ num ], 3,
                  pData->getElementPtr<float>( 0, geomData::TCoord, num ), dstStride);
          }

            // Copy out indices
            // We're going to be really paranoid about buffer overruns and correctness...
            // the various tests are pretty cheap and are only one-time during load.
          geomData::Indices& dstIndices = pData->getIndices();
          size_t numFaces = pSrc->mNumFaces;
          size_t maxVert = pData->size();
          size_t const NumVertsPerTri = 3;
          size_t const NumIndices = numFaces * NumVertsPerTri;
          size_t const MaxIndexNum = UINT16_MAX;
          
          static_assert (sizeof(geomData::IndexType) == sizeof(uint16_t), "geomData::IndexType should be 2 bytes, but isn't!" ); // Make sure we're only using shorts.

          assert(dstIndices.size() == NumIndices);
          assert(maxVert < MaxIndexNum); // Make sure we don't overflow uint16_t

//          dstIndices.resize(numFaces*3);
          size_t dstInd = 0;
          for ( size_t num = 0; num < numFaces; ++num )
          {
            aiFace const& face = pSrc->mFaces[ num ];

            for ( size_t srcInd = 0; srcInd < face.mNumIndices; ++srcInd )
            {
              dstIndices[ dstInd++ ] = face.mIndices[ srcInd ];
              assert(face.mIndices[ srcInd ] < maxVert);
            }
          }

          assert(dstInd == NumIndices);
          assert(dstInd == dstIndices.size());
          
          processMaterials(pSrc, pDst);
        }

      void processMaterials ( aiMesh const* pSrc, geom* pDst )
        {
          if ( pSrc->mMaterialIndex != 0xffffffff )
          {
            if ( aiMaterial const* aiMat = mScene->mMaterials[ pSrc->mMaterialIndex ] )
            {
              processTextures( aiMat, pDst );
            }
          }
        }

    bool resolveImageName( std::string const& src, std::string& dst )
        {
PNIDBG
            // Temporarily add the ase file path head to the search paths,
            // making relative texture file names work.
          std::string tmpPath = pni::pstd::searchPath::head(mFname);
          pni::pstd::searchPath tmpSearch ( *mLoader->getSearchPath () );
          tmpSearch.addPath(tmpPath);

          bool ret = tmpSearch.resolve ( src, dst );

          return ret;
        }

      void processTextures ( aiMaterial const* pSrc, node* pDst )
        {
            // TODO: Support the rest of these types... or more!!!
          aiTextureType typesWeCareAbout[] = {
              aiTextureType_DIFFUSE,
              aiTextureType_AMBIENT,
              aiTextureType_EMISSIVE,
              aiTextureType_LIGHTMAP,
              aiTextureType_NORMALS,
              aiTextureType_SPECULAR,
              aiTextureType_SHININESS
            };
        
          size_t const NumTypes = sizeof(typesWeCareAbout) / sizeof(aiTextureType);
        
          for ( size_t type = 0; type < NumTypes; ++type )
          {
            size_t const NumTexturesForType = pSrc->GetTextureCount(typesWeCareAbout[type]);
            for ( size_t num = 0; num < NumTexturesForType; ++num)
            {
              processTexture(pSrc, typesWeCareAbout[type], num, pDst);
            }
          }
        }
  
      texture::Wrap aiMappingModeToTexWrap ( aiTextureMapMode mode )
        {
          switch ( mode )
          {
            case aiTextureMapMode_Mirror:
              return texture::RepeatMirrored;
            case aiTextureMapMode_Clamp:
              return texture::ClampToEdge;
            case aiTextureMapMode_Wrap:
            default:
              return texture::Repeat;
          }
        }
  
      texture::Semantic aiTypeToTexSemantic ( aiTextureType type )
        {
          switch ( type )
          {
            case aiTextureType_DIFFUSE : return texture::Diffuse;
            case aiTextureType_AMBIENT : return texture::Ambient;
            case aiTextureType_SPECULAR : return texture::Specular;
            case aiTextureType_EMISSIVE : return texture::Emissive;
            case aiTextureType_HEIGHT : return texture::Height;
            case aiTextureType_NORMALS : return texture::Normals;
            case aiTextureType_SHININESS : return texture::Shininess;
            case aiTextureType_OPACITY : return texture::Opacity;
            case aiTextureType_DISPLACEMENT : return texture::Displacement;
            case aiTextureType_LIGHTMAP : return texture::LightMap;
            case aiTextureType_REFLECTION : return texture::Reflection;
            default:
              return texture::Unknown;
          }
        }

      void processTexture ( aiMaterial const* pSrc, aiTextureType type, size_t num, node* pDst )
        {
          aiString texPath;
          aiTextureMapping texMapping;
          unsigned int texUnit;
          float texBlend;
          aiTextureOp texOp;
          aiTextureMapMode texMapMode;
          
          if ( pSrc->GetTexture(
              type,
              ( unsigned int ) num,
              &texPath,
              &texMapping,
              &texUnit,
              &texBlend,    // Currently ignored
              &texOp,       // Currently ignored, always modulate in simple lighting model
              &texMapMode) == aiReturn_SUCCESS )
          {
            texture* pTex = 0;
          
            std::string fname = texPath.C_Str();
            std::string path;
            
            loader::cache* pCache = mLoader->getCache();
            auto found = pCache->mTextures.find(fname);
            if ( found != pCache->mTextures.end () )  // in cache
            {
              pTex = found->second.get();
            }
            else if(resolveImageName(fname, path))                            // build new texture
            {
              pTex = new texture;
              pTex->setName(fname);

                // Currently, the default shader only knows how to deal with diffuse
                // textures, so all other types are loaded, but disabled.
                // TODO: need to add loader properties that will direct how things
                // like this are handled when we have a richer
              if ( type != aiTextureType_DIFFUSE )
                pTex->setEnable(false);
              
              if ( texMapping != aiTextureMapping_UV)
              {
                pTex->setEnable(false);
                PNIDBGSTR("Invalid texture mapping. Only aiTextureMapping_UV is currently supported");
              }
              
                // ASSIMP doesn't separate s and t wrap
              texture::Wrap wrap = aiMappingModeToTexWrap(texMapMode);
              pTex->setWrap( wrap );
              
              pTex->setSemantic(aiTypeToTexSemantic(type));
              
                // Do async load
              ImgFutureTuple* tuple = new ImgFutureTuple;
              tuple->imgFuture = img::factory::getInstance().loadAsync(path);
              tuple->mTex = pTex;
              tuple->mNode = pDst;
              mImgFutures.push_back(tuple);
              
              pCache->mTextures[ fname ] = pTex;
            }
            else
            {
              PNIDBGSTR("Could not resolve image file path for texture");
              PNIDBGSTR(fname);
            }

              // Assign to right texture unit.
            if ( pTex )
              pDst->setState(pTex, static_cast< state::Id > (state::Texture00 + texUnit));
          }
          else
          {
            PNIDBGSTR("ASSIMP GetTexture failed for some reason, we get no tex params");
          }
        }

        void finishTextureFutures ()
        {
          for ( auto tuple : mImgFutures )
          {
            img::base* pImg = tuple->imgFuture.get();    // Will wait indefinitely
            finishTexture(tuple->mTex, tuple->mNode, pImg);
            delete tuple;
          }

          mImgFutures.clear ();
        }

        void finishTexture ( scene::texture* pTex, scene::node* pNode, img::base* pImg )
        {
          pTex->setImage ( pImg );

          if ( pImg->mBuffers.size () == 1 )
            pTex->setMinFilter ( texture::MinLinearMipNearest );

          if ( pImg && pImg->hasAlpha () )
          {
            //printf ( "  %s has alpha\n", pImg->getName ().c_str () );
//            rMap[ state::Blend ] = mCache->mDefBlend.get ();
            pNode->setState(mLoader->getCache()->mDefBlend.get(), state::Blend);
          }
          else
          {
            //printf ( "  %s has no alpha\n", pImg->getName ().c_str () );
          }
        }



  private:
    loader::assimp* mLoader;
    Assimp::Importer importer;
    aiScene const* mScene = 0;
    ImgFutures mImgFutures;
    std::string mFname;
};


}

namespace loader {

/////////////////////////////////////////////////////////////////////

scene::node* assimp::load ( std::string const& fname )
{
  helper hlpr ( this );

  return hlpr.load (fname);
}

/////////////////////////////////////////////////////////////////////

} // end of namespace loader 


