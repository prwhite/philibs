/////////////////////////////////////////////////////////////////////
//
//  file: scenerendersinkdd.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenerendersinkdd.h"
#include "scenerendersink.h"
#include "pnitimer.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

//renderSinkDd::renderSinkDd()
//{
//  
//}
//
//renderSinkDd::~renderSinkDd()
//{
//  
//}
//
//renderSinkDd::renderSinkDd(renderSinkDd const& rhs)
//{
//  
//}
//
//renderSinkDd& renderSinkDd::operator=(renderSinkDd const& rhs)
//{
//  return *this;
//}
//
//bool renderSinkDd::operator==(renderSinkDd const& rhs) const
//{
//  return false;
//}

void renderSinkDd::startGraph ( renderSink const* pSink, TimeType timeStamp )
{
  typedef pni::pstd::autoRef<renderSink const> Ref;
  typedef std::vector<Ref> Refs;

    // Set up initial traversal state.
  mCurTimeStamp = timeStamp;
  
    // Set up initial stacks.
  Refs stack;
  stack.push_back(pSink);
  
    // Iterate over graph... build a list.
  Refs out;
  
  while(!stack.empty())
  {
    Ref front = stack.back();
    stack.pop_back();
    
    for(auto iter : front->getChildren())
      stack.push_back(iter.get());
    
    out.push_back(front.get());
  }
  
    // Iterate over the list _backwards_ to make sure all the deps work out.
  auto rend = out.rend();
  for ( auto rcur = out.rbegin(); rcur != rend; ++rcur)
    ( *rcur )->accept ( this );
  
    // Clean up.
  mLastTimeStamp = mCurTimeStamp;
}

namespace {
  using namespace pni::math;
  
  vec4 const rtCubeMatRotations[ texture::NumCubeImgSlots ] = {
    { -90.0f, 0.0f, 1.0f, 0.0f },   // Pos X
    {  90.0f, 0.0f, 1.0f, 0.0f },   // Neg X
    {  90.0f, 1.0f, 0.0f, 0.0f },   // Pos Y
    { -90.0f, 1.0f, 0.0f, 0.0f },   // Neg Y
    { 180.0f, 0.0f, 1.0f, 0.0f },   // Pos Z
    {   0.0f, 0.0f, 1.0f, 0.0f },   // Neg Z
  };
  
  void setCubemapDdMat ( renderSink const* pSink, texture::ImageId imgId )
  {
      // TODO: Finish extracting world-space pos of camera, then
      // adding appropriate cube map side rotation.  Or, just cancel
      // out rotation in local space of sink path leaf node.
      // HACK: For now we know rt cubemap camera will be at root with no
      // scale or rotation... so we can take the low road.
    vec4 const& vec = rtCubeMatRotations[ imgId - texture::CubePosXImg ];

    matrix4 rot;
    rot.setRot(vec[ 0 ], vec[ 1 ], vec[ 2 ], vec[ 3 ]);

    node* pNode = pSink->mDrawSpec.mSinkPath.getLeaf();
  
    vec3 trans;
    pNode->getMatrix().getTrans(trans);
    
    pNode->matrixOp().setTrans(trans);
    pNode->matrixOp().preMult(rot);
  }
}

framebuffer::TextureImageId calcImageId(
    framebuffer::TextureTarget texTarget,
    framebuffer::TextureImageId texImageId,
    framebuffer::Type type)
{
  if(texTarget == texture::NoTarget)
    return texture::NoImage;
  else
  {
    if(type == framebuffer::Default)
      return texture::NoImage;
    else if(type == framebuffer::Renderbuffer)
      return texture::NoImage;
    else
      return texImageId;
  }
}

  // Called by startGraph via double dispatch
void renderSinkDd::dispatch ( renderSink const* pSink )
{
  if(pSink->mFramebuffer->getSpec().mTextureTarget == texture::CubeMapTarget)
  {
    for(uint32_t uicur = texture::CubePosXImg; uicur <= texture::CubeNegZImg; ++uicur)
    {
        // Can't increment enum types any more
      texture::ImageId cur = ( texture::ImageId ) uicur;
    
        // Need to set the 6 different views
      setCubemapDdMat(pSink, cur);
    
      framebuffer::spec const& spec = pSink->mFramebuffer->getSpec();
    
      framebuffer::TextureImageId colorId = calcImageId(spec.mTextureTarget, cur, spec.mColorType[ 0 ]);
      framebuffer::TextureImageId depthId = calcImageId(spec.mTextureTarget, cur, spec.mDepthType);
      framebuffer::TextureImageId stencilId = calcImageId(spec.mTextureTarget, cur, spec.mStencilType);
    
      dispatch(pSink, colorId, depthId, stencilId);
    }
  }
  else
  {
    framebuffer::spec const& spec = pSink->mFramebuffer->getSpec();

    framebuffer::TextureImageId colorId = calcImageId(spec.mTextureTarget, texture::Tex2DImg, spec.mColorType[ 0 ]);
    framebuffer::TextureImageId depthId = calcImageId(spec.mTextureTarget, texture::Tex2DImg, spec.mDepthType);
    framebuffer::TextureImageId stencilId = calcImageId(spec.mTextureTarget, texture::Tex2DImg, spec.mStencilType);

    dispatch(pSink, colorId, depthId, stencilId);
  }
}

void renderSinkDd::dispatch ( renderSink const* pSink,
    texture::ImageId colorId,
    texture::ImageId depthId,
    texture::ImageId stencilId )
{
  if(pSink->mFramebuffer)
    pSink->mFramebuffer->bind(colorId, depthId, stencilId);

  for(auto iter : { pSink->mDrawSpec, pSink->mSndSpec, pSink->mIsectSpec })
    if ( iter.mDd ) // null dd's are part of the design... they are skipped.
      execGraphDd(iter);
  
  if(pSink->mFramebuffer)
    pSink->mFramebuffer->finish();
}

  // Called by dispatch, maybe up to 6 times if the renderSink is a cube map
void renderSinkDd::execGraphDd ( renderSink::graphDdSpec const& spec )
{
  spec.mDd->setSinkPath(spec.mSinkPath);
  spec.mDd->setTravMask(spec.mTravMask);
  spec.mDd->setTimeStamp(mCurTimeStamp);
  spec.mDd->setLastTimeStamp(mLastTimeStamp);
  
  spec.mDd->startGraph(spec.mRootNode.get());
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


