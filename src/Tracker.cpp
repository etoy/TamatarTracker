#include "Tracker.h"

// intrinsic matrix
CvMat* intrinsic;
CvMat* distortion;

// CvPoint3D32f sequences to hold points of detected tamatar
CvMemStorage* ptStorage = cvCreateMemStorage(0);
CvMemStorage* ptStorageDst = cvCreateMemStorage(0);
CvSeq* ptSeq  = cvCreateSeq(CV_32FC3, sizeof(CvSeq), sizeof(CvPoint3D32f), ptStorage);
CvSeq* ptSeqDst = cvCreateSeq(CV_32FC3, sizeof(CvSeq), sizeof(CvPoint3D32f), ptStorageDst);

// floats for matrices
float mx[16], my[16], mz[16];
float mp[16], mh[16], tm1[16], vp[16], cm[16];
float mpi[16], mti[16];

// rotation matrices
CvMat rotZMatrix        = cvMat(4,4, CV_32F, mz);
CvMat rotYMatrix        = cvMat(4,4, CV_32F, my); 
CvMat rotXMatrix        = cvMat(4,4, CV_32F, mx);

// perspective/translation/viewport matrices
CvMat persMatrix        = cvMat(4,4, CV_32F, mp);
CvMat transMatrix       = cvMat(4,4, CV_32F, tm1);
CvMat viewportMatrix    = cvMat(4,4, CV_32F, vp);
CvMat centerMatrix      = cvMat(4,4, CV_32F, cm);

// inverse matrices
CvMat matPerspectiveInv = cvMat(4,4, CV_32F, mpi);
CvMat matTransformInv   = cvMat(4,4, CV_32F, mti);



bool rotZInc, rotZDec, click;

// gui variables
int camX = 0;
int camY = 0;
int camZ = 0;

bool doTransform = true;

// drawing offsets
int offsetX = 480;
int offsetY = 115;

CvPoint3D32f cameraPt, testPt, testPtDst;

//--------------------------------------------------------------
void Tracker::setup(){
    ofSetFrameRate(30);
    ofEnableSmoothing();
    
    
    tVision.camWidth    = 640;
    tVision.camHeight   = 480;
    tVision.offsetX     = offsetX;
    tVision.offsetY     = offsetY;
    
    intrinsic = (CvMat*)cvLoad("Intrinsics.xml");
    distortion = (CvMat*)cvLoad("Distortion.xml");
    
    plane1 = Plane(0xff6600, 1);
    plane1.offsetX = offsetX;
    plane1.offsetY = offsetY;
    plane1.setSrcPt(-100, 20, 15, 0);
    plane1.setSrcPt(100, 20, 15, 1);
    plane1.setSrcPt(100, 220, 10, 2);
    plane1.setSrcPt(-100, 220, 10, 3);
    plane1.resetDstPt();
    
    plane2 = Plane(0x00ff00, 2);
    plane2.offsetX = offsetX;
    plane2.offsetY = offsetY;
    plane2.loadFromXml();
    
    // 'gui' is a global variable declared in ofxSimpleGuiToo.h
    doSaveSettings=false;
    planeZ = 15;
    gui.addTitle("Camera");
    
    gui.addToggle("Transform", doTransform);	
	gui.addSlider("X-Pos", camX, 0, 640);
    gui.addSlider("Y-Pos", camY, 0, 480);
    gui.addSlider("Z-Pos", camZ, -100, 100);
    
    gui.addSlider("X-Rot", camRotX, 0, 360);
    gui.addSlider("Y-Rot", camRotY, 0, 360);
    gui.addSlider("Z-Rot", camRotZ, 0, 360);
    
    
    
    gui.addTitle("Plane").setNewColumn(true);
    gui.addButton("Save settings", doSaveSettings);
    gui.addSlider("Z-Depth", planeZ, 10, 100);

    gui.loadFromXML();
    gui.show();
    
    testPt.x = 200;
    testPt.y = 180;
    testPt.z = 12.5;
    
    testPtDst.x = 200;
    testPtDst.y = 100;
    testPtDst.z = 10;
    
    //cvSeqPush(ptSeq, &testPt);
    //cvSeqPush(ptSeqDst, &testPtDst);
   
    tVision.setup();
    
    
}




//--------------------------------------------------------------
void Tracker::update(){
    tVision.update();
    
    if (doTransform) {
        plane2.mode = 1;
        
        plane2.setSrcPtZ(planeZ, 0);
        plane2.setSrcPtZ(planeZ, 1);
        
        cameraPt.x = camX;
        cameraPt.y = camY;
        cameraPt.z = camZ;
        
        transformPlane();
        transformPlaneInverse();
        transformPoints();
        
    } else {
        
        plane2.mode = 0;
        plane2.resetDstPt();
        
    }
    
    if (doSaveSettings) {
        plane2.saveToXml();
    }
    
}



//--------------------------------------------------------------
void Tracker::draw(){
    ofBackground(0, 0, 0);
	
    
    ofSetColor(255, 255, 255);
    
    tVision.draw();
    
    plane1.drawAlignOffset();
    plane2.draw();

    ofSetHexColor(0xffffff);    
    ofFill();
    printf("ptsc: %d\n", ptSeq->total);
    for (int i = 0; i < ptSeq->total; i++){
        
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(ptSeq, i);
        int diffx = offsetX - pt->x;
        int diffy = offsetY - pt->y;
        ofCircle(offsetX + pt->x, offsetY + pt->y, 4);
        
        
        
    }       
        
    ofSetColor(255, 255, 255);
    gui.draw();
}


void Tracker::transformPlaneInverse() {

    cvInvert(&persMatrix, &matPerspectiveInv);
    cvInvert(&viewportMatrix, &matTransformInv);
    
    
    for(int i=0; i<plane1.dstPtSeq->total; i++) {
        CvPoint3D32f *p2 = (CvPoint3D32f*) cvGetSeqElem(plane2.dstPtSeq, i);
        CvPoint3D32f *p1 = (CvPoint3D32f*) cvGetSeqElem(plane1.dstPtSeq, i);
        p1->x = p2->x;
        p1->y = p2->y;
        p1->z = p2->z;
        
    }
    
    cvPerspectiveTransform(plane1.dstPtSeq, plane1.dstPtSeq, &matTransformInv);
    cvPerspectiveTransform(plane1.dstPtSeq, plane1.dstPtSeq, &matPerspectiveInv);
    cvPerspectiveTransform(plane1.dstPtSeq, plane1.dstPtSeq, &viewportMatrix);
    
}

void Tracker::transformPoints() {
    int tmax = 20;
    CvPoint3D32f pt[tmax];
    cvClearSeq(ptSeq);
     
    if (tVision.circles && tVision.circles->total > 0) {
        for (int i = 0; i < tVision.circles->total; i++) {
            float* p = (float*) cvGetSeqElem(tVision.circles, i);
            pt[i] = cvPoint3D32f(p[0], p[1], plane2.getZFromY(p[1])); 
            cvSeqPush(ptSeq, &pt[i]);
        }
    }
    
    if (ptSeq->total > 0) {
        cvPerspectiveTransform(ptSeq, ptSeq, &matTransformInv);
        cvPerspectiveTransform(ptSeq, ptSeq, &matPerspectiveInv);
        cvPerspectiveTransform(ptSeq, ptSeq, &viewportMatrix);
    }
}

void Tracker::transformPlane() {
    
    plane2.resetDstPt();
    
    // perspective
    updatePerspectiveMatrix(&persMatrix); 

    // camera shift
    for(int i=0; i<plane2.dstPtSeq->total; i++) {
        CvPoint3D32f *p = (CvPoint3D32f*) cvGetSeqElem(plane2.dstPtSeq, i);
        p->x = p->x - cameraPt.x;
        p->y = p->y - cameraPt.y;
        p->z = p->z - cameraPt.z;
        
    }
    
    // rotation
    rotX(camRotX, &rotXMatrix);
    rotY(camRotY, &rotYMatrix);
    rotZ(camRotZ, &rotZMatrix);
    
    cvMatMulAdd(&rotXMatrix, &rotYMatrix, 0, &rotYMatrix);
    cvMatMulAdd(&rotYMatrix, &rotZMatrix, 0, &rotZMatrix);
    cvMatMulAdd(&rotZMatrix, &persMatrix, 0, &persMatrix);
    
    cvPerspectiveTransform(plane2.dstPtSeq, plane2.dstPtSeq, &persMatrix);
    setTranslation(cameraPt.x, cameraPt.y, cameraPt.z, &viewportMatrix);
    cvPerspectiveTransform(plane2.dstPtSeq, plane2.dstPtSeq, &viewportMatrix);
    
}

// set z-rotation matrix
void Tracker::rotZ(float angle, CvMat *mat) {
    initMatrix(mat);
    if (0 == angle) {
        return;
    }
    double rad = ofDegToRad(angle);
   
//    cvZero(mat);
    
    cvmSet(mat, 0, 0, cos(rad));
    cvmSet(mat, 0, 1, -sin(rad));
    cvmSet(mat, 1, 0, sin(rad));
    cvmSet(mat, 1, 1, cos(rad));
    cvmSet(mat, 2, 2, 1);
}

// set y-rotation matrix
void Tracker::rotY(float angle, CvMat *mat) {
    initMatrix(mat);
    if (0==angle) {
        return;
    }
    double rad = ofDegToRad(angle);
    cvmSet(mat, 0, 0, cos(rad));
    cvmSet(mat, 0, 2, -sin(rad));
    cvmSet(mat, 1, 1, 1);
    cvmSet(mat, 2, 0, sin(rad));
    cvmSet(mat, 2, 2, cos(rad));
}

// set x-rotation matrix
void Tracker::rotX(float angle, CvMat *mat) {
    initMatrix(mat);
    if (0 == angle) {
        return;
    }
    double rad = ofDegToRad(angle);
    cvmSet(mat, 0, 0, 1);
    cvmSet(mat, 1, 1, cos(rad));
    cvmSet(mat, 1, 2, -sin(rad));
    cvmSet(mat, 2, 1, sin(rad));
    cvmSet(mat, 2, 2, cos(rad));
}

void Tracker::updatePerspectiveMatrix(CvMat* mat) {
    float angle2=ofDegToRad(90);
    float near = 1;
    float far = 1000;
    
    float ratio = 4.0/3.0;
    float height = 1/tan(ofDegToRad(140 / 2));
    float width = height / ratio;
    
    float d = near - far;
    
    cvZero(mat);
    cvmSet(mat, 0, 0, (2*near) / width);
    cvmSet(mat, 1, 1, (2*near) / height);
    cvmSet(mat, 2, 2, far / d);
    cvmSet(mat, 2, 3, ((far*near)/d));
    cvmSet(mat, 3, 2, 1);
        
}

void Tracker::setTranslation(float tx, float ty, float tz, CvMat* mat) {
    cvZero(mat);
    cvmSet(mat, 0, 0, 1);
    cvmSet(mat, 1, 1, 1);
    cvmSet(mat, 2, 2, 1);
    
    cvmSet(mat, 0, 3, tx);
    cvmSet(mat, 1, 3, ty);
    cvmSet(mat, 2, 3, tz);
    cvmSet(mat, 3, 3, 1);
}

void Tracker::setViewport(float width, float height, CvMat* mat) {
    cvZero(mat);
    cvmSet(mat, 0, 0, width/2.0);
    cvmSet(mat, 1, 1, -(height/2.0));
    cvmSet(mat, 2, 2, 1);
    
   // cvmSet(mat, 0, 3, width);
   // cvmSet(mat, 1, 3, height);
   // cvmSet(mat, 3, 3, 1);
}


// init identity matrix
void Tracker::initMatrix(CvMat* mat) {
    cvZero(mat);
    cvmSet(mat, 0, 0, 1);
    cvmSet(mat, 1, 1, 1);
    cvmSet(mat, 2, 2, 1);
    cvmSet(mat, 3, 3, 1);
    
}


//--------------------------------------------------------------
void Tracker::keyPressed(int key){
    if(key>='0' && key<='9') {
		gui.setPage(key - '0');
		gui.show();
	} else {
		switch(key) {
			case ' ': gui.toggleDraw(); break;
			case '[': gui.prevPage(); break;
			case ']': gui.nextPage(); break;
			case 'p': gui.nextPageWithBlank(); break;
            case OF_KEY_LEFT:
                //plane2.moveSrcPts(2, 0);
                break;
            case OF_KEY_RIGHT:
                //plane2.moveSrcPts(2, 1);
                break;
            case OF_KEY_UP:
                //plane2.moveSrcPts(2, 2);
                break;
            case OF_KEY_DOWN:
                //plane2.moveSrcPts(2 , 3);
                break;
        }
	}
}

//--------------------------------------------------------------
void Tracker::keyReleased(int key){

}

//--------------------------------------------------------------
void Tracker::mouseMoved(int x, int y ){
    plane1.mouseMoved(x, y);
    plane2.mouseMoved(x, y);
}

//--------------------------------------------------------------
void Tracker::mouseDragged(int x, int y, int button){
    plane1.mouseDragged(x, y);
    plane2.mouseDragged(x, y);
}

//--------------------------------------------------------------
void Tracker::mousePressed(int x, int y, int button){
    plane1.mousePressed(x, y);
    plane2.mousePressed(x, y);
}

//--------------------------------------------------------------
void Tracker::mouseReleased(int x, int y, int button){
    plane1.mouseReleased(x, y);
    plane2.mouseReleased(x, y);
}

//--------------------------------------------------------------
void Tracker::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Tracker::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Tracker::dragEvent(ofDragInfo dragInfo){ 

}