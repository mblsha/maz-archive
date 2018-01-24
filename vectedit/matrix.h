//
//	Shawn's matrix math routines.
//  Very quickly hacked by Damyan for speedhack


#ifndef __MATRIX_H__
#define __MATRIX_H__


#include <math.h>



// fudge factor for avoiding normalization overflows
#define VERY_TINY	0.00001



// why oh why is MSVC too dumb to define this?
#ifndef M_PI
	#define M_PI	3.141592
#endif



// forward declarations
struct Vector;

//#define MATRIX_USE_DOUBLE

// what type of data should we use?
#ifdef MATRIX_USE_DOUBLE
	typedef double mfloat;
#else
	typedef float mfloat;
#endif



// vector structure
struct Vector
{
	// vector contents
	mfloat x, y;

	// constructors
	inline Vector(void) { }
	inline Vector(const Vector &vec) 				{ x = vec.x; y = vec.y; }
	inline Vector(mfloat _x, mfloat _y)				{ x = _x;    y = _y; }
	inline Vector(mfloat _v[2]) 					{ x = _v[0]; y = _v[1]; }

	// member set routine
	inline void set(mfloat _x, mfloat _y)
	{
		x = _x;
		y = _y;
	}

	// member access via "v[index]"
	inline mfloat &operator[] (int i)
	{
		return (&x)[i];
	}

	// "v + v" addition
	inline friend Vector operator + (const Vector &v1, const Vector &v2)
	{
		return Vector(v1.x+v2.x, v1.y+v2.y);
	}

	// "v += v" addition
	inline Vector &operator += (const Vector &vec)
	{
		x += vec.x;
		y += vec.y;

		return *this;
	}

	// "v - v" subtraction
	inline friend Vector operator - (const Vector &v1, const Vector &v2)
	{
		return Vector(v1.x-v2.x, v1.y-v2.y);
	}

	// "v -= v" subtraction
	inline Vector &operator -= (const Vector &vec)
	{
		x -= vec.x;
		y -= vec.y;

		return *this;
	}

  
	// v * v 
	inline Vector operator *(const Vector& vec) 
	{
		return Vector(x * vec.x, y * vec.y);
	}

	// "v * float" scaling
	inline friend Vector operator * (const Vector &vec, mfloat s)
	{
		return Vector(vec.x*s, vec.y*s);
	}

	// "v *= float" scaling
	inline Vector &operator *= (mfloat s)
	{
		x *= s;
		y *= s;

		return *this;
	}

	// "v / float" scaling
	inline friend Vector operator / (const Vector &vec, mfloat s)
	{
		return Vector(vec.x/s, vec.y/s);
	}

	// "v /= float" scaling
	inline Vector &operator /= (mfloat s)
	{
		x /= s;
		y /= s;

		return *this;
	}

	// unary negation
	inline Vector operator - (void) const
	{
		return Vector(-x, -y);
	}

	// get the length of this vector
	inline mfloat magnitude(void) const
	{
		return sqrt(x*x + y*y);
	}

	// get the length of a vector
	inline friend mfloat magnitude(const Vector &vec)
	{
		return sqrt(vec.x*vec.x + vec.y*vec.y);
	}

	// get the squared length of this vector
	inline mfloat square_magnitude(void) const
	{
		return x*x + y*y;
	}

	// get the squared length of a vector
	inline friend mfloat square_magnitude(const Vector &vec)
	{
		return vec.x*vec.x + vec.y*vec.y;
	}

	// return 1.0 / magnitude()
	mfloat inverse_magnitude(void) const;
	friend mfloat inverse_magnitude(const Vector &vec);

	// normalize this vector
	inline void normalize(void)
	{
		mfloat len = inverse_magnitude();
		x *= len;
		y *= len;
	}

	// normalize this vector to a specific length
	inline void normalize(mfloat len)
	{
		len *= inverse_magnitude();
		x *= len;
		y *= len;
	}

	// normalize a vector
	inline friend Vector normalize(const Vector &vec)
	{
		mfloat len = ::inverse_magnitude(vec);
		return Vector(vec.x*len, vec.y*len);
	}

	// normalize a vector to a specific length
	inline friend Vector normalize(const Vector &vec, mfloat len)
	{
		len *= ::inverse_magnitude(vec);
		return Vector(vec.x*len, vec.y*len);
	}

	// overloaded ~ for vector normalization
	inline Vector operator ~ (void) const
	{
		mfloat len = inverse_magnitude();
		return Vector(x*len, y*len);
	}

	// calculate dot product
	inline friend mfloat dot_product(const Vector &v1, const Vector &v2)
	{
		return (v1.x*v2.x + v1.y*v2.y);
	}

	// "v | v" dot product
	inline friend mfloat operator | (const Vector &v1, const Vector &v2)
	{
		return (v1.x*v2.x + v1.y*v2.y);
	}

	// calculate cross product
	inline friend mfloat cross_product(const Vector &v1, const Vector &v2)
	{
        return v1.x * v2.y - v2.x * v1.y;
	}

	// "v ^ v" cross product
	inline friend mfloat operator ^ (const Vector &v1, const Vector &v2)
	{
        return v1.x * v2.y - v2.x * v1.y;
	}

	// "v == v" comparison
	inline friend int operator == (const Vector &v1, const Vector &v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y));
	}

	// "v != v" comparison
	inline friend int operator != (const Vector &v1, const Vector &v2)

	{
		return ((v1.x != v2.x) || (v1.y != v2.y));
	}

	// return a hashcode (for detecting identical verts)
	unsigned int hash();
};




#endif		// __MATRIX_H__

