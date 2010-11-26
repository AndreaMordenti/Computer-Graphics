//===========================================================================
//	splineTutor.c
//===========================================================================

/* ===========================================================================
 *
 *	Author:
 *		Marco Rucci
 *		marco.rucci@gmail.com
 *
 *	Copyright (C) 2008
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ===========================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>

#include "evalSpline.h"
#include "v3d.h"

NodalPartition _nodes;
float spline[NumTotEvaluations][2];
float N[MaxNumPts][MaxOrder+1][NumTotEvaluations];


int switchBSplineBasisVisualization = 1;
int switchCurveVisualization = 1;

int numCV = 0;
float CV[MaxNumPts][2];
float weight[MaxNumPts]; /*Vettore rappresentante il peso associato ad ogni punto di controllo*/

// Window size in pixels
int WindowHeight;
int WindowWidth;

int partitionType = 1; // 1 -> uniform, 2 -> open uniform

int refreshCurve = 0;
int refreshNodalPartition = 0;
int refreshBSplines = 0;

float lineWidth;
float colors[MaxNodes][4];

int hoverNode = -1;
int draggingNode = -1;

int hoverCV = -1;
int draggingCV = -1;
float hside = 0.01f;

float _nodesSectionY = 0.10;
float _nodesPosY = 0.05;

int keyShortcut[32];
enum Commands
{
	COMMAND_INCREASE_SPLINE_ORDER,
	COMMAND_DECREASE_SPLINE_ORDER,

	COMMAND_MAKE_PARTITION_UNIFORM,
	COMMAND_MAKE_PARTITION_OPEN,


	COMMAND_REMOVE_FIRST_POINT,
	COMMAND_REMOVE_LAST_POINT,

	COMMAND_CHANGE_BSPLINE_VISUALIZATION,
	COMMAND_CHANGE_CURVE_VISUALIZATION,
	COMMAND_PRINT_SPLINE_INFO,
	COMMAND_PRINT_HELP,

	COMMAND_LOAD_SPLINE,
	COMMAND_SAVE_SPLINE,

};

char saveFileName[128];
char loadFileName[128];

int _darkBG = 1;

void removeFirstPoint() 
{
	int i;
	if ( numCV>0 ) 
	{
		// Remove the first point, slide the rest down
		numCV--;
		for ( i=0; i<numCV; i++ ) 
		{
			CV[i][0] = CV[i+1][0];
			CV[i][1] = CV[i+1][1];
		}
	}

	refreshNodalPartition = 1;
	refreshBSplines = 1;
	refreshCurve = 1;
}

// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void removeLastPoint() 
{
	if ( numCV>0 ) {
		numCV--;
	}

	refreshNodalPartition = 1;
	refreshBSplines = 1;
	refreshCurve = 1;
}

void printStatus( )
{
	int i;
	printf( "\n\n=================================================\n\n" );
	printf( "  m -> order: %d\n", _nodes.m );
	printf( "  k -> internal nodes: %d\n", _nodes.k );
	printf( "  numNodes: %d\n", _nodes.numNodes );
	printf( "  partition: " );
	for( i=0 ; i<_nodes.numNodes ; i++ )
		printf( "%.2f ", _nodes.pos[i] );
	printf( "\n" );
	//printf( "  a = %d    b = %d", _nodes.a, _nodes.b );
	printf( "\n=================================================\n\n" );
}

void printHelp()
{
	printf("\n-------------------- Splines ---------------------------");
	printf("\n|                                                      |");
	printf("\n| left-click to insert control points                  |");
	printf("\n| left-click and drag on control points to modify      |");
	printf("\n|    their positions.                                  |");
	printf("\n| press 'f' to remove first point                      |");
	printf("\n| press 'l' to remove last  point                      |");
	printf("\n|                                                      |");
	printf("\n| The drawn curve is a quadric spline, you can         |");
	printf("\n|   increase or decrease the spline order              |");
	printf("\n|   with the 'm' and 'M' keys.                         |");
	printf("\n|                                                      |");
	printf("\n| On the right you can see the B-Spline Basis          |");
	printf("\n|   functions and the nodal partition.                 |");
	printf("\n|                                                      |");
	printf("\n| left-click and drag on the nodes of the partition    |");
	printf("\n|    to modify their position                          |");
	printf("\n| press 'u' to make the partition uniform              |");
	printf("\n| press 'o' to make the partition open uniform         |");
	printf("\n|                                                      |");
	printf("\n| press 'v' to change curve visualization              |");
	printf("\n| press 'V' to change B-Spline basis visualization     |");
	printf("\n| press 'p' to print informations on the spline        |");
	printf("\n|                                                      |");
	printf("\n| press 'S' to save                                    |");
	printf("\n| press 'L' to (re)load                                |");
	printf("\n|                                                      |");
	printf("\n| press 'h' to reprint this help at any time           |");
	printf("\n--------------------------------------------------------\n");
}

int loadSpline( char* fName )
{
	FILE* file;
	int i; //,j;
	int res;
	char s[16];


	printf("Opening file %s\n", fName);
#ifdef WIN32
	if (( file = fopen( fName, "r")) == NULL)
#else
    if (( file = fopen( fName, "r")) == NULL)
#endif
	{
		printf("file non trovato\n");
		return 0;
	}

	res = fscanf( file,"%s %d",s, &_nodes.m );
	res = fscanf( file,"%s %d",s, &numCV );
	for( i=0 ; i<numCV ; i++ )
		res = fscanf( file,"%f %f", &CV[i][0], &CV[i][1] );
	
	res = fscanf( file,"%s %d",s, &_nodes.numNodes );
	for( i=0 ; i<_nodes.numNodes ; i++ )
		res = fscanf( file,"%f", &_nodes.pos[i] );

	res = fscanf(file,"%s",s);
	printf("scan %s\n",s);
	for( i=0 ; i<numCV ; i++){
		res = fscanf(file,"%f",&weight[i]);
		printf("new weight[%d]: %f\n",i,weight[i]);
	}
	fclose( file );

	
	_nodes.k = numCV - _nodes.m; ///< num internal nodes
	_nodes.a = _nodes.m - 1;
	_nodes.b = _nodes.m + _nodes.k;
	snapPartitionNodes( &_nodes );

	printf("Spline loaded\n");
	printStatus( );
	
	refreshBSplines = 1;
	refreshCurve = 1;
    
	return 1;
	
}

int saveSpline( char* fName )
{
	FILE* file;
	int i; //,j;


	printStatus( );
	printf("Opening file %s\n", fName );
#ifdef WIN32
	if (( file = fopen( fName, "w")) == NULL)
#else
    if (( file = fopen( fName, "w")) == NULL)
#endif
	{
		printf("problems saving file\n");
		return 0;
	}

	fprintf( file,"Order %d\n",_nodes.m );
	fprintf( file,"NumCV %d\n", numCV );
	for( i=0 ; i<numCV ; i++ )
		fprintf( file,"%f %f\n", CV[i][0], CV[i][1] );
	fprintf( file,"NumNodes %d\n", _nodes.numNodes );
	for( i=0 ; i<_nodes.numNodes ; i++ )
		fprintf( file,"%f\n", _nodes.pos[i] );

	fclose( file );
	printf( "spline saved to %s\n", fName );
	return 1;
	
}

void myKeyboardFunc (unsigned char key, int x, int y)
{
	//int i;
	if( key == keyShortcut[COMMAND_MAKE_PARTITION_UNIFORM] )
	{
		partitionType = 1;
		refreshNodalPartition = 1;
		refreshBSplines = 1;
		refreshCurve = 1;
	}
	else if( key == keyShortcut[COMMAND_MAKE_PARTITION_OPEN] )
	{
		partitionType = 2;
		refreshNodalPartition = 1;
		refreshBSplines = 1;
		refreshCurve = 1;
	}
	else if( key == keyShortcut[COMMAND_CHANGE_BSPLINE_VISUALIZATION] )
	{
		switchBSplineBasisVisualization = (switchBSplineBasisVisualization + 1) % 2;
	}
	else if( key == keyShortcut[COMMAND_CHANGE_CURVE_VISUALIZATION] )
	{
		switchCurveVisualization = (switchCurveVisualization + 1) % 2;
	}
	else if( key == keyShortcut[COMMAND_PRINT_SPLINE_INFO] )
	{
		printStatus( );
	}
	else if( key == keyShortcut[COMMAND_PRINT_HELP] )
	{
		printHelp( );
	}
	else if( key == keyShortcut[COMMAND_INCREASE_SPLINE_ORDER] )
	{
		if( _nodes.m == numCV || _nodes.m == MaxOrder ) ///< MaxOrder e' definito in splineEvalfloat.h
			_nodes.m = 2;
		else
			_nodes.m = (_nodes.m + 1);

		refreshNodalPartition = 1;
		refreshBSplines = 1;
		refreshCurve = 1;
		glutPostRedisplay();

		printf( "\nspline order is: %d\n", _nodes.m );
	}
	else if( key == keyShortcut[COMMAND_DECREASE_SPLINE_ORDER] )
	{
		if( _nodes.m == 2 )
			_nodes.m = ( MaxOrder < numCV ) ? MaxOrder : numCV;
		else
			_nodes.m = (_nodes.m - 1);

		refreshNodalPartition = 1;
		refreshBSplines = 1;
		refreshCurve = 1;
		glutPostRedisplay();
		printf( "\nspline order is: %d\n", _nodes.m );
	}
	else if( key == keyShortcut[COMMAND_REMOVE_FIRST_POINT] )
	{
		removeFirstPoint();
		glutPostRedisplay();
	}
	else if( key == keyShortcut[COMMAND_REMOVE_LAST_POINT] )
	{
		removeLastPoint();
		glutPostRedisplay();
	}
	else if( key == keyShortcut[COMMAND_SAVE_SPLINE] )
	{
		saveSpline( saveFileName );
	}
	else if( key == keyShortcut[COMMAND_LOAD_SPLINE] )
	{
		//loadSpline( loadFileName );
		loadSpline("/home/vmplanet/CG/lab03/circle.spline");
	}
	else if( key == 27 )
	{
		exit(0);
	}
}

void addNewPoint( float x, float y ) 
{
	if( numCV >= MaxNumPts ) 
		removeFirstPoint();

	CV[numCV][0] = x;
	CV[numCV][1] = y;
	numCV++;
	
	refreshNodalPartition = 1;
	refreshBSplines = 1;
	refreshCurve = 1;
	
}

void myMouseFunc( int button, int state, int x, int y ) 
{
	if( hoverCV == -1 && button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) 
	{
		float xPos = ((float)x)/((float)( ( WindowWidth-1 ) / 2.0f));
		float yPos = ((float)y)/((float)( WindowHeight-1 ) );

		yPos = 1.0f-yPos; 

		if( xPos < (1.0-0.03) )
			addNewPoint( xPos, yPos );

		glutPostRedisplay();
	}

	if( hoverCV != -1 && button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) 
		draggingCV = hoverCV;	

	if( draggingCV == hoverCV && button==GLUT_LEFT_BUTTON && state==GLUT_UP ) 
		draggingCV = -1;

	if( hoverNode != -1 && button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) 
		draggingNode = hoverNode;	

	if( draggingNode == hoverNode && button==GLUT_LEFT_BUTTON && state==GLUT_UP ) 
		draggingNode = -1;

}

void myMotionFunc( int x, int y )
{
	int i;
	float snapToNodeThreshold;
	float xPos = ((float)x)/((float)( ( WindowWidth-1 ) / 2.0f));
	float yPos = ((float)y)/((float)( WindowHeight-1 ) );

	yPos = 1.0f-yPos;
	
	if( hoverCV == draggingCV && hoverCV != -1 ) 
	{	
		if( !(xPos > 0.0 && xPos < 1.0 && yPos > 0.0 && yPos < 1.0) )
			return;

		i = draggingCV;
		CV[i][0] = xPos;
		CV[i][1] = yPos;
			
		refreshCurve = 1;
		glutPostRedisplay();
	}
	
	if( hoverNode == draggingNode && hoverNode != -1 ) 
	{	
		xPos -= 1.0;
		i = draggingNode;

		snapToNodeThreshold = 0.03; // snapping per facilitare la creazione di nodi a molteplicita' > 1
		if( xPos <= ( _nodes.pos[i-1] + snapToNodeThreshold ) )
			_nodes.pos[i] = _nodes.pos[i-1];
		else if( xPos >= ( _nodes.pos[i+1] - snapToNodeThreshold ) )
			_nodes.pos[i] = _nodes.pos[i+1];
		else
			_nodes.pos[i] = roundf( xPos / evalStep ) * evalStep;

		glutPostRedisplay();
		refreshBSplines = 1;
		refreshCurve = 1;
	}
}

void myPassiveMotionFunc( int x, int y )
{
	int i;
	float xmin, xmax, ymin, ymax;
	float xPos = ((float)x)/((float)( ( WindowWidth-1 ) / 2.0f));
	float yPos = ((float)y)/((float)( WindowHeight-1 ) );

	yPos = 1.0f-yPos;		   
	
	if( xPos < 1.0 )
	{
		//search CV interception
		for ( i=0; i<numCV; i++ ) 
		{	
			xmin = CV[i][0]-hside; xmax = CV[i][0]+hside;
			ymin = CV[i][1]-hside; ymax = CV[i][1]+hside;
			if( xPos <= xmax && xPos >= xmin && yPos <= ymax && yPos >= ymin )
			{	
				if( hoverCV != i )
				{
					glutPostRedisplay(); 
					hoverCV = i;
				}
				return;
			}
		}
	
		if( hoverCV != -1 ) //cioè se sono appena uscito dal CV handle
		{
			hoverCV = -1;
			glutPostRedisplay();
			return;
		}
		
	}
	else if( xPos > 1.0 && xPos < 2.0 )
	{
		xPos -= 1.0;
		//search partition nodes interception
		for ( i=1; i<_nodes.numNodes-1; i++ ) 
		{	
			xmin = _nodes.pos[i] - hside; xmax = _nodes.pos[i] + hside;
			ymin = _nodesPosY - hside; ymax = _nodesPosY + hside;

			if( xPos <= xmax && xPos >= xmin && yPos <= ymax && yPos >= ymin )
			{	
				if( hoverNode != i ) 
				{
					glutPostRedisplay(); 
					hoverNode = i;
				}
				return;
			}
		}

		if( hoverNode != -1 ) //cioè se sono appena uscito dal node handle
		{
			hoverNode = -1;
			glutPostRedisplay();
			return;
		}
	}
	
}

void drawQuadHandle( float p[], float hsideHandle )
{
	glBegin( GL_LINE_STRIP );
	glVertex2f( p[0]+hsideHandle, p[1]+hsideHandle );
	glVertex2f( p[0]-hsideHandle, p[1]+hsideHandle );
	glVertex2f( p[0]-hsideHandle, p[1]-hsideHandle );
	glVertex2f( p[0]+hsideHandle, p[1]-hsideHandle );
	glVertex2f( p[0]+hsideHandle, p[1]+hsideHandle );
	glEnd();
}

void draw(void)
{
	int i,j, k;
	int param;
	int numeroA, numeroB, interval;

	glClear( GL_COLOR_BUFFER_BIT );

	//==================================================
	// Update Nodal Partition
	//==================================================
	if( refreshNodalPartition )
	{	
		refreshNodalPartition = 0;
		printf( "Recomputing partition...\n" );

		//=== aggiorna la partizione nodale ===

		_nodes.k = numCV - _nodes.m; ///< num internal nodes
		_nodes.numNodes = (_nodes.k + 2) + 2 * (_nodes.m - 1);
		_nodes.a = _nodes.m - 1;
		_nodes.b = _nodes.m + _nodes.k;

		if( partitionType == 2 )
		{
			for( i=0 ; i<_nodes.m ; i++ )
				_nodes.pos[i] = 0.0;
			for( i=0 ; i<_nodes.m ; i++ )
				_nodes.pos[(_nodes.numNodes-1)-i] = 1.0;
			for( i=0 ; i<_nodes.k ; i++ )
				_nodes.pos[i+_nodes.m] = (1.0 / (_nodes.k+1)) * (float)(i+1);
		}
		else if( partitionType == 1 )
		{
			//nodi equispaziati in [0;1]
			for( i=0 ; i<_nodes.numNodes ; i++ )
				_nodes.pos[i] = (1.0 / (_nodes.numNodes-1)) * (float)i;
		}

		snapPartitionNodes( &_nodes );

	}

	//==================================================
	// Compute BSpline basis functions
	//==================================================
	if( refreshBSplines )
	{
		refreshBSplines = 0;
		//printf( "Recomputing B-Spline Basis\n" ); //rallenta troppo la stampa

		for( i=0 ; i< MaxNumPts ; i++ )
			for( j=0 ; j<=MaxOrder ; j++ )
				for( k=0 ; k<NumTotEvaluations ; k++ )
					N[i][j][k] = 0.0;

		computeBSplines( &_nodes, N ); 
	}

	//==================================================
	// Compute spline curve points
	//==================================================
	if( refreshCurve )
	{	
		refreshCurve = 0;
		
		for( i=0 ; i<NumTotEvaluations ; i++ )
		{
			spline[i][0] = 0.0;
			spline[i][1] = 0.0;
		}

		//a quale parametro t corrisponde _nodes.a e _nodes.b? La spline va valutata solo in quell'intervallo
		numeroA = roundf( _nodes.pos[_nodes.a] / evalStep );
		numeroB = roundf( _nodes.pos[_nodes.b] / evalStep );
		interval = 0;
		float denum = 0;

		for( param = numeroA ; param < numeroB ; param++ )
		{
			/*prima troviamo l'intervallo e poi possiamo andare a valutare le spline*/
			interval = findIntervalFromPosition( &_nodes, (float)param * evalStep );
			for( i = interval - (_nodes.m-1) ; i < (interval+1); i++ )
			{
				//printf("index: %d\n",i);
				for( j=0 ; j<2 ; j++ )
				{
					/*valutazione spline, due volte, una per la x e una per la y*/
					spline[param][j] += (CV[i][j] * weight[i] * N[i][_nodes.m][param]);
					//printf("peso: %f riferito a indice: %d\n",weight[i],i);
				}
				denum +=  (weight[i] * N[i][_nodes.m][param]);
			}
			//printf("denom: %f\n",denum);
			spline[param][0] = spline[param][0] / denum;
			spline[param][1] = spline[param][1] / denum;
			denum = 0;
		}
	}

	//==================================================
	// Draw the spline curve
	//==================================================
	if( numCV >= 2 )
	{	
		numeroA = roundf( _nodes.pos[_nodes.a] / evalStep );
		numeroB = roundf( _nodes.pos[_nodes.b] / evalStep );
		interval = 0;

		glLineWidth( 2 );
		glBegin( GL_LINE_STRIP );
		for( i=numeroA ; i<numeroB ; i++ )
		{
			interval = findIntervalFromPosition( &_nodes, (float)i * evalStep );
			glColor4f( colors[interval][0], colors[interval][1], colors[interval][2], 1.0f );
			glVertex2d( spline[i][0], spline[i][1] );
		}
		glEnd( );

	}

	//==================================================
	// Draw the background quad
	//==================================================
	if( _darkBG )
		glColor4f( 0.12, 0.12, 0.12, 1.0 );
	else
		glColor4f( 0.82, 0.82, 0.82, 1.0 );
	glBegin( GL_POLYGON );
	glVertex2f( 1.0f, 0.0f );
	glVertex2f( 2.0f, 0.0f );
	glVertex2f( 2.0f, 1.0f );
	glVertex2f( 1.0f, 1.0f );
	glVertex2f( 1.0f, 0.0f );
	glEnd( );

	if( _darkBG )
		glColor4f( 0.08, 0.08, 0.08, 1.0 );
	else
		glColor4f( 0.72, 0.72, 0.72, 1.0 );
	glBegin( GL_POLYGON );
	glVertex2f( 1.0f, 0.0f );
	glVertex2f( 2.0f, 0.0f );
	glVertex2f( 2.0f, _nodesSectionY );
	glVertex2f( 1.0f, _nodesSectionY );
	glVertex2f( 1.0f, 0.0f );
	glEnd( );
	
	//==================================================
	// Draw separating lines
	//==================================================
	glLineWidth(2);
	glColor3f( 0.0f, 0.0f, 0.0f);
	glBegin( GL_LINES );
	glVertex2f( 1.0f, 0.0f );
	glVertex2f( 1.0f, 1.0f );
	glVertex2f( 0.0f, 0.0f );
	glVertex2f( 0.0f, 1.0f );
	glVertex2f( 0.0f, 1.0f );
	glVertex2f( 2.0f, 1.0f );
	glVertex2f( 2.0f, 0.0f );
	glVertex2f( 2.0f, 1.0f );
	glVertex2f( 0.0f, 0.0f );
	glVertex2f( 2.0f, 0.0f );

	glVertex2f( 1.0f, _nodesSectionY );
	glVertex2f( 2.0f, _nodesSectionY );
	glEnd();
	glLineWidth(lineWidth);


	
	//==================================================
	// Draw the control vertices CV
	//==================================================
	if( switchCurveVisualization == 1 )
	{
		glPointSize(6);
		if( _darkBG )
			glColor3f( 0.9, 0.9, 0.9 );
		else
			glColor3f( 0.0f, 0.0f, 0.0f);
		glBegin( GL_POINTS );
		for ( i=0; i<numCV; i++ ) 
			glVertex2f( CV[i][0], CV[i][1] );
		glEnd();

		glLineWidth(2);
		glColor3f( 1.0f, 0.294f, 0.059f);
		if( hoverCV != -1 )
			drawQuadHandle( CV[hoverCV], hside );

		glLineWidth(lineWidth);
	}
	
	//==================================================
	// Draw the poligonale di controllo
	//==================================================
	if( switchCurveVisualization == 1 )
	{
		if( _darkBG )
			glColor4f( 0.7f, 0.7f, 0.7f, 0.5f );		
		else
			glColor4f( 0.3f, 0.3f, 0.3f, 0.5f );		

		glEnable (GL_LINE_STIPPLE);
		glLineStipple (1, 0x00FF);
		glBegin( GL_LINE_STRIP );
		for ( i=0; i<numCV; i++ ) 
		   glVertex2f( CV[i][0], CV[i][1] );
		glEnd();
		glDisable (GL_LINE_STIPPLE);
	}
	
	
	//==================================================
	// draw he nodal partition 
	//==================================================
	
	glPushMatrix();
	glTranslatef(1.0f, 0.0f, 0.0f);

	//=== draw an horizontal line ===
	if( _darkBG )
		glColor4f( 0.7f, 0.7f, 0.7f, 0.5f );		
	else
		glColor4f( 0.3f, 0.3f, 0.3f, 0.5f );		
	glEnable (GL_LINE_STIPPLE);         
	glLineStipple (1, 0x00FF);
	glBegin( GL_LINE_STRIP );
   	glVertex2f( 0.0, _nodesPosY );
   	glVertex2f( 1.0, _nodesPosY );
	glEnd();
	glDisable (GL_LINE_STIPPLE);

	
	//=== draw the nodes ===
	if( _darkBG )
		glColor3f( 0.9, 0.9, 0.9 );
	else
		glColor3f( 0.0f, 0.0f, 0.0f);
	glBegin( GL_POINTS );
	for( i=0 ; i<_nodes.numNodes ; i++ )
		glVertex2f( _nodes.pos[i] , _nodesPosY );

   	glColor4ub( 154, 24, 25, 255 );
	for( i=0 ; i<_nodes.numNodes - 1; i++ )
		if( _nodes.pos[i] == _nodes.pos[i+1] )
			glVertex2f( _nodes.pos[i] , _nodesPosY );
	glEnd( );

	glLineWidth(2);
	glColor3f( 1.0f, 0.294f, 0.059f);
	if( hoverNode != -1 )
	{
		float node2d[2];
		node2d[0] = _nodes.pos[hoverNode];
		node2d[1] = _nodesPosY;
		drawQuadHandle( node2d, hside );
	}
	glLineWidth(lineWidth);

	//=== draw vertical lines at nodes positions ===
	
	glLineWidth(1);
	if( _darkBG )
		glColor4f( 0.7f, 0.7f, 0.7f, 0.2f );		
	else
		glColor4f( 0.3f, 0.3f, 0.3f, 0.2f );		
	//glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0x0F0F);
	glBegin(GL_LINES);
	for( i=1 ; i<_nodes.numNodes - 1 ; i++ )
	{
		glVertex2f( _nodes.pos[i] ,0.0f );
		glVertex2f( _nodes.pos[i] ,1.0f );
	}
	glEnd();

  	//=== make the a,b nodes more visible ===
	
	if( _darkBG )
		glColor4f( 0.7f, 0.7f, 0.7f, 0.8f );		
	else
		glColor4f( 0.3f, 0.3f, 0.3f, 0.8f );		
	
	glBegin(GL_LINES);
	glVertex2f( _nodes.pos[_nodes.m - 1] ,0.0f );
	glVertex2f( _nodes.pos[_nodes.m - 1] ,1.0f );
	glEnd();

	glBegin(GL_LINES);
	glVertex2f( _nodes.pos[_nodes.m + _nodes.k] ,0.0f );
	glVertex2f( _nodes.pos[_nodes.m + _nodes.k] ,1.0f );
	glEnd();

	glDisable( GL_LINE_STIPPLE );
	glPopMatrix();
	
	//==================================================
	// draw the BSpline basis functions
	//==================================================

	//only the Basis N[i][order]
	if( switchBSplineBasisVisualization == 0 )
	{
		glPushMatrix();
		glTranslatef( 1.0f, _nodesSectionY, 0.0f );
		glScalef( 1.0, (1.0 - _nodesSectionY) - 0.05, 1.0 );
		for( i=0; i<numCV ; i++ )
		{	
			glBegin( GL_LINE_STRIP );
			glColor4f( colors[i][0], colors[i][1], colors[i][2], 1.0f );
			for(j=0; j<NumTotEvaluations; j++)
				glVertex2f( (float)j * evalStep , N[i][_nodes.m][j] );
			glEnd();
		}

		glLineWidth(2);
		glColor4f( 0.0, 0.0, 0.0, 1.0f );
		glBegin( GL_LINES );
		glVertex2f( 0.0, 0.0 );
		glVertex2f( 1.0, 0.0 );
		glEnd();

		glPopMatrix();
	}

	//everything 
	if( switchBSplineBasisVisualization == 1 )
	{
		float yCurr = _nodesSectionY;
		float yStep = (1.0 - yCurr) / (float)_nodes.m;
		int m;
		for( m=1 ; m <= _nodes.m ; m++ )
		{
			glPushMatrix();
			glTranslatef( 1.0f, yCurr, 0.0f );
			glScalef( 1.0, yStep - 0.05, 1.0 );
			glLineWidth(1);
			for( i=0; i< (numCV + (_nodes.m - m)) ; i++ )
			{	
				glBegin( GL_LINE_STRIP );
				glColor4f( colors[i][0], colors[i][1], colors[i][2], 1.0f );
				for(j=0; j<NumTotEvaluations; j++)
					glVertex2f( (float)j * evalStep, N[i][m][j] );
				glEnd();
			}

			glLineWidth(2);
			glColor4f( 0.0, 0.0, 0.0, 1.0f );
            glBegin( GL_LINES );
			glVertex2f( 0.0, 0.0 );
			glVertex2f( 1.0, 0.0 );
			glEnd();

			yCurr += yStep;
			glPopMatrix();
		}
	}

	//==================================================
	// end draw routine
	//==================================================

	glutSwapBuffers();
	glutPostRedisplay();

}

void initRendering() 
{
	int i;
	for(i=0;i<MaxNumPts;i++){
		weight[i]=1.0;
	}
	if( _darkBG )
		glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	else
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines
	lineWidth = 1.0f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	{
		//=== compute colors to use ===

		float alfa = 0.0f, r = 0.2f, g = 0.8f, b = 0.7f;
		
		//int numColorsToGenerate = numCV;
		int numColorsToGenerate = MaxNodes-1;
		float deltaCol = 1.0f/(numColorsToGenerate) ; 
		int i;
		for( i=0; i<numColorsToGenerate ; i++)
		{	
			alfa += deltaCol;
			r = (r+(deltaCol*3.0f + 0.1)); r -= floorf(r);
			g = (g+(deltaCol*2.0f + 0.5)); g -= floorf(g);
			colors[i][0] = r;
			colors[i][1] = g;
			colors[i][2] = b;
			colors[i][3] = alfa;
		}
	}
}

void resizeWindow(int w, int h)
{
	w = 2*h; //force viewport aspect ratio
	
	WindowHeight = (h>1) ? h : 2;
	WindowWidth = (w>1) ? w : 2;
	
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, 2.0f, 0.0f, 1.0f);  // Always view [0;2]x[0;1]. 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	printHelp();

	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA ); 
	glutInitWindowSize(1024, 512);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);

	initRendering();

	glutDisplayFunc(draw);
	glutReshapeFunc(resizeWindow);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMotionFunc);
	glutPassiveMotionFunc(myPassiveMotionFunc);

	strcpy( saveFileName, "save.spline" ); //defaul save file
	_nodes.m = 3; //default spline order

	if( argc == 2 )
	{
		/*strcpy( loadFileName, argv[1] );
		loadSpline( loadFileName );*/
	}

	//map key shortcuts to commands
	keyShortcut[COMMAND_INCREASE_SPLINE_ORDER] = 'm';
	keyShortcut[COMMAND_DECREASE_SPLINE_ORDER] = 'M';
	keyShortcut[COMMAND_MAKE_PARTITION_UNIFORM] = 'u';
	keyShortcut[COMMAND_MAKE_PARTITION_OPEN] = 'o';
	keyShortcut[COMMAND_REMOVE_FIRST_POINT] = 'f';
	keyShortcut[COMMAND_REMOVE_LAST_POINT] = 'l';
	keyShortcut[COMMAND_CHANGE_BSPLINE_VISUALIZATION] = 'V';
	keyShortcut[COMMAND_CHANGE_CURVE_VISUALIZATION] = 'v';
	keyShortcut[COMMAND_PRINT_SPLINE_INFO] = 'p';
	keyShortcut[COMMAND_PRINT_HELP] = 'h';
	keyShortcut[COMMAND_LOAD_SPLINE] = 'L';
	keyShortcut[COMMAND_SAVE_SPLINE] = 'S';
	
	glutMainLoop();

	return 0;

}
