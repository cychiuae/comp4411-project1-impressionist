#ifndef SCATTEDPOINTBRUSH_H
#define SCATTEDPOINTBRUSH_H

#include "ImpBrush.h"
class ScattedPointBrush : public ImpBrush {
public:
	ScattedPointBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
};

#endif