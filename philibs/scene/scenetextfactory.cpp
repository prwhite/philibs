//////////////////////////////////////////////////////////////////////////////
//
//  class: textFactory
//
//////////////////////////////////////////////////////////////////////////////

#include "scenetextfactory.h"
#include "fontfont.h"

#include "scenestate.h"
#include "scenecommon.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

textFactory::textFactory()
{
  initDefaults();
}
//
//textFactory::~textFactory()
//{
//  
//}
//
//textFactory::textFactory(textFactory const& rhs)
//{
//  
//}
//
//textFactory& textFactory::operator=(textFactory const& rhs)
//{
//  return *this;
//}
//
//bool textFactory::operator==(textFactory const& rhs) const
//{
//  return false;
//}

/////////////////////////////////////////////////////////////////////

textFactory& textFactory::getInstance ()
{
  textFactory* pFactory = 0;
  if ( ! pFactory )
    pFactory = new textFactory;
  return *pFactory;
}

text* textFactory::newText ( font::font* pFont )
{
  text* pText = new text;
  pText->setFont(pFont);
  
  pText->setState(mDefProg.get(),state::Prog);
  pText->setState(mDefUniform.get(),state::Uniform00);
  pText->setState(mDefBlend.get(), state::Blend);
  
  texture* pTexture = 0;
  auto found = mTextureMap.find(pFont->getTextureFname());
  if ( found != mTextureMap.end ())
  {
    pTexture = found->second.get();
  }
  else
  {
    pTexture = new texture;
    pTexture->setImage(pFont->getImage());
    pTexture->setMinFilter( scene::texture::MinLinearMipLinear );
    mTextureMap[ pFont->getTextureFname() ] = pTexture;
  }
  
  pText->setState(pTexture, state::Texture00);
  
  return pText;
}

void textFactory::initDefaults()
{
  mDefProg = new prog;

  mDefProg->setProgStr(prog::Vertex, R"(
        attribute vec4 a_position;
        attribute vec2 a_uv00;

        varying lowp vec2 v_uv00;

        uniform mat4 u_mvpMat;

        void main()
        {
            v_uv00 = a_uv00;
            gl_Position = u_mvpMat * a_position;
        }
      )");
  mDefProg->setProgStr(prog::Fragment, R"(

        varying lowp vec2 v_uv00;
        uniform sampler2D u_tex00;

        uniform lowp vec4 u_textColor;
        uniform lowp float u_textMin;
        uniform lowp float u_textMax;

        void main()
        {
          lowp vec4 tex00 = texture2D ( u_tex00, v_uv00 );

          tex00.r = smoothstep ( u_textMin, u_textMax, tex00.r );
            // Use grayscale image data to fill alpha of output.
//          tex00.r *=  * 0.8 + 0.2;
          gl_FragColor = vec4 ( u_textColor.rgb, tex00.r * u_textColor.a);
        }
      
      )");
  
  mDefUniform = new uniform;
  mDefUniform->bindingOp(CommonUniformNames[ UniformTextMin ])
      .set(uniform::binding::Fragment,uniform::binding::Float1, 1);

  mDefUniform->bindingOp(CommonUniformNames[ UniformTextMax ])
      .set(uniform::binding::Fragment,uniform::binding::Float1, 1);

  mDefUniform->bindingOp(CommonUniformNames[ UniformTextColor ])
      .set(uniform::binding::Fragment,uniform::binding::Float4, 1);

  setUniformDefaults(mMinVal, mMaxVal, mColor);

  mDefBlend = new blend;
}

void textFactory::setUniformDefaults ( float minVal, float maxVal, pni::math::vec4 const& color )
{
  mMinVal = minVal;
  mMaxVal = maxVal;
  mColor = color;

  if ( mDefUniform )
  {
    *(mDefUniform->bindingOp(CommonUniformNames[ UniformTextMin ])
        .getFloats()) = minVal;

    *(mDefUniform->bindingOp(CommonUniformNames[ UniformTextMax ])
        .getFloats()) = maxVal;

    color.copyToArray (mDefUniform->bindingOp(CommonUniformNames[ UniformTextMin ])
        .getFloats());
  }
  else
    PNIDBGSTR("mDefUniform not valid, can't set defaults");
}


} // end of namespace scene 

