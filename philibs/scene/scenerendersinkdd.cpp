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

void renderSinkDd::dispatch ( renderSink const* pSink )
{
  if(pSink->mFramebuffer->getSpec().mTextureTarget == texture::CubeMapTarget)
  {
    for(texture::ImageId cur = texture::CubePosXImg; cur <= texture::CubeNegZImg; ++cur)
    {
        // TODO: Need to set the 6 different views
      setCubemapDdMat(pSink, cur);
    
      dispatch(pSink, cur);
    }
  }
  else
  {
    dispatch(pSink, texture::Tex2DImg);
  }
}

void renderSinkDd::dispatch ( renderSink const* pSink, texture::ImageId imgId )
{
  pSink->mFramebuffer->bind (imgId);

  for(auto iter : { pSink->mDrawSpec, pSink->mSndSpec, pSink->mIsectSpec })
    execGraphDd(iter);
}

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


