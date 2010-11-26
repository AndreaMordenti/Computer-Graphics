
/***************************************************************
 ****	Libreria utile per eseguire operazioni su vettori	****
 ***************************************************************	
 Simone Castorri
*/

#include <math.h>
/*#include <stdio.h>*/

#include "v3d.h"

static float evil[3] = {666.0, 666.0, 666.0};

/* dst = src */
void v3dSet( float dst[], const float src[] )
{
	int i;
	for( i=0 ; i<3 ; i++)
		dst[i] = src[i];
}

/* dst = (vx, vy, vz) */
void v3dSet2( float dst[], float vx, float vy, float vz )
{
	dst[0] = vx;
	dst[1] = vy;
	dst[2] = vz;
}

/* return 1 (true) if each coordinate of v1 is equal to each coordinate of v2, otherwise 0 (false) */
int v3dEqual( const float v1[], const float v2[] )
{	
	int i;
	for( i=0 ; i<3 ; i++ )
		if( (v1[i] - v2[i]) != 0.0 )
			return 0;
	return 1;
}

int v3dEqualThreshold( const float v1[], const float v2[], float threshold )
{	
	int i;
	for( i=0 ; i<3 ; i++ )
		if( fabs(v1[i] - v2[i]) > threshold )
			return 0;
	return 1;
}

/* res = v1 + v2 */
void v3dSum( const float v1[], const float v2[], float res[] )
{
    int i=0;
	for(; i<3 ; i++)	res[i] = v1[i] + v2[i];
}

/* res = k * v */
void v3dMul( const float v[], const float k, float res[] )
{
	int i = 0;
	for(; i<3; i++)	res[i] = k * v[i];
}

/* set all cooardinates to zero */
void v3dSetZero( float v[] )
{
	int i = 0;
	for(; i<3 ; i++)	v[i] = 0;
}

/* res = v1 - v2 */
void v3dSub( const float v1[], const float v2[], float res[] )
{
	int i=0;
	for(; i<3 ; i++)	res[i] = v1[i] - v2[i];
}

/* returns the euclidean norm of v */
float v3dLength( const float v[] )
{	
	int i = 0;
	float res = 0.0;
	for(; i<3 ; i++)	res += pow(v[i],2);
	return sqrt(res);

}

/* returns the euclidean norm of v */
float v3dLength2( const float v[] )
{	
	int i = 0;
	float res = 0.0;
	for(; i<3 ; i++)	res += pow(v[i],2);
	return res;
}

/* normalize v */
void v3dNormalize( float v[] )
{
	v3dMul(v , 1/v3dLength(v) , v); 
}

/* returns <v1,v2> (the scalar product between v1 and v2) */
float v3dDot( const float v1[], const float v2[] )
{
	int i = 0;
	float res = 0.0;

	for(; i<3; i++)	res += v1[i]*v2[i];
	return res;
}

/* res = v1 ^ v2 (cross product) */
void v3dCross( const float v1[], const float v2[], float res[])
{	
	res[0] = v1[1]*v2[2] - v1[2]*v2[1];
	res[1] = v1[2]*v2[0] - v1[0]*v2[2];
	res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

/* returns the distance between v1 and v2 */
float v3dDist( const float v1[], const float v2[] )
{	
	float res[3];
	v3dSub(v1 , v2 , res);
	return v3dLength(res);
}


