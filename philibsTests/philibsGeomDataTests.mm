//
//  philibsTests.m
//  philibsTests
//
//  Created by Payton White on 12/13/13.
//  Copyright (c) 2013 PHI. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "scenegeom.h"
#include "scenecommon.h"
#include "pnimathstream.h"
#include "pniseg.h"
#include "scenedata.h"
#include "scenelines.h"

#include <chrono>


//////////////////////////////////////////////////////////////////////////

using namespace scene;

using GeomDataRef = pni::pstd::autoRef< geomData >;

// Messy global stuff

GeomDataRef pData00 = 0;

//////////////////////////////////////////////////////////////////////////


@interface philibsTests : XCTestCase

@end

@implementation philibsTests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
  
  pData00 = new geomData;

  pData00->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
//  pData00->attributesOp().push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );
//  pData00->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord00], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );
//  pData00->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord01], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );

  pData00->resizeTrisWithCurrentAttributes(6,3);
  
  pData00->getIndices() = { 0, 1, 2, 3, 4, 5, 1, 2, 3 };
  
    // Fill values with prototypical data... just rotate through 0, 1, 2.
  size_t counter = 0;
  for ( auto& val : pData00->getValues() )
    val = ( float ) ( counter++ % 3 );
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
  
    pData00 = 0;
}

- (void)testGeomDataSwap
{
  pData00->clearDirty();

  geomData* pData01 = new geomData;

  pData01->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
  pData01->attributesOp().push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );

  pData01->resizeTrisWithCurrentAttributes(4,2);
  
  pData01->getIndices() = { 0, 1, 2, 1, 2, 3 };
  
  pData00->swap(pData01);
  
  XCTAssertEqual(pData00->getAttributes().size(), 2, @"Wrong number of attributes in swapped geomData");
  XCTAssertEqual(pData00->getValues().size(), 4 * pData00->getAttributes().getValueStride(), @"Wrong number of values in swapped geomData");
  XCTAssertEqual(pData00->getIndices().size(), 6, @"Wrong number of indices in swapped geomData");
  XCTAssertEqual(pData00->getDirty(), true, @"Swapped geomData should be dirty");
}

- (void)testGeomDataUnshareVerts
{
  XCTAssertEqual(pData00->getAttributes().size(), 1, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 6 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 9, @"Wrong number of indices");

  pData00->unshareVerts();

  XCTAssertEqual(pData00->getAttributes().size(), 1, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 3 * 3 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 9, @"Wrong number of indices");

}

- (void)testGeomDataShareVerts
{
  GeomDataRef pData01 = new geomData;

  pData01->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
//  pData01->attributesOp().push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );

  pData01->resizeTrisWithCurrentAttributes(6,2);
  
  pData01->getIndices() = { 0, 1, 2, 3, 4, 5 };

    // Fill values with prototypical data... just rotate through 0, 1, 0... 1, 0, 1....
  size_t counter = 0;
  for ( auto& val : pData01->getValues() )
    val = ( float ) ( counter++ % 2 );

  XCTAssertEqual(pData01->getAttributes().size(), 1, @"Wrong number of attributes");
  XCTAssertEqual(pData01->getValues().size(), 6 * pData01->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData01->getIndices().size(), 6, @"Wrong number of indices");

  pData01->shareVerts();

  XCTAssertEqual(pData01->getAttributes().size(), 1, @"Wrong number of attributes");
    // All verts are identical in the src, so they should share one vert in dst.
  XCTAssertEqual(pData01->getValues().size(), 2 * pData01->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData01->getIndices().size(), 6, @"Wrong number of indices");
  
  geomData::Values vals = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
  XCTAssertEqual(pData01->getValues(), vals, @"Attribute values incorrect");
}


- (void)testGeomRealloc
{
  XCTAssertEqual(pData00->getAttributes().size(), 1, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 6 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 9, @"Wrong number of indices");
  XCTAssertEqual(pData00->getAttributes().getValueStride(), 3, @"Wrong value stride");

  geomData::Attributes attrs = pData00->getAttributes();
  attrs.push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );
  
  pData00->reallocWithAttributes(attrs);

  XCTAssertEqual(pData00->getAttributes().size(), 2, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 6 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 9, @"Wrong number of indices");
  XCTAssertEqual(pData00->getAttributes().getValueStride(), 6, @"Wrong value stride");
}

- (void)testUnnormalizedCrossMagnitude
{
  using namespace pni::math;

  vec3 v0 ( 2.0f, 1.0f, 0.5f );
  vec3 v1 ( -4.0f, -2.0f, 1.0f );

  vec3 cross00;
  cross00.cross(v1, v0);
  cross00.normalize();
  
  v0.normalize();
  v1.normalize();

  vec3 cross01;
  cross01.cross(v1, v0);
  cross01.normalize();

  std::cout << "non-norm: " << cross00 << std::endl;
  std::cout << "    norm: " << cross01 << std::endl;

    // Wikipedia says (from her: http://en.wikipedia.org/wiki/Cross_product )
    // that the cross product is "compatible with scalar multiplication" which
    // means that the cross product doesn't need to operate on normalized
    // vectors in order to generate a proper orthogonol result.  Duh.

  XCTAssertTrue(cross00.equal(cross01), "result of non-normalized cross not equal to normalized");
}


- (void)testGenNormals
{
  pData00 = new geomData;

  pData00->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );

  pData00->resizeTrisWithCurrentAttributes(6,2);
  
  pData00->getIndices() = { 0, 1, 2, 3, 4, 5 };
  pData00->getValues() = {
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, -1.0f,
    
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, -1.0f,
      0.0f, 1.0f, -1.0f
  };

  XCTAssertEqual(pData00->getAttributes().size(), 1, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 6 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pData00->getAttributes().getValueStride(), 3, @"Wrong value stride");

    // Should share a few verts because normals were blended together
  pData00->generateNormals(80.0f);
  pData00->shareVerts();

  XCTAssertEqual(pData00->getAttributes().size(), 2, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 4 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pData00->getAttributes().getValueStride(), 6, @"Wrong value stride");
  
    // Should not share any verts because breaking angle is so small that no
    // normals will be blended
  pData00->unshareVerts();
  pData00->generateNormals(5.0f);
  pData00->shareVerts();

  XCTAssertEqual(pData00->getAttributes().size(), 2, @"Wrong number of attributes");
  XCTAssertEqual(pData00->getValues().size(), 6 * pData00->getAttributes().getValueStride(), @"Wrong number of values");
  XCTAssertEqual(pData00->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pData00->getAttributes().getValueStride(), 6, @"Wrong value stride");
}

- (void)testGenNormalsXXX
{
  geomDataXXX* pDataxx = new geomDataXXX;

  pDataxx->mBinding.push_back( { CommonAttributeNames[ geomDataXXX::Position], geomDataXXX::Position, geomDataXXX::DataType_FLOAT, sizeof(float), geomDataXXX::PositionComponents } );

  pDataxx->resizeTrisWithCurrentBinding(6,2);
  
  pDataxx->getIndices() = { 0, 1, 2, 3, 4, 5 };

  auto pcur = pDataxx->begin<pni::math::vec3>(geomDataXXX::Position);
  pcur->set(0.0f, 0.0f, 0.0f); ++pcur;
  pcur->set(1.0f, 0.0f, 0.0f); ++pcur;
  pcur->set(1.0f, 0.0f, -1.0f); ++pcur;

  pcur->set(0.0f, 0.0f, 0.0f); ++pcur;
  pcur->set(1.0f, 0.0f, -1.0f); ++pcur;
  pcur->set(0.0f, 1.0f, -1.0f);

  XCTAssertEqual(pDataxx->mBinding.size(), 1, @"Wrong number of attributes");
  XCTAssertEqual(pDataxx->sizeBytes(), 6 * pDataxx->mBinding.getValueStrideBytes(), @"Wrong byte size for values");
  XCTAssertEqual(pDataxx->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pDataxx->mBinding.getValueStrideBytes(), 3 * sizeof ( float ), @"Wrong value stride bytes");

    // Should share a few verts because normals were blended together
  pDataxx->generateNormals(80.0f);
  pDataxx->shareVerts();

  XCTAssertEqual(pDataxx->mBinding.size(), 2, @"Wrong number of attributes");
  XCTAssertEqual(pDataxx->sizeBytes(), 4 * pDataxx->mBinding.getValueStrideBytes(), @"Wrong number of values");
  XCTAssertEqual(pDataxx->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pDataxx->mBinding.getValueStrideBytes(), 6 * 4, @"Wrong value stride");

    // Should not share any verts because breaking angle is so small that no
    // normals will be blended
  pDataxx->unshareVerts();

  XCTAssertEqual(pDataxx->mBinding.size(), 2, @"Wrong number of attributes");
  XCTAssertEqual(pDataxx->sizeBytes(), 6 * pDataxx->mBinding.getValueStrideBytes(), @"Wrong byte size for values");
  XCTAssertEqual(pDataxx->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pDataxx->mBinding.getValueStrideBytes(), 6 * sizeof ( float ), @"Wrong value stride bytes");

  pDataxx->generateNormals(5.0f);
  pDataxx->shareVerts();

  XCTAssertEqual(pDataxx->mBinding.size(), 2, @"Wrong number of attributes");
  XCTAssertEqual(pDataxx->sizeBytes(), 6 * pDataxx->mBinding.getValueStrideBytes(), @"Wrong number of values");
  XCTAssertEqual(pDataxx->getIndices().size(), 6, @"Wrong number of indices");
  XCTAssertEqual(pDataxx->mBinding.getValueStrideBytes(), 6 * sizeof(float), @"Wrong value stride");
  
  pDataxx->dbg(std::cout);
}

- (void)testSegIsectSeg
{
  using namespace pni::math;

  seg s0 ( { 0, 0, 0 }, { 1, 0, 0 } );
  seg s1 ( { 0, 0, 0 }, { 0, 2, 0 } );
  seg s2 ( { 3, 1, 0 }, { -3, 1, 0 } );

  vec3 dst;
  
  XCTAssertTrue(s0.isect(s1, dst), "no isect for s0 s1");
  XCTAssertTrue(dst.equal({0,0,0}));

  XCTAssertTrue(s1.isect(s2, dst), "no isect for s1 s2");
  XCTAssertTrue(dst.equal({0,1,0}));

  XCTAssertFalse(s0.isect(s2, dst), "isect for s0 s2, but shouldn't be");

}

- (void)testSceneData
{
  using namespace pni::math;

  scene::dataBasic testData;
  scene::dataIndexedBasic testDataIndexed;

}

- (void)testSceneLines
{
  using namespace pni::math;

  lineData* pData = new lineData;
  
  pData->mBinding.push_back ( { {}, lineData::Position, lineData::Float, sizeof(float), 3 } );
  pData->mBinding.push_back ( { {}, lineData::Color, lineData::Float, sizeof(float), 4 } );
  pData->mBinding.push_back ( { {}, lineData::Thickness, lineData::Float, sizeof(float), 1 } );

  pData->resize(10,3);

  lineData const* pConstData = pData;
  
  auto beg = pConstData->begin< vec3 >(lineData::Position);
  auto end = pConstData->end< vec3 >(lineData::Position);

  for ( size_t num = 0; num < 10; ++num )
    ++beg;
  
  XCTAssertTrue(beg == end, "begin didn't correctly increment to end");

  XCTAssertEqual(pData->mBinding.size(), 3, "wrong size for binding after 3 inserts");
  XCTAssertEqual(pData->mBinding.getValueStrideBytes(), ( 3 + 4 + 1 ) * sizeof ( float ), "wrong size for stride bytes");
  XCTAssertEqual(pData->size(), 10, "wrong size for number of elements");
  XCTAssertEqual(pData->sizeBytes(), 10 * pData->mBinding.getValueStrideBytes(), "wrong byte size values storage");

  XCTAssertEqual(
      pData->getElementPtr<char>(0, lineData::Position) + pData->mBinding.getValueStrideBytes(),
      pData->getElementPtr<char>(1, lineData::Position),
      "actual stride doesn't match calculated stride");
  XCTAssertEqual(
      pData->getElementPtr<char>(0, lineData::Position) + 3 * sizeof(float),
      pData->getElementPtr<char>(0, lineData::Color),
      "offset of LineColor not correct" );

  auto& v3 = *pData->getElementPtr< pni::math::vec3 >(0, lineData::Position);
  v3[ 0 ] = 69.0f;
  XCTAssertEqual(*pData->getElementPtr<float>(0, lineData::Position), 69.0f, "overlayed vector didn't write correctly to values array");

  lineData::Binding newBinding;
  newBinding.push_back ( { {}, lineData::Position, lineData::Float, sizeof(float), 3 } );
  newBinding.push_back ( { {}, lineData::Color, lineData::Float, sizeof(float), 4 } );
    // Thickness removed/missing
  
  pData->migrate(newBinding);
  
  XCTAssertEqual(pData->size(), 10, "calculated size is wrong after migrate");
  XCTAssertEqual(pData->mBinding.getValueStrideBytes() * 10, pData->sizeBytes(),
      "byte size of storage not correct after migrate" );
  
  dataIndexedString testInstantiation;
  testInstantiation.mBinding.push_back( { "name0", "stype0", scene::type_float32_t, 4, 1 } );
  testInstantiation.mBinding.push_back( { "name1", "stype1", scene::type_float32_t, 4, 1 } );

  size_t nameOffset = testInstantiation.mBinding.getValueOffsetBytesByName("name1");
  size_t stypeOffset = testInstantiation.mBinding.getValueOffsetBytes("stype1");

  XCTAssertEqual(nameOffset, stypeOffset, "offset lookups don't match");

    // Do the whole deal, including generating output verts
  lines* pLines = new lines;
  lineData* pLineData = new lineData;
  
  pLines->lineDataProp().set ( pLineData );
  
  pLineData->mBinding.push_back ( { {}, lineData::Position, lineData::Float, sizeof(float), 3 } );
  pLineData->mBinding.push_back ( { {}, lineData::Color, lineData::Float, sizeof(float), 4 } );
  pLineData->mBinding.push_back ( { {}, lineData::Thickness, lineData::Float, sizeof(float), 1 } );

  pLineData->resize(2, 1);
  
  auto pos = pLineData->begin< vec3 >(lineData::Position);
  auto col = pLineData->begin< vec4 >(lineData::Color);
  auto thk = pLineData->begin< float > (lineData::Thickness);
  
  pos->set(0.0f, 1.0f, 2.0f);
  col->set(3.0f, 4.0f, 5.0f, 6.0f);
  *thk = 7.0f;
  
  ++pos; ++col; ++thk;
  
  pos->set(8.0f, 9.0f, 10.0f);
  col->set(11.0f, 12.0f, 13.0f, 14.0f);
  *thk = 15.0f;
  
  pLineData->getIndices()[ 0 ] = 2;
  
  pLines->lineDataProp().clearDirty();
  
  XCTAssertEqual(pLines->getGeomData()->getValues().size(), 48, "geom has wrong vert count after lines::test");
  XCTAssertEqual(pLines->getGeomData()->getIndices().size(), 6, "geom has wrong index count after lines::test");

  float* pFloats = pLineData->getPtr<float>();
  for ( size_t val = 0; val < 16; ++val )
    XCTAssertEqual(pFloats[ val ], ( float ) val, "line values not expected");


}


@end
