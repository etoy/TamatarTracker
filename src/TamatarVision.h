/*
 *  TamatarVision.h
 *  tamatarTransform
 *
 *  Created by Silvan Zurbruegg on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
//#ifndef _TAMATAR_VISION
//#define _TAMATAR_VISION


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxSimpleGuiToo.h"

class TamatarVision {
public:
    TamatarVision();

    void setup();
    void update();
    void draw();
    
    ofVideoGrabber 		vidGrabber;
    
    ofxCvColorImage     colorImg;
    ofxCvGrayscaleImage grayImg; 
    ofxCvGrayscaleImage grayImg2;
    ofxCvContourFinder  contourFinder;
    CvSeq               *circles;
    
    int     camWidth;
    int     camHeight;
    int     offsetX;
    int     offsetY;

protected:
    ofColor	aColor;
    bool	doThreshold;
    int     threshold;
    int     thresholdMax;
    bool    doSmoothing;
    int     smoothing;
    int     smoothSigmaColor;
    int     smoothSigmaSpatial;
    bool    doCircles;
    int     circleEdgeThres;
    int     circleAccThres;
    int     circleMinRadius;
    int     circleMaxRadius;
    bool    doHistEqualize;
    
    bool    doMorphEx;
    int     morphExRadius;
    
    bool    doCanny;
    bool    doContours;
    bool    showSourceImg;
    int     cannyThres1;
    int     cannyThres2;
    
    IplConvKernel* structure;
    
    
    float	myFloat1;
    int myInt1;
    int      box1;
    bool	randomizeButton;
};


//#endif