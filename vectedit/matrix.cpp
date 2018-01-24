//
//	Shawn's matrix math routines.
//  Very quickly hacked by Damyan for speedhack

#include <string.h>
#include "matrix.h"

#define ABS abs

// find 1.0 / length of a vector
mfloat Vector::inverse_magnitude() const
{
	mfloat len = sqrt(x*x + y*y);

	if (len < VERY_TINY) {
		// PRINTF("Warning: trying to normalize a zero vector!\n");
		return 0;
	}

	return 1.0 / len;
}



// find 1.0 / length of a vector
mfloat inverse_magnitude(const Vector &vec)
{
	mfloat len = sqrt(vec.x*vec.x + vec.y*vec.y);

	if (len < VERY_TINY) {
		// PRINTF("Warning: trying to normalize a zero vector!\n");
		return 0;
	}

	return 1.0 / len;
}



// simple hashing function
unsigned int Vector::hash()
{
	mfloat hash = x + y;

	if (hash < 0)
		hash = -hash;

	return (unsigned int)hash;
}
