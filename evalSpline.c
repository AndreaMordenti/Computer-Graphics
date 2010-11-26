#include "evalSpline.h"

float roundf( float a )
{
	return ( a >= 0 ) ? floor( a + 0.5 ) : floor ( a - 0.5 ) ;
}

void computeBSplines( struct NodalPartition* nodes, float basis[MaxNumPts][MaxOrder+1][NumTotEvaluations] )
{
	int order = nodes->m;
	int k = nodes->k;

	int i;
	int m;
	float t;
	int param;
	float* pos = &(nodes->pos[0]);

	int a, b;
	float n1, n2, d1, d2;
	
	for( m=1 ; m<=order ; m++ )
	{
		for( i=0 ; i < ((order+k) + (order-m)) ; i++ ) //triangular
		{
			//basis[i][m][t] e' non zero nell'intervallo [i;i+m]

			//nodi coincidenti ( la spline N[i][m][t] e' nulla )
			if( pos[i] == pos[i+m] )
				continue;

			a = roundf( (NumTotEvaluations-1) * pos[i] );
			b = roundf( (NumTotEvaluations-1) * pos[i+m] );
		
			d1 = (pos[i+m-1] - pos[i] );
			d2 = (pos[i+m] - pos[i+1]);

			for( param=a ; param<b ; param++ ) 
			{
				t = (float)param * evalStep;
				if( m == 1 )
				{
					basis[i][m][param] = 1.0;
				}
				else
				{
					if( d1 != 0 )
					{
						n1 = ( t - pos[i] );
						basis[i][m][param] += (n1/d1) * basis[i][m-1][param];
					}
					if( d2 != 0 )
					{
						n2 = (pos[i+m] - t);
						basis[i][m][param] += (n2/d2) * basis[i+1][m-1][param];
					}
				}
			}
		}
	}
}

int findIntervalFromPosition( NodalPartition* nodes, float pos )
{
	int i;
	int nn = nodes->numNodes-1;
	for( i=0 ; i<nn ; i++ )
		if( pos >= nodes->pos[i] && pos < nodes->pos[i+1] )
			return i;

	printf( "Interval not found... " );
	return -1;
}

/** this function moves all nodes of the partition at multiples of evalStep. This avoid numerical errors due to the discretization of the parameter space.
 *
 */ 
void snapPartitionNodes( NodalPartition* nodes )
{
	int i;
	float snappedPos;

	for( i=1 ; i< nodes->numNodes-1 ; i++ )
	{
		snappedPos = roundf( nodes->pos[i] / evalStep ) * evalStep;
		nodes->pos[i] = snappedPos;
	}

}
