/////////////////////////////////////////////////////////////////////
//
//	quat.C
//
/////////////////////////////////////////////////////////////////////

#include "pniquat.h"
#include "pnimatrix4.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////
//
// From plan9 at Lucent:
// A rotation by angle THETA about axis A (where A is a unit vector)
// can be represented by the unit quaternion q=(cos THETA/2, Asin THETA/2).

void
quat::
setRot ( ValueType angle, ValueType xval, ValueType yval, ValueType zval )
{
	ValueType halfAngleRad = TraitType::d2r ( angle / ( ValueType ) 2.0 );
	ValueType sinTheta = TraitType::sin ( halfAngleRad );
	
	vec[ 0 ] = xval * sinTheta;
	vec[ 1 ] = yval * sinTheta;
	vec[ 2 ] = zval * sinTheta;
	vec[ 3 ] = TraitType::cos ( halfAngleRad );
}

bool
quat::
getRot ( ValueType& angle, ValueType& xval, ValueType& yval, ValueType& zval ) const
{
	ValueType halfAngleRad = TraitType::acos ( vec[ 3 ] );
	ValueType sinTheta = TraitType::sin ( halfAngleRad );

	if ( TraitType::equal ( sinTheta, TraitType::zeroVal ) )
	{
		angle = TraitType::zeroVal;
		xval = TraitType::oneVal;
		yval = TraitType::zeroVal;
		zval = TraitType::zeroVal;
	}
	else
	{
		xval = vec[ 0 ] / sinTheta;
		yval = vec[ 1 ] / sinTheta;
		zval = vec[ 2 ] / sinTheta;
		angle = TraitType::r2d ( ( ValueType ) 2.0 * halfAngleRad );
	}

	return true;
}

// notes for norm method
/////////////////////////////////////////////////////////////////////
//
// from the Performer pfquat man page:
// length is defined as the norm of all four quaternion components
// PNI but they don't really mean length... they mean the norm!
//
/////////////////////////////////////////////////////////////////////

void
quat::
exp ()
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    ValueType angle = TraitType::sqrt ( ( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] ) );
    ValueType sina = TraitType::sin ( angle );

    vec[3] = TraitType::cos ( angle );

    if ( !TraitType::equal ( sina, 0 ) )  // not close to zero.
    {
        ValueType coeff = sina / angle;  // this is safe, because if sin ( angle ) != 0 then angle != 0.
        vec[0] *= coeff;
        vec[1] *= coeff;
        vec[2] *= coeff;
    }

	/*
	// shen's original code.
    double angle = sqrt((double)(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]));
    double sina = sin(angle);

    vec[3] = cos(angle);

    if ( fabs(sina) >= 1.0e-6 )
    {
        ValueType fCoeff = sina /angle;
        vec[0] = fCoeff * vec[0];
        vec[1] = fCoeff * vec[1];
        vec[2]=  fCoeff * vec[2];
    }
	*/
}

void
quat::
log ()
{
	ValueType angle;

	ValueType w = vec[3];
    if ( fabs ( w ) < 1.0 )
    {
		if ( w > -1.0 )
			angle = TraitType::acos ( w );
		else
			angle = TraitType::piRadVal;

		ValueType sina = TraitType::sin ( angle );
    
		if ( !TraitType::equal ( sina, 0 ) )
		{
			ValueType coeff = angle / sina;
			vec[0] *= coeff;
			vec[1] *= coeff;
			vec[2] *= coeff;
		}
	}
    vec[3] = 0;

	/*
	// shen's original code.
	double angle;

	double w = vec[3];
    if ( fabs(w) < 1.0 )
    {
		if ( w > -1.0)
				angle = acos(w);
		else
			angle = M_PI;

		double sina = sin(angle);
    
		if ( fabs(sina) > 1.0e-06 )
		{
				float  coeff = float(angle/sina);
				vec[0] *= coeff;
				vec[1] *= coeff;
				vec[2] *= coeff;
		}
	}

    vec[3] = 0.0;
	*/

}




// notes for invert method ( inlined in the header )
/////////////////////////////////////////////////////////////////////
// 
// from http://www.cis.ohio-state.edu/~parent/book/Full.html :
// The inverse of a quaternion is obtained by negating its vector part 
// and dividing both parts by the magnitude squared: 
// 
// q-1 = (1/||q||2)*[s,-v] where ||q||2 = s2 + v.v 
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//from the Performer pfquat man page:
//	 Because both q and -q represent the same rotation (quaternions have a
//     rotation range of [-360,360] degrees) conversions such as
//     pfMatrix::getOrthoquat set an arbitrary choice of the sign of the
//     returned quaternion.  To prevent the arbitrary sign from introducing
//     large, unintended rotations, pfquat::slerp checks the angle theta between
//     q1 and q2.  If theta exceeds 180 degrees, q2 is negated changing the
//     interpolations range from [0,theta] to [0, theta-360 degrees].

void
quat::
lerp ( ValueType t, const quat &q1, const quat &q2 )
{
	if ( q1.dot ( q2 ) < TraitType::zeroVal )
	{
		quat negq2 ( q2 );
		negq2.negate ();
		
		combine ( TraitType::oneVal - t, q1, t, negq2 );
	}
	else
		combine ( TraitType::oneVal - t, q1, t, q2 );
	
	normalize ();						// Performer doesn't normalize result?
}

/////////////////////////////////////////////////////////////////////
//	From http://www.cis.ohio-state.edu/~parent/book/Full.html:
//	  Slerp(q1,q2,u) = (sin((1-u)*¿)/sin(¿))*q1 + (sin(u*¿)/sin(¿))*q2 
//	  where q1.q2 = cos(¿)

void
quat::
slerp ( ValueType t, const quat &q1, const quat &q2 )
{
	ValueType dotVal = q1.dot ( q2 );

	// when the quats are very close, we just do lerp...
	// it behaves better and faster in these conditions
	const ValueType TooClose = ( ValueType ) 0.99;
	if ( TraitType::abs ( dotVal ) >= TooClose )
	{
		PNIMATHUNTESTED;
		lerp ( t, q1, q2 );
	}
	else
	{
		quat tmpq2 ( q2 );
		
		if ( dotVal < TraitType::zeroVal )
			tmpq2.negate ();

		ValueType val = TraitType::acos ( q1.dot ( tmpq2 ) );
		ValueType sinVal = TraitType::sin ( val );
		
		quat tmp ( q1 );
		tmp.vec4::operator *= 
				( TraitType::sin ( ( TraitType::oneVal - t ) * val ) / sinVal );		
		tmpq2.vec4::operator *= 
				( TraitType::sin ( t * val ) / sinVal );
		
		tmp += tmpq2;
		
		*this = tmp;
	}
	
	normalize ();						// Performer doesn't normalize result?
}

void
quat::
squad ( ValueType t, const quat &q1, const quat &q2, const quat &a, const quat &b )
{
	//PNIMATHUNIMPLEMENTED;
    ValueType st = 2.0f* t *(1.0f- t);
    quat slerpP, slerpQ;
	slerpP.slerp(t,q1,q2);
    slerpQ.slerp(t,a,b);
    this->slerp(st,slerpP , slerpQ);
}

void
quat::
meanTangent ( const quat &q1, const quat &q2, const quat &q3 )
{
	PNIMATHUNIMPLEMENTED;
}

// notes on mult method
/////////////////////////////////////////////////////////////////////
//
// from http://www.cis.ohio-state.edu/~parent/book/Full.html :
// quaternion multiplication is defined as: 
// [s1,v1] * [s2,v2] = [(s1*s2-v1.v2),(s1*v2+s2*v1+v1xv2)] 
//
//	from Shoeset:
//	qq' = [v,w][v',w'] = [vxv'+wv'+w'v,ww'-v.v']
//	PNI this unrolled routine is directly quoted from Shoeset

/////////////////////////////////////////////////////////////////////


void
quat::
mult ( const quat& q1, const quat& q2 )
{
	set ( q1.vec[ 3 ] * q2.vec[ 0 ] +
			q1.vec[ 0 ] * q2.vec[ 3 ] +
			q1.vec[ 1 ] * q2.vec[ 2 ] -
			q1.vec[ 2 ] * q2.vec[ 1 ], 
			
			q1.vec[ 3 ] * q2.vec[ 1 ] +
			q1.vec[ 1 ] * q2.vec[ 3 ] +
			q1.vec[ 2 ] * q2.vec[ 0 ] -
			q1.vec[ 0 ] * q2.vec[ 2 ], 
			
			q1.vec[ 3 ] * q2.vec[ 2 ] +
			q1.vec[ 2 ] * q2.vec[ 3 ] +
			q1.vec[ 0 ] * q2.vec[ 1 ] -
			q1.vec[ 1 ] * q2.vec[ 0 ], 
			
			q1.vec[ 3 ] * q2.vec[ 3 ] -
			q1.vec[ 0 ] * q2.vec[ 0 ] -
			q1.vec[ 1 ] * q2.vec[ 1 ] -
			q1.vec[ 2 ] * q2.vec[ 2 ] );
}



/////////////////////////////////////////////////////////////////////

// UNOPTIMAL
void
quat::
setEuler ( ValueType heading, ValueType pitch, ValueType roll )
{
	matrix4 temp;

	temp.setEuler ( heading, pitch, roll );
	temp.getQuat ( vec[0], vec[1], vec[2], vec[3] );
}


// UNOPTIMAL
void
quat::
getEuler ( ValueType& heading, ValueType& pitch, ValueType& roll ) const
{
	matrix4 temp;

	temp.setQuat ( vec );
	temp.getEuler ( heading, pitch, roll );
}

	} // end namespace math
} // end namespace pni
