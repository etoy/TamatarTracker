/*
 *  Plane.cpp
 *  tamatarTransform
 *
 *  Created by Silvan Zurbruegg on 3/6/11.
 *
 */

#include "Plane.h"
Plane::Plane() {
    
}

Plane::Plane(int colr=0xff6600, int i=0) {
    id=i;
    mode = 0;
    color = colr;
    nVertices = 4;
    for(int i=0; i<nVertices; i++) {
        vertices[i].bBeingDragged = false;
        vertices[i].bOver = false;
        vertices[i].radius = 4;
    }
    
    xmlfile = "plane-"+ofToString(id)+".xml";
    
    initPoints();
    
}

void Plane::loadFromXml() {
    
    if (!XML.loadFile(xmlfile)) {
        return;
    }
    XML.pushTag("plane", 0);
    XML.pushTag("points", 0);
    for (int i=0; i<4; i++) {
        float x = XML.getValue("pt:x", 0, i);
        float y = XML.getValue("pt:y", 0, i);
        float z = XML.getValue("pt:z", 0, i);
        int index = XML.getValue("pt:index",0, i);
        setSrcPt(x, y, z, index);
    }
    XML.popTag();
}


void Plane::saveToXml() {
    XML.pushTag("plane", 0);
    XML.pushTag("points", 0);
    for(int i=0; i<4; i++) {
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, i);
        XML.setValue("pt:x", pt->x, i);
        XML.setValue("pt:y", pt->y, i);
        XML.setValue("pt:z", pt->z, i);
        XML.setValue("pt:index", i, i);
    }
    XML.popTag();
    XML.saveFile(xmlfile);
}


// set source point 
void Plane::setSrcPt(float x, float y, float z, int index) {
    
    CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, index);
    pt->x = x;
    pt->y = y;
    pt->z = z;

}

void Plane::setSrcPtZ(float z, int index) {
    CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, index);
    pt->z = z;
}

void Plane::initPoints() {
    
    srcPtStorage = cvCreateMemStorage(0);
    dstPtStorage = cvCreateMemStorage(0);
    srcPtSeq = cvCreateSeq(CV_32FC3, sizeof(CvSeq), sizeof(CvPoint3D32f), srcPtStorage);
    dstPtSeq = cvCreateSeq(CV_32FC3, sizeof(CvSeq), sizeof(CvPoint3D32f), dstPtStorage);
    
    for(int i=0; i<4; i++) {
        srcPt[i].x = 0;
        srcPt[i].y = 0;
        srcPt[i].z = 0;
        dstPt[i].x = 0;
        dstPt[i].y = 0;
        dstPt[i].z = 0;
        cvSeqPush(srcPtSeq, &srcPt[i]);
        cvSeqPush(dstPtSeq, &dstPt[i]);
    }
}


void Plane::resetDstPt() {
    for(int i=0; i<srcPtSeq->total; i++) {
        CvPoint3D32f *psrc = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, i);
        CvPoint3D32f *pdst = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        pdst->x = psrc->x;
        pdst->y = psrc->y;
        pdst->z = psrc->z;
    }
}

draggableVertex* Plane::getVertex(int pos) {
    return &vertices[pos];
}

void Plane::setVertex(int pos, int x, int y) {
    vertices[pos].x = x;
    vertices[pos].y = y;
}


void Plane::setColor(int c) {
    color = c;
}


CvPoint3D32f* Plane::getMaxZ() {
    float z=-100;
    CvPoint3D32f *maxp;
    for (int i = 0; i < dstPtSeq->total; i++){
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        if (pt->z > z) {
            z = pt->z;
            maxp = pt;
        }
    }
    
    return maxp;
}


CvPoint3D32f* Plane::getMinZ() {
    float z = 100;
    CvPoint3D32f *minp;
    for (int i = 0; i < dstPtSeq->total; i++){
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        if (pt->z < z) {
            z = pt->z;
            minp = pt;
        }
    }
    
    return minp;

}

float Plane::getZFromY(float y) {
    CvPoint3D32f* maxp = getMaxZ();
    CvPoint3D32f* minp = getMinZ();
    
    float maxdist = abs(maxp->y - minp->y);
    float mindist = abs(y - min(maxp->y, minp->y));
    float diff = abs(maxp->z - minp->z);
    float diff2 = (mindist * diff) / maxdist;
    return diff2 + min(minp->z, maxp->z);
    
    //return z;
}

void Plane::moveSrcPts(int val, int dir) {
    
    for (int i = 0; i < nVertices; i++){
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, i);
        switch(dir) {
            // left
            case 0:
                pt->x = pt->x - val;
                break;
            // right
            case 1:
                pt->x = pt->x + val;
                break;
            // up
            case 2:
                pt->y = pt->y - val;
                break;
            // down
            case 3: 
                pt->y = pt->y + val;
                break;
        }
    }
}

void Plane::mouseMoved(int x, int y) {
    float diffx, diffy, dist;
    for (int i = 0; i < nVertices; i++){
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, i);
        diffx = x - (offsetX + pt->x);
		diffy = y - (offsetY + pt->y);
		dist = sqrt(diffx*diffx + diffy*diffy);
		if (dist < vertices[i].radius){
			vertices[i].bOver = true;
		} else {
			vertices[i].bOver = false;
		}	
	}
}

void Plane::mousePressed(int x, int y) {
    if (mode != 0) {
        return;
    }
    float diffx, diffy, dist;
    for (int i = 0; i < nVertices; i++){
        CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, i);
        diffx = x - (offsetX + pt->x);
		diffy = y - (offsetY + pt->y);
		dist = sqrt(diffx*diffx + diffy*diffy);
		if (dist < vertices[i].radius){
			vertices[i].bBeingDragged = true;
		} else {
			vertices[i].bBeingDragged = false;
		}	
	}
}

void Plane::mouseReleased(int x, int y) {
    if (mode != 0) {
        return;
    }
    for (int i = 0; i < nVertices; i++){
		vertices[i].bBeingDragged = false;	
	}
}

void Plane::mouseDragged(int x, int y) {
    if (mode != 0) {
        return;
    }
    for (int i = 0; i < nVertices; i++){
		if (vertices[i].bBeingDragged == true){
			CvPoint3D32f *pt = (CvPoint3D32f*) cvGetSeqElem(srcPtSeq, i);
            pt->x = x - offsetX;
            pt->y = y - offsetY;
        }
	}
}

CvPoint3D32f* Plane::getSrcPtLeft() {
    CvPoint3D32f *lp;
    for(int i=0; i<dstPtSeq->total; i++) {
        CvPoint3D32f *pdst = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        if (!lp || pdst->x < lp->x) {
            lp = pdst;
        }
    }
    return lp;
}

void Plane::drawAlignOffset() {
    ofNoFill();
    ofSetHexColor(color);
    ofBeginShape();
    
    CvPoint3D32f *lpt = getSrcPtLeft();
    int diffx = offsetX - lpt->x;
    int diffy = offsetY - lpt->y;
    
    for(int i=0; i<dstPtSeq->total; i++) {
        CvPoint3D32f *pdst = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        ofVertex(pdst->x + diffx, pdst->y + diffy);
    }
    ofEndShape(true);
    
    for(int i=0; i<dstPtSeq->total; i++) {
        CvPoint3D32f *pdst = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        if (vertices[i].bOver == true || i==0) {
            ofFill();
        } else if (i==1) {
            ofSetHexColor(0xffffff);
            
        } else {
            ofSetHexColor(color);
            ofNoFill();
        }
        ofCircle(pdst->x + diffx, pdst->y + diffy, 4);
    }
    
}

void Plane::draw() {
    ofNoFill();
    ofSetHexColor(color);
    
    ofBeginShape();
    for(int i=0; i<dstPtSeq->total; i++) {
        CvPoint3D32f *pdst = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        ofVertex(offsetX + pdst->x, offsetY + pdst->y);
    }
    ofEndShape(true);
    
    for(int i=0; i<dstPtSeq->total; i++) {
        CvPoint3D32f *pdst = (CvPoint3D32f*) cvGetSeqElem(dstPtSeq, i);
        if (vertices[i].bOver == true || i==0) {
            ofFill();
        } else if (i==1) {
            ofSetHexColor(0xffffff);
            
        } else {
            ofSetHexColor(color);
            ofNoFill();
        }
        ofCircle(offsetX + pdst->x, offsetY + pdst->y, 4);
    }
    
	ofDisableAlphaBlending();
    
}
