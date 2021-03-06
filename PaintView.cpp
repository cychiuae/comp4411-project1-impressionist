//
// paintview.cpp
//
// The code maintaining the painting view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"
#include <cmath>

#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#define max(a, b)	( ( (a)>(b) ) ? (a) : (b) )
#endif

static int		eventToDo;
static int		isAnEvent=0;
static Point	coord;
static Point	startCoord;
static int      apaint = 0;
PaintView::PaintView(int			x, 
					 int			y, 
					 int			w, 
					 int			h, 
					 const char*	l)
						: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;
	rightClickStartingPoint = Point(0, 0);
}


void PaintView::draw()
{
	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_FRONT_AND_BACK);
	#endif // !MESA

	if(!valid())
	{

		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );
		// Add this two line to allow alpha
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		ortho();

		glClear( GL_COLOR_BUFFER_BIT );
	}

	Point scrollpos;// = GetScrollPosition();
	scrollpos.x = 0;
	scrollpos.y	= 0;

	m_nWindowWidth	= w();
	m_nWindowHeight	= h();

	int drawWidth, drawHeight;
	drawWidth = min( m_nWindowWidth, m_pDoc->m_nPaintWidth );
	drawHeight = min( m_nWindowHeight, m_pDoc->m_nPaintHeight );
	// printf("win width: %d,paint width: %d\n",m_nWindowWidth,m_pDoc->m_nPaintWidth);

	int startrow = m_pDoc->m_nPaintHeight - (scrollpos.y + drawHeight);
	if ( startrow < 0 ) startrow = 0;

	m_pPaintBitstart = m_pDoc->m_ucPainting + 
		3 * ((m_pDoc->m_nPaintWidth * startrow) + scrollpos.x);

	m_nDrawWidth	= drawWidth;
	m_nDrawHeight	= drawHeight;

	m_nStartRow		= startrow;
	m_nEndRow		= startrow + drawHeight;
	m_nStartCol		= scrollpos.x;
	m_nEndCol		= m_nStartCol + drawWidth;

	if ( m_pDoc->m_ucPainting && !isAnEvent) 
	{
		RestoreContent();

	}

	if ( m_pDoc->m_ucPainting && isAnEvent) 
	{

		// Clear it after processing.
		isAnEvent	= 0;	

		Point source( coord.x + m_nStartCol, m_nEndRow - coord.y );
		Point target( coord.x, m_nWindowHeight - coord.y );

		// This is the event handler
		switch (eventToDo) 
		{
		case LEFT_MOUSE_DOWN:
			startCoord = source;
			m_pDoc->m_pCurrentBrush->BrushBegin( source, target );
			break;
		case LEFT_MOUSE_DRAG:
			m_pDoc->m_pCurrentBrush->BrushMove( startCoord, target);
			startCoord = target;
			break;
		case LEFT_MOUSE_UP:
			m_pDoc->m_pCurrentBrush->BrushEnd( source, target );
			SaveCurrentContent();
			RestoreContent();
			break;
		case RIGHT_MOUSE_DOWN:
			rightClickStartingPoint = target;
			RestoreContent();
			break;
		case RIGHT_MOUSE_DRAG:
			RestoreContent();
			glLineWidth(1);
			glBegin(GL_LINE);
				glColor3f(1.0 , 0.0, 0.0);
				glVertex2d(rightClickStartingPoint.x, rightClickStartingPoint.y);
				glVertex2d(target.x, target.y);
			glEnd();
			break;
		case RIGHT_MOUSE_UP:
			RestoreContent();

			if (m_pDoc->m_pCurrentBrush->BrushName() == "Lines" || m_pDoc->m_pCurrentBrush->BrushName() == "Scattered Circles") {
				int dx = target.x - rightClickStartingPoint.x;
				int dy = target.y - rightClickStartingPoint.y;

				// Angle
				if (dx == 0) {
					m_pDoc->m_pUI->setLineAngle(90);
				} else {
					int angle = 360 + (int)(atan2((double)dy, (double)dx) * 180 / M_PI);
					m_pDoc->m_pUI->setLineAngle(angle);
				}

				// Size
				int size = (int)sqrt((dx * dx + dy * dy));
				if (size >= 40) {
					m_pDoc->m_pUI->setSize(40);
				} else {
					m_pDoc->m_pUI->setSize(size);
				}
			}

			break;
		default:
			printf("Unknown event!!\n");		
			break;
		}
	}

	glFlush();

	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_BACK);
	#endif // !MESA

}


int PaintView::handle(int event)
{
	switch(event)
	{
	case FL_ENTER:
	    redraw();
		break;
	case FL_PUSH:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DOWN;
		else
			eventToDo=LEFT_MOUSE_DOWN;
		isAnEvent=1;
		redraw();
		break;
	case FL_DRAG:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		isAnEvent = 1;
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DRAG;
		else {
			eventToDo = LEFT_MOUSE_DRAG;
		}

		if (coord.x <= 0) {
			coord.x = 0;
		}
		if (coord.x >= m_pDoc->m_nWidth) {
			coord.x = m_pDoc->m_nWidth;
		}
		if (coord.y <= 0) {
			coord.y = 0;
		}
		if (coord.y >= m_pDoc->m_nHeight) {
			coord.y = m_pDoc->m_nHeight - 10;
		}

		redraw();
		m_pDoc->m_pUI->m_origView->moveCursor(Point(coord.x, coord.y));		
		break;
	case FL_RELEASE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_UP;
		else
			eventToDo=LEFT_MOUSE_UP;
		isAnEvent=1;
		redraw();
		break;
	case FL_MOVE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		m_pDoc->m_pUI->m_origView->moveCursor(Point(coord.x, coord.y));
		break;
	default:
		return 0;
		break;

	}



	return 1;
}

void PaintView::refresh()
{
	redraw();
}

void PaintView::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
}

void PaintView::SaveCurrentContent()
{
	// Tell openGL to read from the front buffer when capturing
	// out paint strokes
	glReadBuffer(GL_FRONT);

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	
	glReadPixels( 0, 
				  m_nWindowHeight - m_nDrawHeight, 
				  m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart );
}


void PaintView::RestoreContent()
{
	glDrawBuffer(GL_BACK);

	glClear( GL_COLOR_BUFFER_BIT );

	glRasterPos2i( 0, m_nWindowHeight - m_nDrawHeight );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	glDrawPixels( m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart);

//	glDrawBuffer(GL_FRONT);
}

void PaintView::paintCanvas()
{
	int width = m_pDoc->m_nWidth;
	int height = m_pDoc->m_nHeight;
	int paintHeight = m_pDoc->m_nPaintHeight;
	int space = m_pDoc->getSpace();
	int size = m_pDoc->getSize();
	m_pDoc->m_pCurrentBrush->BrushBegin(Point(0, 0), Point(0, 0));
	for (int i = 0; i < width; i += space + size){
		for (int j = 0; j < height; j += space){
			Point source(i, j);
			Point target(i, j);
			m_pDoc->m_pCurrentBrush->BrushMove(source, target);
		}
	}
	m_pDoc->m_pCurrentBrush->BrushEnd(Point(0, 0), Point(0, 0));
	glReadBuffer(GL_FRONT_AND_BACK);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth);

	glReadPixels(0,
		m_nWindowHeight - m_nDrawHeight,
		m_nDrawWidth,
		m_nDrawHeight,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		m_pPaintBitstart);
	RestoreContent();

	redraw();
}
