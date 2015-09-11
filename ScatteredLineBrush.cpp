#include "ScatteredLineBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <cmath>

ScatteredLineBrush::ScatteredLineBrush(ImpressionistDoc* pDoc, char* name) : ImpBrush(pDoc, name) {
}

void ScatteredLineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();



	glPointSize((float)size);

	BrushMove(source, target);
}

void ScatteredLineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("PointBrush::BrushMove  document is NULL\n");
		return;
	}
	printf("hihi");
	int size = dlg->getSize();
	int angle = dlg->getLineAngle() * M_PI / 180;
	double sinAngle = sin(angle);
	double cosAngle = cos(angle);
	
	for (int i = 0; i< rand() % size; i++){
		int x = target.x - size / 2 + rand() % size;
		int y = target.y - size / 2 + rand() % size;
		glLineWidth(dlg->getLineWidth());
		glBegin(GL_LINE);
		SetColor(source);
		glVertex2d(x + (size / 2) * cosAngle, y + (size / 2) * sinAngle);
		glVertex2d(x - (size / 2) * cosAngle, y - (size / 2) * sinAngle);
		glEnd();
	}

	
}

void ScatteredLineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}