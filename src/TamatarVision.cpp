/*
 *  TamatarVision.cpp
 *  tamatarTransform
 *
 *  Created by Silvan Zurbruegg on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "TamatarVision.h"



TamatarVision::TamatarVision() {
    
    thresholdMax=255;
    circleEdgeThres=300;
    circleAccThres=100;
    circleMinRadius=20;
    circleMaxRadius=80;
    doHistEqualize=true;
    morphExRadius = 20;
    doCanny=false;
    showSourceImg=false;
    doContours = false;
}

void TamatarVision::setup() {
    
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
    
    colorImg.allocate(camWidth, camHeight);
    grayImg.allocate(camWidth, camHeight);
    grayImg2.allocate(camWidth, camHeight);
    
    gui.addPage("Tamatar Vision");

	gui.addTitle("Source Image");
    gui.addContent("Source Image", colorImg);
    gui.addToggle("Show source image", showSourceImg);
    //gui.addToggle("Threshold", doThreshold);
	//gui.addSlider("Threshold value", threshold, 0, 255);
    //gui.addSlider("Threshold max", thresholdMax, 0, 255);
    
    gui.addToggle("Smoothing", doSmoothing);
	//gui.addSlider("Color sigma", smoothSigmaColor, 0, 255); 
    //gui.addSlider("Spatial sigma", smoothSigmaSpatial, 0, 255);
    gui.addToggle("Hist. equalize", doHistEqualize);
    
    gui.addToggle("Circle morph", doMorphEx);
    gui.addSlider("Radius", morphExRadius, 0, 30);
    
    gui.addTitle("Tamatar Detection").setNewColumn(true);
    gui.addToggle("Canny", doCanny);
    gui.addSlider("Thres 1", cannyThres1, 10, 500);
    gui.addSlider("Thres 2", cannyThres2, 10, 500);
    
    gui.addToggle("Cirlces", doCircles);
    gui.addSlider("Edge threshold", circleEdgeThres, 10, 500);
    gui.addSlider("Accum. threshold", circleAccThres, 10, 500);
    gui.addSlider("Min. radius", circleMinRadius, 0, camHeight / 4);
    gui.addSlider("Max. radius", circleMaxRadius, 0, camHeight / 4);

    gui.loadFromXML();
    
}


void TamatarVision::update() {
    vidGrabber.grabFrame();
    if (vidGrabber.isFrameNew()) {
        // load image from videograbber
        colorImg.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
        // convert to grayscale
        cvCvtColor( colorImg.getCvImage(), grayImg.getCvImage(), CV_RGB2GRAY );
        grayImg.flagImageChanged();
        
        // equalize histogram
        if (doHistEqualize) {
            cvEqualizeHist(grayImg.getCvImage(), grayImg.getCvImage() );
        }
        
        // `morphological opening`
        if (doMorphEx) {
            int anchor = morphExRadius / 2;
            structure = cvCreateStructuringElementEx(morphExRadius, morphExRadius, anchor, anchor, CV_SHAPE_ELLIPSE);
            cvCopy(grayImg.getCvImage(), grayImg2.getCvImage());
            cvMorphologyEx(grayImg2.getCvImage(), grayImg.getCvImage(), NULL, structure, CV_MOP_OPEN);
        }
        
        if (doSmoothing) {
            //grayImg2 = grayImg;
            //smoothSigmaColor=20;
            //smoothSigmaSpatial=20;
            //cvSmooth(grayImg2.getCvImage(), grayImg.getCvImage(), CV_BILATERAL, 9, 9, smoothSigmaColor, smoothSigmaSpatial);
            cvSmooth(grayImg.getCvImage(), grayImg.getCvImage(), CV_GAUSSIAN, 3, 3, 2, 2);
        }
        
        //grayImg.threshold(120);
        
        // threshold
        if (doThreshold) {
            //            grayImg.threshold(threshold);
            grayImg2 = grayImg;
            cvThreshold(grayImg2.getCvImage(), grayImg.getCvImage(), threshold, thresholdMax, CV_THRESH_TOZERO);
            //   cvAdaptiveThreshold(grayImg2.getCvImage(), grayImg.getCvImage(), threshold, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_, 3, 5);
        }
        
        if (doCanny) {
            cvCanny(grayImg.getCvImage(), grayImg.getCvImage(), cannyThres1, cannyThres2, 3);
        }
        
        //cvCanny5grayImg.getCvImage(),grayImg.getCvImage(), 120, 180, 3);
        //cvSobel(grayImg.getCvImage(), grayImg.getCvImage(), 1, 1, 3);
        if (doCircles) {
            CvMemStorage* storage = cvCreateMemStorage(0);
            circles = cvHoughCircles(grayImg.getCvImage(), storage, CV_HOUGH_GRADIENT, 2, grayImg.getHeight()/4, circleEdgeThres, circleAccThres, circleMinRadius, circleMaxRadius);
        }
        
        if (doContours) {
            contourFinder.findContours(grayImg, 10, (camWidth*camHeight)/2, 20, false, true);
        }
    }        
}

void TamatarVision::draw() {
    ofSetColor(255, 255, 255);
    
    if (showSourceImg) {
        colorImg.draw(offsetX, offsetY, camWidth, camHeight);
    } else {
        grayImg.draw(offsetX, offsetY);
    }
    
    if (doContours) {
        contourFinder.draw(offsetX, offsetY, camWidth, camHeight);
    }
    
    ofSetColor(255, 105, 0);
    if (circles && circles->total > 0) {
        for (int i = 0; i < circles->total; i++) {
            float* p = (float*) cvGetSeqElem(circles, i);
            
            //CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
            ofCircle(offsetX+p[0],offsetY+p[1], p[2]);
        }
        //cvClearSeq(circles);
    }
}

