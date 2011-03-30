/*
 *  Plane.h
 *  tamatarTransform
 *
 *  Created by Silvan Zurbruegg on 3/6/11.
 *
 */
#ifndef _TAMATAR_PLANE
#define _TAMATAR_PLANE

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

typedef struct {
    CvPoint3D32f* pt;
	float 	x;
	float 	y;
    float   z;
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;
	
}draggableVertex;

class Plane {

public:
    Plane();
    Plane(int color, int i);
    
    void loadFromXml();
    void saveToXml();
    void draw();
    void drawAlignOffset();
    
    void setColor(int color);
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y);
    void mouseDragged(int x, int y);
    void mouseReleased(int x, int y);
    void setVertex(int pos, int x, int y);
    
    void setSrcPt(float x, float y, float z, int index);
    void setSrcPtZ(float z, int index);
    
    void resetDstPt();
    void moveSrcPts(int val, int dir);
    
    float getZFromY(float y);
    CvPoint3D32f* getMaxZ();
    CvPoint3D32f* getMinZ();
    CvPoint3D32f* getSrcPtLeft();
    
    draggableVertex* getVertex(int pos);
    
    int     id;
    string  xmlfile;
    
    // drawing offsets
    int offsetX;
    int offsetY;
    
    // mode (0 -> standard, 1 -> perspective)
    int mode;
    
    // source/destination point sequences
    CvSeq* srcPtSeq;
    CvSeq* dstPtSeq;
    CvPoint3D32f srcPt[4];
    CvPoint3D32f dstPt[4];
    
protected:
    
    void initPoints();
    
    draggableVertex vertices[4];
    int nVertices;
    int color;
    
    // xml configuration
    ofxXmlSettings      XML;
    
    // source/destination points/storage
    CvMemStorage* srcPtStorage;
    CvMemStorage* dstPtStorage;

};


#endif