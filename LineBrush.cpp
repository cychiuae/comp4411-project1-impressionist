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
		case STROKE_DIRECTION_GRADIENT:
			int grids[3][3];
			int sobelXOperator[3][3] = {
				{ -1, 0, 1 },
				{ -2, 0, 2 },
				{ -1, 0, 1 }
			};
			int sobelYOperator[3][3] = {
				{ 1, 2, 1 },
				{ 0, 0, 0 },
				{ -1, -2, -1 }
			};
			int vectorX = 0, vectorY = 0;
			for (int row = 0; row < 3; row++) {
				for (int col = 0; col < 3; col++) {
					Point point = Point(target.x + row - 1, target.y + col - 1);
					GLubyte color[3];
					memcpy(color, pDoc->GetOriginalPixel(point), 3);
					grids[row][col] = 0.299*color[0] + 0.587*color[1] + 0.114*color[2];
				}
			}
			for (int row = 0; row < 3; row++) {
				for (int col = 0; col < 3; col++) {
					vectorX += grids[row][col] * sobelXOperator[row][col];
					vectorY += grids[row][col] * sobelYOperator[row][col];
				}
			}
			if (vectorX == 0) {
				angle = 90;
			} else {
				angle = (int)(360 + atan2(vectorY, vectorX) * 180 / M_PI);
			}
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