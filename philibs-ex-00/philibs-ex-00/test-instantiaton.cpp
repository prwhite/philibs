//
//  test-instantiaton.cpp
//  philibs-ex-00
//
//  Created by Payton White on 12/31/13.
//  Copyright (c) 2013 PREHITI HEAVY INDUSTRIES. All rights reserved.
//
// This file is lame.  It just forces compilation of a few things to make sure
// some classes are not abstract and a few other things.

#include "philibs/sceneddogl.h"
#include "philibs/scenedupdd.h"
#include "philibs/sceneddogllist.h"
#include "philibs/sceneconverter.h"
#include "philibs/scenedbgdd.h"

#include <vector>
#include <map>
#include <unordered_map>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

void testInstantiateSceneDds ()
{
  scene::graphDd* pGraphDd = new scene::ddOgl;
  
  scene::graphDd* pDupDd = new scene::dupDd;
  
  scene::converter* pConvDd = new scene::converter;
  
  scene::graphDd* dbgDd = new scene::dbgDd;
  
  scene::stateDd* pSceneDd = new scene::ddOglList;
}

void testAutoRefCompleteness ()
{
  enum SomeEnum { AA, BB, CC };

  typedef pni::pstd::autoRef< scene::node > NodeRef;
  typedef std::vector< NodeRef > NodeVec;
  typedef std::map< SomeEnum, NodeRef > NodeMap;
  
    // This is money... when using an enum as the key for an unordered_map,
    // there is no hash specialization in the stdlib.  So, we have to
    // explicetly provide the int specialization (for instance) as a
    // template param.
  typedef std::unordered_map< SomeEnum, NodeRef, std::hash< int > > NodeUnorderedMap;
  
  NodeUnorderedMap unorderedMap;
}


#pragma clang diagnostic pop

