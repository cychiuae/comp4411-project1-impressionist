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
	
	int size = dlg->getSize();
	
	for (int i = 0; i< rand() % size; i++){
		int x = target.x - size / 2 + rand() % size;
		int y = target.y - size / 2 + rand() % size;
		
		int angle = 0;
		switch (pDoc->getBrushStrokeDirection()) {
			case STROKE_DIRECTION_SLIDER: {
				angle = dlg->getLineAngle();
				break;
			}
			case STROKE_DIRECTION_GRADIENT: {
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
						// should be target.y - col + 1
						Point point = Point(x + row - 1, y + col - 1);
						GLubyte color[3];
						memcpy(color, pDoc->GetOriginalPixel(point), 3);
						grids[row][col] = 0.299*color[0] + 0.587*color[1] + 0.114*color[2];
						// can we combine two double forloop?
						// vectorX += grids[row][col] * sobelXOperator[row][col];
						// vectorY += grids[row][col] * sobelYOperator[row][col];
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
				}
				else {
					angle = (int)(360 + atan2(vectorY, vectorX) * 180 / M_PI);
				}
				break;
			}
			case STROKE_DIRECTION_BRUSH_DIRECTION: {
				int dx = source.x - target.x;
				int dy = source.y - target.y;
				if (dx == 0) {
					angle = 90;
				}
				else {
					angle = (int)(360 + atan2(dy, dx) * 180 / M_PI);
				}
				break;
			}
		}

		double radianAngle = (angle % 360) * M_PI / 180;
		double sinAngle = sin(radianAngle);
		double cosAngle = cos(radianAngle);

		glLineWidth(dlg->getLineWidth());
		glBegin(GL_LINE);
		SetColor(Point(x, y));
		glVertex2d(x + (size / 2) * cosAngle, y + (size / 2) * sinAngle);
		glVertex2d(x - (size / 2) * cosAngle, y - (size / 2) * sinAngle);
		glEnd();
	}
}

void ScatteredLineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}