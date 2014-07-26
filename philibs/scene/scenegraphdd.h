/////////////////////////////////////////////////////////////////////
//
//    class: graphDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenegraphdd_h
#define scenescenegraphdd_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "scenenode.h"
#include "scenenodepath.h"

#include "pniautoref.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  class camera;
  class geom;
  class geomFx;
  class group;
  class light;
  class sndEffect;
  class sndListener;
  
/////////////////////////////////////////////////////////////////////

class graphDd :
  public pni::pstd::refCount
{
    // We don't want the dispatch methods to be generally accessible.
    friend class camera;
    friend class geom;
    friend class geomFx;
    friend class group;
    friend class light;
    friend class sndEffect;
    friend class sndListener;

  public:
    graphDd() = default;
    virtual ~graphDd() = default;
//     graphDd(graphDd const& rhs);
//     graphDd& operator=(graphDd const& rhs);
//     bool operator==(graphDd const& rhs) const;
    
    // Traversal masks.
        // From node...

//      typedef unsigned int MaskType;
//      typedef std::vector< MaskType > TravMasks;

          typedef node::MaskType MaskType;
      
//      enum Trav {
//      Draw,
//      Cull,
//      Isect,
//          TravCount
//      };

		class fxUpdate {
			public:
			  fxUpdate ( 
			    TimeType timeStamp,
			    TimeType dt,
			    float distSqr,
			    pni::math::vec3 const& camPos,
			    nodePath const& nPath,
			    nodePath const& camPath,
			    camera* pCam ) :
			      mTimeStamp ( timeStamp ),
			      mDt ( dt ),
			      mDistanceSqr ( distSqr ),
			      mCamPos ( camPos ),
			      mNodePath ( nPath ),
			      mCamPath ( camPath ),
			      mCam ( pCam )
			        {}
			
				scene::TimeType mTimeStamp;
				scene::TimeType mDt;
				float mDistanceSqr;
				pni::math::vec3 const& mCamPos;
				nodePath const& mNodePath;
				nodePath const& mCamPath;
				camera const* mCam;

				//float mLodMetric;
				//effect* thisNode;
				//pni::math::matrix4 mMat;
				//pni::math::matrix4 mCamMat;
				//pni::math::matrix4 mCamToNodeMat;
				//pstd::autoRef< camera > cam;
		};
    
    void setTravMask ( unsigned int val ) { mTravMask = val; }
    MaskType getTravMask () const { return mTravMask; }

      ///////
      // Time stamp management.
    void setTimeStamp ( TimeType timeStamp ) { mTimeStamp = timeStamp; }
    TimeType getTimeStamp () const { return mTimeStamp; }

      // This is not managed internally because the stateless-ness of
      // this class means it can be re-used multiple times per-frame.
      // Thus, we need to allow external logic to update the last time stamp.
    void setLastTimeStamp ( TimeType timeStamp ) { mLastTimeStamp = timeStamp; }
    TimeType getLastTimeStamp () const { return mLastTimeStamp; }

      // Set to camera path for graphics, sound listener for audio.
    void setSinkPath ( nodePath const& cPath ) { mSinkPath = cPath; }
    bool initSinkPath ( node* pNode ) { return mSinkPath.init ( pNode ); }
    nodePath const& getSinkPath () const { return mSinkPath; }

    // graphDd framework:
    virtual void startGraph ( node const* pNode ) = 0;
    virtual void bindTextures ( node const* pNode ) {}

    enum DbgVals 
    {
      DbgNone = 0,
      DbgSort = 1 <<  0
    };
    
    void setDbgVals ( unsigned int vals ) { mDbgVals = vals; }
    unsigned int getDbgVals () const { return mDbgVals; }
    
  protected:
    
    virtual void dispatch ( camera const* pNode ) = 0;
    virtual void dispatch ( geom const* pNode ) = 0;
    virtual void dispatch ( group const* pNode ) = 0;
    virtual void dispatch ( light const* pNode ) = 0;
    virtual void dispatch ( geomFx const* pNode ) = 0;
    virtual void dispatch ( sndEffect const* pNode ) = 0;
    virtual void dispatch ( sndListener const* pNode ) = 0;

    nodePath mSinkPath;
    MaskType mTravMask = 0b1;
    TimeType mTimeStamp = 0.0f;
    TimeType mLastTimeStamp = 0.0f;

    unsigned int mDbgVals = DbgNone;
    
  private:
  

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenegraphdd_h


