#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxSimpleGuiToo.h"
#include "ofxXmlSettings.h"

#include "TamatarVision.h"
#include "Plane.h"

class Tracker : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        
        void rotX(float angle, CvMat *mat);
        void rotY(float angle, CvMat *mat);
        void rotZ(float angle, CvMat *mat);
        void updatePerspectiveMatrix(CvMat* mat);
        
        void initMatrix(CvMat* mat);
    
        void setTranslation(float tx, float ty, float tz, CvMat* mat);
        void setViewport(float width, float height, CvMat* mat);
        
        void transformPlaneInverse();
        void transformPoints();
        void transformPlane();
        
        bool    doSaveSettings;
        int     planeZ;
        ofVideoGrabber 		vidGrabber;
        
        Plane   plane1;
        Plane   plane2;
        
        TamatarVision       tVision;
        
        float camRotX;
        float camRotY;
        float camRotZ;
    
        
        
		
};
