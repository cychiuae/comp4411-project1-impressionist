#ifndef SCATTEREDTRIANGLE_H
#define SCATTEREDTRIANGLE_H

#include "ImpBrush.h"
class ScatteredTriangleBrush : public ImpBrush
{
public:
	ScatteredTriangleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
};

#endif