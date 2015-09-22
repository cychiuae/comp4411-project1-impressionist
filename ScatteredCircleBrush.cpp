//
// ScatteredCircleBrush.cpp
//
// The implementation of Circle Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//
#include <math.h>
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ScatteredCircleBrush.h"

extern float frand();
extern int irand(int max);
ScatteredCircleBrush::ScatteredCircleBrush( ImpressionistDoc* pDoc, char* name ) :
	
	ImpBrush(pDoc,name)
{
}

void ScatteredCircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize( (float)size );

	BrushMove( source, target );
}

void ScatteredCircleBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "ScatteredCircleBrush::BrushMove  document is NULL\n" );
		return;
	}
	
	int chunk = 24;
	int radius = pDoc->getSize() / 2;

	for (int i = 0; i < rand() % 6 + 2; i++){
		glBegin(GL_POLYGON);
		int randx = target.x - radius + irand(radius * 2);
		int randy = target.y - radius + irand(radius * 2);
		Point new_point = Point(randx, randy);
		SetColor(new_point);
		for(double angle=0; angle< 2 * M_PI; angle+=2 * M_PI/chunk){
			double x = randx + radius * cos(angle);
			double y = randy + radius * sin(angle);
			glVertex2d( x, y );
		}
		glEnd();
	}
}

void ScatteredCircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

