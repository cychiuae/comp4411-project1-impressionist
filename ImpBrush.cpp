//
// ImpBrush.cpp
//
// The implementation of virtual brush. All the other brushes inherit from it.
//
//
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ImpBrush.h"

// Static class member initializations
int			ImpBrush::c_nBrushCount	= 0;
ImpBrush**	ImpBrush::c_pBrushes	= NULL;

ImpBrush::ImpBrush(ImpressionistDoc*	pDoc, 
				   char*				name) :
					m_pDoc(pDoc), 
					m_pBrushName(name)
{
}

//---------------------------------------------------
// Return m_pDoc, which connects the UI and brushes
//---------------------------------------------------
ImpressionistDoc* ImpBrush::GetDocument(void)
{
	return m_pDoc;
}

//---------------------------------------------------
// Return the name of the current brush
//---------------------------------------------------
char* ImpBrush::BrushName(void)
{
	return m_pBrushName;
}

//----------------------------------------------------
// Set the color to paint with to the color at source,
// which is the coord at the original window to sample 
// the color from
//----------------------------------------------------
void ImpBrush::SetColor (const Point source)
{
	ImpressionistDoc* pDoc = GetDocument();


	// GLubyte color[3];
	GLubyte color[4];
	memcpy ( color, pDoc->GetOriginalPixel( source ), 3 );
	color[0] = GLubyte(pDoc->m_pUI->getRed() * color[0]);
	color[1] = GLubyte(pDoc->m_pUI->getGreen() * color[1]);
	color[2] = GLubyte(pDoc->m_pUI->getBlue() * color[2]);
	color[3] = GLubyte(pDoc->m_pUI->getAlpha() * 255);
	// printf("color: %d, %d, %d, %d\n",color[0], color[1], color[2], color[3]);
	
	// glColor3ubv( color );
	glColor4ubv(color);
}

void ImpBrush::AddPaintToHistory() {
	ImpressionistDoc* pDoc = GetDocument();
	int width = pDoc->m_nPaintWidth;
	int height = pDoc->m_nPaintHeight;
	unsigned char *m_ucPainting = new unsigned char[width * height * 3];
	memcpy(m_ucPainting, pDoc->m_ucPainting, width * height * 3);
	pDoc->m_ucPainting_History.push_back(m_ucPainting);
}