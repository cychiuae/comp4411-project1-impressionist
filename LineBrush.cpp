#include "LineBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <cmath>

LineBrush::LineBrush(ImpressionistDoc* pDoc, char* name) : ImpBrush(pDoc, name) {
}

void LineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize((float)size);

	BrushMove(source, target);
}

void LineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("PointBrush::BrushMove  document is NULL\n");
		return;
	}

	int size = dlg->getSize();

	int angle = 0;
	switch (pDoc->getBrushStrokeDirection()) {
		case STROKE_DIRECTION_SLIDER:
			angle = dlg->getLineAngle();
			break;
	}

	double radianAngle = (angle % 360) * M_PI / 180;
	double sinAngle = sin(radianAngle);
	double cosAngle = cos(radianAngle);
	
	glLineWidth(dlg->getLineWidth());
	glBegin(GL_LINE);
	SetColor(source);
	glVertex2d(target.x + (size / 2) * cosAngle, target.y + (size / 2) * sinAngle);
	glVertex2d(target.x - (size / 2) * cosAngle, target.y - (size / 2) * sinAngle);
	glEnd();
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}