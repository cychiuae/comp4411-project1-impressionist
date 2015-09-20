//
// CircleBrush.cpp
//
// The implementation of Circle Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//
#include <math.h>
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "CircleBrush.h"

extern float frand();

CircleBrush::CircleBrush( ImpressionistDoc* pDoc, char* name ) :
	
	ImpBrush(pDoc,name)
{
}

void CircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	int size = pDoc->getSize();


	glPointSize( (float)size );

	BrushMove( source, target );
}

void CircleBrush::BrushMove( const Point source, const Point target )
{
	printf("target x %d, y %d\n",target.x, target.y);
	printf("source x %d, y %d\n",source.x, source.y);
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
	printf("size %d\n", pDoc->getSize());

	if ( pDoc == NULL ) {
		printf( "CircleBrush::BrushMove  document is NULL\n" );
		return;
	}
	
	int chunk = 24;
	int radius = pDoc->getSize();
	if(pDoc->getIsRandSize()){
		radius =  rand() % pDoc->getSize();
	}

	glBegin( GL_POLYGON );

		SetColor( source );

		for(double angle=0; angle< 2 * M_PI; angle+=2 * M_PI/chunk){
			double x = target.x + radius * cos(angle);
			double y = target.y + radius * sin(angle);
			glVertex2d( x, y );
		}

	glEnd();


}

void CircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

