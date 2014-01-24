/////////////////////////////////////////////////////////////////////
//
//    file: sceneloaderase.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PSTDPROFDISABLE

#include "sceneloaderassimp.h"

#include "scenegroup.h"
#include "scenegeom.h"
#include "scenecommon.h"

#include "pnisearchpath.h"

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

    bool checkFile ( std::string const& fname )
      {
        return pni::pstd::searchPath::doStat(fname);
      }

    node* load ( std::string const& fname )
      {
        if ( checkFile(fname))
        {
          group* pRoot = new group;

          Assimp::Logger* pLog = Assimp::DefaultLogger::create(
              ASSIMP_DEFAULT_LOG_NAME, Assimp::Logger::NORMAL, aiDefaultLogStream_STDOUT);

          size_t const MaxVerts = 0x7fff; // 32k
          importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, MaxVerts);
//          importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, MaxVerts / 3);  // No point

          aiScene const* mScene = importer.ReadFile(fname.c_str(),
//              aiProcessPreset_TargetRealtime_Quality |
              aiProcess_JoinIdenticalVertices |
              aiProcess_ImproveCacheLocality |
              aiProcess_SplitLargeMeshes |
              aiProcess_Triangulate |
              aiProcess_GenSmoothNormals |
              aiProcess_ValidateDataStructure |
              aiProcess_GenUVCoords);

          pRoot->setName(fname);

          processScene(mScene, pRoot);

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
          processMaterials(pSrc, pNode);
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
          }
        }

      void setupGeomDataAttributes ( aiMesh const* pSrc, geomData* pData )
        {
            // First get the attribute bindings right, so we can properly allocate.
          geomData::attributes& attrs = pData->attributesOp();

          attrs.push_back ( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );

          if ( pSrc->mNormals )
            attrs.push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );

            // TEMP: We only take the first color channel currently
          if ( pSrc->mColors[ 0 ] )
            attrs.push_back ( { CommonAttributeNames[ geomData::Color ], geomData::Color, geomData::DataType_FLOAT, geomData::ColorComponents } );

          static_assert ( AI_MAX_NUMBER_OF_TEXTURECOORDS <= geomData::NumTexUnits, "Number of texcoords supported by ASSIMP changed!" );

          for ( size_t num = 0; num < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++num )
          {
              // There is some funny biz with offsetting from TCoord00 as we walk through the array... and
              // yet we use geomData::TCoord00Components throughout, because the number of UV components is always the same.
            if ( pSrc->mTextureCoords[ num ] )
              attrs.push_back ( { CommonAttributeNames[ geomData::TCoord00 + num ], ( geomData::AttributeType ) ( geomData::TCoord00 + num ), geomData::DataType_FLOAT, geomData::TCoord00Components } );
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
          pData->resizeTrisWithCurrentAttributes(pSrc->mNumVertices, pSrc->mNumFaces);

            // Fill in vertex attribute array values
          size_t dstStride = pData->getAttributes().getValueStride();

            // In all cases, were casting AI vector types to float*... luckily they
            // are packed PODs with only floats, so this _should_ work.
            // Let's freak out statically if there is a problem.
          static_assert ( sizeof ( aiVector3D ) == sizeof ( geomData::ValueType ) * geomData::PositionComponents,
              "aiVector3D size is not compatible with 3 floats" );
          static_assert ( sizeof ( aiColor4D ) == sizeof ( geomData::ValueType ) * geomData::ColorComponents,
              "aiVector3D size is not compatible with 4 floats" );

          if ( pSrc->mVertices )
            copySrcDst(pSrc->mNumVertices, geomData::PositionComponents,
                ( geomData::ValueType* ) pSrc->mVertices, geomData::PositionComponents,
                pData->getAttributePtr(geomData::Position), dstStride);

          if ( pSrc->mNormals )
            copySrcDst(pSrc->mNumVertices, geomData::NormalComponents,
                ( geomData::ValueType* ) pSrc->mNormals, geomData::NormalComponents,
                pData->getAttributePtr(geomData::Normal), dstStride);

          if ( pSrc->mColors[ 0 ] )
            copySrcDst(pSrc->mNumVertices, geomData::ColorComponents,
                ( geomData::ValueType* ) pSrc->mColors[ 0 ], geomData::ColorComponents,
                pData->getAttributePtr(geomData::Color), dstStride);

          for ( size_t num = 0; num < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++num )
          {
            if ( pSrc->mTextureCoords[ num ] )
              copySrcDst(pSrc->mNumVertices, geomData::TCoord00Components,
                  ( geomData::ValueType* ) pSrc->mTextureCoords[ num ], 3,
                  pData->getAttributePtr( ( geomData::AttributeType ) ( geomData::TCoord00 + num ) ), dstStride);
          }

            // Copy out indices
          geomData::Indices& dstIndices = pData->getIndices();
          for ( size_t num = 0, dstInd = 0; num < pSrc->mNumFaces; ++num )
          {
            aiFace const& face = pSrc->mFaces[ num ];

            for ( size_t srcInd = 0; srcInd < face.mNumIndices; ++srcInd )
              dstIndices[ dstInd++ ] = face.mIndices[ srcInd ];
          }
        }

      void processMaterials ( aiNode const* pSrc, node* pDst )
        {

        }

      void processTexture ( aiNode const* pSrc, node* pDst )
        {

        }

  private:
    Assimp::Importer importer;
    aiScene const* mScene = 0;
};


}

namespace loader {

/////////////////////////////////////////////////////////////////////

scene::node* assimp::load ( std::string const& fname )
{
  helper hlpr;

  return hlpr.load (fname);
}

/////////////////////////////////////////////////////////////////////


} // end of namespace loader 


