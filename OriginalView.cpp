//
// originalview.cpp
//
// The code maintaining the original view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "originalview.h"
#include "math.h"

#define CUSOR_EVENT		1

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#endif

OriginalView::OriginalView(int			x, 
						   int			y, 
						   int			w, 
						   int			h, 
						   const char*	l)
							: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;

}

void OriginalView::draw()
{
	if(!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		// Tell openGL to read from the front buffer when capturing
		// out paint strokes 
		glReadBuffer( GL_FRONT );
		ortho();

	}

	glClear( GL_COLOR_BUFFER_BIT );

	switch (m_pDoc->m_nMode) {
		case SHOW_ORIGINAL_IMAGE:
			if (m_pDoc->m_ucBitmap)
			{
				// note that both OpenGL pixel storage and the Windows BMP format
				// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
				// around with startrow.

				m_nWindowWidth = w();
				m_nWindowHeight = h();

				int drawWidth, drawHeight;
				GLvoid* bitstart;

				// we are not using a scrollable window, so ignore it
				Point scrollpos;	// = GetScrollPosition();
				scrollpos.x = scrollpos.y = 0;

				drawWidth = min(m_nWindowWidth, m_pDoc->m_nWidth);
				drawHeight = min(m_nWindowHeight, m_pDoc->m_nHeight);

				int	startrow = m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
				if (startrow < 0)
					startrow = 0;

				bitstart = m_pDoc->m_ucBitmap + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

				// just copy image to GLwindow conceptually
				glRasterPos2i(0, m_nWindowHeight - drawHeight);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth);
				glDrawBuffer(GL_BACK);
				glDrawPixels(drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart);

				if (isAnEvent) {
					// printf("is an event\n");
					isAnEvent = false;
					switch (eventToDo) {
					case CUSOR_EVENT:
						glPointSize(4);
						glBegin(GL_POINTS);
						glColor3f(1.0, 0, 0);
						glVertex2d(cursor.x, cursor.y);
						glEnd();
						break;
					}
				}
			}
			break;
		case SHOW_EDGE_IMAGE:
			if (m_pDoc->m_nEdgeImage)
			{
				// note that both OpenGL pixel storage and the Windows BMP format
				// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
				// around with startrow.

				m_nWindowWidth = w();
				m_nWindowHeight = h();

				int drawWidth, drawHeight;
				GLvoid* bitstart;

				// we are not using a scrollable window, so ignore it
				Point scrollpos;	// = GetScrollPosition();
				scrollpos.x = scrollpos.y = 0;

				drawWidth = min(m_nWindowWidth, m_pDoc->m_nWidth);
				drawHeight = min(m_nWindowHeight, m_pDoc->m_nHeight);

				int	startrow = m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
				if (startrow < 0)
					startrow = 0;

				bitstart = m_pDoc->m_nEdgeImage + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

				// just copy image to GLwindow conceptually
				glRasterPos2i(0, m_nWindowHeight - drawHeight);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth);
				glDrawBuffer(GL_BACK);
				glDrawPixels(drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart);

				if (isAnEvent) {
					// printf("is an event\n");
					isAnEvent = false;
					switch (eventToDo) {
					case CUSOR_EVENT:
						glPointSize(4);
						glBegin(GL_POINTS);
						glColor3f(1.0, 0, 0);
						glVertex2d(cursor.x, cursor.y);
						glEnd();
						break;
					}
				}
			}
			break;
		case SHOW_ANOTHER_IMAGE:
			if (m_pDoc->m_nAnotherImage)
			{
				// note that both OpenGL pixel storage and the Windows BMP format
				// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
				// around with startrow.

				m_nWindowWidth = w();
				m_nWindowHeight = h();

				int drawWidth, drawHeight;
				GLvoid* bitstart;

				// we are not using a scrollable window, so ignore it
				Point scrollpos;	// = GetScrollPosition();
				scrollpos.x = scrollpos.y = 0;

				drawWidth = min(m_nWindowWidth, m_pDoc->m_nWidth);
				drawHeight = min(m_nWindowHeight, m_pDoc->m_nHeight);

				int	startrow = m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
				if (startrow < 0)
					startrow = 0;

				bitstart = m_pDoc->m_nAnotherImage + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

				// just copy image to GLwindow conceptually
				glRasterPos2i(0, m_nWindowHeight - drawHeight);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth);
				glDrawBuffer(GL_BACK);
				glDrawPixels(drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart);

				if (isAnEvent) {
					// printf("is an event\n");
					isAnEvent = false;
					switch (eventToDo) {
					case CUSOR_EVENT:
						glPointSize(4);
						glBegin(GL_POINTS);
						glColor3f(1.0, 0, 0);
						glVertex2d(cursor.x, cursor.y);
						glEnd();
						break;
					}
				}
			}
			break;
		case SHOW_DISSOLVE_IMAGE:
			if (m_pDoc->m_nDissolveImage)
			{
				// note that both OpenGL pixel storage and the Windows BMP format
				// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
				// around with startrow.

				m_nWindowWidth = w();
				m_nWindowHeight = h();

				int drawWidth, drawHeight;
				GLvoid* bitstart;

				// we are not using a scrollable window, so ignore it
				Point scrollpos;	// = GetScrollPosition();
				scrollpos.x = scrollpos.y = 0;

				drawWidth = min(m_nWindowWidth, m_pDoc->m_nWidth);
				drawHeight = min(m_nWindowHeight, m_pDoc->m_nHeight);

				int	startrow = m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
				if (startrow < 0)
					startrow = 0;

				bitstart = m_pDoc->m_nDissolveImage + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

				// just copy image to GLwindow conceptually
				glRasterPos2i(0, m_nWindowHeight - drawHeight);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth);
				glDrawBuffer(GL_BACK);
				glDrawPixels(drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart);

				if (isAnEvent) {
					// printf("is an event\n");
					isAnEvent = false;
					switch (eventToDo) {
					case CUSOR_EVENT:
						glPointSize(4);
						glBegin(GL_POINTS);
						glColor3f(1.0, 0, 0);
						glVertex2d(cursor.x, cursor.y);
						glEnd();
						break;
					}
				}
			}
			break;
		default:
			break;
	}

	glFlush();
}

void OriginalView::moveCursor(Point pt){
	cursor.x = pt.x;
	cursor.y = h() - pt.y;
	isAnEvent = true;
	eventToDo = CUSOR_EVENT;
	refresh();
}

void OriginalView::refresh()
{
	redraw();
}

void OriginalView::resizeWindow(int	width, 
								int	height)
{
	resize(x(), y(), width, height);
}
