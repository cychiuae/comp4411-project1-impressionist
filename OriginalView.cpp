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
#define EDGE_IMAGE      2

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

	if ( m_pDoc->m_ucBitmap ) 
	{
		// note that both OpenGL pixel storage and the Windows BMP format
		// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
		// around with startrow.

		m_nWindowWidth=w();
		m_nWindowHeight=h();

		int drawWidth, drawHeight;
		GLvoid* bitstart;

		// we are not using a scrollable window, so ignore it
		Point scrollpos;	// = GetScrollPosition();
		scrollpos.x=scrollpos.y=0;

		drawWidth	= min( m_nWindowWidth, m_pDoc->m_nWidth );
		drawHeight	= min( m_nWindowHeight, m_pDoc->m_nHeight );

		int	startrow	= m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
		if ( startrow < 0 ) 
			startrow = 0;

		bitstart = m_pDoc->m_ucBitmap + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

		// just copy image to GLwindow conceptually
		glRasterPos2i( 0, m_nWindowHeight - drawHeight );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth );
		glDrawBuffer( GL_BACK );
		glDrawPixels( drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart );

		// my rubbish code start
		int width = m_pDoc->m_nWidth;
		int height = m_pDoc->m_nHeight;
		int paintHeight = m_pDoc->m_nPaintHeight;
		int space = m_pDoc->getSpace();
		int size = m_pDoc->getSize();

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

		double* gray = new double [width*height];
		double* sobelX = new double [width*height];
		double* sobelY = new double [width*height];
		unsigned char* edge_image = new unsigned char [width*height*3];

		memset(gray, 0, width*height);
		memset(sobelX, 0, width*height);
		memset(sobelY, 0, width*height);
		memset(edge_image, 0, width*height*3);
		// my rubbish code end

		if (isAnEvent) {
			printf("is an event\n");
			isAnEvent = false;
			switch (eventToDo) {
			case CUSOR_EVENT:
				glBegin(GL_POLYGON);
				glVertex2i(cursor.x - 2, cursor.y + 2);
				glVertex2i(cursor.x + 2, cursor.y + 2);
				glVertex2i(cursor.x + 2, cursor.y - 2);
				glVertex2i(cursor.x - 2, cursor.y - 2);
				glColor3f(1.0, 0, 0);
				glEnd();
				break;
			case EDGE_IMAGE:
				printf("edge image\n");
				glBegin(GL_POLYGON);
				glVertex2i(50 - 2, 50 + 2);
				glVertex2i(50 + 2, 50 + 2);
				glVertex2i(50 + 2, 50 - 2);
				glVertex2i(50 - 2, 50 - 2);
				glColor3f(1.0, 0, 0);
				glEnd();
				m_pDoc->m_pCurrentBrush->BrushBegin(Point(0, 0), Point(0, 0));

				// image to gray
				for (int i = 0; i < width; i +=1){
					for (int j = 0; j < height; j +=1){
						Point source(i, j);
						GLubyte color[3];
						memcpy(color, m_pDoc->GetOriginalPixel(source), 3);
						gray[i+j*drawWidth] = 0.299*color[0] + 0.587*color[1] + 0.114*color[2];
						// edge_image[(j*width + i) * 3 + 0] = gray[i+j*drawWidth];
						// edge_image[(j*width + i) * 3 + 1] = gray[i+j*drawWidth];
						// edge_image[(j*width + i) * 3 + 2] = gray[i+j*drawWidth];
					}
				}

				// gray to sobel
				for (int i = 0; i < width; i +=1){
					for (int j = 0; j < height; j +=1){
						sobelX[j*width + i] = 0;
						sobelY[j*width + i] = 0;
						for (int row = 0; row < 3; row++) {
							for (int col = 0; col < 3; col++) {
								int l = i + col - 1;
								int k = j + row - 1;
								if (l < 0 || l >= width) l = 0;
								if (k < 0 || k >= height) k = 0;
								sobelX[j*width+i] += gray[l+k*width] * sobelXOperator[row][col];
								sobelY[j*width+i] += gray[l+k*width] * sobelYOperator[row][col];
							}
						}
						sobelX[i+j*width];
						sobelY[i+j*width];
						// printf("%d\n",sobelX[i+j*width]);
						int solbel = (int)sqrt(sobelX[i + j*width] * sobelX[i + j*width] + sobelY[i + j*width] * sobelY[i + j*width]);
						
						edge_image[(j*width+i)*3 + 0] = 255 * (solbel >= m_pDoc->getEdgeThreshold());
						edge_image[(j*width+i)*3 + 1] = 255 * (solbel >= m_pDoc->getEdgeThreshold());
						edge_image[(j*width+i)*3 + 2] = 255 * (solbel >= m_pDoc->getEdgeThreshold());
						//printf("%d\n",solbel);
					}
				}
				m_pDoc->m_ucBitmap = edge_image;
				refresh();

			//	m_pDoc->m_pCurrentBrush->BrushEnd(Point(0, 0), Point(0, 0));

				// glRasterPos2i( 0, m_nWindowHeight - drawHeight );
				// glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
				// glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth );
				// glDrawBuffer( GL_BACK );
				// glDrawPixels( drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart );
				break;
			}
		}

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

float OriginalView::getGradient(Point pt){

	// int vectorX = 0, vectorY = 0;
	// for (int row = 0; row < 3; row++) {
	// 	for (int col = 0; col < 3; col++) {
	// 		Point point = Point(pt.x + row - 1, pt.y + col - 1);
	// 		GLubyte color[3];
	// 		memcpy(color, m_pDoc->GetOriginalPixel(point), 3);
	// 		gray[row][col] = 0.299*color[0] + 0.587*color[1] + 0.114*color[2];
	// 		gray_point += gray[row][col];
	// 	}
	// }
	// gray_point /= 9;
	// for (int row = 0; row < 3; row++) {
	// 	for (int col = 0; col < 3; col++) {
	// 		vectorX += gray[row][col] * sobelXOperator[row][col];
	// 		vectorY += gray[row][col] * sobelYOperator[row][col];
	// 	}
	// }
	// // vectorX= vectorX / 9;
	// // vectorY= vectorY / 9;
	// return sqrt(vectorX * vectorX + vectorY * vectorY);
	return 0;
}

void OriginalView::drawEdgeImage(){
	isAnEvent = true;
	eventToDo = EDGE_IMAGE;
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
