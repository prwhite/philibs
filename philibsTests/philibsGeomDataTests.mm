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



@end
