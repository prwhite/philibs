/////////////////////////////////////////////////////////////////////
//
//	matrix4.C
//
/////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cmath>
#include "pnimatrix4.h"
#include "pnivec3.h"
#include "pnivec4.h"
#include "pniquat.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

void
matrix4::
setTrans ( ValueType xval, ValueType yval, ValueType zval )
{ 
	set ( TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal,
		  TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal,
		  TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal,
		  xval, yval, zval, TraitType::oneVal );
}

// NON-OPTIMAL

void
matrix4::
preTrans ( ValueType xval, ValueType yval, ValueType zval )
{
	matrix4 tmp ( NoInit );
	tmp.setTrans ( xval, yval, zval );
	
	this->preMult ( tmp );
}

// NON-OPTIMAL

// From Graphics Gems 1, p476
// Efficient post concatenation of Transformation Matrices
// by Cychosz

void
matrix4::
postTrans ( ValueType xval, ValueType yval, ValueType zval )
{
	for ( int col = 0; col < 4; col++ )
	{
		mat[ col ][ 0 ] += mat[ col ][ 3 ] * xval;
		mat[ col ][ 1 ] += mat[ col ][ 3 ] * yval;
		mat[ col ][ 2 ] += mat[ col ][ 3 ] * zval;
	}
}

bool
matrix4::
getTrans ( ValueType& xval, ValueType& yval, ValueType& zval ) const
{ 
	xval = mat[ 3 ][ 0 ];
	yval = mat[ 3 ][ 1 ];
	zval = mat[ 3 ][ 2 ];
	
	return true;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// rotation methods

void
matrix4::
setRot ( ValueType degrees, ValueType xval, ValueType yval, ValueType zval )
{ 
	ValueType angle = TraitType::d2r ( degrees );
	ValueType c = TraitType::cos ( angle );
	ValueType s = TraitType::sin ( angle );
	ValueType t = TraitType::oneVal - c;
	
	mat[ 0 ][ 0 ] = t * xval * xval + c;
	mat[ 1 ][ 0 ] = t * xval * yval - s * zval;
	mat[ 2 ][ 0 ] = t * xval * zval + s * yval;
	mat[ 3 ][ 0 ] = TraitType::zeroVal;

	mat[ 0 ][ 1 ] = t * xval * yval + s * zval;
	mat[ 1 ][ 1 ] = t * yval * yval + c;
	mat[ 2 ][ 1 ] = t * yval * zval - s * xval;
	mat[ 3 ][ 1 ] = TraitType::zeroVal;

	mat[ 0 ][ 2 ] = t * xval * zval - s * yval;
	mat[ 1 ][ 2 ] = t * yval * zval + s * xval;
	mat[ 2 ][ 2 ] = t * zval * zval + c;
	mat[ 3 ][ 2 ] = TraitType::zeroVal;

	mat[ 0 ][ 3 ] = mat[ 1 ][ 3 ] = mat[ 2 ][ 3 ] = TraitType::zeroVal;
	mat[ 3 ][ 3 ] = TraitType::oneVal;
}

void
matrix4::
setRot ( ValueType degrees, RotAxis axis )
{
	ValueType angle = TraitType::d2r ( degrees );
	ValueType co = TraitType::cos ( angle );
	ValueType si = TraitType::sin ( angle );

	if ( axis == Xaxis )
	{	
		set ( TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, 
			  TraitType::zeroVal,   co,   si, TraitType::zeroVal, 
			  TraitType::zeroVal,  -si,   co, TraitType::zeroVal, 
			  TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal );
	}
	else if ( axis == Yaxis )
	{
		set (   co, TraitType::zeroVal,  -si, TraitType::zeroVal, 
			  TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, 
			    si, TraitType::zeroVal,   co, TraitType::zeroVal, 
			  TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal );
	}
	else if ( axis == Zaxis )
	{
		set (   co,   si, TraitType::zeroVal, TraitType::zeroVal, 
			   -si,   co, TraitType::zeroVal, TraitType::zeroVal, 
			  TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, 
			  TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal );
	}
}

// NON-OPTIMAL

void
matrix4::
preRot ( ValueType degrees, RotAxis axis )
{
	matrix4 tmp ( NoInit );
	tmp.setRot ( degrees, axis );
	
	this->preMult ( tmp );
}

// NON-OPTIMAL

void
matrix4::
preRot ( ValueType degrees, ValueType xval, ValueType yval, ValueType zval )
{
	matrix4 tmp ( NoInit );
	tmp.setRot ( degrees, xval, yval, zval );
	this->preMult ( tmp );
}

// NON-OPTIMAL

// From Graphics Gems 1, p476
// Efficient post concatenation of Transformation Matrices
// by Cychosz

void
matrix4::
postRot ( ValueType degrees, RotAxis axis )
{
	ValueType angle = TraitType::d2r ( degrees );
	ValueType co = TraitType::cos ( angle );
	ValueType si = TraitType::sin ( angle );

	if ( axis == Xaxis )
	{
		for ( int col = 0; col < 4; col++ )
		{
			ValueType t = mat[ col ][ 1 ];
			mat[ col ][ 1 ] = t * co - mat[ col ][ 2 ] * si;
			mat[ col ][ 2 ] = t * si + mat[ col ][ 2 ] * co;
		}
	}
	else if ( axis == Yaxis )
	{
		for ( int col = 0; col < 4; col++ )
		{
			ValueType t = mat[ col ][ 0 ];
			mat[ col ][ 0 ] = t * co + mat[ col ][ 2 ] * si;
			mat[ col ][ 2 ] = mat[ col ][ 2 ] * co - t * si;
		}
	}
	else if ( axis == Zaxis )
	{
		for ( int col = 0; col < 4; col++ )
		{
			ValueType t = mat[ col ][ 0 ];
			mat[ col ][ 0 ] = t * co - mat[ col ][ 1 ] * si;
			mat[ col ][ 1 ] = t * si + mat[ col ][ 1 ] * co;
		}
	}
}

// NON-OPTIMAL

void
matrix4::
postRot ( ValueType degrees, ValueType xval, ValueType yval, ValueType zval )
{
	matrix4 tmp ( NoInit );
	tmp.setRot ( degrees, xval, yval, zval );
	this->postMult ( tmp );
}

// NON-OPTIMAL

bool
matrix4::
getRot ( ValueType& degrees, ValueType& xval, ValueType& yval, ValueType& zval ) const
{
	vec3 axis ( vec3::NoInit );
	bool retVal = getRot ( degrees, axis );
	
	xval = axis.vec[ 0 ];
	yval = axis.vec[ 1 ];
	zval = axis.vec[ 2 ];
	
	return retVal;
}

bool
matrix4::
getRot ( ValueType& degrees, vec3& axis ) const
{
    // We need to cancel scale for the degrees calc to be meaningful...
    // scaled matrices will blow the comparison to 0.0.
    // 2008/08/02 This tested out OK so far.
  vec3 scale ( vec3::NoInit );
  getScale ( scale ); 

	degrees = TraitType::acos ( 
	    (   mat[0][0] / scale[ 0 ]
	      + mat[1][1] / scale[ 1 ]
	      + mat[2][2] / scale[ 2 ] 
	      - TraitType::oneVal ) 
	    / ( ( ValueType ) 2.0 ) );

    // No rotation.
	if ( TraitType::equal ( TraitType::zeroVal, degrees ) )
		axis.set ( TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal );
		// Some rotation.
	else
	{
		axis.set (	( mat[1][2] - mat[2][1] ) / ( ( ( ValueType ) 2.0 ) * TraitType::sin( degrees ) ), 
					( mat[2][0] - mat[0][2] ) / ( ( ( ValueType ) 2.0 ) * TraitType::sin( degrees ) ), 
					( mat[0][1] - mat[1][0] ) / ( ( ( ValueType ) 2.0 ) * TraitType::sin( degrees ) ) );

		axis.normalize ();

  	degrees = TraitType::r2d ( degrees );
	}
	
	return true;
}

// NON-OPTIMAL

bool
matrix4::
getRot ( matrix4& rotMat ) const
{
	ValueType degrees;
	vec3 axis ( vec3::NoInit );
	
	getRot ( degrees, axis );
	rotMat.setRot ( degrees, axis );

	return true;
}

/////////////////////////////////////////////////////////////////////
// quat methods

/////////////////////////////////////////////////////////////////////
// this method implementation acts like the scalar part is the first
// part of the quat, but we still pass in the arguments in the
// convention of our quat

void
matrix4::
setQuat ( ValueType sq2, ValueType sq3, ValueType sq4, ValueType sq1 )
{
	ValueType q2q3 = sq2 * sq3;
	ValueType q1q4 = sq1 * sq4;
	ValueType q2q4 = sq2 * sq4;
	ValueType q1q3 = sq1 * sq3;
	ValueType q3q4 = sq3 * sq4;
	ValueType q1q2 = sq1 * sq2;
	
	sq1 *= sq1;
	sq2 *= sq2;
	sq3 *= sq3;
	sq4 *= sq4;

	// Comments indicate post-multiplication solution
	// The order has been changed to transpose the solution
	// for pre-multiplication as in Performer, SmartScene, OpenGL, etc.
	mat[0][0] = sq1 + sq2 - sq3 - sq4; // m[0][0]
	mat[0][1] = ( ( ValueType ) 2.0 ) * ( q2q3 + q1q4 ); // m[1][0]
	mat[0][2] = ( ( ValueType ) 2.0 ) * ( q2q4 - q1q3 ); // m[2][0]
	mat[0][3] = Trait::zeroVal;
	
	mat[1][0] = ( ( ValueType ) 2.0 ) * ( q2q3 - q1q4 ); // m[0][1]
	mat[1][1] = sq1 - sq2 + sq3 - sq4; // m[1][1]
	mat[1][2] = ( ( ValueType ) 2.0 ) * ( q1q2 + q3q4 ); // m[2][1]
	mat[1][3] = Trait::zeroVal;

	mat[2][0] = ( ( ValueType ) 2.0 ) * ( q2q4 + q1q3 ); // m[0][2]
	mat[2][1] = ( ( ValueType ) 2.0 ) * ( q3q4 - q1q2 ); // m[1][2]
	mat[2][2] = sq1 - sq2 - sq3 + sq4; // m[2][2]
	mat[2][3] = Trait::zeroVal;

	mat[3][0] = Trait::zeroVal;
	mat[3][1] = Trait::zeroVal;
	mat[3][2] = Trait::zeroVal;
	mat[3][3] = Trait::oneVal;
}

// NON-OPTIMAL

void
matrix4::
preQuat ( ValueType xval, ValueType yval, ValueType zval, ValueType sval )
{
	matrix4 tmp ( NoInit );
	tmp.setQuat ( xval, yval, zval, sval );
	this->preMult ( tmp );
}

// NON-OPTIMAL

void
matrix4::
postQuat ( ValueType xval, ValueType yval, ValueType zval, ValueType sval )
{
	matrix4 tmp ( NoInit );
	tmp.setQuat ( xval, yval, zval, sval );
	this->postMult ( tmp );
}

bool
matrix4::
getQuat ( ValueType& xval, ValueType& yval, ValueType& zval, ValueType& sval ) const
{
	ValueType Tr = mat[0][0] + mat[1][1] + mat[2][2];

	int greatest = 2;
	ValueType v = mat[0][0];
	if ( mat[1][1] > v ) { v = mat[1][1]; greatest = 3; }
	if ( mat[2][2] > v ) { v = mat[2][2]; greatest = 4; }
	v = ( ( ValueType ) 2.0 ) * v - Tr;
	if ( Tr > v ) { v = Tr; greatest = 1; }

	v = TraitType::sqrt ( TraitType::abs ( TraitType::oneVal + v ) ) / ( ( ValueType ) 2.0 );

	switch ( greatest ) { 
	case 1 :
		sval = v;
		xval = ( mat[1][2] - mat[2][1] ) / ( ( ( ValueType ) 4.0 ) * v );
		yval = ( mat[2][0] - mat[0][2] ) / ( ( ( ValueType ) 4.0 ) * v );
		zval = ( mat[0][1] - mat[1][0] ) / ( ( ( ValueType ) 4.0 ) * v );
		break;
	case 2 :
		sval = ( mat[1][2] - mat[2][1] ) / ( ( ( ValueType ) 4.0 ) * v );
		xval = v;
		yval = ( mat[1][0] + mat[0][1] ) / ( ( ( ValueType ) 4.0 ) * v );
		zval = ( mat[2][0] + mat[0][2] ) / ( ( ( ValueType ) 4.0 ) * v );
		break;
	case 3 :
		sval = ( mat[2][0] - mat[0][2] ) / ( ( ( ValueType ) 4.0 ) * v );
		xval = ( mat[1][0] + mat[0][1] ) / ( ( ( ValueType ) 4.0 ) * v );
		yval = v;
		zval = ( mat[2][1] + mat[1][2] ) / ( ( ( ValueType ) 4.0 ) * v );
		break;
	case 4 :
		sval = ( mat[0][1] - mat[1][0] ) / ( ( ( ValueType ) 4.0 ) * v );
		xval = ( mat[2][0] + mat[0][2] ) / ( ( ( ValueType ) 4.0 ) * v );
		yval = ( mat[2][1] + mat[1][2] ) / ( ( ( ValueType ) 4.0 ) * v );
		zval = v;
		break;
	}
    
	return true;
}

/////////////////////////////////////////////////////////////////////
// euler methods

// NON-OPTIMAL

void
matrix4::
setEuler ( ValueType hval, ValueType pval, ValueType rval )
{ 
	setIdentity ();
#define ZISUPNOTOUT				// ADDTOTRAIT
#ifdef ZISUPNOTOUT
	// performer convention: h-z p-x r-y
	postRot ( rval, Yaxis );
	postRot ( pval, Xaxis );
	postRot ( hval, Zaxis );
#else
	// normal convention: h-y p-x r-z
	postRot ( rval, Zaxis );
	postRot ( pval, Xaxis );
	postRot ( hval, Yaxis );
#endif
}

void
matrix4::
preEuler ( ValueType hval, ValueType pval, ValueType rval )
{ 
	matrix4 tmp ( NoInit );
	tmp.setEuler ( hval, pval, rval );
	this->preMult ( tmp );
}

// NON-OPTIMAL

void
matrix4::
postEuler ( ValueType hval, ValueType pval, ValueType rval )
{ 
	// performer convention: h-z p-x r-y
#ifdef ZISUPNOTOUT
	postRot ( rval, Yaxis );
	postRot ( pval, Xaxis );
	postRot ( hval, Zaxis );
#else
	// normal convention: h-y p-x r-z
	postRot ( rval, Zaxis );
	postRot ( pval, Xaxis );
	postRot ( hval, Yaxis );
#endif
}

bool
matrix4::
getEuler ( ValueType& hval, ValueType& pval, ValueType& rval ) const
{ 
#ifdef ZISUPNOTOUT
    hval = TraitType::r2d ( TraitType::atan2 ( -mat[1][0] , mat[1][1] ) );
    pval = TraitType::r2d ( TraitType::asin (  mat[1][2] ) );
    rval = TraitType::r2d ( TraitType::atan2 ( -mat[0][2] , mat[2][2] ) );
#else
    hval = TraitType::r2d ( TraitType::atan2 ( mat[2][0] , mat[2][2] ) );
    pval = TraitType::r2d ( -TraitType::oneVal * TraitType::asin (  mat[2][1] ) );
    rval = TraitType::r2d ( TraitType::atan2 ( mat[0][1] , mat[1][1] ) );
#endif
		
	return true;
}

/////////////////////////////////////////////////////////////////////

void
matrix4::
setVecRotVec ( const vec3& v1, const vec3& v2 )
{ 	
	if ( v1.equal ( v2 ) )
	{	
		setIdentity ();
		return;
	}

	vec3 axis ( vec3::NoInit );
	ValueType angle;
	
	angle = axis.getAxisAngle ( v1, v2 );

	axis.normalize ();
	setRot ( angle, axis );
}

/////////////////////////////////////////////////////////////////////

// NON-OPTIMAL

void
matrix4::
setCoord ( const vec3& trans, const vec3& eul )
{ 
	setEuler ( eul );
	postTrans ( trans );
}

// NON-OPTIMAL

bool
matrix4::
getCoord ( vec3& trans, vec3& eul ) const
{
	bool ret1 = getTrans ( trans );
	bool ret2 = getEuler ( eul );

	return ret1 && ret2;	
}

/////////////////////////////////////////////////////////////////////

void
matrix4::
setScale ( ValueType xval, ValueType yval, ValueType zval )
{ 
	set (  xval, TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, 
		   TraitType::zeroVal, yval, TraitType::zeroVal, TraitType::zeroVal, 
		   TraitType::zeroVal, TraitType::zeroVal, zval, TraitType::zeroVal, 
		   TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal );
}

// NON-OPTIMAL

void
matrix4::
preScale ( ValueType xval, ValueType yval, ValueType zval )
{ 
	matrix4 tmp ( NoInit );
	tmp.setScale ( xval, yval, zval );
	this->preMult ( tmp );
}

// NON-OPTIMAL

// From Graphics Gems 1, p476
// Efficient post concatenation of Transformation Matrices
// by Cychosz

void
matrix4::
postScale ( ValueType xval, ValueType yval, ValueType zval )
{ 
	for ( int col = 0; col < 4; col++ )
	{
		mat[ col ][ 0 ] *= xval;
		mat[ col ][ 1 ] *= yval;
		mat[ col ][ 2 ] *= zval;
	}
}

// From Graphics Gems 2, p320
// Thomas       Decomposing a Matrix into Simple Transformations

bool
matrix4::
getScale ( ValueType& xval, ValueType& yval, ValueType& zval ) const
{ 	
	// this works if you are trying to cancel scale with a pre
	// xform
//	vec3 row0 ( mat[ 0 ][ 0 ], mat[ 0 ][ 1 ], mat[ 0 ][ 2 ] );
//	vec3 row1 ( mat[ 1 ][ 0 ], mat[ 1 ][ 1 ], mat[ 1 ][ 2 ] );
//	vec3 row2 ( mat[ 2 ][ 0 ], mat[ 2 ][ 1 ], mat[ 2 ][ 2 ] );

	// this works if you are trying to cancel scale with a post
	// xform... which is preferred
	vec3 row0 ( mat[ 0 ][ 0 ], mat[ 1 ][ 0 ], mat[ 2 ][ 0 ] );
	vec3 row1 ( mat[ 0 ][ 1 ], mat[ 1 ][ 1 ], mat[ 2 ][ 1 ] );
	vec3 row2 ( mat[ 0 ][ 2 ], mat[ 1 ][ 2 ], mat[ 2 ][ 2 ] );

	///////
	// Compute X scale factor and normalize first row
	xval = row0.normalize ();
	
	///////
	// Compute XY shear factor and set 2nd row orthogonal to 1st
	ValueType xyShear = row0.dot ( row1 );

	// next line is same as row1.combine ( TraitType::oneVal, row1, -xyShear, row0 );
	row1 -= row0 * xyShear;	// equiv of line above

	///////
	// Now, compute Y scale and normalize 2nd row
	yval = row1.normalize ();
	xyShear /= yval;

	///////
	// Compute XZ and YZ shears, orthogonalize 3rd row
	ValueType xzShear = row0.dot ( row2 );

	// next line is same as row2.combine ( TraitType::oneVal, row2, -xzShear, row0 );
	row2 -= row0 * xzShear;	// equiv of line above

	ValueType yzShear = row1.dot ( row2 );

	// next line is same as row2.combine ( TraitType::oneVal, row2, -yzShear, row1 );
	row2 -= row1 * yzShear;

	///////
	// Next, get Z scale and normalize 3rd row
	zval = row2.normalize ();

	// These two lines are only necessary if we want to keep
	// the shear, but we don't.  PNI
//	xzShear /= zval;
//	yzShear /= zval;
	
	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.

	// This seems to be bogus... it gives false negative scale
	// values for some scales known to be positive.  11/7/97 PNI
	// I still think this is bogus 10/21/99 PNI
	// I need this to detect negative scale, 
	// I uncommented this code and it doesn't seem to affect anyone. so far so good. AJT 9/7/00

	// You guys are fucking dorks.... This matrix isn't always orthonormal, so instead of
	// looking for -1 or 0, we check if it's less then -0.5.... ie. REALLY left handed.
	// Oh, and don't touch the spacing ( marcus ) --HAA 07/27/01

	// Ok, here's the deal, The above scale calculations are correct but the variables named 
	// row0, row1, row2 are actually the columns.  So, in order to compute the determinant 
	// we need to account for this.  Previously, we were not calcuating the determinant 
	// correctly resulting in false negative scales.  Oh, and Howard, I fixed your spacing.
	// --AJT 7/29/00 (a fucking sunday) <-- no life.

	vec3 realRow0 ( row0 [ 0 ], row1 [ 0 ], row2 [ 0 ] );
	vec3 realRow1 ( row0 [ 1 ], row1 [ 1 ], row2 [ 1 ] );
	vec3 realRow2 ( row0 [ 2 ], row1 [ 2 ], row2 [ 2 ] );
	vec3 temp;

	temp.cross ( realRow1, realRow2 );
	if ( TraitType::equal ( temp.dot ( realRow0 ), -TraitType::oneVal, 0.001f ) )
	{
		// if deterinant is -1, negate scale.
		xval *= -TraitType::oneVal;
		yval *= -TraitType::oneVal;
		zval *= -TraitType::oneVal;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////

void
matrix4::
setOrtho (	ValueType l, ValueType r,
			ValueType b, ValueType t, 
			ValueType n, ValueType f )
{
	mat[ 0 ][ 0 ] = ( ( ValueType ) 2.0 ) / ( r - l );
	mat[ 1 ][ 0 ] = TraitType::zeroVal;
	mat[ 2 ][ 0 ] = TraitType::zeroVal;
	mat[ 3 ][ 0 ] = -( r + l ) / ( r - l );
	
	mat[ 0 ][ 1 ] = TraitType::zeroVal;
	mat[ 1 ][ 1 ] = ( ( ValueType ) 2.0 ) / ( t - b );
	mat[ 2 ][ 1 ] = TraitType::zeroVal;
	mat[ 3 ][ 1 ] = -( t + b ) / ( t - b );

	mat[ 0 ][ 2 ] = TraitType::zeroVal;
	mat[ 1 ][ 2 ] = TraitType::zeroVal;
	mat[ 2 ][ 2 ] = -( ( ValueType ) 2.0 ) / ( f - n );
	mat[ 3 ][ 2 ] = - ( f + n ) / ( f - n );

	mat[ 0 ][ 3 ] = TraitType::zeroVal;
	mat[ 1 ][ 3 ] = TraitType::zeroVal;
	mat[ 2 ][ 3 ] = TraitType::zeroVal;
	mat[ 3 ][ 3 ] = TraitType::oneVal;
}

void
matrix4::
setFrustum (	ValueType l, ValueType r,
			ValueType b, ValueType t, 
			ValueType n, ValueType f )
{
	mat[ 0 ][ 0 ] = ( ( ValueType ) 2.0 ) * n / ( r - l );
	mat[ 1 ][ 0 ] = TraitType::zeroVal;
	mat[ 2 ][ 0 ] = ( r + l ) / ( r - l );
	mat[ 3 ][ 0 ] = TraitType::zeroVal;
	
	mat[ 0 ][ 1 ] = TraitType::zeroVal;
	mat[ 1 ][ 1 ] = ( ( ValueType ) 2.0 ) * n / ( t - b );
	mat[ 2 ][ 1 ] = ( t + b ) / ( t - b );
	mat[ 3 ][ 1 ] = TraitType::zeroVal;

	mat[ 0 ][ 2 ] = TraitType::zeroVal;
	mat[ 1 ][ 2 ] = TraitType::zeroVal;
	mat[ 2 ][ 2 ] = - ( f + n ) / ( f - n );
	mat[ 3 ][ 2 ] = ( ( ValueType ) -2.0 ) * f * n / ( f - n );

	mat[ 0 ][ 3 ] = TraitType::zeroVal;
	mat[ 1 ][ 3 ] = TraitType::zeroVal;
	mat[ 2 ][ 3 ] = -TraitType::oneVal;
	mat[ 3 ][ 3 ] = TraitType::zeroVal;
}

void
matrix4::
setPerspective ( ValueType fovy, ValueType aspect, ValueType nearVal, ValueType farVal )
{
	ValueType tb = nearVal * TraitType::tan ( TraitType::d2r ( fovy ) * ( ( ValueType ) 0.5 ) );
	ValueType lr = nearVal * TraitType::tan ( TraitType::d2r ( aspect * fovy ) * ( ( ValueType ) 0.5 ) );
	
	setFrustum ( -lr, lr, -tb, tb, nearVal, farVal );
}

// setViewport:
// -the first set trans is to move normalized device coordinates (NDC)
// to 0,0,0
// -NDC's are in the range [-1,1]

void
matrix4::
setViewport ( ValueType xorig, ValueType yorig, ValueType width, ValueType height )
{
	setTrans ( vec3 ( TraitType::oneVal, TraitType::oneVal, TraitType::zeroVal ) );
	postScale ( vec3 ( width * ( ( ValueType ) 0.5 ), height * ( ( ValueType ) 0.5 ), TraitType::oneVal ) );
	postTrans ( vec3 ( xorig, yorig, TraitType::zeroVal ) );
}

void
matrix4::
setLookat ( const vec3& from, const vec3& to, const vec3& up )
{
	vec3 jvec ( to );
	jvec -= from;
	jvec.normalize ();

	vec3 ivec ( vec3::NoInit );
	ivec.cross ( jvec, up );

	vec3 kvec ( vec3::NoInit );
	kvec.cross ( ivec, jvec );

	// AJT: The cross product of 2 normalized vectors is not guaranteed to be normalized
	// so we need to normalize these to prevent scale from leaking into the matrix.
	ivec.normalize ();
	kvec.normalize ();

	setRow ( 0, ivec[ 0 ], ivec[ 1 ], ivec[ 2 ], 0.0f );
	setRow ( 1, jvec[ 0 ], jvec[ 1 ], jvec[ 2 ], 0.0f );
	setRow ( 2, kvec[ 0 ], kvec[ 1 ], kvec[ 2 ], 0.0f );
	setRow ( 3, from[ 0 ], from[ 1 ], from[ 2 ], 1.0f );
}

/////////////////////////////////////////////////////////////////////

template< class ValueType >
inline static	// can this func inline
void
MatSwap ( ValueType& a, ValueType& b )
{
	ValueType tmp = a;
	a = b;
	b = tmp;
}

template< class ValueType >
inline		// can this func inline?
static void
swapRow ( ValueType* aa, ValueType* bb )
{
	ValueType tmp = aa[ 0 ];	aa[ 0 ] = bb[ 0 ];	bb[ 0 ] = tmp;
			  tmp = aa[ 1 ];	aa[ 1 ] = bb[ 1 ];	bb[ 1 ] = tmp;
			  tmp = aa[ 2 ];	aa[ 2 ] = bb[ 2 ];	bb[ 2 ] = tmp;
			  tmp = aa[ 3 ];	aa[ 3 ] = bb[ 3 ];	bb[ 3 ] = tmp;
}

void
matrix4::
transpose ()
{ 
	MatSwap ( mat[ 0 ][ 1 ], mat[ 1 ][ 0 ] );
	MatSwap ( mat[ 0 ][ 2 ], mat[ 2 ][ 0 ] );
	MatSwap ( mat[ 0 ][ 3 ], mat[ 3 ][ 0 ] );
	MatSwap ( mat[ 1 ][ 2 ], mat[ 2 ][ 1 ] );
	MatSwap ( mat[ 1 ][ 3 ], mat[ 3 ][ 1 ] );
	MatSwap ( mat[ 2 ][ 3 ], mat[ 3 ][ 2 ] );
}

#ifdef NOUNWIND

void
matrix4::
invert ()
{ 
	matrix4 dest;

	for ( int d1 = 0; d1 < 4; d1++ )
	{
		int tmpd = d1;
		
		// find row with biggest value in this column
		{for ( int d2 = d1 + 1; d2 < 4; d2++ )
		{
			if ( TraitType::abs ( mat[ d2 ][ d1 ] ) > TraitType::abs ( mat[ tmpd ][ d1 ] ) )
				tmpd = d2;
		}}
		
		// swap rows
		if ( tmpd != d1 )
		{
			for ( int d2 = 0; d2 < 4; d2++ )
			{
				MatSwap ( mat[ tmpd ][ d2 ], mat[ d1 ][ d2 ] );
				MatSwap ( dest.mat[ tmpd ][ d2 ], dest.mat[ d1 ][ d2 ] );
			}
		}
		
		if ( mat[ d1 ][ d1 ] == TraitType::zeroVal )
		{
			PNIMATHCERR << "cannot invert singular matrix" << PNIMATHENDL;
			return;
		}
		
		// divide rows by the diagonal
		ValueType val = mat[ d1 ][ d1 ];
		{for ( int d2 = 0; d2 < 4; d2++ )
		{
			dest.mat[ d1 ][ d2 ] /= val;
			mat[ d1 ][ d2 ] /= val;
		}}
		
		// cancel out non-diagonal values
		{for ( int d2 = 0; d2 < 4; d2++ )
		{
			if ( d2 != d1 )
			{
				ValueType val2 = mat[ d2 ][ d1 ];
				
				for ( int d3 = 0; d3 < 4; d3++ )
				{
					dest.mat[ d2 ][ d3 ]	-= val2 * dest.mat[ d1 ][ d3 ];
					mat[ d2 ][ d3 ]			-= val2 * mat[ d1 ][ d3 ];
				}
			}
		}}
	}
	
	*this = dest;
}

#else

bool
matrix4::
invert ()
{ 
	matrix4 dest;

	for ( int d1 = 0; d1 < 4; d1++ )
	{
		int tmpd = d1;
		
		// find row with biggest value in this column
		for ( int d3 = d1 + 1; d3 < 4; d3++ )
		{
			ValueType tmpdVal = TraitType::abs ( mat[ tmpd ][ d1 ] );
			if ( TraitType::abs ( mat[ d3 ][ d1 ] ) > tmpdVal )
				tmpd = d3;
		}
		
		// swap rows
		if ( tmpd != d1 )
		{
			swapRow ( mat[ tmpd ], mat[ d1 ] );
			swapRow ( dest.mat[ tmpd ], dest.mat[ d1 ] );
		}
		
		if ( mat[ d1 ][ d1 ] == TraitType::zeroVal )
		{
#ifdef PNIMATHDEBUG
			PNIMATHCERR << "cannot invert singular matrix" << PNIMATHENDL;
#endif
			return false;
		}
		
		// divide rows by the diagonal
		ValueType val = mat[ d1 ][ d1 ];
		
		dest.mat[ d1 ][ 0 ] /= val;
		mat[ d1 ][ 0 ] /= val;

		dest.mat[ d1 ][ 1 ] /= val;
		mat[ d1 ][ 1 ] /= val;

		dest.mat[ d1 ][ 2 ] /= val;
		mat[ d1 ][ 2 ] /= val;

		dest.mat[ d1 ][ 3 ] /= val;
		mat[ d1 ][ 3 ] /= val;
		
		// cancel out non-diagonal values
		for ( int d2 = 0; d2 < 4; d2++ )
		{
			if ( d2 != d1 )
			{
				ValueType val2 = mat[ d2 ][ d1 ];
				
				dest.mat[ d2 ][ 0 ]	-= val2 * dest.mat[ d1 ][ 0 ];
				mat[ d2 ][ 0 ]			-= val2 * mat[ d1 ][ 0 ];

				dest.mat[ d2 ][ 1 ]	-= val2 * dest.mat[ d1 ][ 1 ];
				mat[ d2 ][ 1 ]			-= val2 * mat[ d1 ][ 1 ];

				dest.mat[ d2 ][ 2 ]	-= val2 * dest.mat[ d1 ][ 2 ];
				mat[ d2 ][ 2 ]			-= val2 * mat[ d1 ][ 2 ];

				dest.mat[ d2 ][ 3 ]	-= val2 * dest.mat[ d1 ][ 3 ];
				mat[ d2 ][ 3 ]			-= val2 * mat[ d1 ][ 3 ];
			}
		}
	}
	
	*this = dest;
	
	return true;
}

#endif
		
bool
matrix4::
operator == ( const matrix4& matIn ) const
{
	const ValueType* start = mat[ 0 ];
	const ValueType* data = matIn.mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
	{
		if ( start[ num ] != data[ num ] )
			return false;
	}
	
	return true;
}

bool
matrix4::
equal ( const matrix4& matIn, ValueType fuzz ) const
{
	const ValueType* start = mat[ 0 ];
	const ValueType* data = matIn.mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
	{
		if ( ! TraitType::equal ( start[ num ], data[ num ], fuzz ) )
			return false;
	}
	
	return true;
}

matrix4&
matrix4::
operator = ( const matrix4& matIn )
{ 
	set ( matIn.mat[ 0 ] );		
	return *this;
}

matrix4
matrix4::
operator + ( const matrix4& matIn ) const
{ 
	matrix4 retMat;
	ValueType* ret = retMat.mat[ 0 ];
	
	const ValueType* start = mat[ 0 ];
	const ValueType* data = matIn.mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = start[ num ] + data[ num ];
		
	return retMat;
}

matrix4
matrix4::
operator - ( const matrix4& matIn ) const
{ 
	matrix4 retMat;
	ValueType* ret = retMat.mat[ 0 ];
	
	const ValueType* start = mat[ 0 ];
	const ValueType* data = matIn.mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = start[ num ] - data[ num ];
		
	return retMat;
}

inline		// can this func inline?
matrix4::ValueType 
RCD( const matrix4& A, const matrix4& B, int i, int j)
{
  return	A.mat[i][0] * B.mat[0][j] +
			A.mat[i][1] * B.mat[1][j] +
			A.mat[i][2] * B.mat[2][j] +
			A.mat[i][3] * B.mat[3][j];
}

matrix4
matrix4::
operator * ( const matrix4& M ) const
{ 
	return matrix4 (  
		   RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), 
		   RCD(*this, M, 0, 2), RCD(*this, M, 0, 3),
		   RCD(*this, M, 1, 0), RCD(*this, M, 1, 1),
		   RCD(*this, M, 1, 2), RCD(*this, M, 1, 3),
		   RCD(*this, M, 2, 0), RCD(*this, M, 2, 1),
		   RCD(*this, M, 2, 2), RCD(*this, M, 2, 3),
		   RCD(*this, M, 3, 0), RCD(*this, M, 3, 1),
		   RCD(*this, M, 3, 2), RCD(*this, M, 3, 3) );
}

matrix4
matrix4::
operator / ( const matrix4& matIn ) const
{ 
	PNIMATHUNIMPLEMENTED;
	
	return matrix4 ();
}

matrix4
matrix4::
operator * ( ValueType val ) const
{ 
	matrix4 retMat;
	ValueType* ret = retMat.mat[ 0 ];
	
	const ValueType* start = mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = start[ num ] * val;
		
	return retMat;
}

matrix4
matrix4::
operator / ( ValueType val ) const
{ 
	matrix4 retMat;
	ValueType* ret = retMat.mat[ 0 ];
	
	const ValueType* start = mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = start[ num ] / val;
		
	return retMat;
}

matrix4&
matrix4::
operator += ( const matrix4& matIn )
{ 
	ValueType* ret = mat[ 0 ];
	const ValueType* start = matIn.mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = start[ num ] + ret[ num ];
		
	return *this;
}

matrix4&
matrix4::
operator -= ( const matrix4& matIn )
{ 
	ValueType* ret = mat[ 0 ];
	const ValueType* start = matIn.mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = start[ num ] - ret[ num ];
		
	return *this;
}

matrix4&
matrix4::
operator *= ( const matrix4& M )
{ 
	set (  
		   RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), 
		   RCD(*this, M, 0, 2), RCD(*this, M, 0, 3),
		   RCD(*this, M, 1, 0), RCD(*this, M, 1, 1),
		   RCD(*this, M, 1, 2), RCD(*this, M, 1, 3),
		   RCD(*this, M, 2, 0), RCD(*this, M, 2, 1),
		   RCD(*this, M, 2, 2), RCD(*this, M, 2, 3),
		   RCD(*this, M, 3, 0), RCD(*this, M, 3, 1),
		   RCD(*this, M, 3, 2), RCD(*this, M, 3, 3) );
		   
	return *this;
}

matrix4&
matrix4::
operator /= ( const matrix4& amat )
{ 
	PNIMATHUNIMPLEMENTED;
	
	return *this;
}

matrix4&
matrix4::
operator *= ( ValueType val )
{ 
	ValueType* ret = mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = ret[ num ] * val;
		
	return *this;
}

matrix4&
matrix4::
operator /= ( ValueType val )
{ 
	ValueType* ret = mat[ 0 ];
	for ( int num = 0; num < 16; num++ )
		ret[ num ] = ret[ num ] / val;
		
	return *this;
}

/////////////////////////////////////////////////////////////////////
// row/column methods

void
matrix4::
setRow ( int row, ValueType v0, ValueType v1, ValueType v2, ValueType v3 )
{ 
	mat[ row ][ 0 ] = v0;
	mat[ row ][ 1 ] = v1;
	mat[ row ][ 2 ] = v2;
	mat[ row ][ 3 ] = v3;
}

void
matrix4::
getRow ( int row, ValueType& v0, ValueType& v1, ValueType& v2, ValueType& v3 ) const
{ 
	v0 = mat[ row ][ 0 ];
	v1 = mat[ row ][ 1 ];
	v2 = mat[ row ][ 2 ];
	v3 = mat[ row ][ 3 ];
}

void
matrix4::
setRow ( int row, ValueType v0, ValueType v1, ValueType v2 )
{ 
	mat[ row ][ 0 ] = v0;
	mat[ row ][ 1 ] = v1;
	mat[ row ][ 2 ] = v2;
}

void
matrix4::
getRow ( int row, ValueType& v0, ValueType& v1, ValueType& v2 ) const
{ 
	v0 = mat[ row ][ 0 ];
	v1 = mat[ row ][ 1 ];
	v2 = mat[ row ][ 2 ];
}

//

void
matrix4::
setRow ( int row, const vec4& vec )
{ 
	mat[ row ][ 0 ] = vec.vec[ 0 ];
	mat[ row ][ 1 ] = vec.vec[ 1 ];
	mat[ row ][ 2 ] = vec.vec[ 2 ];
	mat[ row ][ 3 ] = vec.vec[ 3 ];
}

void
matrix4::
getRow ( int row, vec4& vec ) const
{ 
	vec.vec[ 0 ] = mat[ row ][ 0 ];
	vec.vec[ 1 ] = mat[ row ][ 1 ];
	vec.vec[ 2 ] = mat[ row ][ 2 ];
	vec.vec[ 3 ] = mat[ row ][ 3 ];
}

void
matrix4::
setRow ( int row, const vec3& vecIn )
{ 
	mat[ row ][ 0 ] = vecIn.vec[ 0 ];
	mat[ row ][ 1 ] = vecIn.vec[ 1 ];
	mat[ row ][ 2 ] = vecIn.vec[ 2 ];
}

void
matrix4::
getRow ( int row, vec3& vecIn ) const
{ 
	vecIn.vec[ 0 ] = mat[ row ][ 0 ];
	vecIn.vec[ 1 ] = mat[ row ][ 1 ];
	vecIn.vec[ 2 ] = mat[ row ][ 2 ];
}

////

void
matrix4::
setCol ( int col, ValueType v0, ValueType v1, ValueType v2, ValueType v3 )
{ 
	mat[ 0 ][ col ] = v0;
	mat[ 1 ][ col ] = v1;
	mat[ 2 ][ col ] = v2;
	mat[ 3 ][ col ] = v3;
}

void
matrix4::
getCol ( int col, ValueType& v0, ValueType& v1, ValueType& v2, ValueType& v3 ) const
{ 
	v0 = mat[ 0 ][ col ];
	v1 = mat[ 1 ][ col ];
	v2 = mat[ 2 ][ col ];
	v3 = mat[ 3 ][ col ];
}
void
matrix4::
setCol ( int col, ValueType v0, ValueType v1, ValueType v2 )
{ 
	mat[ 0 ][ col ] = v0;
	mat[ 1 ][ col ] = v1;
	mat[ 2 ][ col ] = v2;
}

void
matrix4::
getCol ( int col, ValueType& v0, ValueType& v1, ValueType& v2 ) const
{ 
	v0 = mat[ 0 ][ col ];
	v1 = mat[ 1 ][ col ];
	v2 = mat[ 2 ][ col ];
}

//

void
matrix4::
setCol ( int col, const vec4& vec )
{ 
	mat[ 0 ][ col ] = vec.vec[ 0 ];
	mat[ 1 ][ col ] = vec.vec[ 1 ];
	mat[ 2 ][ col ] = vec.vec[ 2 ];
	mat[ 3 ][ col ] = vec.vec[ 3 ];
}

void
matrix4::
getCol ( int col, vec4& vec ) const
{ 
	vec.vec[ 0 ] = mat[ 0 ][ col ];
	vec.vec[ 1 ] = mat[ 1 ][ col ];
	vec.vec[ 2 ] = mat[ 2 ][ col ];
	vec.vec[ 3 ] = mat[ 3 ][ col ];
}
void
matrix4::
setCol ( int col, const vec3& vecIn )
{ 
	mat[ 0 ][ col ] = vecIn.vec[ 0 ];
	mat[ 1 ][ col ] = vecIn.vec[ 1 ];
	mat[ 2 ][ col ] = vecIn.vec[ 2 ];
}

void
matrix4::
getCol ( int col, vec3& vecIn ) const
{ 
	vecIn.vec[ 0 ] = mat[ 0 ][ col ];
	vecIn.vec[ 1 ] = mat[ 1 ][ col ];
	vecIn.vec[ 2 ] = mat[ 2 ][ col ];
}

/////////////////////////////////////////////////////////////////////

void
matrix4::
preMult ( const matrix4& M )
{ 
	set (  
		   RCD(M, *this, 0, 0), RCD(M, *this, 0, 1), 
		   RCD(M, *this, 0, 2), RCD(M, *this, 0, 3),
		   RCD(M, *this, 1, 0), RCD(M, *this, 1, 1),
		   RCD(M, *this, 1, 2), RCD(M, *this, 1, 3),
		   RCD(M, *this, 2, 0), RCD(M, *this, 2, 1),
		   RCD(M, *this, 2, 2), RCD(M, *this, 2, 3),
		   RCD(M, *this, 3, 0), RCD(M, *this, 3, 1),
		   RCD(M, *this, 3, 2), RCD(M, *this, 3, 3) );
}

void
matrix4::
postMult ( const matrix4& M )
{ 
	set (  
		   RCD(*this, M, 0, 0), RCD(*this, M, 0, 1), 
		   RCD(*this, M, 0, 2), RCD(*this, M, 0, 3),
		   RCD(*this, M, 1, 0), RCD(*this, M, 1, 1),
		   RCD(*this, M, 1, 2), RCD(*this, M, 1, 3),
		   RCD(*this, M, 2, 0), RCD(*this, M, 2, 1),
		   RCD(*this, M, 2, 2), RCD(*this, M, 2, 3),
		   RCD(*this, M, 3, 0), RCD(*this, M, 3, 1),
		   RCD(*this, M, 3, 2), RCD(*this, M, 3, 3) );
}

/////////////////////////////////////////////////////////////////////

void
matrix4::
rowMult ( matrix4& dmat, const vec4& svec ) const
{
	dmat.set ( 
			dmat[ 0 ][ 0 ] * svec[ 0 ],
			dmat[ 0 ][ 1 ] * svec[ 1 ],
			dmat[ 0 ][ 2 ] * svec[ 2 ],
			dmat[ 0 ][ 3 ] * svec[ 3 ],

			dmat[ 1 ][ 0 ] * svec[ 0 ],
			dmat[ 1 ][ 1 ] * svec[ 1 ],
			dmat[ 1 ][ 2 ] * svec[ 2 ],
			dmat[ 1 ][ 3 ] * svec[ 3 ],

			dmat[ 2 ][ 0 ] * svec[ 0 ],
			dmat[ 2 ][ 1 ] * svec[ 1 ],
			dmat[ 2 ][ 2 ] * svec[ 2 ],
			dmat[ 2 ][ 3 ] * svec[ 3 ],

			dmat[ 3 ][ 0 ] * svec[ 0 ],
			dmat[ 3 ][ 1 ] * svec[ 1 ],
			dmat[ 3 ][ 2 ] * svec[ 2 ],
			dmat[ 3 ][ 3 ] * svec[ 3 ] );
}

void
matrix4::
rowMult ( vec4& dvec, const matrix4& smat ) const
{
	dvec.set (
			smat[ 0 ][ 0 ] * dvec[ 0 ] +
			smat[ 1 ][ 0 ] * dvec[ 1 ] +
			smat[ 2 ][ 0 ] * dvec[ 2 ] +
			smat[ 3 ][ 0 ] * dvec[ 3 ],

			smat[ 0 ][ 1 ] * dvec[ 0 ] +
			smat[ 1 ][ 1 ] * dvec[ 1 ] +
			smat[ 2 ][ 1 ] * dvec[ 2 ] +
			smat[ 3 ][ 1 ] * dvec[ 3 ],

			smat[ 0 ][ 2 ] * dvec[ 0 ] +
			smat[ 1 ][ 2 ] * dvec[ 1 ] +
			smat[ 2 ][ 2 ] * dvec[ 2 ] +
			smat[ 3 ][ 2 ] * dvec[ 3 ],

			smat[ 0 ][ 3 ] * dvec[ 0 ] +
			smat[ 1 ][ 3 ] * dvec[ 1 ] +
			smat[ 2 ][ 3 ] * dvec[ 2 ] +
			smat[ 3 ][ 3 ] * dvec[ 3 ] );
}

void
matrix4::
colMult ( const matrix4& smat, vec4& dvec ) const
{
	smat.rowMult ( dvec, smat );

//	dvec.set (
//			smat[ 0 ][ 0 ] * dvec[ 0 ] +
//			smat[ 1 ][ 0 ] * dvec[ 1 ] +
//			smat[ 2 ][ 0 ] * dvec[ 2 ] +
//			smat[ 3 ][ 0 ] * dvec[ 3 ],
//
//			smat[ 0 ][ 1 ] * dvec[ 0 ] +
//			smat[ 1 ][ 1 ] * dvec[ 1 ] +
//			smat[ 2 ][ 1 ] * dvec[ 2 ] +
//			smat[ 3 ][ 1 ] * dvec[ 3 ],
//
//			smat[ 0 ][ 2 ] * dvec[ 0 ] +
//			smat[ 1 ][ 2 ] * dvec[ 1 ] +
//			smat[ 2 ][ 2 ] * dvec[ 2 ] +
//			smat[ 3 ][ 2 ] * dvec[ 3 ],
//
//			smat[ 0 ][ 3 ] * dvec[ 0 ] +
//			smat[ 1 ][ 3 ] * dvec[ 1 ] +
//			smat[ 2 ][ 3 ] * dvec[ 2 ] +
//			smat[ 3 ][ 3 ] * dvec[ 3 ] );
}

void
matrix4::
colMult ( const vec4& svec, matrix4& dmat ) const
{
	dmat.rowMult ( dmat, svec );

//	dmat.set ( 
//			dmat[ 0 ][ 0 ] * svec[ 0 ],
//			dmat[ 0 ][ 1 ] * svec[ 1 ],
//			dmat[ 0 ][ 2 ] * svec[ 2 ],
//			dmat[ 0 ][ 3 ] * svec[ 3 ],
//
//			dmat[ 1 ][ 0 ] * svec[ 0 ],
//			dmat[ 1 ][ 1 ] * svec[ 1 ],
//			dmat[ 1 ][ 2 ] * svec[ 2 ],
//			dmat[ 1 ][ 3 ] * svec[ 3 ],
//
//			dmat[ 2 ][ 0 ] * svec[ 0 ],
//			dmat[ 2 ][ 1 ] * svec[ 1 ],
//			dmat[ 2 ][ 2 ] * svec[ 2 ],
//			dmat[ 2 ][ 3 ] * svec[ 3 ],
//
//			dmat[ 3 ][ 0 ] * svec[ 0 ],
//			dmat[ 3 ][ 1 ] * svec[ 1 ],
//			dmat[ 3 ][ 2 ] * svec[ 2 ],
//			dmat[ 3 ][ 3 ] * svec[ 3 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni


