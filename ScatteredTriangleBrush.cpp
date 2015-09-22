#include "ScatteredTriangleBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <cmath>

extern int irand(int max);

ScatteredTriangleBrush::ScatteredTriangleBrush(ImpressionistDoc* pDoc, char* name) :
ImpBrush(pDoc, name)
{
}

void ScatteredTriangleBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	glPointSize((float)size);

	BrushMove(source, target);
}

void ScatteredTriangleBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int size = pDoc->getSize();

	glPointSize((float)size);

	if (pDoc == NULL) {
		printf("PointBrush::BrushMove  document is NULL\n");
		return;
	}
	for (int i = 0; i < rand() % 6 + 2; i++){
		int randx = target.x - size / 2 + irand(size);
		int randy = target.y - size / 2 + irand(size);
		glBegin(GL_TRIANGLES);
		SetColor(source);
		glVertex2d(randx, randy + sqrt(0.75 * size * size) / 2);
		glVertex2d(randx - 0.5 * size, randy - sqrt(0.75 * size * size) / 2);
		glVertex2d(randx + 0.5 * size, randy - sqrt(0.75 * size * size) / 2);
		glEnd();
	}
}

void ScatteredTriangleBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
	AddPaintToHistory();
}