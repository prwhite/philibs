/////////////////////////////////////////////////////////////////////
//
//  class: algo
//
/////////////////////////////////////////////////////////////////////

#ifndef imgimgalgo_h
#define imgimgalgo_h

/////////////////////////////////////////////////////////////////////

#include "imgbase.h"

#include "pnimath.h"
#include "pnivec2.h"
#include "pnivec4.h"
#include "pnirand.h"

#include <map>

#pragma warning ( push )
#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4018 )

/////////////////////////////////////////////////////////////////////

namespace img {
    
/////////////////////////////////////////////////////////////////////

class algoBase
{
  public:
    algoBase () :
      mMode ( Set )
        {}
        
    typedef pni::pstd::autoRef< base > ImgRef;
    typedef pni::pstd::autoRef< base const > ImgConstRef;
  
    typedef base::Dim Dim;
    
    enum Mode { Set, Sub, Add, Blend };
 
    virtual ~algoBase () {}
    
    void setMode ( Mode mode ) { mMode = mode; }
    Mode getMode () const { return mMode; }
  
  protected:
    Mode mMode;

    virtual void applyPixel () {}
    
    base::BufferType clampToUint8 ( float val )
        {
          if ( val > 255.0f )
            return 0xff;
          else if ( val < 0.0f )
            return 0x00;
          else
            return ( base::BufferType ) val;
        }

    base::BufferType clampToUint8 ( int val )
        {
          if ( val > 255 )
            return 0xff;
          else if ( val < 0 )
            return 0x00;
          else
            return ( base::BufferType ) val;
        }
    
    template< class Type >
    Type maxVal ( Type lhs, Type rhs )
        { return lhs > rhs ? lhs : rhs; }

    template< class Type >
    Type minVal ( Type lhs, Type rhs )
        { return lhs < rhs ? lhs : rhs; }
    
    void rgbToHsv ( pni::math::vec4& color )
        {
          float cmax = maxVal ( color[ 0 ], maxVal ( color[ 1 ],  color[ 2 ] ) );
          float cmin = minVal ( color[ 0 ], minVal ( color[ 1 ],  color[ 2 ] ) );
          float diff = cmax - cmin;
          
          float hval; // Uninit, but if/else will catch all posibilities.
          
            // Calc h
          if ( cmax == cmin )
            hval = 0.0f;
          else if ( cmax == color[ 0 ] )
            hval = fmodf ( 60.0f * ( color[ 1 ] - color[ 2 ] ) / diff + 0.0f, 360.0f );
          else if ( cmax == color[ 1 ] )
            hval = 60.0f * ( color[ 2 ] - color[ 0 ] ) / diff + 120.0f;
          else
            hval = 60.0f * ( color[ 0 ] - color[ 1 ] ) / diff + 240.0f;
            
            // Calc s
          float sval; // Uninit, but if/else will catch all possibilities.
          
          if ( cmax == 0.0f )
            sval = 0.0f;
          else
            sval = diff / cmax;
          
          color.set ( hval, sval, cmax, color[ 3 ] );
        }
        
    void hsvToRgb ( pni::math::vec4& color )
        {
          float hdiv60 = color[ 0 ] / 60.0f;
          float hprime = fmodf ( hdiv60, 6.0f );
          float fval = hdiv60 - fabsf ( hdiv60 );
          float pval = color[ 2 ] * ( 1.0f - color[ 1 ] );
          float qval = color[ 2 ] * ( 1.0f - fval * color[ 1 ] );
          float tval = color[ 2 ] * ( 1.0f - ( 1.0f - fval ) * color[ 1 ] );
          
          int hprimei = ( int ) hprime;
          
          switch ( hprimei )
          {
            case 0: color.set ( color[ 2 ], tval, pval, color[ 3 ] ); break;
            case 1: color.set ( qval, color[ 2 ], pval, color[ 3 ] ); break;
            case 2: color.set ( pval, color[ 2 ], tval, color[ 3 ] ); break;
            case 3: color.set ( pval, qval, color[ 2 ], color[ 3 ] ); break;
            case 4: color.set ( tval, pval, color[ 2 ], color[ 3 ] ); break;
            case 5: color.set ( color[ 2 ], pval, qval, color[ 3 ] ); break;
          }
        }
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class srcDstIter :
  public algoBase
{
  protected:
    base* mDst;
    base const* mSrc;

    Dim sSize[ 2 ];
    Dim sPitch;
    size_t sStride;

    base::BufferType const* sPtr;

    base::BufferType* dPtr;
    
    int dCur[ 2 ];
    
  public:
    srcDstIter ()
        {
        
        }

      // TODO: Set/get iter start/end positions for src and dst.

    void setSrc ( base const* pSrc ) 
        { 
          mSrc = pSrc;
          mSrc->getSize ( sSize[ 0 ], sSize[ 1 ], sPitch );
          sStride = mSrc->calcFormatStride ( mSrc->getFormat () );
        }
    base const* getSrc () const { return mSrc; }
    
    void setDst ( base* pDst ) { mDst = pDst; }
    base* getDst () const { return mDst; }

    void apply ()
        {
            // TODO: Assert src and dst are identical in format and size.
            // Alternatively, always reformat dst to make sure it matches src.
        
          for ( dCur[ 1 ] = 0; dCur[ 1 ] < sSize[ 1 ]; ++dCur[ 1 ] )
          {
            for ( dCur[ 0 ] = 0; dCur[ 0 ] < sSize[ 0 ]; ++dCur[ 0 ] )
            {
                // TODO: Optimize the management of the src and dst ptrs.
              sPtr = & ( *mSrc->mBuffers[ 0 ] )[ 0 ];
              sPtr += dCur[ 1 ] * sPitch + dCur[ 0 ] * sStride;
                       
              dPtr = & ( *mDst->mBuffers[ 0 ] )[ 0 ];
              dPtr += dCur[ 1 ] * sPitch + dCur[ 0 ] * sStride;
              
              applyPixel ();
            }
          }
        }
        
  private:
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class convolveBase :
  public srcDstIter
{
  protected:


    int sStart[ 2 ];
    int sEnd[ 2 ];

    base const* mOp;
    Dim opSize[ 2 ];
    Dim opPitch;
    size_t opStride;

    int opStart[ 2 ];
    int opEnd[ 2 ];

    base::BufferType* opPtr;

    int opHalf[ 2 ];
    
  public:
    convolveBase ()
        {
        
        }
        
    void setOp ( base const* pOp ) 
        { 
          mOp = pOp; 
          mOp->getSize ( opSize[ 0 ], opSize[ 1 ], opPitch );
          opStride = mOp->calcFormatStride ( mOp->getFormat () );
            // CHECK: Hopefully the right thing happens here for odd sizes.
          opHalf[ 0 ] = opSize[ 0 ] / 2;
          opHalf[ 1 ] = opSize[ 1 ] / 2;
        }
    base const* getOp () const { return mOp; }
        
    void apply ( base* pDst, base const* pSrc, base* pOp )
        {
          setSrc ( pSrc );
          setDst ( pDst );
          setOp ( pOp );
        
            // TODO: Assert src and dst are identical in format and size.
            // Alternatively, always reformat dst to make sure it matches src.

          for ( dCur[ 1 ] = 0; dCur[ 1 ] < sSize[ 1 ]; ++dCur[ 1 ] )
          {
            clipOpDimensions ( 1 );

            for ( dCur[ 0 ] = 0; dCur[ 0 ] < sSize[ 0 ]; ++dCur[ 0 ] )
            {
                // Clip extents of iteration to image in y direction.
              clipOpDimensions ( 0 );

              opPtr = & ( *mOp->mBuffers[ 0 ] )[ 0 ];
              opPtr += opStart[ 1 ] * opPitch + opStart[ 0 ] * opStride;
              
              sPtr = & ( *mSrc->mBuffers[ 0 ] )[ 0 ];
              sPtr += sStart[ 1 ] * sPitch + sStart[ 0 ] * sStride;

              dPtr = & ( *mDst->mBuffers[ 0 ] )[ 0 ];
              dPtr += dCur[ 1 ] * sPitch + dCur[ 0 ] * sStride;

              applyPixel ();
            }
          }
        }
        
  private:
        
    void clipOpDimensions ( size_t dim )
        {
            // TODO: Generalize clipping against zero to work for any
            // user-specified lower-bound.
          int startVal = dCur[ dim ] - opHalf[ dim ];
          if ( startVal < 0 )
          {
            opStart[ dim ] = -startVal;
            sStart[ dim ] = 0;
          }
          else
          {
            opStart[ dim ] = 0;
            sStart[ dim ] = startVal;;
          }

          int endVal = dCur[ dim ] - opHalf[ dim ] + opSize[ dim ];
          if ( endVal > sSize[ dim ] )
          {
            opEnd[ dim ] = opSize[ dim ] - ( endVal - sSize[ dim ] );
            sEnd[ dim ] = sSize[ dim ];
          }
          else
          {
            opEnd[ dim ] = opSize[ dim ];
            sEnd[ dim ] = endVal;
          }
        }
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class convolveRGB888Gray8 :
  public convolveBase
{
  protected:
  
    virtual void applyPixel ()
        {
          int sum[ 3 ] = { 0 };
          int opSum = 0;
          
          for ( Dim opy = opStart[ 1 ]; opy < opEnd[ 1 ]; ++opy )
          {
            base::BufferType* opPtrTmp = opPtr;
            base::BufferType const* sPtrTmp = sPtr;

            for ( Dim opx = opStart[ 0 ]; opx < opEnd[ 0 ]; ++opx )
            {
              opSum += *opPtrTmp;
            
              sum[ 0 ] += *opPtrTmp * *sPtrTmp;
              
              ++sPtrTmp;

              sum[ 1 ] += *opPtrTmp * *sPtrTmp;
              
              ++sPtrTmp;

              sum[ 2 ] += *opPtrTmp * *sPtrTmp;
              
              ++sPtrTmp;
              
              ++opPtrTmp;
            }

            opPtr += opPitch;
            sPtr += sPitch;
          }
                              
          sum[ 0 ] /= opSum;
          sum[ 1 ] /= opSum;
          sum[ 2 ] /= opSum;

          *dPtr = sum[ 0 ];
          ++dPtr;
          *dPtr = sum[ 1 ];
          ++dPtr;
          *dPtr = sum[ 2 ];
          //++dPtr;
        }
};

/////////////////////////////////////////////////////////////////////

class convolveGray8Gray8 :
  public convolveBase
{
  protected:
  
    virtual void applyPixel ()
        {
          int sum = 0;
          int opSum = 0;
          
          for ( Dim opy = opStart[ 1 ]; opy < opEnd[ 1 ]; ++opy )
          {
            base::BufferType* opPtrTmp = opPtr;
            base::BufferType const* sPtrTmp = sPtr;

            for ( Dim opx = opStart[ 0 ]; opx < opEnd[ 0 ]; ++opx )
            {
              opSum += *opPtrTmp;
            
              sum += *opPtrTmp * *sPtrTmp;
                            
              ++sPtrTmp;
              
              ++opPtrTmp;
            }

            opPtr += opPitch;
            sPtr += sPitch;
          }
                              
          sum /= opSum;

          *dPtr = sum;
          //++dPtr;
        }
};
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template< class ConvolutionType >
class blur :
  public algoBase
{
  public:
    blur ()
        {
          
        }

    void apply ( base* pDst, base const* pSrc, Dim radius )
        {
          base* pX = 0;
          base* pY = 0;
          
          BaseRef pTmp = new base;
          pTmp->cloneFormatAndSize ( pSrc, true );
          
          getGaussianPair ( pX, pY, radius );
          
          ConvolutionType convolver;
          convolver.apply ( pTmp.get (), pSrc, pX );
          convolver.apply ( pDst, pTmp.get (), pY );
        }

  private:
    typedef pni::pstd::autoRef< base > BaseRef;

    typedef std::map< Dim, BaseRef > BaseMap;
    typedef BaseMap::iterator MapIter;

      // Cache x/y images for convolution by radius.
      // Refcounts will free these when done with blur instance.
    BaseMap mXmap;
    BaseMap mYmap;
    
    Dim calcSizeFromRadius ( Dim radius )
        {
          return radius * 2 + 1;
        }

    void getGaussianPair ( base*& pX, base*& pY, Dim radius )
        {
          MapIter found = mXmap.find ( radius );
          if ( found != mXmap.end () )
          {
            pX = found->second.get ();
            pY = mYmap[ radius ].get ();   // Eh... a little asymmetric.
          }
          else
          {
            genGaussianPair ( pX, pY, radius );
          }
        }

    void genGaussianPair ( base*& pX, base*& pY, Dim radius )
        {
          base* pXtmp = genGaussian ( radius );
          
            // TRICKY: The y image will alias the first and only buffer
            // from the x image.
          base* pYtmp = new base;
          pYtmp->setSize ( 1, calcSizeFromRadius ( radius ), 1 );
          pYtmp->setFormat ( base::Gray8 );
          pYtmp->mBuffers.push_back ( pXtmp->mBuffers[ 0 ] );
          
          pX = pXtmp;
          pY = pYtmp;
          
          mXmap[ radius ] = pXtmp;
          mYmap[ radius ] = pYtmp;
        }
        
    base* genGaussian ( Dim radius )
        {
          using namespace pni::math;
        
          base* tmp = new base;
          Dim size = calcSizeFromRadius ( radius );
          tmp->setSize ( size, 1, size );
          tmp->setFormat ( base::Gray8 );
          
          base::Buffer* pBuf = new base::Buffer;
          base::Buffer& buf = *pBuf;
          pBuf->resize ( calcSizeFromRadius ( radius ) );
          tmp->mBuffers.push_back ( pBuf );
          
            // SILLY: We assign the center twice due to '<= radius'.
            // Better that than not at all. :)
          for ( Dim num = 0; num <= radius; ++num )
          {
              // This is not really gaussian... it's a cosine approximation.
            //buf[ radius + num ] = buf[ radius - num ] = ( base::BufferType )
                //( pni::math::Trait::cos ( num / ( float )  radius ) * radius * 2 );

              // This is also not really gaussian... just a better cos func.
            float result = 0.5f * Trait::cos ( 
                    Trait::piRadVal * num / ( float ) ( radius + 1 ) ) 
                    + 0.5f;
            buf[ radius + num ] = buf[ radius - num ] = ( base::BufferType )
                 ( result * ( float ) 0xff );
          }
          
          return tmp;
        }
    
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class circleToolBase :
  public algoBase
{
  protected:
    base *mSrcDst;

    Dim sStart[ 2 ];
    Dim sEnd[ 2 ];

    Dim sSize[ 2 ];
    Dim sPitch;
    size_t sStride;

    base::BufferType* sPtr;

    int dCur[ 2 ];
    
    float mRadius;
    float mRadiusSqr;
    float mDistSqr;
    float mPower;
    pni::math::vec4 mColor;
    pni::math::vec2 mPos;

    float calcPowerVal ( float val )
        {
          //if ( val == 0.0f )
          //  return val;
          //else
            val = val * pni::math::Trait::pow 
                ( 1.0f - ( mDistSqr / mRadiusSqr ), mPower );

          if ( val < 0.0f ) val = 0.0f;
          if ( val > 1.0f ) val = 1.0f;
  
          return val;
        }

  public:
  
      // power = 0.0f gives constant value.
      // 0.0f < power < 1.0f gives soft edge.
      // power = 1.0f gives linear dropoff.
      // 1.0f < power < oo gives center peak.
    void apply ( base* pSrcDst, float radius, float power, 
        pni::math::vec2 const& pos, pni::math::vec4 const& color )
        {
            // TODO: Assert src and dst are identical in format and size.
            // Alternatively, always reformat dst to make sure it matches src.
          pSrcDst->getSize ( sSize[ 0 ], sSize[ 1 ], sPitch );
          sStride = pSrcDst->calcFormatStride ( pSrcDst->getFormat () );
        
          mSrcDst = pSrcDst;
          mPos = pos;
          mColor = color;
          mPower = power;
        
          clipStartEndDim ( pos[ 0 ] - radius, pos[ 0 ] + radius, 0 );
          clipStartEndDim ( pos[ 1 ] - radius, pos[ 1 ] + radius, 1 );
          
          mRadiusSqr = radius * radius;
        
          for ( dCur[ 1 ] = sStart[ 1 ]; dCur[ 1 ] < sEnd[ 1 ]; ++dCur[ 1 ] )
          {
            for ( dCur[ 0 ] = sStart[ 0 ]; dCur[ 0 ] < sEnd[ 0 ]; ++dCur[ 0 ] )
            {
                // TODO: Optimize the management of the src and dst ptrs.
                // TODO: Optimize to leverage symmetry of circle.
              sPtr = & ( *mSrcDst->mBuffers[ 0 ] )[ 0 ];
              sPtr += dCur[ 1 ] * sPitch + dCur[ 0 ] * sStride;

              float diff[ 2 ];
              diff[ 0 ] = dCur[ 0 ] - mPos[ 0 ];
              diff[ 1 ] = dCur[ 1 ] - mPos[ 1 ];
              mDistSqr = diff[ 0 ] * diff[ 0 ] + diff[ 1 ] * diff[ 1 ];
              
              if ( mDistSqr < mRadiusSqr )
              {
                applyPixel ();
              }
            }
          }
        }
    
    void clipStartEndDim ( int start, int end, size_t dim )
        {
          sStart[ dim ] = start < 0 ? 0 : start;
          sEnd[ dim ] = end > sSize[ dim ] ? sSize[ dim ] : end;
        }
        
    base::BufferType maxValU8 ( float val )
        {
          if ( val > ( float ) 0xff )
            val = ( float ) 0xff;
          
          return ( base::BufferType ) val;
        }

    base::BufferType minValU8 ( float val )
        {
          if ( val < 0.0f )
            val = 0.0f;
          
          return ( base::BufferType ) val;
        }
        
    base::BufferType clampUB ( float val )
        {
          if ( val < 0.0f ) val = 0.0f;
          if ( val > ( float ) 0xff ) val = ( float ) 0xff;
          return ( base::BufferType ) val;
        }
};

class circleToolGray8 :
  public circleToolBase
{
  protected:
    virtual void applyPixel ()
        {
            // Currently ignoring alpha except for blend.
          float out = mColor[ 0 ] * 0xff;   // from [0,1] to [0,255].
          float const PowerVal = 1.0f;      // out;
          
          switch ( mMode )
          {
            case Set:
              *sPtr = clampUB ( calcPowerVal ( PowerVal ) * out );
              break;
            case Add:
              *sPtr = clampUB ( *sPtr + calcPowerVal ( PowerVal ) * out );
              break;
            case Sub:
              *sPtr = clampUB ( *sPtr - calcPowerVal ( PowerVal ) * out );
              break;
            case Blend:
              {
                float alpha = calcPowerVal ( mColor[ 3 ] );
                
                float src = *sPtr;
                src *= ( 1.0f - alpha );
                
                float dst = mColor[ 0 ] * 0xff;
                dst *= alpha;
                
                *sPtr = clampUB ( src + dst );
              }
              break;
            }
        }

};

class circleToolRGB888 :
  public circleToolBase
{
  protected:
    virtual void applyPixel ()
        {
            // Currently ignoring alpha except for blend.
          float const PowerVal = 1.0f;  // out;
          float const PowerValResult = calcPowerVal ( PowerVal );
          
          pni::math::vec4 color ( mColor );
          color *= ( float ) 0xff;
          
          switch ( mMode )
          {
            case Set:
              sPtr[ 0 ] = clampUB ( PowerValResult * color[ 0 ] );
              sPtr[ 1 ] = clampUB ( PowerValResult * color[ 1 ] );
              sPtr[ 2 ] = clampUB ( PowerValResult * color[ 2 ] );
              break;
            case Add:
              sPtr[ 0 ] = clampUB ( sPtr[ 0 ] + PowerValResult * color[ 0 ] );
              sPtr[ 1 ] = clampUB ( sPtr[ 1 ] + PowerValResult * color[ 1 ] );
              sPtr[ 2 ] = clampUB ( sPtr[ 2 ] + PowerValResult * color[ 2 ] );
              break;
            case Sub:
              sPtr[ 0 ] = clampUB ( sPtr[ 0 ] - PowerValResult * color[ 0 ] );
              sPtr[ 1 ] = clampUB ( sPtr[ 1 ] - PowerValResult * color[ 1 ] );
              sPtr[ 2 ] = clampUB ( sPtr[ 2 ] - PowerValResult * color[ 2 ] );
              break;
            case Blend:
              {
                float alpha = calcPowerVal ( mColor[ 3 ] );
                
                sPtr[ 0 ] = clampUB ( 
                    ( 1.0f - alpha ) * sPtr[ 0 ] + 
                    alpha * color[ 0 ] );

                sPtr[ 1 ] = clampUB ( 
                    ( 1.0f - alpha ) * sPtr[ 1 ] + 
                    alpha * color[ 1 ] );

                sPtr[ 2 ] = clampUB ( 
                    ( 1.0f - alpha ) * sPtr[ 2 ] + 
                    alpha * color[ 2 ] );
              }
              break;
            }
        }

};


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class noiseifyToolBase :
  public srcDstIter
{
  protected:
    pni::math::vec4 mAmount;

    void randomizeDstPtrVal ( int dim )
        {
          float val = *dPtr;
          
          val *= 1.0f + pni::math::getTheRand () 
              ( -mAmount[ dim ], mAmount[ dim ] );
              
          *dPtr = clampToUint8 ( val );
        }

  public:
      // Amount should be centered on zero... it will be biased by +1
      // and then multiplied by *dPtr.
    void apply ( base* pSrcDst, pni::math::vec4 const& amount )
        {
          setSrc ( pSrcDst );
          setDst ( pSrcDst );
          mAmount = amount;
          
          srcDstIter::apply ();
        }
};

class noisifyToolGray8 :
  public noiseifyToolBase
{
  protected:
    virtual void applyPixel ()
        {
            // TODO: Optimize the randVal call with table lookup.
            // TODO: Optimize float math to integer math.
          randomizeDstPtrVal ( 0 );
        }
};

class noisifyToolRGB888Independent :
  public noiseifyToolBase
{
  protected:
    virtual void applyPixel ()
        {
            // TODO: Optimize the randVal call with table lookup.
            // TODO: Optimize float math to integer math.
          randomizeDstPtrVal ( 0 );
          ++dPtr;
          randomizeDstPtrVal ( 1 );
          ++dPtr;
          randomizeDstPtrVal ( 2 );
        }
};

  // Only uses [ 0 ] component of input amount.
class noisifyToolRGB888Dependent :
  public noiseifyToolBase
{
  protected:
    virtual void applyPixel ()
        {
            // TODO: Optimize the randVal call with table lookup.
            // TODO: Optimize float math to integer math.
            // TODO: Consider using hsv space for this transform...
            // but it's probably way overkill.
          float randVal = 1.0f + pni::math::getTheRand () 
              ( -mAmount[ 0 ], mAmount[ 0 ] );
          
          float val = *dPtr;
          val *= randVal;
          *dPtr = clampToUint8 ( val );
          ++dPtr;
          
          val = *dPtr;
          val *= randVal;
          *dPtr = clampToUint8 ( val );
          ++dPtr;
          
          val = *dPtr;
          val *= randVal;
          *dPtr = clampToUint8 ( val );
        }
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class clearToolBase :
  public srcDstIter
{
  protected:
    pni::math::vec4 mColor;
    
  public:
    clearToolBase ()
        {
        
        }

    void apply ( base* pSrcDst, pni::math::vec4 const& color )
        {
          setSrc ( pSrcDst );
          setDst ( pSrcDst );
          
          mColor = color;
          srcDstIter::apply ();
        }
};

class clearlToolGray8 :
  public clearToolBase
{
  protected:
    virtual void applyPixel ()
        {
          switch ( mMode )
          {
            case Set:
              *dPtr = mColor[ 0 ] * 0xff;
              break;
            case Add:
              // TODO
              break;
            case Sub:
              // TODO
              break;
            case Blend:
              // TODO
              break;
          }
        }    
};

class clearToolRGB888 :
  public clearToolBase
{
  protected:
    virtual void applyPixel ()
        {
          switch ( mMode )
          {
            case Set:
              *dPtr = mColor[ 0 ] * 0xff;
              ++dPtr;
              *dPtr = mColor[ 1 ] * 0xff;
              ++dPtr;
              *dPtr = mColor[ 2 ] * 0xff;
              //++sPtr;              
              break;
            case Add:
              // TODO
              break;
            case Sub:
              // TODO
              break;
            case Blend:
              // TODO
              break;
          }
        }    
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class samplerBase :
  public algoBase
{
  public:

    void setSrc ( base const* pSrc ) 
        {
          mSrc = pSrc;
          mSrc->getSize ( sSize[ 0 ], sSize[ 1 ], sPitch );
          sStride = mSrc->calcFormatStride ( mSrc->getFormat () );
        }
    base const* getSrc () const { return mSrc.get (); }
    
      // TODO: Dim[ 2 ] precludes sampling in mip levels.
      // pDst must be big enough for format being sampled, e.g.,
      // 3 bytes for RGB, 1 byte for Gray8, etc.
    bool sample ( base::BufferType* pDst, Dim pos[ 2 ] )
        {
          if ( rangeCheck ( pos ) )
          {
            sPtr = &( *mSrc->mBuffers[ 0 ] )[ 0 ];
            sPtr += pos[ 1 ] * sPitch + pos[ 0 ] * sStride;
            
            dPtr = pDst;  // Yes... copying pointer, not value.
            
            applyPixel ();
           
            return true;
          }
          else
            return false;        
        }
    
  protected:
    Dim sSize[ 2 ];
    Dim sPitch;
    size_t sStride;
    
    base::BufferType const* sPtr;
    base::BufferType* dPtr;
    
    Dim sCur[ 2 ];

    ImgConstRef mSrc;
    
    bool rangeCheck ( Dim pos[ 2 ] )
        {
          return ( /* pos[ 0 ] >= 0 && */ pos[ 0 ] < sSize[ 0 ] &&
              /* pos[ 1 ] >= 0 && */ pos[ 1 ] < sSize[ 1 ] );
        }
    
};

/////////////////////////////////////////////////////////////////////

class samplerPointGray8 :
  public samplerBase
{
  public:
    virtual void applyPixel ()
        {
            *dPtr = *sPtr;
        }
};

/////////////////////////////////////////////////////////////////////

class samplerPointRGB888 :
  public samplerBase
{
  public:
    virtual void applyPixel ()
        {
            *dPtr = *sPtr;
            ++dPtr; ++sPtr;
            *dPtr = *sPtr;
            ++dPtr; ++sPtr;
            *dPtr = *sPtr;
        }
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class rangeMapperBase :
  public srcDstIter
{
  public:
      // minVal & maxVal :: [0,1]
    void apply ( base* pSrcDst, int minVal, int maxVal )
        {
          mMin = minVal;
          mMax = maxVal;
          mDiff = maxVal - minVal;
          
          setSrc ( pSrcDst );
          setDst ( pSrcDst );
          
          srcDstIter::apply ();
        }

  protected:
    int mMin;
    int mMax;
    int mDiff;
};

class rangeMapperGray8 :
  public rangeMapperBase
{
  public:
  
  protected:
    virtual void applyPixel ()
        {
          int val = ( int ) *dPtr;
          
          val *= mDiff;
          val /= 0xff;
          val += mMin;
          
          *dPtr = ( base::BufferType ) val;          
          
        }  
};


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma warning ( pop )

} // end of namespace img 

#endif // imgimgalgo_h


