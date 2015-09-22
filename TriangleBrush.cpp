#include "TriangleBrush.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <cmath>

TriangleBrush::TriangleBrush(ImpressionistDoc* pDoc, char* name) :
ImpBrush(pDoc, name)
{
}

void TriangleBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	glPointSize((float)size);

	BrushMove(source, target);
}

void TriangleBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int size = pDoc->getSize();

	glPointSize((float)size);

	if (pDoc == NULL) {
		printf("PointBrush::BrushMove  document is NULL\n");
		return;
	}

	glBegin(GL_TRIANGLES);
	SetColor(source);

	glVertex2d(target.x, target.y + sqrt(0.75 * size * size) / 2);
	glVertex2d(target.x - 0.5 * size, target.y - sqrt(0.75 * size * size) / 2);
	glVertex2d(target.x + 0.5 * size, target.y - sqrt(0.75 * size * size) / 2);

	glEnd();
}

void TriangleBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
	AddPaintToHistory();
}

