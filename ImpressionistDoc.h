// 
// impressionistDoc.h
//
// header file for Doc 
//

#ifndef ImpressionistDoc_h
#define ImpressionistDoc_h

#include "impressionist.h"
#include "bitmap.h"
#include <vector>

#define SHOW_ORIGINAL_IMAGE 0
#define SHOW_EDGE_IMAGE 1
#define SHOW_ANOTHER_IMAGE 2
#define SHOW_DISSOLVE_IMAGE 3

class ImpressionistUI;

class ImpressionistDoc 
{
public:
	ImpressionistDoc();

	void	setUI(ImpressionistUI* ui);		// Assign the UI to use

	int		loadImage(char *iname);			// called by the UI to load image
	int		loadAnotherImage(char *iname);			// called by the UI to load image
	int		loadEdgeImage(char *iname);			// called by the UI to load image
	int		loadDissolveImage(char *iname);
	int		swapImage();			// called by the UI to load image
	int		saveImage(char *iname);			// called by the UI to save image

	void	undo();
	int     clearCanvas();                  // called by the UI to clear the drawing canvas
	void	setBrushType(int type);			// called by the UI to set the brushType

	int		getSize();						// get the UI size
	void	setSize(int size);				// set the UI size
	
	char*	getImageName();					// get the current image name

	int     getSpace();
	int     getEdgeThreshold();
	boolean getIsEdgeClipping();
	boolean getIsAnotherGradient();
	boolean getIsRandSize();
	
	void	setBrushStrokeDirection(int direction);
	int		getBrushStrokeDirection();

	void paintCanvas(int space);

	void applyFilterKernal();

// Attributes
public:
	// Dimensions of original window.
	int				m_nWidth, 
					m_nHeight;
	// Dimensions of the paint window.
	int				m_nPaintWidth, 
					m_nPaintHeight;	
	// Bitmaps for original image and painting.
	unsigned char*	m_ucBitmap;
	unsigned char*	m_ucPainting;
	unsigned char*  m_nOrginalBitmap;
	unsigned char*  m_nEdgeImage;
	unsigned char*  m_nAnotherImage;
	unsigned char*	m_nDissolveImage;

	// Values for generate the gray and sobel image
	double*         m_nGray_val;
	int*            m_nSobel_val;

	// Painting history
	std::vector<unsigned char *> m_ucPainting_History;

	// The current active brush.
	ImpBrush* m_pCurrentBrush;	
	// Size of the brush.
	int m_nSize;							
	// Direction of brush
	int m_pStrokeDirection;

	ImpressionistUI*	m_pUI;

	// Mode
	int m_nMode;

// Operations
public:
	// Get the color of the original picture at the specified coord
	GLubyte*  GetOriginalPixel( int x, int y );   
	// Get the color of the original picture at the specified point	
	GLubyte*  GetOriginalPixel( const Point p );  
	void	  createDissolveImage(unsigned char* data, int width, int height);
	void      createEdgeImage();
	void      switchOriginalImage();
	void      switchEdgeImage();
	void      switchAnotherImage();
	
	double*   calGrayVal(int width, int height);
	int*      calSobelVal(double* gray_image, int width, int height);
	double	  applyFilter(int x, int y, int rgbChannel);

private:
	char			m_imageName[256];

public:
};

extern void MessageBox(char *message);

#endif
