/*****************************************************************************
** ZunaVision
** Copyright (c) 2009 
**
** FILENAME:    frame.h
** AUTHOR(S):   Sid Batra <sid@zunavision.com>
** DESCRIPTION:
** Delcares a data structure for encapsulating the functinality of a video frame
*****************************************************************************/

#pragma once

#include "base.h"
#include "position.h"

class Frame
{
 private:
    IplImage* _grey;
    IplImage* _color;
    
    int _index;
    int _clusterIndex;

 public:
    
	Frame();
	Frame( string filename , unsigned index , unsigned clusterIndex);
	Frame( IplImage* image , unsigned index , unsigned clusterIndex);
	Frame(const Frame& f);

	IplImage* generateCutout(Position pos);
	IplImage* generateThumbnail();
	bool isEmpty();

	bool computeNextPosition(Position* p , Position* n , IplImage* vx , IplImage* vy);

	int getIndex();
	unsigned int getClusterIndex();	
	IplImage* getGreyImage();
	IplImage* getColorImage();
	IplImage* getMiniImage();
	
    void free();
    
};

