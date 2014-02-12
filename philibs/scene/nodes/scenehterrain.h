/////////////////////////////////////////////////////////////////////
//
//  class: hterrain
//
/////////////////////////////////////////////////////////////////////

#ifndef scenehterrain_h
#define scenehterrain_h

/////////////////////////////////////////////////////////////////////

#include "scenegeom.h"

#include "pniautoref.h"

#include "pnivec2.h"
#include "pnivec3.h"
#include "pnimatrix4.h"

#include "imgbase.h"

#include "scenetexture.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class hterrain :
  public scene::geom
{
  public:
    typedef pni::pstd::autoRef< img::base > ImgRef;
    typedef img::base::Dim Dim;

    hterrain ();
    virtual ~hterrain ();
    //hterrain ( hterrain const& rhs );
    //hterrain& operator= ( hterrain const& rhs );
    //bool operator== ( hterrain const& rhs ) const;

    void initHeightScale ( float val ) { mHeightScale = val; }
    float getHeightScale () const { return mHeightScale; }

      // Call before initDiffImg if you want to use a pre-defined img.
    void initFxImg ( img::base* pImg );
    img::base* getFxImg () const { return mFxImg.get (); }

      // Must be a power of 2 in both dimensions.
    void initDiffImg ( img::base* pImg );
    img::base* getDiffImg () const { return mDiffImg.get (); }
    
      // Does not need to be a power of 2, etc.  Size is totally arbitrary
      // and will directly determine the hterrain tessellation.
    void initHeightImg ( img::base* pImg );
    img::base* getHeightImg () const { return mHeightImg.get (); }


    class sqrtLut
        {
            enum Vals { Size = 1024 };
            
            typedef std::vector< float > VecType;
            VecType mVec;
            float mMod;

          public:
            
            sqrtLut ( float maxSqr = ( float ) Size, bool recip = false )
                {
                  mMod = ( float ) Size / maxSqr;
                
                  for ( size_t val = 0; val < Size; ++val )
                  {
                    mVec.push_back ( pni::math::Trait::sqrt ( ( float ) val / mMod ) );
                    if ( recip )
                      mVec.back () = 1.0f / mVec.back ();
                  }
                }
                
              // Result of floor ( in ) lookup.
            float operator[] ( float in )
                {
                    // 0.5f for rounding behavior.
                  size_t val = ( size_t ) ( in * mMod + 0.5f );
                  if ( val >= mVec.size () )
                    return mVec.back ();
                  else
                    return mVec[ val ];
                }
                
              // TODO: Blended result.
            float operator () ( float in )
                {
                  // TODO
                  return 0.0f;
                }
        };

  protected:
    typedef pni::pstd::autoRef< scene::texture > TexRef;

    ImgRef mDiffImg;
    ImgRef mFxImg;
    ImgRef mHeightImg;
    
    TexRef mDiffTex;
    TexRef mFxTex;
    
    Dim mDiffSize[ 3 ];
    Dim mHeightSize[ 3 ]; 
    float mDz;
    float mHeightScale;
    
    pni::math::matrix4 mInvMatrix;
    
    sqrtLut mSqrt64;

    bool isPointInImg ( img::base* pImg, int xPos, int yPos );
    void clipPointToImg ( img::base* pImg, int& xPos, int& yPos );
    void rebuildGeom ( Dim xs, Dim ys, Dim xe, Dim ye );
    float calcHeightDz ();
    geomData::SizeType xyToGeomValueIndex ( Dim xVal, Dim yVal );

    struct genHeightStack
    {
      size_t mSize;
      size_t mGenStart;
      float mHeight;
      pni::math::vec2 mPos;
    };

    void convertToHeightFrame ( pni::math::vec3& where, float& radius );
    void convertToDiffuseFrame ( pni::math::vec3& where, float& radius );
    
  private:


    // interface from scene::geom
  public:

  protected:
    virtual void generateGeomBounds () const;
    virtual void generateGeomPartition () const;

		virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenehterrain_h


