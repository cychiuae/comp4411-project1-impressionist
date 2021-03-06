#include "ScattedPointBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <cmath>

ScattedPointBrush::ScattedPointBrush(ImpressionistDoc* pDoc, char* name) : ImpBrush(pDoc, name) {
}

void ScattedPointBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize((float)size);

	BrushMove(source, target);
}

void ScattedPointBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("PointBrush::BrushMove  document is NULL\n");
		return;
	}
	
	int pointSize = pDoc->getSize();
	int pointSize2 = pDoc->getSize();


	// rand size start
	if(dlg->getIsPaint() && dlg->getIsRandSize()){
		pointSize = rand() % pDoc->getSize() + 1;
	}
	// rand size end
	
	glPointSize(1.0);
	for (int i = 0; i < pointSize2 * pointSize2 - rand() % pointSize2 - pointSize2 / 2; i++) {
		int randX = target.x - pointSize / 2 + rand() % pointSize;
		int randY = target.y - pointSize / 2 + rand() % pointSize;
		glBegin(GL_POINTS);
		SetColor(Point(randX, randY));
		glVertex2d(randX, randY);
		glEnd();
	}
}

void ScattedPointBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
	AddPaintToHistory();
}