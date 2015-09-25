// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//

#include <FL/fl_ask.H>

#include "impressionistDoc.h"
#include "impressionistUI.h"

#include "ImpBrush.h"

// Include individual brush headers here.
#include "PointBrush.h"
#include "CircleBrush.h"
#include "LineBrush.h"
#include "ScattedPointBrush.h"
#include "ScatteredCircleBrush.h"
#include "ScatteredLineBrush.h"
#include "TriangleBrush.h"
#include "ScatteredTriangleBrush.h"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

ImpressionistDoc::ImpressionistDoc() 
{
	// Set NULL image name as init. 
	m_imageName[0]	   ='\0';	

	m_nWidth		   = -1;
	m_ucBitmap		   = NULL;
	m_ucPainting	   = NULL;
	m_nGray_val        = NULL;
	m_nSobel_val       = NULL;
	m_nOrginalBitmap   = NULL;
	m_nEdgeImage       = NULL;
	m_nAnotherImage    = NULL;


	// create one instance of each brush
	ImpBrush::c_nBrushCount	= NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes	= new ImpBrush* [ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS]	= new PointBrush( this, "Points" );

	// Note: You should implement these 5 brushes.  They are set the same (PointBrush) for now
	ImpBrush::c_pBrushes[BRUSH_LINES]				
		= new LineBrush( this, "Lines" );
	ImpBrush::c_pBrushes[BRUSH_CIRCLES]				
		= new CircleBrush(this, "Circles");
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]	
		= new ScattedPointBrush( this, "Scattered Points" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]		
		= new ScatteredLineBrush( this, "Scattered Lines" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]	
		= new ScatteredCircleBrush( this, "Scattered Circles" );
	ImpBrush::c_pBrushes[BRUSH_TRIANGLE]
		= new TriangleBrush(this, "Triangle");
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_TRIANGLE]
		= new ScatteredTriangleBrush(this, "Scattered Triangle");

	// make one of the brushes current
	m_pCurrentBrush	= ImpBrush::c_pBrushes[0];


	// Set the stroke direction
	m_pStrokeDirection = STROKE_DIRECTION_SLIDER;
}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName() 
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush	= ImpBrush::c_pBrushes[type];
}

void ImpressionistDoc::setBrushStrokeDirection(int direction) {
	m_pStrokeDirection = direction;
}

int ImpressionistDoc::getBrushStrokeDirection() {
	return m_pStrokeDirection;
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}

boolean ImpressionistDoc::getIsRandSize(){
	return m_pUI->getIsRandSize();
}

int ImpressionistDoc::getSpace(){
	return m_pUI->getSpace();
}
int ImpressionistDoc::getEdgeThreshold(){
	return m_pUI->getEdgeThreshold();
}
boolean ImpressionistDoc::getIsEdgeClipping(){
	return m_pUI->getIsEdgeClipping();
}
boolean ImpressionistDoc::getIsAnotherGradient(){
	return m_pUI->getIsAnotherGradient();
}

void ImpressionistDoc::undo() {
	if (!m_ucPainting_History.empty()) {
		if (m_ucPainting) delete[] m_ucPainting;
		m_ucPainting = m_ucPainting_History.back();
		m_ucPainting_History.pop_back();
		m_pUI->m_paintView->refresh();
	}
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(iname, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth		= width;
	m_nPaintWidth	= width;
	m_nHeight		= height;
	m_nPaintHeight	= height;

	// release old storage
	if ( m_ucBitmap )      delete [] m_ucBitmap;
	if ( m_ucPainting )    delete [] m_ucPainting;
	if ( m_nGray_val )     delete [] m_nGray_val;
	if ( m_nSobel_val )    delete [] m_nSobel_val;
	if ( m_nEdgeImage )    delete [] m_nEdgeImage;
	if ( m_nAnotherImage ) delete [] m_nAnotherImage;

	m_ucPainting_History.clear();

	m_ucBitmap		 = data;
	m_nOrginalBitmap = data; // backup the original bitmap
	m_nAnotherImage  = new unsigned char[m_nWidth*m_nHeight*3];
	memcpy(m_nAnotherImage, data, width*height*3);

	// cal the gray and sobel value of image for create the edge image
	m_nGray_val = calGrayVal(m_nWidth, m_nHeight);
	m_nSobel_val = calSobelVal(m_nGray_val, m_nWidth, m_nHeight);
	createEdgeImage();



	// allocate space for draw view
	m_ucPainting = new unsigned char [width*height*3];
	memset(m_ucPainting, 0, width*height*3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width*2, 
								height+25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);	
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);	
	m_pUI->m_paintView->refresh();

	m_nMode = SHOW_ORIGINAL_IMAGE;

	return 1;
}

int ImpressionistDoc::loadEdgeImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(iname, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if(	m_nWidth != width || m_nHeight != height){
		fl_alert("Can't load bitmap file, because the width and height does not mathc with the original image.");
		return 0;
	}

	if ( m_nEdgeImage )    delete [] m_nEdgeImage;


	m_nEdgeImage = data;

	m_nMode = SHOW_EDGE_IMAGE;

	return 1;
}
int ImpressionistDoc::loadAnotherImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;


	if ( (data=readBMP(iname, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if(	m_nWidth != width || m_nHeight != height){
		fl_alert("Can't load bitmap file, because the width and height does not mathc with the original image.");
		return 0;
	}

	if ( m_nAnotherImage ) delete [] m_nAnotherImage;

	m_nMode = SHOW_ANOTHER_IMAGE;
	m_nAnotherImage = data;

	return 1;
}
int ImpressionistDoc::swapImage() 
{
	if(!m_ucBitmap || !m_ucPainting) return 0;
	unsigned char* temp = m_ucBitmap;
	m_ucBitmap = m_ucPainting;
	m_ucPainting = temp;
	
	m_pUI->m_origView->refresh();
	m_pUI->m_paintView->refresh();

	return 1;
}


//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char *iname) 
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);
	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas() 
{

	// Release old storage
	if ( m_ucPainting ) 
	{
		delete [] m_ucPainting;

		// allocate space for draw view
		m_ucPainting	= new unsigned char [m_nPaintWidth*m_nPaintHeight*3];
		memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight*3);

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}
	return 1;

}

//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( int x, int y )
{
	if ( x < 0 ) 
		x = 0;
	else if ( x >= m_nWidth ) 
		x = m_nWidth-1;

	if ( y < 0 ) 
		y = 0;
	else if ( y >= m_nHeight ) 
		y = m_nHeight-1;

	return (GLubyte*)(m_ucBitmap + 3 * (y*m_nWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( const Point p )
{
	return GetOriginalPixel( p.x, p.y );
}

void ImpressionistDoc::paintCanvas(int space) {
	for (int i = 0; i < m_nWidth; i += space) {
		for (int j = 0; j < m_nHeight; j += space) {
		
		}
	}
}

//----------------------------------------------------------------
// create the edge image
//----------------------------------------------------------------

void ImpressionistDoc::switchOriginalImage(){
	//m_ucBitmap = m_nOrginalBitmap;
	m_nMode = SHOW_ORIGINAL_IMAGE;
	m_pUI->m_origView->refresh();
}
void ImpressionistDoc::createEdgeImage(){

	if ( m_nEdgeImage ) delete [] m_nEdgeImage;

	unsigned char* edge_image = new unsigned char[m_nWidth*m_nHeight * 3];
	for (int i = 0; i < m_nWidth; i += 1){
		for (int j = 0; j < m_nHeight; j += 1){
			int index = i + j * m_nWidth;
			edge_image[(index)* 3 + 0] =
				edge_image[(index)* 3 + 1] =
				edge_image[(index)* 3 + 2] =
				255 * (m_nSobel_val[index] >= getEdgeThreshold());
		}
	}
	m_nEdgeImage = edge_image;


}
void ImpressionistDoc::switchEdgeImage(){
	//m_ucBitmap = m_nEdgeImage;
	m_nMode = SHOW_EDGE_IMAGE;
	m_pUI->m_origView->refresh();
}
void ImpressionistDoc::switchAnotherImage(){

	//m_ucBitmap = m_nAnotherImage;
	m_nMode = SHOW_ANOTHER_IMAGE;
	m_pUI->m_origView->refresh();

}

//----------------------------------------------------------------
// create the gray value matrix
//----------------------------------------------------------------
double* ImpressionistDoc::calGrayVal(int width, int height){

	double* gray_image = new double[width*height];

	for (int i = 0; i < width; i +=1){
		for (int j = 0; j < height; j +=1){
			int index = i+j*width;
			GLubyte color[3];
			memcpy(color, GetOriginalPixel(Point(i, j)), 3);
			gray_image[index] = 0.299*color[0] + 0.587*color[1] + 0.114*color[2];
		}
	}
	return gray_image;
}

//----------------------------------------------------------------
// create the sobel value matrix
//----------------------------------------------------------------

int* ImpressionistDoc::calSobelVal(double* gray_image, int width, int height){

	int* sobel_val = new int[width*height];
	double* sobelX = new double[width*height];
	double* sobelY = new double[width*height];

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

	// gray to sobel
	for (int i = 0; i < width; i +=1){
		for (int j = 0; j < height; j +=1){
			int index = i+j*width;
			sobelX[index] = sobelY[index] = 0;
			for (int row = 0; row < 3; row++) {
				for (int col = 0; col < 3; col++) {
					int l = i + col - 1;
					int k = j + row - 1;
					if (l < 0 || l >= width) l = 0;
					if (k < 0 || k >= height) k = 0;
					sobelX[index] += gray_image[l+k*width] * sobelXOperator[row][col];
					sobelY[index] += gray_image[l+k*width] * sobelYOperator[row][col];
				}
			}

			sobel_val[index] = (int) sqrt(sobelX[index]*sobelX[index] + sobelY[index]*sobelY[index]);


		}
	}
	return sobel_val;
}

void ImpressionistDoc::applyFilterKernal() {
	
	unsigned char* filteredImage = new unsigned char[m_nWidth * m_nHeight * 3];
	memset(filteredImage, 0, m_nWidth * m_nHeight * 3);

	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidth; j++) {
			int position = (i * m_nWidth + j) * 3;
			for (int k = 0; k < 3; k++) {
				filteredImage[position + k] = applyFilter(j, i, k);
			}
		}
	}

	m_ucPainting = filteredImage;
	m_pUI->m_paintView->refresh();
}

double ImpressionistDoc::applyFilter(int x, int y, int rgbChannel) {
	const int filterWidth = m_pUI->getFilterWidth();
	const int filterHeight = m_pUI->getFilterHeight();
	double result = 0.0;
	double weightSum = 0.0;
	double* filterKernal = m_pUI->getFilterKernal();
	int startX = x - filterWidth / 2;
	int startY = y - filterHeight / 2;

	for (int m = 0; m < filterHeight; m++) {
		int y = m + startY;
		if (y < 0 || y >= m_nHeight) {
			continue;
		}
		for (int n = 0; n < filterWidth; n++) {
			int x = n + startX;
			if (x < 0 || x >= m_nWidth) {
				continue;
			}
			result += (double)(m_ucBitmap[(y * m_nWidth + x) * 3 + rgbChannel]) * filterKernal[m * filterWidth + n];
			weightSum += filterKernal[m * filterWidth + n];
		}
	}

	return result / weightSum;
}